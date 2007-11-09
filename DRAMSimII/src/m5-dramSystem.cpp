#include <map>
#include <sstream>
//#include "sim/builder.hh"
#include "enumTypes.h"
#include "m5-dramSystem.h"
#include <stdlib.h>

using namespace std;
using namespace DRAMSimII;

#define STATS_INTERVAL 1000000

M5dramSystem::M5dramSystem(Params *p):
PhysicalMemory(p), tickEvent(this), needRetry(false)
{	
	timingOutStream << "M5dramSystem constructor" << std::endl;
	std::map<file_io_token_t,std::string> parameter;

	parameter[output_file_token] = p->outFilename;
	parameter[dram_type_token] = p->dramType;
	parameter[row_buffer_management_policy_token] = p->rowBufferManagmentPolicy;
	parameter[auto_precharge_token] = p->autoPrecharge;
	parameter[addr_mapping_scheme_token] = p->addrMappingScheme;
	parameter[datarate_token] = p->datarate;
	parameter[refresh_time_token] = p->refreshTime;
	parameter[read_write_grouping_token] = p->readWriteGrouping;
	parameter[refresh_policy_token] = p->refreshPolicy;
	parameter[seniority_age_limit_token] = p->seniorityAgeLimit;
	parameter[posted_cas_token] = p->postedCas;
	parameter[clock_granularity_token] = p->clockGranularity;
	parameter[row_count_token] = p->rowCount;
	parameter[cachelines_per_row_token] = p->cachelinesPerRow;
	parameter[col_count_token] = p->colCount;
	parameter[row_size_token] = p->rowSize;
	parameter[cacheline_size_token] = p->cachelineSize;
	parameter[history_queue_depth_token] = p->historyQueueDepth;
	parameter[completion_queue_depth_token] = p->completionQueueDepth;
	parameter[transaction_queue_depth_token] = p->transactionQueueDepth;
	parameter[event_queue_depth_token] = p->eventQueueDepth;
	parameter[per_bank_queue_depth_token] = p->perBankQueueDepth;
	parameter[command_ordering_algorithm_token] = p->orderingAlgorithm;
	parameter[chan_count_token] = p->chanCount;
	parameter[rank_count_token] = p->rankCount;
	parameter[bank_count_token] = p->bankCount;
	parameter[refresh_queue_depth_token] = p->refreshQueueDepth;
	parameter[t_al_token] = p->tAL;
	parameter[t_burst_token] = p->tBURST;
	parameter[t_cas_token] = p->tCAS;
	parameter[t_faw_token] = p->tFAW;
	parameter[t_ras_token] = p->tRAS;
	parameter[t_rp_token] = p->tRP;
	parameter[t_rcd_token] = p->tRCD;
	parameter[t_rfc_token] = p->tRFC;
	parameter[t_rrd_token] = p->tRRD;
	parameter[t_rtp_token] = p->tRTP;
	parameter[t_rtrs_token] = p->tRTRS;
	parameter[t_wr_token] = p->tWR;
	parameter[t_wtr_token] = p->tWTR;
	const char **settingsMap = new const char*[2];
	settingsMap[0] = "--settings";
	settingsMap[1] = p->settingsFile.c_str();
	dramSettings settings(2,settingsMap);
	settings.inFile = "";

	// if this is a normal system or a fbd system
	if (settings.systemType == BASELINE_CONFIG)
		ds = new dramSystem(settings);
	else
		ds = new fbdSystem(settings);
	//delete settings;

	nextStats = STATS_INTERVAL;

	cpuRatio = (int)((double)Clock::Frequency/(ds->Frequency()));

	timingOutStream << *ds << std::endl;
}

M5dramSystem::MemPort::MemPort(const std::string &_name, M5dramSystem *_memory):
SimpleTimingPort(_name),
memory(_memory)
{ }

Port *M5dramSystem::getPort(const string &if_name, int idx)
{
	if (if_name == "port" && idx == -1) 
	{
		if (port != NULL)
			panic("M5dramSystem::getPort: additional port requested to memory!"); 
		memoryPort = new MemPort(name() + "-port", this);
		return memoryPort; 
	}
	else if (if_name == "functional") 
	{
		/* special port for functional writes at startup. And for memtester */
		return new MemPort(name() + "-funcport", this);
	}
	else 
	{
		panic("M5dramSystem::getPort: unknown port %s requested", if_name);
	}
	timingOutStream << "called M5dramSystem::getPort" << endl;
}

void M5dramSystem::init()
{
	if (!memoryPort)
		panic("M5dramSystem not connected to anything!");
	memoryPort->sendStatusChange(Port::RangeChange); 

}

