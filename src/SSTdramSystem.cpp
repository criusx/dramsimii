#include "SSTdramSystem.h"

#include <configuration.h>
#include <instruction.h>

#include <debug.h>


SSTdramSystem::SSTdramSystem(std::string prefix):
component(prefix),
randomNumberGenerator(std::time(0)),
rngDistributionModel(0,1),
rngGenerator(randomNumberGenerator, rngDistributionModel),
cpuRatio(0)
{
	cpuRatio =(int)round(((float)ClockMhz()/((float)ds->Frequency())));

}

SSTdramSystem::~SSTdramSystem()
{
	delete ds;
}

/// @brief used by prefetcher to determine whether to prefetch or not
/// @details returns the number of currently used DRAMs
int load() 
{
	return portCount[FROM_DRAM];
}

void SSTdramSystem::setup() {}

void SSTdramSystem::finish() {}

void SSTdramSystem::handleParcel(parcel *p)
{	
#ifdef TESTNEW

		// do the read or write for this transaction
		int randomDelay = rngGenerator();
		DEBUG_TIMING_LOG("sending packet back at " << std::dec << static_cast<tick>(Timestamp() + randomDelay));
		/// @todo avoid port contention, ensure that the port is available
		sendParcel(p,p->source(), TimeStamp() + randomDelay);
#else
	instruction *inst = (instruction*)p->inst();
	simAddress wb = (simAddress)(size_t)p->data();

	if (!inst && !wb) // if a normal access or a writeback
	{
		cerr << "error: DS2SW2 received parcel w/o instruction or writeback data\n";
	}
	else
	{
		simAddress effectiveAddress = wb ? wb : (inst->state() <= FETCHED ? inst->PC() : inst->memEA());
		Transaction newTransaction = new Transaction() inst->
	}
#endif
}

void SSTdramSystem::preTic()
{
	tick curTick = TimeStamp();

	//ds->checkStats();

	M5_TIMING_LOG("intWake [" << std::dec << curTick << "][" << std::dec << currentMemCycle << "]");

	moveToTime(curTick);

	// determine the next time to wake up

	// schedule a time to be woken
}

bool SSTdramSystem::doAtomicAccess(parcel *p)
{
	return true;
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
	boost::uint64_t curTick = TimeStamp();
	parcel *packet;
	// if transactions are returned, then send them back,
	// else if time is not brought up to date, then a refresh transaction has finished
	while ((packet = (parcel *)ds->moveAllChannelsToTime(now, finishTime)) || finishTime < now)
	{
		if (packet)
		{			
			//doFunctionalAccess(packet);

			doAtomicAccess(packet);		

			// needs a response
			if (packet->inst())
			{			
				assert(curTick <= static_cast<tick>(finishTime * cpuRatio));

				SST_TIMING_LOG("<-T [@" << std::dec << static_cast<tick>(finishTime * getCpuRatio()) << "][+" << static_cast<Tick>(finishTime * getCpuRatio() - curTick) << "] at" << curTick);

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
