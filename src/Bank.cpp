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

#include "Bank.h"

using std::max;
using std::cerr;
using std::endl;

using namespace DRAMsimII;

//////////////////////////////////////////////////////////////////////////
/// @brief constructor with timing spec and system config values
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const Settings& settings, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal, Statistics &stats):
timing(timingVal),
systemConfig(systemConfigVal),
statistics(stats),
perBankQueue(settings.perBankQueueDepth),
lastRASTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
lastPrechargeTime(-1ll * settings.tRP),
lastCASLength(8),
lastCASWLength(8),
nextActivateTime(0),
nextReadTime(0),
nextWriteTime(0),
nextPrechargeTime(0),
openRowID(-1),
activated(/*settings.rowBufferManagementPolicy == OPEN_PAGE*/true), // close page starts with RAS, open page starts with Pre
RASCount(0),
totalRASCount(0),
CASCount(0),
totalCASCount(0),
CASWCount(0),
totalCASWCount(0),
allHits(false)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor with timing spec and sysconfig information
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const Bank &rhs, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal, Statistics &stats):
timing(timingVal),
systemConfig(systemConfigVal),
statistics(stats),
perBankQueue(rhs.perBankQueue),
lastRASTime(rhs.lastRASTime),
lastCASTime(rhs.lastCASTime),
lastCASWTime(rhs.lastCASWTime),
lastPrechargeTime(rhs.lastPrechargeTime),
lastCASLength(rhs.lastCASLength),
lastCASWLength(rhs.lastCASWLength),
nextActivateTime(rhs.nextActivateTime),
nextReadTime(rhs.nextReadTime),
nextWriteTime(rhs.nextWriteTime),
nextPrechargeTime(rhs.nextPrechargeTime),
openRowID(rhs.openRowID),
activated(rhs.activated),
RASCount(rhs.RASCount),
totalRASCount(rhs.totalRASCount),
CASCount(rhs.CASCount),
totalCASCount(rhs.totalCASCount),
CASWCount(rhs.CASWCount),
totalCASWCount(rhs.totalCASWCount),
allHits(rhs.allHits)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const Bank &rhs):
timing(rhs.timing),
systemConfig(rhs.systemConfig),
statistics(rhs.statistics),
perBankQueue(rhs.perBankQueue),
lastRASTime(rhs.lastRASTime),
lastCASTime(rhs.lastCASTime),
lastCASWTime(rhs.lastCASWTime),
lastPrechargeTime(rhs.lastPrechargeTime),
lastCASLength(rhs.lastCASLength),
lastCASWLength(rhs.lastCASWLength),
nextActivateTime(rhs.nextActivateTime),
nextReadTime(rhs.nextReadTime),
nextWriteTime(rhs.nextWriteTime),
nextPrechargeTime(rhs.nextPrechargeTime),
openRowID(rhs.openRowID),
activated(rhs.activated),
RASCount(rhs.RASCount),
totalRASCount(rhs.totalRASCount),
CASCount(rhs.CASCount),
totalCASCount(rhs.totalCASCount),
CASWCount(rhs.CASWCount),
totalCASWCount(rhs.totalCASWCount),
allHits(rhs.allHits)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief deserialization constructor
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal, Statistics &stats):
timing(timingVal),
systemConfig(systemConfigVal),
statistics(stats),
perBankQueue(0),
lastRASTime(0),
lastCASTime(0),
lastCASWTime(0),
lastPrechargeTime(0),
lastCASLength(0),
lastCASWLength(0),
nextActivateTime(0),
nextReadTime(0),
nextWriteTime(0),
nextPrechargeTime(0),
openRowID(0),
activated(0),
RASCount(0),
totalRASCount(0),
CASCount(0),
totalCASCount(0),
CASWCount(0),
totalCASWCount(0),
allHits(false)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief this logically issues a RAS command and updates all variables to reflect this
//////////////////////////////////////////////////////////////////////////
void Bank::issueRAS(const tick currentTime, const Command *currentCommand)
{
	// make sure activates follow precharges
	assert(!activated);
	assert(currentTime >= lastPrechargeTime + timing.tRP());

	activated = true;

	lastRASTime = currentTime;
	openRowID = currentCommand->getAddress().getRow();
	RASCount++;

	// calculate when the next few commands can happen
	nextActivateTime = max(nextActivateTime, currentTime + timing.tRC());
	nextReadTime = max(nextReadTime, currentTime + timing.tRCD() - timing.tAL());
	nextWriteTime = max(nextWriteTime, currentTime + timing.tRCD() - timing.tAL());
	nextPrechargeTime = max(nextPrechargeTime, currentTime + timing.tRAS());
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a precharge command to this bank
//////////////////////////////////////////////////////////////////////////
void Bank::issuePRE(const tick currentTime, const Command *currentCommand)
{
	switch (currentCommand->getCommandType())
	{
	case Command::READ_AND_PRECHARGE:
		//lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCAS() + timing.tBurst() + timing.tRTP(), lastRASTime + timing.tRAS());
		// see figure 11.28 in Memory Systems: Cache, DRAM, Disk by Bruce Jacob, et al.
		lastPrechargeTime = max(lastPrechargeTime, max(currentTime + (timing.tAL() - timing.tCCD() + timing.tBurst() + timing.tRTP()), lastRASTime + timing.tRAS()));
		break;
	case Command::WRITE_AND_PRECHARGE:
		// see figure 11.29 in Memory Systems: Cache, DRAM, Disk by Bruce Jacob, et al.
		// obeys minimum timing, but also supports tRAS lockout
		lastPrechargeTime = max(lastPrechargeTime, max(currentTime + (timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR()), lastRASTime + timing.tRAS()));
		break;
	case Command::PRECHARGE:
		lastPrechargeTime = max(lastPrechargeTime, currentTime);
		break;
	default:
		cerr << "Unhandled CAS variant" << endl;
		break;
	}	

	// make sure precharges follow activates
	// technically, you can pre after pre, but there's no good reason for this
	assert(activated == true);
	activated = false;

	// calculate when the next few commands can happen
	nextActivateTime = max(nextActivateTime, lastPrechargeTime + timing.tRP());
}

//////////////////////////////////////////////////////////////////////////
// @brief issue a CAS command to this bank
//////////////////////////////////////////////////////////////////////////
void Bank::issueCAS(const tick currentTime, const Command *currentCommand)
{
	//assert(activated);
	assert(openRowID == currentCommand->getAddress().getRow());

	//lastCASTime = currentTime + timing.tAL();
	lastCASTime = currentTime;

	lastCASLength = currentCommand->getLength();

	CASCount++;

	// calculate when the next few commands can happen
	/// @todo which is correct?
	//nextPrechargeTime = max(nextPrechargeTime, currentTime + timing.tAL() + timing.tBurst() + timing.tRTP() - timing.tCCD());
	nextPrechargeTime = max(nextPrechargeTime, currentTime + timing.tAL() + timing.tCAS() + timing.tBurst() + max(0,timing.tRTP() - timing.tCMD()));
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a CASW command to this bank
//////////////////////////////////////////////////////////////////////////
void Bank::issueCASW(const tick currentTime, const Command *currentCommand)
{	
	//assert(activated);
	assert(openRowID == currentCommand->getAddress().getRow());

	//lastCASWTime = currentTime + timing.tAL();
	lastCASWTime = currentTime;

	lastCASWLength = currentCommand->getLength();

	CASWCount++;

	// calculate when the next few commands can happen
	nextPrechargeTime = max(nextPrechargeTime, currentTime + timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR());

	// if there is a write, because it is write-through then the activate needed to happen
	allHits = false;
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a refresh command to this bank
//////////////////////////////////////////////////////////////////////////
void Bank::issueREF()
{
	assert(!activated);
}

//////////////////////////////////////////////////////////////////////////
/// @brief returns the next time this command type may be issued
//////////////////////////////////////////////////////////////////////////
tick Bank::next(Command::CommandType nextCommandType) const
{
	switch (nextCommandType)
	{
	case Command::ACTIVATE:
		return nextActivateTime;
		break;
	case Command::READ:
	case Command::READ_AND_PRECHARGE:
		return nextReadTime;
		break;
	case Command::WRITE:
	case Command::WRITE_AND_PRECHARGE:
		return nextWriteTime;
		break;
	case Command::PRECHARGE:
		return nextPrechargeTime;
		break;
	case Command::REFRESH_ALL:
		return 0;
		break;
	default:
		return TICK_MAX;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief reset statistics so that it appears the last command was not long ago
/// @details choose recent times for the lastX actions so that values are not so
/// large when looking to see when the next available time to execute any dependent
/// command. Often issued just after fast-forwarding finishes in a simulator
//////////////////////////////////////////////////////////////////////////
void Bank::resetToTime(const tick time)
{
	lastRASTime = time - timing.tRC();
	lastPrechargeTime = time - timing.tRP();
	lastCASTime = time - timing.tCAS() - timing.tBurst();
	lastCASWTime = time - timing.tCWD() - timing.tWTR() - timing.tBurst();

	nextPrechargeTime = lastCASWTime + timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR();
	nextPrechargeTime = max(nextPrechargeTime,lastCASTime + timing.tAL() + timing.tCAS() + timing.tBurst() + max(0,timing.tRTP() - timing.tCMD()));
	nextActivateTime = lastPrechargeTime + timing.tRP();

	nextActivateTime = max(nextActivateTime, lastRASTime + timing.tRC());
	nextReadTime = lastRASTime + timing.tRCD() - timing.tAL();
	nextWriteTime = lastRASTime + timing.tRCD() - timing.tAL();
	nextPrechargeTime = max(nextPrechargeTime, lastRASTime + timing.tRAS());
}


//////////////////////////////////////////////////////////////////////
/// @brief check to see if this transaction can be inserted successfully via the open page aggressive insert mechanism
/// @details goes through the per bank queue to see that there is a slot to insert into and that
/// there is a precharge command to the same row that it can insert before
/// also looks for CAS, Pre commands that can be compressed in order to fit this
/// @author Joe Gross
/// @param incomingTransaction the transaction to test
/// @param time the current time, used to check and prevent against starvation of commands
/// @return true if it is able to be inserted, false otherwise
//////////////////////////////////////////////////////////////////////
bool Bank::openPageAggressiveInsertCheck(const Transaction *incomingTransaction, const tick time) const
{
	if (perBankQueue.freecount() >= 3)
	{
		return true;
	}
	else 
	{
		unsigned availableSlots = perBankQueue.freecount();

		// if the queue ends with a R/W(+P), then all that is needed is A, R/W
		if (perBankQueue.back()->isReadOrWrite())
		{
			if (availableSlots >= 2)
				return true;
		}

		for (unsigned i = 0; i < perBankQueue.size() - 1; i++)
		{
			if (perBankQueue[i]->isReadOrWrite() && perBankQueue[i+1] && perBankQueue[i+1]->isBasicPrecharge())
			{
				availableSlots++;
				if (availableSlots >= 3)
					return true;
			}
		}

		if (availableSlots == 0)
			return false;

		const unsigned currentRow = incomingTransaction->getAddress().getRow();

		// look in the bank_q and see if there's a precharge for this row to insert before		
		// go from tail to head
		for (int currentIndex = perBankQueue.size() - 1; currentIndex >= 0; --currentIndex)
		{	
			const Command *currentCommand = perBankQueue.read(currentIndex);

			// channel, rank, bank, row all match, insert just before this precharge command
			if (currentCommand->isReadOrWrite() && (currentCommand->getAddress().getRow() == currentRow))
			{
				assert(currentCommand->getAddress().getChannel() == incomingTransaction->getAddress().getChannel());
				assert(currentCommand->getAddress().getRank() == incomingTransaction->getAddress().getRank());
				assert(currentCommand->getAddress().getBank() == incomingTransaction->getAddress().getBank());
				assert(currentCommand->getAddress().getRow() == incomingTransaction->getAddress().getRow());

				return true;
			}
			// strict order may add to the end of the queue only
			// if this has not happened already then this method of insertion fails
			else if (systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER)
			{
				return false;
			}
			// then this command has been delayed by too many times and no more
			// commands can preempt it
			else if (time - currentCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
			{
				return false;
			}
		}
		// if the correct row is already open, just insert there
		// already guaranteed not to have RAW/WAR errors
		if (activated && openRowID == currentRow)
		{	
			return true;
		}
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief reduce discrete CAS and Precharge commands to CAS+P to free up space
//////////////////////////////////////////////////////////////////////////
void Bank::collapse()
{
	for (unsigned i = 0; i < perBankQueue.size() - 1; i++)
	{
		if (perBankQueue[i]->isReadOrWrite() && perBankQueue[i+1] && perBankQueue[i+1]->isBasicPrecharge())
		{
			if (perBankQueue[i]->getAddress().getBank() != perBankQueue[i+1]->getAddress().getBank())
				assert(false);
			assert(!perBankQueue[i]->isPrecharge());
			perBankQueue[i]->setAutoPrecharge(true);
			Command *toDelete = perBankQueue.remove(i+1);
			assert(!toDelete->isReadOrWrite() && toDelete->isBasicPrecharge());
			assert(!toDelete->getHost());
			//cerr << *toDelete << endl;
			delete toDelete;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief see if there is room to insert a command using the Close Page Aggressive algorithm and then insert
/// @param incomingTransaction the transaction to insert
/// @param time the current time, used to check and prevent against starvation of commands
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Bank::aggressiveInsert(Transaction *incomingTransaction, const tick time)
{
	if (!perBankQueue.isFull())
	{
		const unsigned currentRow = incomingTransaction->getAddress().getRow();

		// go from the end to the beginning to ensure no starvation or RAW/WAR errors
		for (int currentIndex = perBankQueue.size() - 1; currentIndex >= 0; --currentIndex)
		{	
			const Command *currentCommand = perBankQueue.read(currentIndex);

			// see if there is an available command to piggyback on
			if (currentCommand->isReadOrWrite() && currentCommand->getAddress().getRow() == currentRow)
			{
				bool needsPrecharge = currentCommand->isPrecharge();

				if (needsPrecharge)
				{
					currentCommand->setAutoPrecharge(false);
				}			

				// if the precharge was stripped from the n-1 command, add it to this one
#ifndef NDEBUG
				bool result = 
#endif
					perBankQueue.insert(new Command(incomingTransaction, time, needsPrecharge, timing.tBurst()), currentIndex + 1);
				assert(perBankQueue[currentIndex + 1]->getAddress() == incomingTransaction->getAddress());
				assert(result);
				return true;
			}
			// strict order may add to the end of the queue only
			// if this has not happened already then this method of insertion fails
			else if (systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER)
			{
				return false;
			}
			else if (time - currentCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
			{
				return false;
			}
		}

		// if the correct row is already open, just insert there
		// already guaranteed not to have RAW/WAR errors
		if (activated && openRowID == currentRow)
		{
#ifndef NDEBUG
			bool result =
#endif
				perBankQueue.push_front(new Command(incomingTransaction, time, false, timing.tBurst()));
			assert(result);

			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
/// @brief see if there is room to insert a command using the Close Page Aggressive algorithm
/// @param incomingTransaction the transaction to insert
/// @param time the current time, used to check and prevent against starvation of commands
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Bank::closePageAggressiveInsertCheck(const Transaction *incomingTransaction, const tick time) const
{
	if (!perBankQueue.isFull())
	{
		const unsigned currentRow = incomingTransaction->getAddress().getRow();
		// go from the end to the beginning to ensure no starvation or RAW/WAR errors
		for (int currentIndex = perBankQueue.size() - 1; currentIndex >= 0; --currentIndex)
		{	
			const Command *currentCommand = perBankQueue[currentIndex];
			// see if there is an available command to piggyback on
			if (currentCommand->isReadOrWrite() && currentCommand->getAddress().getRow() == currentRow)
			{
				assert(currentCommand->getAddress().getChannel() == incomingTransaction->getAddress().getChannel());
				assert(currentCommand->getAddress().getRank() == incomingTransaction->getAddress().getRank());
				assert(currentCommand->getAddress().getBank() == incomingTransaction->getAddress().getBank());
				assert(currentCommand->getAddress().getRow() == incomingTransaction->getAddress().getRow());

				if (!systemConfig.isAutoPrecharge())
				{
					// check that things are in order
					assert(perBankQueue[currentIndex + 1]->isPrecharge());
				}
				return true;
			}
			else if (systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER)
			{
				return false;
			}
			// don't starve commands
			else if (time - currentCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
			{
				return false;
			}
		}
		if (activated && openRowID == currentRow)
		{
			return true;
		}
	}
	return false;
}

bool Bank::isEmpty() const
{
	return perBankQueue.isEmpty();
}

bool Bank::hasNoReadWrite() const
{
	for (unsigned i = 0; i < perBankQueue.size(); i++)
	{
		if (!perBankQueue[i]->isRefresh())
			return false;
	}
	return true;

}

//////////////////////////////////////////////////////////////////////////
/// @brief assignment operator to copy non-reference values
/// @param rhs the incomingTransaction that will be copied into this object
//////////////////////////////////////////////////////////////////////////
Bank& Bank::operator =(const Bank& rhs)
{
	perBankQueue = rhs.perBankQueue;
	lastRASTime = rhs.lastRASTime;
	lastCASTime = rhs.lastCASTime;
	lastCASWTime = rhs.lastCASWTime;
	lastPrechargeTime = rhs.lastPrechargeTime;
	lastCASLength = rhs.lastCASLength;		
	lastCASWLength = rhs.lastCASWLength;		
	openRowID = rhs.openRowID;			
	activated = rhs.activated;			
	RASCount = rhs.RASCount;			
	totalRASCount = rhs.totalRASCount;		
	CASCount = rhs.CASCount;			
	totalCASCount = rhs.totalCASCount;		
	CASWCount = rhs.CASWCount;			
	totalCASWCount = rhs.totalCASWCount;	

	return *this;
}

//////////////////////////////////////////////////////////////////////////
/// @brief equality operator to check values for equality
/// @param rhs the incomingTransaction that will be copied into this object
//////////////////////////////////////////////////////////////////////////
bool Bank::operator==(const Bank& rhs) const
{
	return (timing == rhs.timing && systemConfig == rhs.systemConfig && perBankQueue == rhs.perBankQueue && lastRASTime == rhs.lastRASTime &&
		lastCASTime == rhs.lastCASTime && lastCASWTime == rhs.lastCASWTime && lastPrechargeTime == rhs.lastPrechargeTime &&
		lastCASLength == rhs.lastCASLength && lastCASWLength == rhs.lastCASWLength &&
		openRowID == rhs.openRowID && activated == rhs.activated && RASCount == rhs.RASCount && totalRASCount == rhs.totalRASCount &&
		CASCount == rhs.CASCount && totalCASCount == rhs.totalCASCount && CASWCount == rhs.CASWCount && totalCASWCount == rhs.totalCASWCount);
}

//////////////////////////////////////////////////////////////////////////
/// @brief insertion operator to serialize the object in summary
//////////////////////////////////////////////////////////////////////////
std::ostream& DRAMsimII::operator<<(std::ostream& in, const Bank& pc)
{
	return in << "PBQ" << endl << pc.perBankQueue << "last RAS [" << pc.lastRASTime << "] act[" <<
		pc.activated << "] open row[" << pc.openRowID << "]" << endl;	
}
