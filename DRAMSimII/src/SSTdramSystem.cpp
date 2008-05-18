
#include "SSTdramSystem.h"



SSTdramSystem::SSTdramSystem():
generator(42u),
uni_dist(0,65535),
uni(generator, uni_dist)
{

}

SSTdramSystem::~SSTdramSystem()
{
	delete ds;
}

void SSTdramSystem::setup() {}

void SSTdramSystem::finish() {}

void SSTdramSystem::handleParcel(parcel *p)
{

	// if this is a read
	if (instruction *inst = (instruction*)p->inst())
	{
		simAddress memoryEffectiveAddress = (inst->state() <= FETCHED) ? inst->PC() : inst->memEAphys();	

#ifdef TESTNEW

		// do the read or write for this transaction
		doAtomicAccess(p);
		int randomDelay = uni();
		DEBUG_TIMING_LOG("sending packet back at " << std::dec << static_cast<tick>(Timestamp() + randomDelay));
		/// @todo avoid port contention, ensure that the port is available
		sendParcel(p,p->source(), Timestamp() + randomDelay);
#else

#endif
	}
	// if this is a write
	else if (simAddress wb = (simAddress)(size_t)p->data()) 
	{
#ifdef TESTNEW
		parcel::deleteParcel(p);
#else
#endif
	}

}

SSTdramSystem::preTic()
{
	tick curTick = TimeStamp();

	if (curTick >= nextStats)
	{
		ds->doPowerCalculation();
		ds->printStatistics();
	}
	while (curTick >= nextStats)
		nextStats += STATS_INTERVAL;

	M5_TIMING_LOG("intWake [" << std::dec << curTick << "][" << std::dec << currentMemCycle << "]");

	moveToTime(curTick);

	// determine the next time to wake up

	// schedule a time to be woken
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
void SSTdramSystem::moveToTime(const tick now)
{
	tick finishTime;	

	parcel *packet;
	// if transactions are returned, then send them back,
	// else if time is not brought up to date, then a refresh transaction has finished
	while ((packet = (parcel *)ds->moveAllChannelsToTime(now, &finishTime)) || finishTime < now)
	{
		if (packet)
		{			
			//doFunctionalAccess(packet);

			doAtomicAccess(packet);		

			// needs a response
			if (packet->inst())
			{			
				assert(curTick <= static_cast<tick>(finishTime * getCpuRatio()));

				//M5_TIMING_LOG("<-T [@" << std::dec << static_cast<tick>(finishTime * getCpuRatio()) << "][+" << static_cast<Tick>(finishTime * getCpuRatio() - curTick) << "] at" << curTick);
				
				//ports[lastPortIndex]->doSendTiming((Packet *)packet, static_cast<Tick>(finishTime * getCpuRatio()));
				/// @todo check for port contention when returning parcels
				sendParcel(packet,packet->source(), TimeStamp());
			}
			else
			{
				parcel::deleteParcel(packet);
			}			
		}	
	}

	
}

bool SSTdramSystem::doAtomicAccess(parcel *p)
{
	return true;
}