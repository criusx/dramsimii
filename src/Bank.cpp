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
using std::endl;

using namespace DRAMsimII;

//////////////////////////////////////////////////////////////////////////
/// @brief constructor with timing spec and system config values
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const Settings& settings, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal):
timing(timingVal),
systemConfig(systemConfigVal),
perBankQueue(settings.perBankQueueDepth),
lastRASTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
lastPrechargeTime(0),
lastRefreshAllTime(-100),
lastCASLength(8),
lastCASWLength(8),
openRowID(0),
activated(settings.rowBufferManagementPolicy == OPEN_PAGE), // close page starts with RAS, open page starts with Pre
RASCount(0),
totalRASCount(0),
CASCount(0),
totalCASCount(0),
CASWCount(0),
totalCASWCount(0)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor with timing spec and sysconfig information
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const Bank &b, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal):
timing(timingVal),
systemConfig(systemConfigVal),
perBankQueue(b.perBankQueue),
lastRASTime(b.lastRASTime),
lastCASTime(b.lastCASTime),
lastCASWTime(b.lastCASWTime),
lastPrechargeTime(b.lastPrechargeTime),
lastRefreshAllTime(b.lastRefreshAllTime),
lastCASLength(b.lastCASLength),
lastCASWLength(b.lastCASWLength),
openRowID(b.openRowID),
activated(b.activated),
RASCount(b.RASCount),
totalRASCount(b.totalRASCount),
CASCount(b.CASCount),
totalCASCount(b.totalCASCount),
CASWCount(b.CASWCount),
totalCASWCount(b.totalCASWCount)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const Bank &rhs):
timing(rhs.timing),
systemConfig(rhs.systemConfig),
perBankQueue(rhs.perBankQueue),
lastRASTime(rhs.lastRASTime),
lastCASTime(rhs.lastCASTime),
lastCASWTime(rhs.lastCASWTime),
lastPrechargeTime(rhs.lastPrechargeTime),
lastRefreshAllTime(rhs.lastRefreshAllTime),
lastCASLength(rhs.lastCASLength),
lastCASWLength(rhs.lastCASWLength),
openRowID(rhs.openRowID),
activated(rhs.activated),
RASCount(rhs.RASCount),
totalRASCount(rhs.totalRASCount),
CASCount(rhs.CASCount),
totalCASCount(rhs.totalCASCount),
CASWCount(rhs.CASWCount),
totalCASWCount(rhs.totalCASWCount)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief deserialization constructor 
//////////////////////////////////////////////////////////////////////////
Bank::Bank(const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal):
timing(timingVal),
systemConfig(systemConfigVal),
perBankQueue(0),
lastRASTime(0),
lastCASTime(0),
lastCASWTime(0),
lastPrechargeTime(0),
lastRefreshAllTime(0),
lastCASLength(0),
lastCASWLength(0),
openRowID(0),
activated(0),
RASCount(0),
totalRASCount(0),
CASCount(0),
totalCASCount(0),
CASWCount(0),
totalCASWCount(0)
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
		lastPrechargeTime = max(currentTime + (timing.tAL() - timing.tCCD() + timing.tBurst() + timing.tRTP()), lastRASTime + timing.tRAS());
		break;
	case Command::WRITE_AND_PRECHARGE:
		// see figure 11.29 in Memory Systems: Cache, DRAM, Disk by Bruce Jacob, et al.
		// obeys minimum timing, but also supports tRAS lockout
		lastPrechargeTime = max(currentTime + (timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR()), lastRASTime + timing.tRAS());
		break;
	case Command::PRECHARGE:
		lastPrechargeTime = currentTime;
		break;
	default:
		cerr << "Unhandled CAS variant" << endl;
		break;
	}	

	// make sure precharges follow activates
	// technically, you can pre after pre, but there's no good reason for this
	assert(activated == true);
	activated = false;
}