M5dramSystem::~M5dramSystem()
{
	//if (pmemAddr)
	//	munmap(pmemAddr, params()->addrRange.size());
	
	timingOutStream << "M5dramSystem destructor" << std::endl;
	delete ds;
}

int 
M5dramSystem::MemPort::deviceBlockSize()
{
	return memory->deviceBlockSize();
}

void
M5dramSystem::MemPort::recvFunctional(PacketPtr pkt)
{
	if (!checkFunctional(pkt)) {
		// Default implementation of SimpleTimingPort::recvFunctional()
		// calls recvAtomic() and throws away the latency; we can save a
		// little here by just not calculating the latency.
		memory->doFunctionalAccess(pkt);
	}

}

Tick
M5dramSystem::MemPort::recvAtomic(PacketPtr pkt)
{ 
#ifdef M5DEBUG
	timingOutStream << "M5dramSystem recvAtomic()" << endl;
#endif

	memory->doFunctionalAccess(pkt); 
	return memory->calculateLatency(pkt);
}

void
M5dramSystem::MemPort::recvStatusChange(Port::Status status)
{ 
	memory->recvStatusChange(status); 
}

void
M5dramSystem::MemPort::getDeviceAddressRanges(AddrRangeList &resp,
												   AddrRangeList &snoop)
{
	memory->getAddressRanges(resp, snoop);
}

void
M5dramSystem::getAddressRanges(AddrRangeList &resp, AddrRangeList &snoop)
{
	snoop.clear();
	resp.clear();
	resp.push_back(RangeSize(params()->addrRange.start,
		params()->addrRange.size()));
}

M5dramSystem::TickEvent::TickEvent(M5dramSystem *c)
: Event(&mainEventQueue, CPU_Tick_Pri), memory(c)
{}

const char *
M5dramSystem::TickEvent::description()
{
	return "m5dramSystem tick event";	
}



//#define TESTNORMAL

bool
M5dramSystem::MemPort::recvTiming(PacketPtr pkt)
{ 
	// FIXME: shouldn't need to turn away packets, the requester should hold off once NACK'd
	if (memory->needRetry)
	{
		static unsigned numCallsWhileBlocked = 1;
		if (numCallsWhileBlocked++ % 100000 == 0)
			cerr << numCallsWhileBlocked << "\r";
		return false;
	}

	tick_t currentMemCycle = curTick/memory->getCpuRatio();

	if (currentMemCycle >= memory->nextStats)
	{		
		memory->ds->doPowerCalculation();
		memory->ds->printStatistics();
	}
	while (currentMemCycle >= memory->nextStats)
		memory->nextStats += STATS_INTERVAL;

#ifdef M5DEBUG
	timingOutStream << "extWake [" << curTick << "]/[" << currentMemCycle << "]";
	// calculate the time elapsed from when the transaction started
	if (pkt->isRead())
		timingOutStream << "R ";
	else if (pkt->isWrite())
		timingOutStream << "W ";
	else if (pkt->isRequest())
		timingOutStream << "RQ ";
	else if (pkt->isInvalidate())
		timingOutStream << "I  ";
	else
		timingOutStream << "? ";


	timingOutStream << "0x" << std::hex << pkt->getAddr() << endl;

	if (pkt->getSize() != 0x40)
		cerr << "0x" << std::hex << pkt->getSize() << endl;
#endif

	// any packet which doesn't need a response and isn't a write
	if (!pkt->needsResponse() && !pkt->isWrite())
	{
#ifdef M5DEBUG
		timingOutStream << "packet not needing response." << endl;
#endif
		if (pkt->cmd != MemCmd::UpgradeReq)
		{
			delete pkt->req;
			delete pkt;
		}
		else
		{
			timingOutStream << "####################### not upgrade request" << endl;
		}
		return true;
	}
	else
	{
#ifdef TESTNORMAL
		memory->doFunctionalAccess(pkt);
		if (!pkt->isWrite())
		{
			pkt->makeTimingResponse();
			timingOutStream << "sending packet back at " << std::dec << static_cast<Tick>(curTick + 95996) << endl;
			sendTiming(pkt, rand() % 95996);
		}
#else		
		int packetType = 0;

		if (pkt->isRead())
			packetType = 1;
		else if (pkt->isWrite())
			packetType = 2;

		transaction *trans = new transaction(packetType,currentMemCycle,pkt->getSize(),pkt->getAddr(),(void *)pkt);

		// convert the physical address to chan, rank, bank, etc.
		//memory->ds->convertAddress(trans->getAddresses());

		// move channels to current time so that calculations based on current channel times work
		// should also not start/finish any commands, since this would happen at a scheduled time
		// instead of now
		memory->moveDramSystemToTime(currentMemCycle);

		//tick_t finishTime;
		// wake the channel and do everything it was going to do up to this point
		//assert(!ds->moveChannelToTime(trans->arrival_time,trans->addr.chan_id, &finishTime));

		assert(pkt->result != Packet::Nacked);
		// turn packet around to go back to requester if response expected

		// attempt to add the transaction to the memory system
		if (!memory->ds->enqueue(trans))
		{
#ifdef M5DEBUG						
			static tick_t numberOfDelays = 0;
			if (++numberOfDelays % 100000 == 0)
				cerr << "\r" << numberOfDelays;
#endif

			// if the packet did not fit, then send a NACK
			// tell the sender that the memory system is full until it hears otherwise
			// and do not send packets until that time
			//pkt->result = Packet::Nacked;
			//short memID = pkt->getDest();
			//pkt->makeTimingResponse();
			//pkt->setSrc(memID);
			//doSendTiming(pkt,0);

			
			// http://m5.eecs.umich.edu/wiki/index.php/Memory_System
			// keep track of the fact that the memory system is waiting to hear that it is ok to send again
			// as well as what channel it is likely to retry to (make sure there is room before sending the OK)
			memory->needRetry = true;
			memory->mostRecentChannel = trans->getAddresses().channel;
			delete trans;

#ifdef M5DEBUG
			timingOutStream << "Wait for retry before sending more to ch[" << trans->getAddresses().channel << "]" << endl;
#endif
			return false;
		}
		else
		{
			if (memory->tickEvent.scheduled())
				memory->tickEvent.deschedule();

			tick_t next = min(memory->nextStats,memory->ds->nextTick());
			assert(next < TICK_T_MAX);
			if (next < TICK_T_MAX)
			{
#ifdef M5DEBUG
				timingOutStream << "schWake [" << std::dec << memory->getCpuRatio() * next << "][" << next << ")" << " at " << curTick << "(" << currentMemCycle << "]" << endl;
#endif
				memory->tickEvent.schedule(memory->getCpuRatio() * next);
			}
			return true;
		}
#endif
	}
}

