// Copyright (C) 2008 University of Maryland.
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

#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>
#include <stdlib.h>
#include <cmath>
#include <zlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "enumTypes.h"
#include "m5-dramSystem.h"

using std::ofstream;
using boost::archive::text_oarchive;
using std::dec;
using std::hex;
using std::endl;
using std::cerr;
using std::string;
using std::ostream;
using std::vector;
using std::pair;
using std::queue;
using namespace DRAMsimII;
using namespace boost;

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
	ds->moveToTime(now);

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

		if (packet)
		{
			assert(packet->isRead() || packet->isWrite());

			bool needsResponse = packet->needsResponse();

			doAtomicAccess(packet);		

			if (needsResponse)
			{			
				assert(curTick <= static_cast<Tick>(currentValue.second * getCPURatio()));

				M5_TIMING_LOG("<-T [@" << dec << static_cast<Tick>(currentValue.second * getCPURatio()) << "][+" << static_cast<Tick>(currentValue.second * getCPURatio() - curTick) << "] at" << curTick);

				ports[lastPortIndex]->doSendTiming(packet, static_cast<Tick>(currentValue.second * getCPURatio()));
#if 0
				static tick returnCount;


				if (++returnCount % 10000 == 0)
				{
					cerr << returnCount << "\r";
				}
#endif
			}
			else
			{				
				delete packet;
			}			
		}	
		else
		{
			cerr << "warn: no Packet found for corresponding transaction" << endl;
		}
	}

	// if there is now room, allow a retry to happen
	if (needRetry && !ds->isFull(mostRecentChannel))
	{
		M5_TIMING_LOG("Allow retrys");

		needRetry = false;
		ports[lastPortIndex]->sendRetry();
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
needRetry(false),
mostRecentChannel(0),
cpuRatio(0),
transactionLookupTable(),
currentTransactionID(0)
{	
	timingOutStream << "M5dramSystem constructor" << endl;

	const char *settingsMap[] = {"m5", "--config-file", p->settingsFile.c_str(), "--modifiers",p->extraParameters.c_str()};

	Settings settings(5,const_cast<char **>(settingsMap));

	settings.inFile = "";
	if (p->outFilename.length() > 0)
		settings.outFile = p->outFilename;

	if (p->commandLine.length() > 0)
		settings.commandLine = p->commandLine;	

	// if this is a normal system or a fbd system
	if (settings.systemType == FBD_CONFIG)
		ds = new fbdSystem(settings);	
	else
		ds = new System(settings);

	//std::cerr << p->extraParameters << std::endl;
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
	

	if (settings.outFileType == GZ)
	{
		stringstream tracefilename;
		path tracepath;
		unsigned counter = 0;
		//string tracefilename;
		string baseFilename = settings.outFile;


		do
		{
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

	cpuRatio =(int)round(((float)Clock::Frequency/((float)ds->Frequency())));
	//cerr << cpuRatio << endl;
	//invCpuRatio = (float)((double)ds->Frequency()/(Clock::Frequency));
	//cerr << invCpuRatio << endl;

	timingOutStream << *ds << endl;
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
	timingOutStream << "called M5dramSystem::getPort" << endl;
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

//////////////////////////////////////////////////////////////////////////
/// @brief deletes the DRAMsim object
//////////////////////////////////////////////////////////////////////////
M5dramSystem::~M5dramSystem()
{
	//if (pmemAddr)
	//	munmap(pmemAddr, params()->addrRange.size());

	timingOutStream << "M5dramSystem destructor" << endl;
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

	// write the
	string theDs2File = Checkpoint::dir() + "/" + dsFilename;

	ofstream outStream(theDs2File.c_str());
	text_oarchive outputArchive(outStream);

	const System* const dsConst = ds;
	outputArchive << dsConst;
	outputArchive << lastPortIndex;
	outputArchive << needRetry;
	outputArchive << mostRecentChannel;
	outputArchive << cpuRatio;
	//outputArchive << transactionLookupTable;
	outStream.close();
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
		fatal("Close failed on physical memory checkpoint file '%s'\n",
		filename);


}

//////////////////////////////////////////////////////////////////////
/// @brief receive a packet and do something with it
/// @details receive a packet, check it to make sure it's valid, then try to enqueue it in DS2
/// @author Joe Gross
/// @param pkt the pointer to the memory transaction packet
/// @return true if the packet is accepted, false otherwise
//////////////////////////////////////////////////////////////////////
bool M5dramSystem::MemoryPort::recvTiming(PacketPtr pkt)
{ 		
#ifdef TRACE_GENERATE
	bool needsResponse = pkt->needsResponse();
	if (pkt->memInhibitAsserted())
	{
		// snooper will supply based on copy of packet
		// still target's responsibility to delete packet
		//cerr << "deleted memInhibitAsserted" << endl;
		delete pkt;
		return true;
	}
	memory->doAtomicAccess(pkt);

	if (pkt->isRead() || pkt->isWrite())
		memory->traceOutStream << std::hex << pkt->getAddr() << " " << (pkt->isWrite() ? "W" : "R") << " " << std::fixed <<  curTick * 1E9 / Clock::Frequency << " " << std::hex << (pkt->req->hasPC() ? pkt->req->getPC() : 0) <<  endl;

	if (needsResponse)
	{
		//timingOutStream << "sending packet back at " << dec << static_cast<Tick>(curTick + 95996) << endl;
		memory->ports[memory->lastPortIndex]->doSendTiming(pkt, static_cast<Tick>(curTick + 1));
		//memory->ports[memory->lastPortIndex]->doSendTiming(pkt,curTick + 18750 + rand()%400);
		//schedSendTiming(pkt,curTick + 20000 + rand()%20000);
		//schedSendTiming(pkt,curTick + 200000 + randomGen.random((Tick)0, (Tick)200000));
	}
	else
	{
		//delete pkt->req;
		delete pkt;
	}
	return true;
#endif

	//////////////////////////////////////////////////////////////////////////
	// FIXME: shouldn't need to turn away packets, the requester should hold off once NACK'd
	if (memory->needRetry)
	{
		//static unsigned numCallsWhileBlocked = 1;
		//if (numCallsWhileBlocked++ % 100000 == 0)
		//	cerr << numCallsWhileBlocked << "\r";
		M5_TIMING_LOG("warn: attempted packet send after packet is nacked");
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	tick currentMemCycle = (curTick + memory->getCPURatio() - 1) / memory->getCPURatio();

	// everything that reaches the memory system should be a request of some sort
	assert(pkt->isRequest());

#if defined(M5DEBUG) && defined(DEBUG) && !defined(NDEBUG)
	using std::setw;
	timingOutStream << "+recvTiming [" << std::dec << currentMemCycle << "] ";
	// calculate the time elapsed from when the transaction started
	timingOutStream << setw(2) << (pkt->isRead() ? "Rd" : "");
	timingOutStream << setw(2) << (pkt->isWrite() ? "Wr" : "");
	//timingOutStream << setw(2) << (pkt->isRequest() ? "Rq" : "");
	timingOutStream << setw(3) << (pkt->isInvalidate() ? "Inv" : "");
	timingOutStream << setw(3) << (pkt->isResponse() ? "Rsp" : "");
	timingOutStream << setw(2) << (pkt->isReadWrite() ? "RW" : "");
	timingOutStream << setw(2) << (pkt->isError() ? "Er" : "");
	timingOutStream << setw(2) << (pkt->isPrint() ? "Pr" : "");
	timingOutStream << setw(2) << (pkt->needsExclusive() ? "Ex" : "");
	timingOutStream << setw(2) << (pkt->needsResponse() ? "NR" : "");
	timingOutStream << setw(2) << (pkt->isLocked() ? "L" : "");

	timingOutStream << " 0x" << hex << pkt->getAddr();

	timingOutStream << " s[0x" << hex << pkt->getSize() << "]" << endl;
#endif
	if (pkt->memInhibitAsserted())
	{
		// snooper will supply based on copy of packet
		// still target's responsibility to delete packet
		//cerr << "deleted memInhibitAsserted" << endl;
		delete pkt;
		return true;
	}
	//upgrade or invalidate	
	else if (!pkt->isRead() && pkt->needsResponse())
		//else if (pkt->isInvalidate())
	{
		//cerr << "Invalidate" << endl;
		assert(pkt->cmd != MemCmd::SwapReq);
		assert(pkt->isInvalidate());
		if (pkt->needsResponse())
		{
			pkt->makeAtomicResponse();
		}

		schedSendTiming(pkt,curTick + 1);			

		return true;
	}
#if 0
	else if (!pkt->isRead() && pkt->needsResponse())
	{
		cerr << "didn't catch all " << endl;
		return true;
	}
#endif
	// must look at packets that need to affect the memory system
	else if (pkt->needsResponse() || pkt->isWrite())
	{	
		assert((pkt->isRead() && pkt->needsResponse()) ||
			(!pkt->isRead() && !pkt->needsResponse()));
		assert(!pkt->wasNacked());


		// move channels to current time so that calculations based on current channel times work
		// should also not start/finish any commands, since this would happen at a scheduled time
		// instead of now
		memory->moveToTime(currentMemCycle);

		// turn packet around to go back to requester if response expected
		Address addr(pkt->getAddr());

		// attempt to add the transaction to the memory system
		if (memory->ds->isFull(addr.getChannel()))
		{
#ifdef M5DEBUG						
			static tick numberOfDelays = 0;
			if (++numberOfDelays % 100000 == 0)
				cerr << "\r" << numberOfDelays;
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
			memory->needRetry = true;
			//memory->mostRecentChannel = trans->getAddresses().getChannel();
			memory->mostRecentChannel = addr.getChannel();
			//delete trans;

			M5_TIMING_LOG("Wait for retry before sending more to ch[" << addr.getChannel() << "]");
			return false;
		}
		else
		{
			if (pkt->cmd == MemCmd::SwapReq)
				cerr << "swap" << endl;

			PhysicalAddress pC;
			int threadID;
			if (pkt->req->hasPC())
			{
				pC = pkt->req->getPC();
				threadID = pkt->req->threadId();
			}
			else
			{
				pC = threadID = 0;
			}

			Transaction::TransactionType packetType = Transaction::PREFETCH_TRANSACTION;

			if (pkt->isRead())
				packetType = Transaction::READ_TRANSACTION;
			else if (pkt->isWrite())
				packetType = Transaction::WRITE_TRANSACTION;
			else
				cerr << "warn: not read or write" << endl;
			
#ifndef NDEBUG
			bool result = 
#endif
				memory->ds->enqueue(new Transaction(packetType,currentMemCycle,pkt->getSize() / 8,pkt->getAddr(), pC, threadID, memory->currentTransactionID));

			assert(result == true);

			memory->transactionLookupTable[memory->currentTransactionID] = pkt;

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
			
			M5_TIMING_LOG("-recvTiming goto[" << next << "]");
			
			return true;
		}
	}
	else
	{
		M5_TIMING_LOG("warn: packet not needing response.");

		if (pkt->cmd != MemCmd::UpgradeReq)
		{
			cerr << "warn: deleted pkt, not upgradereq, not write, needs no resp" << endl;
			delete pkt->req;
			delete pkt;
		}
		else
		{
			cerr << "warn: not upgrade request" << endl;
		}
		return true;
	}
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

	M5_TIMING_LOG("+process [" << dec << currentMemCycle << "]");
	
	// move memory channels to the current time
	memory->moveToTime(currentMemCycle);

	// deschedule yourself
	if (memory->tickEvent.scheduled())
		memory->tickEvent.deschedule();

	// determine the next time to wake up
	tick next = memory->ds->nextTick();	


	assert(next > currentMemCycle);
	assert(next * memory->getCPURatio() > curTick);

	schedule(next * memory->getCPURatio());

	M5_TIMING_LOG("-process [" << currentMemCycle << "] sch[" << next << "]");
}

//////////////////////////////////////////////////////////////////////////
/// @brief the TickEvent constructor
/// @details sets a pointer to the underlying DRAMsim memory system
//////////////////////////////////////////////////////////////////////////
M5dramSystem::TickEvent::TickEvent(M5dramSystem *c)
: Event(CPU_Tick_Pri), memory(c)
{
	// &mainEventQueue,
	//this->
}

//////////////////////////////////////////////////////////////////////////
/// @brief returns a description of this component
//////////////////////////////////////////////////////////////////////////
const char *M5dramSystem::TickEvent::description()
{
	return "m5dramSystem tick event";	
}

