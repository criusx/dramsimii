#include <sstream>
#include <stdlib.h>
#include <cmath>

#include "enumTypes.h"
#include "m5-dramSystem.h"

using std::dec;
using std::hex;
using std::endl;
using std::string;
using namespace DRAMSimII;

//#define TESTNEW

//////////////////////////////////////////////////////////////////////
/// @brief receive a packet and do something with it
/// @details receive a packet, check it to make sure it's valid, then try to enqueue it in DS2
/// @author Joe Gross
/// @param pkt the pointer to the memory transaction packet
/// @return true if the packet is accepted, false otherwise
//////////////////////////////////////////////////////////////////////
bool M5dramSystem::MemoryPort::recvTiming(PacketPtr pkt)
{ 	
#ifdef TESTNEW
	bool nr = pkt->needsResponse();
	memory->doAtomicAccess(pkt);
	if (nr)
	{
		timingOutStream << "sending packet back at " << dec << static_cast<Tick>(curTick + 95996) << endl;
		//memory->ports[memory->lastPortIndex]->doSendTiming(pkt,curTick + 18750 + rand()%400);
		//schedSendTiming(pkt,curTick + 20000 + rand()%20000);
		schedSendTiming(pkt,curTick + 200000 + randomGen.random((Tick)0, (Tick)200000));
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
		M5_TIMING_LOG("attempted packet send after packet is nacked");
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	tick currentMemCycle = (curTick + memory->getCPURatio() - 1) / memory->getCPURatio();
	
	assert(pkt->isRequest());

	if (pkt->memInhibitAsserted()) 
	{
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


	timingOutStream << "0x" << hex << pkt->getAddr() << endl;

	if (pkt->getSize() != 0x40)
		cerr << "0x" << hex << pkt->getSize() << endl;
#endif


	// any packet which doesn't need a response and isn't a write
	//if (pkt->needsResponse())
	if (pkt->needsResponse() || pkt->isWrite())
	{
		TransactionType packetType = PREFETCH_TRANSACTION;

		if (pkt->isRead())
			packetType = READ_TRANSACTION;
		else if (pkt->isWrite())
			packetType = WRITE_TRANSACTION;

		Transaction *trans = new Transaction(packetType,currentMemCycle,pkt->getSize(),pkt->getAddr(),(void *)pkt);

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
			static tick numberOfDelays = 0;
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

			M5_TIMING_LOG("Wait for retry before sending more to ch[" << trans->getAddresses().channel << "]");
			return false;
		}
		else
		{
			// deschedule and reschedule yourself to wake at the next event time
			if (memory->tickEvent.scheduled())
				memory->tickEvent.deschedule();

			tick next = memory->ds->nextTick();
			assert(next < TICK_MAX);


			M5_TIMING_LOG("schWake [" << dec << memory->getCPURatio() * next << "][" << next << ")" << " at " << curTick << "(" << currentMemCycle << "]");

			assert(next > currentMemCycle);
			assert(next * memory->getCPURatio() > curTick);
			
			memory->tickEvent.schedule(next * memory->getCPURatio());

			//memory->tickEvent.schedule(memory->getCpuRatio() * next);

			return true;
		}
	}
	else
	{
		M5_TIMING_LOG("packet not needing response.");

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
}

//////////////////////////////////////////////////////////////////////
/// @brief wake up via the global event queue
/// @details wake up and perform some action at this time, then determine when the next wakeup time should be
/// inserts the memory system into the event queue and handles being woken by it
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void M5dramSystem::TickEvent::process()
{	
	//tick currentMemCycle = curTick / memory->getCPURatio();
	tick currentMemCycle = (curTick + memory->getCPURatio() - 1) / memory->getCPURatio();
	
	M5_TIMING_LOG("intWake [" << dec << curTick << "][" << dec << currentMemCycle << "]");

	// move memory channels to the current time
	memory->moveToTime(currentMemCycle);


	// deschedule yourself
	if (memory->tickEvent.scheduled())
		memory->tickEvent.deschedule();

	// determine the next time to wake up
	tick next = memory->ds->nextTick();	
	

	M5_TIMING_LOG("schWake [" << static_cast<Tick>(next * memory->getCPURatio()) << "][" << next << "]");

	assert(next > currentMemCycle);
	assert(next * memory->getCPURatio() > curTick);
	
	schedule(next * memory->getCPURatio());
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
	tick finishTime;	

	Packet *packet;
	// if transactions are returned, then send them back,
	// else if time is not brought up to date, then a refresh transaction has finished
	//while ((packet = (Packet *)ds->moveAllChannelsToTime(now, finishTime)) || finishTime < now)
	while ((packet = (Packet *)ds->moveAllChannelsToTime(now, finishTime)) || ds->getTime() < now)
	{
		if (packet)
		{
			assert(packet->isRead() || packet->isWrite() || packet->isInvalidate());

			bool needsResponse = packet->needsResponse();

			doAtomicAccess(packet);		

			if (needsResponse)
			{			
				assert(curTick <= static_cast<Tick>(finishTime * getCPURatio()));

				M5_TIMING_LOG("<-T [@" << dec << static_cast<Tick>(finishTime * getCPURatio()) << "][+" << static_cast<Tick>(finishTime * getCPURatio() - curTick) << "] at" << curTick);

				ports[lastPortIndex]->doSendTiming(packet, static_cast<Tick>(finishTime * getCPURatio()));

				static tick returnCount;

				if (++returnCount % 10000 == 0)
					cerr << returnCount << "\r";
			}
			else
			{				
				delete packet;
			}			
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
needRetry(false)
{	
	timingOutStream << "M5dramSystem constructor" << endl;

	const char **settingsMap = new const char*[2];

	settingsMap[0] = "--settings";
	settingsMap[1] = p->settingsFile.c_str();

	Settings settings(2,settingsMap);

	settings.inFile = "";

	// if this is a normal system or a fbd system
	if (settings.systemType == BASELINE_CONFIG)
		ds = new System(settings);
	else
		ds = new fbdSystem(settings);	

	cpuRatio = (int)round(((float)Clock::Frequency/((float)ds->Frequency())));
	//cerr << cpuRatio << endl;
	//invCpuRatio = (float)((double)ds->Frequency()/(Clock::Frequency));
	//cerr << invCpuRatio << endl;

	
	timingOutStream << *ds << endl;
}


//////////////////////////////////////////////////////////////////////////
/// @brief stuff taken from PhysicalMemory.cc
//////////////////////////////////////////////////////////////////////////
M5dramSystem *M5dramSystemParams::create()
{
	return new M5dramSystem(this);
}

//////////////////////////////////////////////////////////////////////
/// @brief builds a memory port to interact with other components
/// @param _name the friendly name of this object
/// @param _memory the pointer to the M5dramSystem object that will be backing this object
/// @return a new memoryPort object associated with a dramSystem object
//////////////////////////////////////////////////////////////////////
M5dramSystem::MemoryPort::MemoryPort(const string &_name, M5dramSystem *_memory):
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

	timingOutStream << "M5dramSystem destructor" << endl;
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

//////////////////////////////////////////////////////////////////////
/// @brief receive a read/write request to be handled without a timing model
/// @author Joe Gross
/// @param pkt a packet containing a transaction
/// @return
//////////////////////////////////////////////////////////////////////
Tick M5dramSystem::MemoryPort::recvAtomic(PacketPtr pkt)
{ 
	M5_TIMING_LOG("M5dramSystem recvAtomic()");

	// because this is simply a read or write, go to the M5 memory object exclusively
	// and ignore the timing from DRAMSimII
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