void
M5dramSystem::TickEvent::process()
{	
	tick_t currentMemCycle = curTick / memory->getCpuRatio(); // TODO: make this a multiply operation

	if (currentMemCycle >= memory->nextStats)
	{		
		memory->ds->doPowerCalculation();
		memory->ds->printStatistics();
	}
	while (currentMemCycle >= memory->nextStats)
		memory->nextStats += STATS_INTERVAL;
#ifdef M5DEBUG
	timingOutStream << "intWake [" << std::dec << curTick << "][" << std::dec << currentMemCycle << "]" << endl;
#endif

	// move memory channels to the current time
	memory->moveDramSystemToTime(currentMemCycle);

	// search for eligible refresh transactions

	// deschedule yourself
	if (memory->tickEvent.scheduled())
		memory->tickEvent.deschedule();

	// determine the next time to wake up
	tick_t next = min(memory->nextStats,memory->ds->nextTick());	

	// nextTick() returns TICK_T_MAX if there is nothing else to wake up for
	if (next < TICK_T_MAX)
	{	
#ifdef M5DEBUG
		timingOutStream << "schWake [" << static_cast<Tick>(next * memory->getCpuRatio()) << "][" << next << "]" << endl;
#endif
		assert(next * memory->getCpuRatio() > curTick);
		schedule(static_cast<Tick>(next * memory->getCpuRatio()));
	}	
}

void M5dramSystem::moveDramSystemToTime(tick_t now)
{
	tick_t finishTime;	

	Packet *packet;
	// if transactions are returned, then send them back, else if time is not brought up to date, then a refresh transaction has finished
	while ((packet = (Packet *)ds->moveAllChannelsToTime(now, &finishTime)) || finishTime < now)
	{
		if (packet)
		{
			// for debug purposes, remove this later
			assert(packet->isRead() | packet->isWrite() || packet->isInvalidate());

			doFunctionalAccess(packet);

			if (packet->needsResponse())
			{			
				packet->makeTimingResponse();
				assert(curTick <= static_cast<Tick>(finishTime * getCpuRatio()));

#ifdef M5DEBUG
				timingOutStream << "<-T [@" << std::dec << static_cast<Tick>(finishTime * getCpuRatio()) << "][+" << static_cast<Tick>(finishTime * getCpuRatio() - curTick) << "] at" << curTick << endl;
#endif

				memoryPort->doSendTiming((Packet *)packet, static_cast<Tick>(finishTime * getCpuRatio() - curTick));
			}
			else
			{
				delete packet->req;
				delete packet;
			}			
		}	
	}

	// if there is now room, allow a retry to happen
	if (needRetry && !ds->isFull(mostRecentChannel))
	{
#ifdef M5DEBUG
		timingOutStream << "Allow retrys" << endl;
#endif
		needRetry = false;
		memoryPort->sendRetry();
	}
}

