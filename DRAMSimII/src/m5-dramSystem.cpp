#include <sstream>
#include <stdlib.h>

#include "enumTypes.h"
#include "m5-dramSystem.h"

using namespace std;
using namespace DRAMSimII;

#define STATS_INTERVAL 1000000

//#define TESTNORMAL

//#define TESTNEW

//////////////////////////////////////////////////////////////////////////
// recvTiming, override from MemoryPort
//////////////////////////////////////////////////////////////////////////
bool M5dramSystem::MemoryPort::recvTiming(PacketPtr pkt)
{ 	
#ifdef TESTNEW
	bool nr = pkt->needsResponse();
	memory->doAtomicAccess(pkt);
	if (nr)
	{
		timingOutStream << "sending packet back at " << std::dec << static_cast<Tick>(curTick + 95996) << endl;
		//memory->ports[memory->lastPortIndex]->doSendTiming(pkt,curTick + 18750 + rand()%400);
		//schedSendTiming(pkt,curTick + 20000 + rand()%20000);
		schedSendTiming(pkt,curTick + 200000 + randomGen.random((Tick)0, (Tick)200000));
	}
	else
	{
		delete pkt->req;
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
		cerr << "attempted packet send after packet is nacked" << endl;
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	tick_t currentMemCycle = curTick/memory->getCpuRatio();

	if (currentMemCycle >= memory->nextStats)
	{		
		memory->ds->doPowerCalculation();
		memory->ds->printStatistics();
	}
	while (currentMemCycle >= memory->nextStats)
		memory->nextStats += STATS_INTERVAL;

	assert(pkt->isRequest());

	if (pkt->memInhibitAsserted()) {
		// snooper will supply based on copy of packet
		// still target's responsibility to delete packet
		cerr << "deleted memInhibitAsserted" << endl;
		delete pkt;
		return true;
	}

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
	//if (!pkt->needsResponse())
	if (!pkt->needsResponse() && !pkt->isWrite())
	{
		M5_TIMING_LOG("packet not needing response.")

			if (pkt->cmd != MemCmd::UpgradeReq)
			{
				cerr << "deleted pkt, not upgradereq, not write, needs no resp" << endl;
				delete pkt->req;
				delete pkt;
			}
			else
			{
				cerr << "####################### not upgrade request" << endl;
			}
			return true;
	}
	else
	{
#ifdef TESTNORMAL
		//memory->doFunctionalAccess(pkt);
		bool nr = pkt->needsResponse();
		memory->doAtomicAccess(pkt);
		if (nr)
		{
			//pkt->makeTimingResponse();
			timingOutStream << "sending packet back at " << std::dec << static_cast<Tick>(curTick + 95996) << endl;
			memory->ports[memory->lastPortIndex]->doSendTiming(pkt,(currentMemCycle + 101 + rand() % 100)*memory->getCpuRatio());
		}
		return true;
#else		
		int packetType = 0;

		if (pkt->isRead())
			packetType = 1;
		else if (pkt->isWrite())
			packetType = 2;

		transaction *trans = new transaction(packetType,currentMemCycle,pkt->getSize(),pkt->getAddr(),(void *)pkt);

		assert((pkt->isRead() && pkt->needsResponse()) || (!pkt->isRead() && !pkt->needsResponse()));

		//memory->doAtomicAccess(pkt); // maybe try to do the access prior to simulating timing?

		// move channels to current time so that calculations based on current channel times work
		// should also not start/finish any commands, since this would happen at a scheduled time
		// instead of now
		memory->moveToTime(currentMemCycle);

		assert(!pkt->wasNacked());
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

			M5_TIMING_LOG("Wait for retry before sending more to ch[" << trans->getAddresses().channel << "]")
				return false;
		}
		else
		{
			// deschedule and reschedule yourself to wake at the next event time
			if (memory->tickEvent.scheduled())
				memory->tickEvent.deschedule();

			tick_t next = min(memory->nextStats,memory->ds->nextTick());
			assert(next < TICK_T_MAX);


			M5_TIMING_LOG("schWake [" << std::dec << memory->getCpuRatio() * next << "][" << next << ")" << " at " << curTick << "(" << currentMemCycle << "]")

				memory->tickEvent.schedule(memory->getCpuRatio() * next);

			return true;
		}
#endif
	}
}

