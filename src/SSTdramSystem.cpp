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

#include "SSTdramSystem.h"

#include <configuration.h>
#include <instruction.h>

#include <debug.h>

using namespace DRAMSimII;

SSTdramSystem::SSTdramSystem(string cfgstr, const vector<DRAM*> &d):
SW2(cfgstr, d),
randomNumberGenerator(std::time(0)),
rngDistributionModel(0,1),
rngGenerator(randomNumberGenerator, rngDistributionModel),
cpuRatio(0)
{
	const char *settingsMap[2] = {"--settings", "/home/crius/m5-stable/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml"};

	Settings settings(2,settingsMap);

	settings.inFile = "";

	// if this is a normal system or a fbd system
	ds = new System(settings);
	//cpuRatio =(int)round(((float)ClockMhz()/((float)ds->Frequency())));	
}

SSTdramSystem::~SSTdramSystem()
{
	delete ds;
}

/// @brief used by prefetcher to determine whether to prefetch or not
/// @details returns the number of currently used DRAMs
/*int load() 
{
return portCount[FROM_DRAM];
}*/

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

		Address addr(effectiveAddress);

		if (ds->isFull(addr.getChannel()))
		{
			cerr << "can't fit this transaction into the queue" << std::endl;
		}
		else
		{
			Transaction *newTransaction = new Transaction(wb ? WRITE_TRANSACTION : READ_TRANSACTION,TimeStamp(), 8, addr,currentTransactionID);

			bool result = ds->enqueue(newTransaction);

			assert(result == true);

			transactionLookupTable[currentTransactionID] = p;
			currentTransactionID = (currentTransactionID + 1) % UINT_MAX;
		}

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

void SSTdramSystem::postTic()
{

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
	unsigned transactionID;
	while (((transactionID = ds->moveAllChannelsToTime(now, finishTime)) < UINT_MAX) || ds->getTime() < now)
	{
		parcel *packet = NULL;
		if (transactionID < UINT_MAX)
		{	
			std::map<unsigned,parcel*>::iterator packetIterator = transactionLookupTable.find(transactionID);
			assert(packetIterator != transactionLookupTable.end());
			packet = packetIterator->second;
			transactionLookupTable.erase(packetIterator);
		}
		if (packet)
		{
			static tick returnCount;

			if (++returnCount % 10000 == 0)
				cerr << returnCount << "\r";

			//assert(curTick <= static_cast<tick>(finishTime * cpuRatio));

			SST_TIMING_LOG("<-T [@" << std::dec << static_cast<tick>(finishTime ) << "]");

			//ports[lastPortIndex]->doSendTiming((Packet *)packet, static_cast<Tick>(finishTime * getCpuRatio()));
			/// @todo check for port contention when returning parcels
			if (packet->data() != 0x0)
				parcel::deleteParcel(packet);
			else
				sendParcel(packet,packet->source(), TimeStamp());
		}			
	}
}
