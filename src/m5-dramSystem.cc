// Copyright (C) 2010 University of Maryland.
// This file is part of DRAMsimII.
//
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <stdlib.h>
#include <cmath>
#include <zlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "enumTypes.hh"
#include "m5-dramSystem.hh"

using std::ofstream;
using std::dec;
using std::hex;
using std::endl;
using std::cerr;
using std::string;
using std::ostream;
using std::vector;
using std::pair;
using std::queue;
using DRAMsimII::Settings;
using DRAMsimII::PhysicalAddress;
using DRAMsimII::Transaction;
using DRAMsimII::System;
using DRAMsimII::Address;
using boost::lexical_cast;
using boost::is_any_of;
using boost::token_compress_on;
using boost::split;
using ::Packet;

//#define TESTNEW

//////////////////////////////////////////////////////////////////////////
/// @brief stuff taken from PhysicalMemory.cc
//////////////////////////////////////////////////////////////////////////
M5dramSystem *M5dramSystemParams::create()
{
	return new M5dramSystem(this);
}

//////////////////////////////////////////////////////////////////////////
/// @brief returns the address range available in this device
//////////////////////////////////////////////////////////////////////////
void M5dramSystem::getAddressRanges(AddrRangeList &resp, bool &snoop)
{
	snoop = false;
	resp.clear();
	resp.push_back(RangeSize(start(), params()->range.size()));
}

//////////////////////////////////////////////////////////////////////////
/// @brief drain all commands from the system in preparation to switch timing modes
//////////////////////////////////////////////////////////////////////////
unsigned int M5dramSystem::drain(::Event *de)
{
	ds->resetToTime((curTick + getCPURatio() - 1) / getCPURatio());

	// copied from PhysicalMemory::drain(Event *de)
	int count = 0;
	for (PortIterator pi = ports.begin(); pi != ports.end(); ++pi) {
		count += (*pi)->drain(de);
	}

	if (count)
		changeState(Draining);
	else
		changeState(Drained);
	return count;
}

//////////////////////////////////////////////////////////////////////
/// @brief move all channels in this system to the specified time
/// @details tells the channels that nothing arrived since the last wakeup and
/// now, so go do whatever would have happened during this time and return finished transactions
/// note that this should probably only process one event per channel at most so that
/// finished transactions can be returned in a timely fashion
/// @author Joe Gross
/// @param now the current time
//////////////////////////////////////////////////////////////////////
void M5dramSystem::moveToTime(const tick now)
{
	// if transactions are returned, then send them back,
	// else if time is not brought up to date, then a refresh transaction has finished
	movement = ds->moveToTime(now);

	ds->getPendingTransactions(finishedTransactions);

	while (finishedTransactions.size() > 0)
	{
		pair<unsigned,tick> currentValue = finishedTransactions.front();
		finishedTransactions.pop();

		assert(currentValue.first < UINT_MAX);
		std::tr1::unordered_map<unsigned,Packet*>::iterator packetIterator = transactionLookupTable.find(currentValue.first);
		assert(packetIterator != transactionLookupTable.end());
		Packet *packet = packetIterator->second;
		transactionLookupTable.erase(packetIterator);
		outstandingPackets--;

		if (packet)
		{
			assert(packet->isRead() || packet->isWrite());
#if 0
			for (std::tr1::unordered_map<unsigned,Packet*>::const_iterator packetIt = transactionLookupTable.begin(); packetIt != transactionLookupTable.end(); packetIt++)
			{
				if (packet->getAddr() == packetIt->second->getAddr())
				{
					cerr << "match at " << std::hex << packet->getAddr() << endl;
				}
			}
#endif

			bool needsResponse = 
#ifdef PROCESS_BEFORE
				true;
#else
				packet->needsResponse();
			doAtomicAccess(packet);		
#endif

			if (needsResponse)
			{	
				assert(!packet->isWrite());
				assert(curTick < static_cast<Tick>(currentValue.second * getCPURatio()));

				M5_TIMING("<-T [@" << dec << static_cast<Tick>(currentValue.second * getCPURatio()) << "][+" << static_cast<Tick>(currentValue.second * getCPURatio() - curTick) << "]")

					ports[lastPortIndex]->doSendTiming(packet, static_cast<Tick>(currentValue.second * getCPURatio()));
				assert(lastPortIndex == 0);
#if 0
				static tick returnCount;


				if (++returnCount % 100000 == 0)
				{
					time_t rawtime;
					time(&rawtime);
					struct tm *timeinfo = localtime(&rawtime);		
					//char *timeString = asctime(timeinfo);
					//char *pos = strchr(timeString,'\n');
					//*(pos-1) = NULL;
					cerr << "\r" << std::dec << returnCount / 100000 << " " << asctime(timeinfo) << "\r";
				}
#endif
			}
			else
			{	
				assert(!packet->isRead());
				M5_TIMING("xT [" << hex << packet->getAddr() << "]")

					delete packet;
			}			
		}	
		else
		{
#ifndef PROCESS_BEFORE
			cerr << "warn: no Packet found for corresponding transaction" << endl;
#endif
		}
	}	
}