//////////////////////////////////////////////////////////////////////////
// process()
// runs whenever a tick event happens, essentially allows the system to be
// woken at certain times to process commands, retrieve results, etc.
//////////////////////////////////////////////////////////////////////////
void M5dramSystem::TickEvent::process()
{	
	tick_t currentMemCycle = curTick / memory->getCpuRatio(); // TODO: make this a multiply operation

	if (currentMemCycle >= memory->nextStats)
	{		
		memory->ds->doPowerCalculation();
		memory->ds->printStatistics();
	}
	while (currentMemCycle >= memory->nextStats)
		memory->nextStats += STATS_INTERVAL;

	M5_TIMING_LOG("intWake [" << std::dec << curTick << "][" << std::dec << currentMemCycle << "]")

		// move memory channels to the current time
		memory->moveToTime(currentMemCycle);


	// deschedule yourself
	if (memory->tickEvent.scheduled())
		memory->tickEvent.deschedule();

	// determine the next time to wake up
	tick_t next = min(memory->nextStats,memory->ds->nextTick());	
	assert(next < TICK_T_MAX);

	M5_TIMING_LOG("schWake [" << static_cast<Tick>(next * memory->getCpuRatio()) << "][" << next << "]")

		assert(next * memory->getCpuRatio() > curTick);
	schedule(static_cast<Tick>(next * memory->getCpuRatio()));
}

//////////////////////////////////////////////////////////////////////////
// moveToTime()
// tells the channels that nothing has arrived since the last wakeup and
// now, so go ahead and do whatever would have been done if running in real 
// time
// note that this should be called and should process about one event each 
// time it is called per channel, else things start getting sent back in
// the past
//////////////////////////////////////////////////////////////////////////
void M5dramSystem::moveToTime(const tick_t now)
{
	tick_t finishTime;	

	Packet *packet;
	// if transactions are returned, then send them back,
	// else if time is not brought up to date, then a refresh transaction has finished
	while ((packet = (Packet *)ds->moveAllChannelsToTime(now, &finishTime)) || finishTime < now)
	{
		if (packet)
		{
			assert(packet->isRead() || packet->isWrite() || packet->isInvalidate());

			bool needsResponse = packet->needsResponse();

			//doFunctionalAccess(packet);

			doAtomicAccess(packet);		

			if (needsResponse)
			{			
				assert(curTick <= static_cast<Tick>(finishTime * getCpuRatio()));

				M5_TIMING_LOG("<-T [@" << std::dec << static_cast<Tick>(finishTime * getCpuRatio()) << "][+" << static_cast<Tick>(finishTime * getCpuRatio() - curTick) << "] at" << curTick)

					ports[lastPortIndex]->doSendTiming((Packet *)packet, static_cast<Tick>(finishTime * getCpuRatio()));
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
		M5_TIMING_LOG("Allow retrys")

			needRetry = false;
		ports[lastPortIndex]->sendRetry();
	}
}

//////////////////////////////////////////////////////////////////////////
// the constructor
//////////////////////////////////////////////////////////////////////////
M5dramSystem::M5dramSystem(const Params *p):
PhysicalMemory(p), 
tickEvent(this), 
needRetry(false)
{	
	timingOutStream << "M5dramSystem constructor" << std::endl;

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

	cpuRatio = (int)((double)Clock::Frequency/(ds->Frequency()));
	invCpuRatio = (int)((double)ds->Frequency()/(Clock::Frequency));

	nextStats = STATS_INTERVAL;

	timingOutStream << *ds << std::endl;
}


//////////////////////////////////////////////////////////////////////////
// stuff taken from PhysicalMemory.cc
//////////////////////////////////////////////////////////////////////////

M5dramSystem *M5dramSystemParams::create()
{
	return new M5dramSystem(this);
}


M5dramSystem::MemoryPort::MemoryPort(const std::string &_name, M5dramSystem *_memory):
SimpleTimingPort(_name),
memory(_memory)
{ }

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

	MemoryPort *port =
		new MemoryPort(csprintf("%s-port%d", name(), idx), this);

	lastPortIndex = idx;
	ports[idx] = port;
	timingOutStream << "called M5dramSystem::getPort" << endl;
	return port;
}

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

M5dramSystem::~M5dramSystem()
{
	//if (pmemAddr)
	//	munmap(pmemAddr, params()->addrRange.size());

	timingOutStream << "M5dramSystem destructor" << std::endl;
	delete ds;
}

int M5dramSystem::MemoryPort::deviceBlockSize()
{
	return memory->deviceBlockSize();
}

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

Tick M5dramSystem::MemoryPort::recvAtomic(PacketPtr pkt)
{ 
	M5_TIMING_LOG("M5dramSystem recvAtomic()")

		return memory->doAtomicAccess(pkt);
}

void M5dramSystem::MemoryPort::recvStatusChange(Port::Status status)
{
	memory->recvStatusChange(status);
}

void M5dramSystem::MemoryPort::getDeviceAddressRanges(AddrRangeList &resp,
													  bool &snoop)
{
	memory->getAddressRanges(resp, snoop);
}

void M5dramSystem::getAddressRanges(AddrRangeList &resp, bool &snoop)
{
	snoop = false;
	resp.clear();
	resp.push_back(RangeSize(start(), params()->range.size()));
}

M5dramSystem::TickEvent::TickEvent(M5dramSystem *c)
: Event(&mainEventQueue, CPU_Tick_Pri), memory(c)
{}

const char *M5dramSystem::TickEvent::description()
{
	return "m5dramSystem tick event";	
}