#if 0
#ifndef DOXYGEN_SHOULD_SKIP_THIS

//////////////////////////////////////////////////////////////////////////
// simObject parameters
//////////////////////////////////////////////////////////////////////////
BEGIN_DECLARE_SIM_OBJECT_PARAMS(M5dramSystem)

Param<std::string> settingsFile;
Param<std::string> file;
Param<Range<Addr> > range;
Param<Tick> latency;
/* additional params for dram protocol*/
Param<int> cpu_ratio;
Param<std::string> outFilename;
Param<std::string> dramType;
Param<std::string> rowBufferManagmentPolicy;
Param<std::string> autoPrecharge;
Param<std::string> addrMappingScheme;
Param<std::string> datarate;
Param<std::string> refreshTime;
Param<std::string> readWriteGrouping;
Param<std::string> refreshPolicy;
Param<std::string> seniorityAgeLimit;
Param<std::string> postedCas;
Param<std::string> clockGranularity;
Param<std::string> rowCount;
Param<std::string> cachelinesPerRow;
Param<std::string> colCount;
Param<std::string> colSize;
Param<std::string> rowSize;
Param<std::string> cachelineSize;
Param<std::string> historyQueueDepth;
Param<std::string> completionQueueDepth;
Param<std::string> transactionQueueDepth;
Param<std::string> eventQueueDepth;
Param<std::string> perBankQueueDepth;
Param<std::string> orderingAlgorithm;
Param<std::string> configType;
Param<std::string> chanCount;
Param<std::string> rankCount;
Param<std::string> bankCount;
Param<std::string> refreshQueueDepth;
Param<std::string> tAL;
Param<std::string> tBURST;
Param<std::string> tCAS;
Param<std::string> tFAW;
Param<std::string> tRAS;
Param<std::string> tRP;
Param<std::string> tRCD;
Param<std::string> tRFC;
Param<std::string> tRRD;
Param<std::string> tRTP;
Param<std::string> tRTRS;
Param<std::string> tWR;
Param<std::string> tWTR;

END_DECLARE_SIM_OBJECT_PARAMS(M5dramSystem)


BEGIN_INIT_SIM_OBJECT_PARAMS(M5dramSystem)

INIT_PARAM_DFLT(file, "memory mapped file", ""),
INIT_PARAM(range, "Device Address Range"),
INIT_PARAM(latency, "Memory access latency"),