//////////////////////////////////////////////////////////////////////////
// @brief issue a CAS command to this bank
//////////////////////////////////////////////////////////////////////////
void Bank::issueCAS(const tick currentTime, const Command *currentCommand)
{
	//assert(activated);
	assert(openRowID == currentCommand->getAddress().getRow());
	
	lastCASTime = currentTime + timing.tAL();

	lastCASLength = currentCommand->getLength();
	
	CASCount++;
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a CASW command to this bank
//////////////////////////////////////////////////////////////////////////
void Bank::issueCASW(const tick currentTime, const Command *currentCommand)
{	
	//assert(activated);
	assert(openRowID == currentCommand->getAddress().getRow());

	lastCASWTime = currentTime + timing.tAL();

	lastCASWLength = currentCommand->getLength();

	CASWCount++;
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a refresh command to this bank
//////////////////////////////////////////////////////////////////////////
void Bank::issueREF(const tick currentTime, const Command *currentCommand)
{
	assert(!activated);
	lastRefreshAllTime = currentTime;
}

//////////////////////////////////////////////////////////////////////
/// @brief attempts to insert a transaction as a single CAS command, taking advantage of existing RAS-PRE commands
/// @details goes through the entire per bank queue to find a matching precharge command\n
/// if one is found, then the transaction is converted into the appropriate CAS command and inserted
/// @author Joe Gross
/// @param value the transaction to be inserted
/// @return true if the transaction was converted and inserted successfully, false otherwise
//////////////////////////////////////////////////////////////////////
bool Bank::openPageInsert(DRAMsimII::Transaction *value, tick time)
{
	if (!perBankQueue.isFull())
	{
		// look in the bank_q and see if there's a precharge for this row to insert before		
		// go from tail to head
		for (int currentIndex = perBankQueue.size() - 1; currentIndex >= 0; --currentIndex)
		{	
			const Command *currentCommand = perBankQueue.read(currentIndex);

			// then this command has been delayed by too many times and no more
			// commands can preempt it
			if (time - currentCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
			{
				return false;
			}
			// channel, rank, bank, row all match, insert just before this precharge command
			else if ((currentCommand->isReadOrWrite()) && (currentCommand->getAddress().getRow() == value->getAddress().getRow())) 
			{
				bool result = perBankQueue.insert(new Command(*value, time, systemConfig.isPostedCAS(), false, timing.tBurst()), currentIndex + 1);
				assert(result);

				return true;
			}			
			// strict order may add to the end of the queue only
			// if this has not happened already then this method of insertion fails
			else if (systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER)
			{
				return false;
			}
		}
		// if the correct row is already open, just insert there
		// already guaranteed not to have RAW/WAR errors
		if (activated && openRowID == value->getAddress().getRow())
		{
			bool result = perBankQueue.push_front(new Command(*value, time, systemConfig.isPostedCAS(), false, timing.tBurst()));
			assert(result);

			return true;
		}
		return false;
	}
	else
	{
		// no place to insert it
		return false;
	}
}

//////////////////////////////////////////////////////////////////////
/// @brief check to see if this transaction can be inserted successfully via the open page insert mechanism
/// @details goes through the per bank queue to see that there is a slot to insert into and that
/// there is a precharge command to the same row that it can insert before
/// @author Joe Gross
/// @param value the transaction to test
/// @return true if it is able to be inserted, false otherwise
//////////////////////////////////////////////////////////////////////
bool Bank::openPageInsertCheck(const Transaction *value, const tick time) const
{
	if (!perBankQueue.isFull())
	{
		// look in the bank_q and see if there's a precharge for this row to insert before		
		// go from tail to head
		for (int currentIndex = perBankQueue.size() - 1; currentIndex >= 0; --currentIndex)
		{	
			const Command *currentCommand = perBankQueue.read(currentIndex);

			// then this command has been delayed by too many times and no more
			// commands can preempt it
			if (time - currentCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
			{
				return false;
			}
			// channel, rank, bank, row all match, insert just before this precharge command
			else if (currentCommand->isReadOrWrite() && (currentCommand->getAddress().getRow() == value->getAddress().getRow())) 
			{
				return true;
			}
			// strict order may add to the end of the queue only
			// if this has not happened already then this method of insertion fails
			else if (systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER)
			{
				return false;
			}
		}
		// if the correct row is already open, just insert there
		// already guaranteed not to have RAW/WAR errors
		if (activated && openRowID == value->getAddress().getRow())
		{	
			return true;
		}
		return false;
	}
	else
	{
		// no place to insert it
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief see if there is room to insert a command using the Close Page Aggressive algorithm and then insert
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Bank::closePageAggressiveInsert(Transaction *incomingTransaction, const tick time)
{
	// go from the end to the beginning to ensure no starvation or RAW/WAR errors
	for (int index = perBankQueue.size() - 1; index >= 0; --index)
	{	
		// see if there is an available command to piggyback on
		if (perBankQueue[index]->isReadOrWrite() && 
			perBankQueue[index]->getAddress().getRow() == incomingTransaction->getAddress().getRow())
		{
			if (systemConfig.isAutoPrecharge())
			{
				perBankQueue[index]->setAutoPrecharge(false);
			}
			else
			{
				// check that things are in order
				assert(perBankQueue[index + 1]->isPrecharge());
			}

			bool result = perBankQueue.insert(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timing.tBurst()), index + 1);
			assert(perBankQueue[index + 1]->getAddress() == incomingTransaction->getAddress());
			assert(result);
			return result;

		}
		// don't starve commands
		if (time - perBankQueue[index]->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
			break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
/// @brief see if there is room to insert a command using the Close Page Aggressive algorithm
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Bank::closePageAggressiveInsertCheck(const Transaction *incomingTransaction, const tick time) const
{
	// go from the end to the beginning to ensure no starvation or RAW/WAR errors
	for (int index = perBankQueue.size() - 1; index >= 0; --index)
	{	
		// see if there is an available command to piggyback on
		if (perBankQueue[index]->isReadOrWrite() && 
			perBankQueue[index]->getAddress().getRow() == incomingTransaction->getAddress().getRow())
		{
			if (!systemConfig.isAutoPrecharge())
			{
				// check that things are in order
				assert(perBankQueue[index + 1]->isPrecharge());
			}
			return true;
		}
		// don't starve commands
		if (time - perBankQueue[index]->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
			break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
/// @brief assignment operator to copy non-reference values
//////////////////////////////////////////////////////////////////////////
Bank& Bank::operator =(const Bank& rhs)
{
	//::new(this)DRAMsimII::Bank(rhs.timing,rhs.systemConfig);
	//timing = rhs.timing;		
	//systemConfig = rhs.systemConfig;
	perBankQueue = rhs.perBankQueue;
	lastRASTime = rhs.lastRASTime;
	lastCASTime = rhs.lastCASTime;
	lastCASWTime = rhs.lastCASWTime;
	lastPrechargeTime = rhs.lastPrechargeTime;
	lastRefreshAllTime = rhs.lastRefreshAllTime;	
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
//////////////////////////////////////////////////////////////////////////
bool Bank::operator==(const Bank& rhs) const
{
	return (timing == rhs.timing && systemConfig == rhs.systemConfig && perBankQueue == rhs.perBankQueue && lastRASTime == rhs.lastRASTime &&
		lastCASTime == rhs.lastCASTime && lastCASWTime == rhs.lastCASWTime && lastPrechargeTime == rhs.lastPrechargeTime && 
		lastRefreshAllTime == rhs.lastRefreshAllTime && lastCASLength == rhs.lastCASLength && lastCASWLength == rhs.lastCASWLength && 
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