//////////////////////////////////////////////////////////////////////
/// @brief builds a M5dramSystem object
/// @details looks for the settings file and constructs a dramSystem object from that
/// @author Joe Gross
/// @param p the M5 parameters object to extract parameters from
/// @return a new M5dramSystem object
//////////////////////////////////////////////////////////////////////
M5dramSystem::M5dramSystem(const Params *p):
PhysicalMemory(p),
tickEvent(this),
ports(1),
lastPortIndex(0),
ds(NULL),
cpuRatio(0),
transactionLookupTable(),
currentTransactionID(0),
movement(true),
outstandingPackets(0)
{	
	Settings settings;

	// initialize the settings according to the autogenerated python interface
	settings.setKeyValue("type",p->dramType);
	settings.setKeyValue("clockGranularity",p->clockGranularity);
	settings.setKeyValue("channels",p->channels);
	settings.setKeyValue("dimms",p->dimms);
	settings.setKeyValue("ranks",p->ranks);
	settings.setKeyValue("banks",p->banks);
	settings.setKeyValue("rows",p->rows);
	settings.setKeyValue("columns",p->columns);
	settings.setKeyValue("physicalAddressMappingPolicy",p->physicalAddressMappingPolicy);
	settings.setKeyValue("rowBufferPolicy",p->rowBufferPolicy);
	settings.setKeyValue("rowSize",p->rowSize);
	settings.setKeyValue("columnSize",p->columnSize);
	settings.setKeyValue("channelWidth", lexical_cast<string>(p->channelWidth));
	settings.setKeyValue("postedCAS",p->postedCas);
	settings.setKeyValue("autoRefreshPolicy",p->autoRefreshPolicy);
	settings.setKeyValue("commandOrderingAlgorithm",p->commandOrderingAlgorithm);
	settings.setKeyValue("transactionOrderingAlgorithm",p->transactionOrderingAlgorithm);
	settings.setKeyValue("perBankQueueDepth",p->perBankQueueDepth);
	settings.setKeyValue("cacheLineSize",p->cacheLineSize);
	settings.setKeyValue("transactionQueueDepth",p->transactionQueueDepth);
	settings.setKeyValue("decodeWindow",p->decodeWindow);
	settings.setKeyValue("readWriteGrouping",p->readWriteGrouping);
	settings.setKeyValue("autoPrecharge",p->autoPrecharge);
	// timing parameters
	settings.setKeyValue("tBufferDelay",lexical_cast<string>(p->tBufferDelay));
	settings.setKeyValue("tBurst",lexical_cast<string>(p->tBurst));
	settings.setKeyValue("tCAS",lexical_cast<string>(p->tCAS));
	settings.setKeyValue("tCMD",lexical_cast<string>(p->tCMD));
	settings.setKeyValue("tCWD",lexical_cast<string>(p->tCWD));
	settings.setKeyValue("tFAW",lexical_cast<string>(p->tFAW));
	settings.setKeyValue("tRAS",lexical_cast<string>(p->tRAS));
	settings.setKeyValue("tRC",lexical_cast<string>(p->tRC));
	settings.setKeyValue("tRCD",lexical_cast<string>(p->tRCD));
	settings.setKeyValue("tRFC",lexical_cast<string>(p->tRFC));
	settings.setKeyValue("tRRD",lexical_cast<string>(p->tRRD));
	settings.setKeyValue("tRP",lexical_cast<string>(p->tRP));
	settings.setKeyValue("tRTP",lexical_cast<string>(p->tRTP));
	settings.setKeyValue("tRTRS",lexical_cast<string>(p->tRTRS));
	settings.setKeyValue("tWR",lexical_cast<string>(p->tWR));
	settings.setKeyValue("tWTR",lexical_cast<string>(p->tWTR));
	settings.setKeyValue("tAL",lexical_cast<string>(p->tAL));
	settings.setKeyValue("tREFI",lexical_cast<string>(p->tREFI));
	settings.setKeyValue("seniorityAgeLimit",lexical_cast<string>(p->seniorityAgeLimit));
	// power settings
	settings.setKeyValue("PdqRD",lexical_cast<string>(p->PdqRD));
	settings.setKeyValue("PdqWR",lexical_cast<string>(p->PdqWR));
	settings.setKeyValue("PdqRDoth",lexical_cast<string>(p->PdqRDoth));
	settings.setKeyValue("PdqWRoth",lexical_cast<string>(p->PdqWRoth));
	settings.setKeyValue("DQperDRAM",lexical_cast<string>(p->DQperDRAM));
	settings.setKeyValue("DQSperDRAM",lexical_cast<string>(p->DQSperDRAM));
	settings.setKeyValue("DMperDRAM",lexical_cast<string>(p->DMperDRAM));
	settings.setKeyValue("frequencySpec",lexical_cast<string>(p->frequencySpec));
	settings.setKeyValue("maxVCC",lexical_cast<string>(p->maxVCC));
	settings.setKeyValue("systemVDD",lexical_cast<string>(p->systemVDD));
	settings.setKeyValue("IDD0",lexical_cast<string>(p->IDD0));
	settings.setKeyValue("IDD2P",lexical_cast<string>(p->IDD2P));
	settings.setKeyValue("IDD2N",lexical_cast<string>(p->IDD2N));
	settings.setKeyValue("IDD3N",lexical_cast<string>(p->IDD3N));
	settings.setKeyValue("IDD3P",lexical_cast<string>(p->IDD3P));
	settings.setKeyValue("IDD4R",lexical_cast<string>(p->IDD4R));
	settings.setKeyValue("IDD4W",lexical_cast<string>(p->IDD4W));
	settings.setKeyValue("IDD5A",lexical_cast<string>(p->IDD5A));
	// DIMM cache
	settings.setKeyValue("associativity", lexical_cast<string>(p->associativity));
	settings.setKeyValue("cacheSize", lexical_cast<string>(p->cacheSize));
	settings.setKeyValue("blockSize", lexical_cast<string>(p->blockSize));
	settings.setKeyValue("hitLatency", lexical_cast<string>(p->hitLatency));
	settings.setKeyValue("replacementPolicy", p->replacementPolicy);
	settings.setKeyValue("nmruTrackingCount", p->nmruTrackingCount);	
	// other simulation parameters
	settings.setKeyValue("requestCount", lexical_cast<string>(p->requestCount));
	settings.setKeyValue("epoch",lexical_cast<string>(p->epoch));
	settings.setKeyValue("outFileType", p->outFileType);

	if (p->settingsFile.length() > 0)
	{
		const char *settingsMap[] = {"m5", "--config-file", p->settingsFile.c_str(), "--modifiers", p->extraParameters.c_str()};

		settings.loadSettingsFromFile(5, const_cast<char **>(settingsMap));
	}
	else if (p->extraParameters.length() > 0)
	{
		string params = p->extraParameters;
		boost::algorithm::trim(params);
		std::vector<string> paramList;
		split(paramList, params, is_any_of(" "), token_compress_on);

		settings.loadSettings(paramList);
	}

	settings.inFile = "";
	if (p->outFilename.length() > 0)
		settings.outFile = p->outFilename;

	if (p->commandLine.length() > 0)
		settings.commandLine = p->commandLine;	

	ds = new System(settings);

#ifdef TRACE_GENERATE
	traceOutStream.push(gzip_compressor(gzip_params(9)));
	//traceOutStream.push(cout);

	path outDir(settings.outFileDir.c_str());

	if (!exists(outDir))
	{
		if (!create_directory(outDir))
		{
			cerr << "Could not create dir " << outDir.leaf() << " and dir does not exist." << endl;
			exit(-1);
		}
	}
	else
	{
		if (!is_directory(outDir))
		{
			cerr << "Directory " << outDir.leaf() << " exists, but is not a directory." << endl;
			exit(-1);
		}
	}


	if (settings.outFileType == DRAMsimII::GZ)
	{
		path tracepath;
		unsigned counter = 0;
		//string tracefilename;
		string baseFilename = settings.outFile;

		do
		{
			stringstream tracefilename;

			tracefilename.str("");
			tracefilename << baseFilename << setfill('0') << setw(3) << counter << "-trace.gz";
			counter++;
			tracepath = outDir / tracefilename.str();
		} while (exists(tracepath));

		traceOutStream.push(file_sink(tracepath.native_directory_string().c_str()));

		if (!traceOutStream.good())
		{
			cerr << "Error opening file \"" << tracepath.native_directory_string() << "\" for writing" << endl;
			exit(-12);
		}

	}

#endif

	cpuRatio =(int)round(((float)SimClock::Frequency/((float)ds->Frequency())));

	M5_TIMING(*ds)
}