/* additional params for dram protocol*/
INIT_PARAM_DFLT(settingsFile,"the xml file with the settings","/home/crius/m5/src/mem/DRAMSimII/memoryDefinitions/DDR2-800-4-4-4.xml"),
INIT_PARAM_DFLT(cpu_ratio,"ratio between CPU speed and memory bus speed",5), 
INIT_PARAM_DFLT(outFilename,"output file name","dramSimIIout.gz"),
INIT_PARAM_DFLT(dramType,"type of DRAM, sdram, ddr, etc.","ddr2"),
INIT_PARAM_DFLT(rowBufferManagmentPolicy,"open_page, close_page, auto_page","close_page"),
INIT_PARAM_DFLT(autoPrecharge,"true or false","true"),
INIT_PARAM_DFLT(addrMappingScheme,"sdram_close_page_map, burger_base_map, close_page_baseline, INTEL845G_MAP, sdram_base_map","sdram_hiperf_map"),
INIT_PARAM_DFLT(datarate,"200, 400, etc.","200"),
INIT_PARAM_DFLT(refreshTime,"refresh time in ms",""),
INIT_PARAM_DFLT(readWriteGrouping,"true or false","false"),
INIT_PARAM_DFLT(refreshPolicy,"none, bank_concurrent, bank_staggered_hidden","none"),
INIT_PARAM_DFLT(seniorityAgeLimit,"in ticks","200"),
INIT_PARAM_DFLT(postedCas,"true or false","false"),
INIT_PARAM_DFLT(clockGranularity,"int","2"),
INIT_PARAM_DFLT(rowCount,"number of rows","16384"),
INIT_PARAM_DFLT(cachelinesPerRow,"int or invalid=-1","-1"),
INIT_PARAM_DFLT(colCount,"number of columns","512"),
INIT_PARAM_DFLT(colSize,"column size in bytes","8"),
INIT_PARAM_DFLT(rowSize,"row size","-1"),
INIT_PARAM_DFLT(cachelineSize,"cacheline size","64"),
INIT_PARAM_DFLT(historyQueueDepth,"history queue depth","8"),
INIT_PARAM_DFLT(completionQueueDepth,"completion queue depth","8"),
INIT_PARAM_DFLT(transactionQueueDepth,"transaction queue depth","32"),
INIT_PARAM_DFLT(eventQueueDepth,"event queue depth","32"),
INIT_PARAM_DFLT(perBankQueueDepth,"per bank queue depth","6"),
INIT_PARAM_DFLT(orderingAlgorithm,"strict_order, bank_round_robin, rank_round_robin","strict_order"),
INIT_PARAM_DFLT(configType,"baseline for now","baseline"),
INIT_PARAM_DFLT(chanCount,"channel count","1"),
INIT_PARAM_DFLT(rankCount,"rank count","8"),
INIT_PARAM_DFLT(bankCount,"bank count","8"),
INIT_PARAM_DFLT(refreshQueueDepth,"refresh queue depth","131072"),
INIT_PARAM_DFLT(tAL,"accumulated latency","50"),
INIT_PARAM_DFLT(tBURST,"burst length","8"),
INIT_PARAM_DFLT(tCAS,"cas latency","8"),
INIT_PARAM_DFLT(tFAW,"four activation window","25"),
INIT_PARAM_DFLT(tRAS,"row access latency","30"),
INIT_PARAM_DFLT(tRP,"precharge to activate latency","8"),
INIT_PARAM_DFLT(tRCD,"row to column delay","8"),
INIT_PARAM_DFLT(tRFC,"refresh to activate delay","85"),
INIT_PARAM_DFLT(tRRD,"rank to rank delay","5"),
INIT_PARAM_DFLT(tRTP,"read to precharge","5"),
INIT_PARAM_DFLT(tRTRS,"rank to rank switching delay","2"),
INIT_PARAM_DFLT(tWR,"write recovery, write to precharge delay","10"),
INIT_PARAM_DFLT(tWTR,"write to read recovery time","6")

END_INIT_SIM_OBJECT_PARAMS(M5dramSystem)

CREATE_SIM_OBJECT(M5dramSystem)
{
	M5dramSystem::Params *p = new M5dramSystem::Params;
	p->name = getInstanceName();
	p->addrRange = range;
	p->latency = latency;

	/* additional params for dram */
	p->cpu_ratio = cpu_ratio;
	p->outFilename = outFilename;
	p->dramType = dramType;
	p->rowBufferManagmentPolicy = rowBufferManagmentPolicy;
	p->autoPrecharge = autoPrecharge;
	p->addrMappingScheme = addrMappingScheme;
	p->datarate = datarate;
	p->refreshTime = refreshTime;
	p->readWriteGrouping = readWriteGrouping;
	p->refreshPolicy = refreshPolicy;
	p->seniorityAgeLimit = seniorityAgeLimit;
	p->postedCas = postedCas;
	p->clockGranularity = clockGranularity;
	p->rowCount = rowCount;
	p->cachelinesPerRow = cachelinesPerRow;
	p->colCount = colCount;
	p->rowSize = rowSize;
	p->cachelineSize = cachelineSize;
	p->historyQueueDepth = historyQueueDepth;
	p->completionQueueDepth = completionQueueDepth;
	p->transactionQueueDepth = transactionQueueDepth;
	p->eventQueueDepth = eventQueueDepth;
	p->perBankQueueDepth = perBankQueueDepth;
	p->orderingAlgorithm = orderingAlgorithm;
	p->chanCount = chanCount;
	p->rankCount = rankCount;
	p->bankCount = bankCount;
	p->refreshQueueDepth = refreshQueueDepth;
	p->tAL = tAL;
	p->tBURST = tBURST;
	p->tCAS = tCAS;
	p->tFAW = tFAW;
	p->tRAS = tRAS;
	p->tRP = tRP;
	p->tRCD = tRCD;
	p->tRFC = tRFC;
	p->tRRD = tRRD;
	p->tRTP = tRTP;
	p->tRTRS = tRTRS;
	p->tWR = tWR;
	p->tWTR = tWTR;
	p->settingsFile = settingsFile;

	return new M5dramSystem(p); 
}

REGISTER_SIM_OBJECT("M5dramSystem", M5dramSystem)

#endif // DOXYGEN_SHOULD_SKIP_THIS
#endif