//////////////////////////////////////////////////////////////////////////
/// @brief returns a port for other devices to connect to
//////////////////////////////////////////////////////////////////////////
Port *M5dramSystem::getPort(const string &if_name, int idx)
{
	// Accept request for "functional" port for backwards compatibility
	// with places where this function is called from C++.  I'd prefer
	// to move all these into Python someday.
	if (if_name == "functional")
	{
		return new MemoryPort(csprintf("%s-functional", name()), this);
	}

	if (if_name != "port")
	{
		panic("PhysicalMemory::getPort: unknown port %s requested", if_name);
	}

	if (idx >= ports.size())
	{
		ports.resize(idx+1);
	}

	if (ports[idx] != NULL)
	{
		panic("PhysicalMemory::getPort: port %d already assigned", idx);
	}

	MemoryPort *port = new MemoryPort(csprintf("%s-port%d", name(), idx), this);

	lastPortIndex = idx;
	ports[idx] = port;
	M5_TIMING("called M5dramSystem::getPort")
		return port;
}

//////////////////////////////////////////////////////////////////////////
/// @brief initializes the ports of this object
//////////////////////////////////////////////////////////////////////////
void M5dramSystem::init()
{
	if (ports.size() == 0)
	{
		fatal("M5dramSystem object %s is unconnected!", name());
	}

	for (PortIterator pi = ports.begin(); pi != ports.end(); ++pi)
	{
		if (*pi)
			(*pi)->sendStatusChange(Port::RangeChange);
	}
}

void M5dramSystem::regStats()
{
	registerExitCallback(new ExitEvent(ds));
}

//////////////////////////////////////////////////////////////////////////
/// @brief deletes the DRAMsim object
//////////////////////////////////////////////////////////////////////////
M5dramSystem::~M5dramSystem()
{	
	M5_TIMING("M5dramSystem destructor");

#ifdef TRACE_GENERATE
	traceOutStream.flush();
	boost::iostreams::close(traceOutStream);	
	//traceOutStream.close();
#endif
	cerr.flush();
	delete ds;
}

//////////////////////////////////////////////////////////////////////////
/// @brief dumps the memory array to a file as part of a systemwide checkpoint
//////////////////////////////////////////////////////////////////////////
void M5dramSystem::serialize(ostream &os)
{
	gzFile compressedMem;
	string filename = name() + ".physmem";
	string dsFilename = name() + ".ds2";

	SERIALIZE_SCALAR(filename);
	SERIALIZE_SCALAR(dsFilename);

	// write memory file
	string thefile = Checkpoint::dir() + "/" + filename.c_str();

	int fd = creat(thefile.c_str(), 0664);
	if (fd < 0)
	{
		perror("creat");
		fatal("Can't open physical memory checkpoint file '%s'\n", filename);
	}

	compressedMem = gzdopen(fd, "wb");

	if (compressedMem == NULL)
	{
		fatal("Insufficient memory to allocate compression state for %s\n", filename);
	}

	if (gzwrite(compressedMem, pmemAddr, params()->range.size()) != params()->range.size())
	{
		fatal("Write failed on physical memory checkpoint file '%s'\n", filename);
	}

	if (gzclose(compressedMem))
	{
		fatal("Close failed on physical memory checkpoint file '%s'\n", filename);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief restores the memory array after resuming from a checkpoint
//////////////////////////////////////////////////////////////////////////
void M5dramSystem::unserialize(Checkpoint *cp, const std::string &section)
{
	gzFile compressedMem;
	long *tempPage;
	long *pmem_current;
	uint64_t curSize;
	uint32_t bytesRead;
	const int chunkSize = 16384;


	string filename, dsFilename;

	UNSERIALIZE_SCALAR(filename);
	UNSERIALIZE_SCALAR(dsFilename);

	filename = cp->cptDir + "/" + filename;

	// mmap memoryfile
	int fd = open(filename.c_str(), O_RDONLY);

	if (fd < 0)
	{
		perror("open");
		fatal("Can't open physical memory checkpoint file '%s'", filename);
	}

	compressedMem = gzdopen(fd, "rb");

	if (compressedMem == NULL)
	{
		fatal("Insufficient memory to allocate compression state for %s\n", filename);
	}

	// unmap file that was mmaped in the constructor
	// This is done here to make sure that gzip and open don't muck with our
	// nice large space of memory before we reallocate it
	munmap((char*)pmemAddr, params()->range.size());

	pmemAddr = (uint8_t *)mmap(NULL, params()->range.size(),
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	if (pmemAddr == (void *)MAP_FAILED)
	{
		perror("mmap");
		fatal("Could not mmap physical memory!\n");
	}

	curSize = 0;
	tempPage = (long*)malloc(chunkSize);
	if (tempPage == NULL)
		fatal("Unable to malloc memory to read file %s\n", filename);
	/* Only copy bytes that are non-zero, so we don't give the VM system hell */
	while (curSize < params()->range.size())
	{
		bytesRead = gzread(compressedMem, tempPage, chunkSize);
		if (bytesRead != chunkSize &&
			bytesRead != params()->range.size() - curSize)
			fatal("Read failed on physical memory checkpoint file '%s'"
			" got %d bytes, expected %d or %d bytes\n",
			filename, bytesRead, chunkSize,
			params()->range.size() - curSize);

		assert(bytesRead % sizeof(long) == 0);

		for (int x = 0; x < bytesRead/sizeof(long); x++)
		{
			if (*(tempPage+x) != 0) {
				pmem_current = (long*)(pmemAddr + curSize + x * sizeof(long));
				*pmem_current = *(tempPage+x);
			}
		}
		curSize += bytesRead;
	}

	free(tempPage);

	if (gzclose(compressedMem))
		fatal("Close failed on physical memory checkpoint file '%s'\n",filename);
}

//////////////////////////////////////////////////////////////////////
/// @brief receive a packet and do something with it
/// @details receive a packet, check it to make sure it's valid, then try to enqueue it in DS2
/// @author Joe Gross
/// @param packet the pointer to the memory transaction packet
/// @return true if the packet is accepted, false otherwise
//////////////////////////////////////////////////////////////////////
bool M5dramSystem::MemoryPort::recvTiming(PacketPtr packet)
{ 	
#if defined(M5DEBUG) && defined(DEBUG) && !defined(NDEBUG)
	using std::setw;
	M5_TIMING3("+recvTiming [" << std::dec << curTick << "] ")
		// calculate the time elapsed from when the transaction started
		M5_TIMING3(setw(2) << (packet->isRead() ? "Rd" : ""))
		M5_TIMING3(setw(2) << (packet->isWrite() ? "Wr" : ""))
		//M5_TIMING2((packet->isRequest() ? "Rq" : ""))
		M5_TIMING3(setw(3) << (packet->isInvalidate() ? "Inv" : ""))
		M5_TIMING3(setw(3) << (packet->isResponse() ? "Rsp" : ""))
		M5_TIMING3(setw(2) << (packet->isReadWrite() ? "RW" : ""))
		M5_TIMING3(setw(2) << (packet->isError() ? "Er" : ""))
		M5_TIMING3(setw(2) << (packet->isPrint() ? "Pr" : ""))
		M5_TIMING3(setw(2) << (packet->needsExclusive() ? "Ex" : ""))
		M5_TIMING3(setw(2) << (packet->needsResponse() ? "NR" : ""))
		M5_TIMING3(setw(2) << (packet->isLLSC() ? "LL" : ""))
		M5_TIMING3(" 0x" << hex << packet->getAddr() << endl)
		M5_TIMING2(" s[0x" << hex << packet->getSize() << "]");
#endif
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
#ifdef TRACE_GENERATE
	{
		if (packet->memInhibitAsserted()) 
		{
			delete packet;
			return true;
		}
		if (packet->isRead() || packet->isWrite())
		{
			memory->traceOutStream << std::hex << packet->getAddr() << " " << (packet->isWrite() ? "W" : "R") << " " << std::fixed <<  curTick * 1E9 / Clock::Frequency << " " << std::hex << (packet->req->hasPC() ? packet->req->getPC() : 0) <<  endl;
		}

		bool needsResponse = packet->needsResponse();
		recvAtomic(packet);
		// turn packet around to go back to requester if response expected
		if (needsResponse) 
		{
			assert(packet->isResponse());
			schedSendTiming(packet, curTick + 1);
		}
		else 
		{
			delete packet;
		}

		return true;
	}	
#endif
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	// everything that reaches the memory system should be a request of some sort
	assert(packet->isRequest());


	if (packet->memInhibitAsserted())
	{
		// snooper will supply based on copy of packet
		// still target's responsibility to delete packet
		delete packet;
		return true;
	}	

	bool needsResponse = packet->needsResponse();

	// must look at packets that need to affect the memory system
	if (packet->isRead() || packet->isWrite())
	{	
		assert((packet->isRead() && packet->needsResponse()) ||
			(!packet->isRead() && !packet->needsResponse()));
		assert(!packet->wasNacked());

		//assert(memory->ds->pendingTransactionCount() == 0);

		// turn packet around to go back to requester if response expected
		Address addr(packet->getAddr());

		// attempt to add the transaction to the memory system
		if (memory->ds->isFull(addr.getChannel()))
		{
#ifdef M5DEBUG						
			static tick numberOfDelays = 0;
			if (++numberOfDelays % 100000 == 0)
				cerr << "\rdelays = " << numberOfDelays;
#endif
#if 0
			// if the packet did not fit, then send a NACK
			// tell the sender that the memory system is full until it hears otherwise
			// and do not send packets until that time
			pkt->result = Packet::Nacked;
			short memID = pkt->getDest();
			pkt->makeTimingResponse();
			pkt->setSrc(memID);
			doSendTiming(pkt,0);
#endif

			// http://m5.eecs.umich.edu/wiki/index.php/Memory_System
			// keep track of the fact that the memory system is waiting to hear that it is ok to send again
			// as well as what channel it is likely to retry to (make sure there is room before sending the OK)
			//memory->needRetry = true;
			//memory->mostRecentChannel = trans->getAddresses().getChannel();
			//memory->mostRecentChannel = addr.getChannel();
			//delete trans;
			M5_TIMING2("Wait for retry before sending more to ch[" << addr.getChannel() << "]");
			return false;
		}
		else
		{
			tick currentMemCycle = (curTick + memory->getCPURatio() - 1) / memory->getCPURatio();

			// move channels to current time so that calculations based on current channel times work
			// should also not start/finish any commands, since this would happen at a scheduled time
			// instead of now
			memory->moveToTime(currentMemCycle);

			int threadID = packet->req->hasContextId() ? packet->req->contextId() : 0;

			PhysicalAddress pC = packet->req->hasPC() ? packet->req->getPC() : 0;

			Transaction::TransactionType packetType = packet->req->isPrefetch() ? Transaction::PREFETCH_TRANSACTION :
				(packet->req->isInstFetch() ? Transaction::READ_TRANSACTION :
				(packet->isRead() ? Transaction::READ_TRANSACTION :
				(packet->isWrite() ? Transaction::WRITE_TRANSACTION :
				Transaction::READ_TRANSACTION)));

			assert(packet->isRead() ^ (Transaction::WRITE_TRANSACTION == packetType));
			assert(packet->isWrite() ^ (Transaction::READ_TRANSACTION == packetType));

#ifndef NDEBUG
			bool result = 
#endif
				memory->ds->enqueue(new Transaction(packetType,currentMemCycle,packet->getSize() / 8,packet->getAddr(), pC, threadID, memory->currentTransactionID));

			(memory->outstandingPackets)++;

			assert(result == true);

			assert(memory->transactionLookupTable.find(memory->currentTransactionID) == memory->transactionLookupTable.end());

#ifdef PROCESS_BEFORE
			bool needsResponse = packet->needsResponse();
			memory->doAtomicAccess(packet);
			memory->transactionLookupTable[memory->currentTransactionID] = needsResponse ? packet : NULL;
			if (!needsResponse)
				delete packet;
#else
			memory->transactionLookupTable[memory->currentTransactionID] = packet;			
#endif

			// make sure not to use any that are already being used
			while (memory->transactionLookupTable.find(memory->currentTransactionID) != memory->transactionLookupTable.end())
			{
				memory->currentTransactionID = (memory->currentTransactionID + 1) % UINT_MAX;
			}

			// find out when the next event is
			tick next = memory->ds->nextTick();
			assert(next < TICK_MAX);
			assert(next > currentMemCycle);
			assert(next * memory->getCPURatio() > curTick);

			// deschedule and reschedule yourself to wake at the next event time
			if (memory->tickEvent.scheduled())
				memory->tickEvent.deschedule();

			memory->tickEvent.schedule(next * memory->getCPURatio());

			M5_TIMING2("-recvTiming sch[" << next << "]");
		}
	}
	else
	{
		recvAtomic(packet);
		if (needsResponse)
		{
			assert(packet->isResponse());
			schedSendTiming(packet,curTick + 1);
		}
		else
		{
			delete packet;
		}
	}
	//upgrade or invalidate	
	//else if (!packet->isRead() && packet->needsResponse())
#if 0
	else if (packet->isInvalidate())
	{
		assert(packet->cmd != MemCmd::SwapReq);
		assert(!packet->isRead() && packet->needsResponse());
		if (packet->needsResponse())
		{
			packet->makeAtomicResponse();
			schedSendTiming(packet,curTick + 1);	
		}
	}
	else
	{
		assert(!packet->needsResponse());
		M5_TIMING2("warn: packet not needing response.");

		if (packet->cmd != MemCmd::UpgradeReq)
		{
			cerr << "warn: deleted packet, not upgradereq, not write, needs no resp" << endl;
			delete packet->req;
			delete packet;
		}
		else
		{
			cerr << "warn: not upgrade request" << endl;
		}		
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////
/// @brief builds a memory port to interact with other components
/// @param _name the friendly name of this object
/// @param _memory the pointer to the M5dramSystem object that will be backing this object
/// @return a new memoryPort object associated with a dramSystem object
//////////////////////////////////////////////////////////////////////
M5dramSystem::MemoryPort::MemoryPort(const string &_name, M5dramSystem *_memory):
SimpleTimingPort(_name, _memory),
memory(_memory)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief returns the block size for this module
//////////////////////////////////////////////////////////////////////////
int M5dramSystem::MemoryPort::deviceBlockSize()
{
	return memory->deviceBlockSize();
}

//////////////////////////////////////////////////////////////////////////
/// @brief replies to requests when in functional mode
/// @details simply returns a memory request instantly when running in atomic mode \n
/// does not affect the underlying memory system
//////////////////////////////////////////////////////////////////////////
void M5dramSystem::MemoryPort::recvFunctional(PacketPtr pkt)
{
	if (!checkFunctional(pkt))
	{
		// Default implementation of SimpleTimingPort::recvFunctional()
		// calls recvAtomic() and throws away the latency; we can save a
		// little here by just not calculating the latency.
		memory->doFunctionalAccess(pkt);
	}
}

//////////////////////////////////////////////////////////////////////
/// @brief receive a read/write request to be handled without a timing model
/// @author Joe Gross
/// @param pkt a packet containing a transaction
/// @return
//////////////////////////////////////////////////////////////////////
Tick M5dramSystem::MemoryPort::recvAtomic(PacketPtr pkt)
{
	//M5_TIMING_LOG("M5dramSystem recvAtomic()");

	// because this is simply a read or write, go to the M5 memory object exclusively
	// and ignore the timing from DRAMsimII
	/// @todo have DS2 also account for this but avoid doing timing calculations
	return memory->doAtomicAccess(pkt);
}

//////////////////////////////////////////////////////////////////////
/// @brief receive a change of status
/// @param status the new status for this port
//////////////////////////////////////////////////////////////////////
void M5dramSystem::MemoryPort::recvStatusChange(Port::Status status)
{
	memory->recvStatusChange(status);
}

//////////////////////////////////////////////////////////////////////
/// @brief get the range of addresses that this device will accept
/// @param resp the range reference to be completed
/// @param snoop whether this is snooped
//////////////////////////////////////////////////////////////////////
void M5dramSystem::MemoryPort::getDeviceAddressRanges(AddrRangeList &resp, bool &snoop)
{
	memory->getAddressRanges(resp, snoop);
}

//////////////////////////////////////////////////////////////////////
/// @brief wake up via the global event queue
/// @details wake up and perform some action at this time, then determine when the next wakeup time should be
/// inserts the memory system into the event queue and handles being woken by it
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void M5dramSystem::TickEvent::process()
{		
	tick currentMemCycle = (curTick + memory->getCPURatio() - 1) / memory->getCPURatio();

	M5_TIMING2("+process [" << dec << currentMemCycle << "]");

	// move memory channels to the current time
	memory->moveToTime(currentMemCycle);

	if (!memory->movement)
	{
		cerr << "no r/w bytes, outstanding: " << memory->outstandingPackets << ", ";
		cerr << (memory->transactionLookupTable.empty() ? "is empty," : "has outstanding transactions,");
		cerr << (memory->ds->isEmpty() ? " reports empty" : " reports not empty") << endl;
	}

	// deschedule yourself
	if (memory->tickEvent.scheduled())
		memory->tickEvent.deschedule();

	// determine the next time to wake up
	tick next = memory->ds->nextTick();	

	assert(next > currentMemCycle);
	assert(next * memory->getCPURatio() > curTick);

	schedule(next * memory->getCPURatio());

	M5_TIMING2("-process sch[" << next << "]")
}

//////////////////////////////////////////////////////////////////////////
/// @brief the TickEvent constructor
/// @details sets a pointer to the underlying DRAMsim memory system
//////////////////////////////////////////////////////////////////////////
M5dramSystem::TickEvent::TickEvent(M5dramSystem *c)
: Event(CPU_Tick_Pri), memory(c)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief returns a description of this component
//////////////////////////////////////////////////////////////////////////
const char *M5dramSystem::TickEvent::description()
{
	return "m5dramSystem tick event";	
}


