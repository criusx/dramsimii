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

#include <iostream>
#include <boost/functional.hpp>
#include "Rank.h"

using std::vector;
using std::max;
using std::endl;
using std::cerr;
using namespace DRAMsimII;

Rank::Rank(const Settings& settings, const TimingSpecification &timing, const SystemConfiguration &sysConfig, Statistics& stats):
timing(timing),
systemConfig(sysConfig),
statistics(stats),
lastRefreshTime(-1ll * settings.tRFC),
lastPrechargeAnyBankTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
otherLastCASTime(-100),
otherLastCASWTime(-100),
prechargeTime(0),
totalPrechargeTime(0),
nextActivateTime(0),
nextReadTime(0),
nextWriteTime(0),
nextRefreshTime(0),
lastCalculationTime(0),
lastCASLength(0),
lastCASWLength(0),
otherLastCASLength(0),
otherLastCASWLength(0),
CASLength(0),
CASWLength(0),
rankID(UINT_MAX),
lastBankID(settings.bankCount - 1),
banksPrecharged(/*settings.rowBufferManagementPolicy == OPEN_PAGE ? 0 : settings.bankCount */ 0),
tags(settings),
lastActivateTimes(4, 4, -100), // make the queue hold four (tFAW)
bank(sysConfig.getBankCount(),Bank(settings, timing, sysConfig, stats))
{}

Rank::Rank(const Rank &rhs):
timing(rhs.timing),
systemConfig(rhs.systemConfig),
statistics(rhs.statistics),
lastRefreshTime(rhs.lastRefreshTime),
lastPrechargeAnyBankTime(rhs.lastPrechargeAnyBankTime),
lastCASTime(rhs.lastCASTime),
lastCASWTime(rhs.lastCASWTime),
otherLastCASTime(rhs.otherLastCASTime),
otherLastCASWTime(rhs.otherLastCASWTime),
prechargeTime(rhs.prechargeTime),
totalPrechargeTime(rhs.totalPrechargeTime),
nextActivateTime(rhs.nextActivateTime),
nextReadTime(rhs.nextReadTime),
nextWriteTime(rhs.nextWriteTime),
nextRefreshTime(rhs.nextRefreshTime),
lastCalculationTime(0),
lastCASLength(rhs.lastCASLength),
lastCASWLength(rhs.lastCASWLength),
otherLastCASLength(rhs.otherLastCASLength),
otherLastCASWLength(rhs.otherLastCASWLength),
CASLength(rhs.CASLength),
CASWLength(rhs.CASWLength),
rankID(rhs.rankID),
lastBankID(rhs.lastBankID),
banksPrecharged(rhs.banksPrecharged),
tags(rhs.tags),
lastActivateTimes(rhs.lastActivateTimes),
bank(rhs.bank)
{}

Rank::Rank(const Rank &rhs, const TimingSpecification &timing, const SystemConfiguration &sysConfig, Statistics& stats):
timing(timing),
systemConfig(sysConfig),
statistics(rhs.statistics),
lastRefreshTime(rhs.lastRefreshTime),
lastPrechargeAnyBankTime(rhs.lastPrechargeAnyBankTime),
lastCASTime(rhs.lastCASTime),
lastCASWTime(rhs.lastCASWTime),
otherLastCASTime(rhs.otherLastCASTime),
otherLastCASWTime(rhs.otherLastCASWTime),
prechargeTime(rhs.prechargeTime),
totalPrechargeTime(rhs.totalPrechargeTime),
nextActivateTime(rhs.nextActivateTime),
nextReadTime(rhs.nextReadTime),
nextWriteTime(rhs.nextWriteTime),
nextRefreshTime(rhs.nextRefreshTime),
lastCalculationTime(0),
lastCASLength(rhs.lastCASLength),
lastCASWLength(rhs.lastCASWLength),
otherLastCASLength(rhs.otherLastCASLength),
otherLastCASWLength(rhs.otherLastCASWLength),
CASLength(rhs.CASLength),
CASWLength(rhs.CASWLength),
rankID(rhs.rankID),
lastBankID(rhs.lastBankID),
banksPrecharged(rhs.banksPrecharged),
tags(rhs.tags),
lastActivateTimes(rhs.lastActivateTimes),
bank((unsigned)sysConfig.getBankCount(), Bank(rhs.bank[0], timing, sysConfig, stats))
{
	// TODO: copy over values in banks now that reference members are init	
	//for (unsigned i = 0; i < systemConfig.getBankCount(); i++)
	//{
	//	bank[i] = rhs.bank[i];
	//}
	bank = rhs.bank;
}

Rank::Rank(const TimingSpecification &timingSpec, const std::vector<Bank>& newBank, Statistics& stats, SystemConfiguration& sysConfig):
timing(timingSpec),
systemConfig(sysConfig),
statistics(stats),
lastRefreshTime(-1ll * timingSpec.tRFC()),
lastPrechargeAnyBankTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
otherLastCASTime(-100),
otherLastCASWTime(-100),
prechargeTime(0),
totalPrechargeTime(0),
nextActivateTime(0),
nextReadTime(0),
nextWriteTime(0),
nextRefreshTime(0),
lastCalculationTime(0),
lastCASLength(0),
lastCASWLength(0),
otherLastCASLength(0),
otherLastCASWLength(0),
CASLength(0),
CASWLength(0),
rankID(UINT_MAX),
lastBankID(0),
banksPrecharged(0),
tags(128,64,4,5),
lastActivateTimes(4, 4, -100), // make the queue hold four (tFAW)
bank(newBank)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief this logically issues a RAS command and updates all variables to reflect this
//////////////////////////////////////////////////////////////////////////
void Rank::issueRAS(const tick currentTime, const Command *currentCommand)
{
	//////////////////////////////////////////////////////////////////////////
	bank[currentCommand->getAddress().getBank()].setAllHits(true);
	//////////////////////////////////////////////////////////////////////////

	// RAS time history queue, per rank
	const tick thisRASTime = currentTime;
	// record these for tFAW calculations
	lastActivateTimes.push_front(thisRASTime);

	lastBankID = currentCommand->getAddress().getBank();

	// for power modeling, if all banks were precharged and now one is being activated, record the interval that one was precharged	
	if (banksPrecharged == bank.size())
	{
		prechargeTime += max(currentTime - max(lastCalculationTime, lastPrechargeAnyBankTime),(tick)0);
		totalPrechargeTime += max(currentTime - max(lastCalculationTime, lastPrechargeAnyBankTime), (tick)0);
		for (vector<Bank>::const_iterator curBnk = bank.begin(); curBnk != bank.end(); ++curBnk)
			assert(!curBnk->isActivated());
	}	
	banksPrecharged--;
	assert(banksPrecharged < bank.size());
	// update the bank to reflect this change also
	bank[currentCommand->getAddress().getBank()].issueRAS(currentTime,currentCommand);

	// calculate when the next few commands can happen
	nextActivateTime = max(currentTime + timing.tRRD(), max(lastActivateTimes.back() + timing.tFAW() , nextActivateTime));
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a precharge command to this rank
//////////////////////////////////////////////////////////////////////////
void Rank::issuePRE(const tick currentTime, const Command *currentCommand)
{
	//////////////////////////////////////////////////////////////////////////
	if (bank[currentCommand->getAddress().getBank()].isAllHits() && currentCommand->isBasicPrecharge())
	{
		statistics.reportRasReduction(currentCommand);
	}
	//////////////////////////////////////////////////////////////////////////

	// update the bank to reflect this change also
	Bank &currentBank = bank[currentCommand->getAddress().getBank()];
	currentBank.issuePRE(currentTime, currentCommand);

	switch (currentCommand->getCommandType())
	{
	case Command::READ_AND_PRECHARGE:		
	case Command::WRITE_AND_PRECHARGE:
		lastPrechargeAnyBankTime = max(lastPrechargeAnyBankTime, currentBank.getLastPrechargeTime());
		break;
	case Command::PRECHARGE:
		// choose the latest time since there may be +Pre commands that have queued a Pre internally
		lastPrechargeAnyBankTime = max(lastPrechargeAnyBankTime, currentTime);
		break;
	default:
		cerr << "Unhandled CAS variant" << endl;
		break;
	}

	banksPrecharged++;
	assert(banksPrecharged > 0);
	assert(banksPrecharged <= bank.size());

	// calculate when the next few commands can happen
	nextRefreshTime = max(nextRefreshTime, lastPrechargeAnyBankTime + timing.tRP());
	assert (nextRefreshTime == lastPrechargeAnyBankTime + timing.tRP() || nextRefreshTime == lastRefreshTime + timing.tRFC());

}

//////////////////////////////////////////////////////////////////////////
// @brief issue a CAS command to this rank
//////////////////////////////////////////////////////////////////////////
bool Rank::issueCAS(const tick currentTime, const Command *currentCommand)
{
	//////////////////////////////////////////////////////////////////////////
	bool satisfied = tags.timingAccess(currentCommand, currentCommand->getStartTime());
	if (!satisfied)
		bank[currentCommand->getAddress().getBank()].setAllHits(false);
	if (bank[currentCommand->getAddress().getBank()].isAllHits() && currentCommand->isPrecharge())
	{
		statistics.reportRasReduction(currentCommand);
	}
	//std::cout << (satisfied ? "|" : ".");
	//////////////////////////////////////////////////////////////////////////

	// update the bank to reflect this change also
	bank[currentCommand->getAddress().getBank()].issueCAS(currentTime, currentCommand);

	lastCASTime = currentTime;

	lastCASLength = currentCommand->getLength();

	CASLength += currentCommand->getLength();

	assert(currentCommand->getAddress().getBank() == lastBankID);

	// calculate when the next few commands can happen
	nextReadTime = max(nextReadTime, currentTime + timing.tBurst());
	nextWriteTime = max(nextWriteTime, currentTime + timing.tCAS() + timing.tBurst() + timing.tRTRS() - timing.tCWD());

	return satisfied;
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a CASW command to this rank
//////////////////////////////////////////////////////////////////////////
bool Rank::issueCASW(const tick currentTime, const Command *currentCommand)
{
	//////////////////////////////////////////////////////////////////////////
	bool satisfied = tags.timingAccess(currentCommand, currentCommand->getStartTime());
	bank[currentCommand->getAddress().getBank()].setAllHits(false);
	//std::cout << (satisfied ? "|" : ".");
	//////////////////////////////////////////////////////////////////////////

	// update the bank to reflect this change also
	bank[currentCommand->getAddress().getBank()].issueCASW(currentTime, currentCommand);

	lastCASWTime = currentTime;

	lastCASWLength = currentCommand->getLength();

	CASWLength += currentCommand->getLength();

	assert(currentCommand->getAddress().getBank() == lastBankID);
	
	// calculate when the next few commands can happen
	// ensure that the next read does not happen until the write is done with the I/O drivers
	nextReadTime = max(nextReadTime, currentTime + timing.tCWD() + timing.tBurst() + timing.tWTR());

	nextWriteTime = max(nextWriteTime, currentTime + timing.tBurst());

	return satisfied;
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a refresh command to this rank
//////////////////////////////////////////////////////////////////////////
void Rank::issueREF(const tick currentTime)
{
	lastRefreshTime = currentTime;

	// FIXME: should this not count as a RAS + PRE command to all banks?
	std::for_each(bank.begin(), bank.end(), boost::mem_fun_ref(&Bank::issueREF));

	// calculate when the next few commands can happen
	nextRefreshTime = max(nextRefreshTime, currentTime + timing.tRFC());
	assert (nextRefreshTime == lastPrechargeAnyBankTime + timing.tRP() || nextRefreshTime == lastRefreshTime + timing.tRFC() || currentTime < 250);
	nextActivateTime = max(nextActivateTime, currentTime + timing.tRFC());
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a read command to another rank at this time
//////////////////////////////////////////////////////////////////////////
void Rank::issueCASother(const tick currentTime, const Command *currentCommand)
{
	assert(currentTime + timing.tAL() > otherLastCASTime); 
	otherLastCASTime = currentTime; 
	otherLastCASLength = currentCommand->getLength();

	// calculate when the next few commands can happen
	nextReadTime = max(nextReadTime, currentTime + timing.tBurst() + timing.tRTRS());
	nextWriteTime = max(nextWriteTime, currentTime + timing.tCAS() + timing.tBurst() + timing.tRTRS() - timing.tCWD());
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a write command to another rank at this time
//////////////////////////////////////////////////////////////////////////
void Rank::issueCASWother(const tick currentTime, const Command *currentCommand)
{
	assert(currentTime + timing.tAL() > otherLastCASWTime);
	otherLastCASWTime = currentTime;
	otherLastCASWLength = currentCommand->getLength();

	// calculate when the next few commands can happen
	nextReadTime = max(nextReadTime, currentTime + timing.tCWD() + timing.tBurst() + timing.tRTRS() - timing.tCAS());
	nextWriteTime = max(nextWriteTime, currentTime + timing.tOST() + timing.tBurst());
}

//////////////////////////////////////////////////////////////////////////
/// @brief reset the accumulated precharge time for this rank
//////////////////////////////////////////////////////////////////////////
void Rank::resetPrechargeTime(const tick time)
{
	prechargeTime = 0;
	lastCalculationTime = time;
	assert (nextRefreshTime == lastPrechargeAnyBankTime + timing.tRP() || nextRefreshTime == lastRefreshTime + timing.tRFC());
}

void Rank::setRankID(const unsigned channelID, const unsigned rankID)
{
	this->rankID = rankID;

	if (systemConfig.getRowBufferManagementPolicy() == CLOSE_PAGE || systemConfig.getRowBufferManagementPolicy() == CLOSE_PAGE_AGGRESSIVE)
	{		
		unsigned bankID = 0;
		for (vector<Bank>::iterator i = bank.begin(); i != bank.end(); ++i)
		{
			Transaction t(Transaction::AUTO_PRECHARGE_TRANSACTION,0,timing.tBurst(), Address(channelID,rankID,bankID++, 0,0));
			i->push(new Command(&t, 0, false, timing.tBurst(), Command::PRECHARGE));
		}
	}
}


//////////////////////////////////////////////////////////////////////////
/// @brief returns the next time this command type may be issued
//////////////////////////////////////////////////////////////////////////
tick Rank::next(Command::CommandType nextCommandType) const
{
	switch (nextCommandType)
	{
	case Command::READ:
	case Command::READ_AND_PRECHARGE:
		return nextReadTime;
		break;
	case Command::ACTIVATE:
		return nextActivateTime;
		break;	
	case Command::WRITE:
	case Command::WRITE_AND_PRECHARGE:
		return nextWriteTime;
		break;
	case Command::PRECHARGE:
		return 0;
		break;
	case Command::REFRESH_ALL:
		return nextRefreshTime;
		break;
	default:
		return TICK_MAX;
		break;
	}
}

//////////////////////////////////////////////////////////////////////
/// @brief get the next command in the queue for this bank
/// @details simply return the command for this bank if it is not a refresh all command
/// otherwise remove the refresh all commands from all the other queues, assuming there is
/// a refresh all command at the head of each, else return null
/// @author Joe Gross
/// @param thisBank the ordinal of the bank to get the next command for (0..n)
/// @return the next command to be issued for this bank
//////////////////////////////////////////////////////////////////////
Command *Rank::getCommand(const unsigned thisBank)
{
	if (bank[thisBank].nextCommandType() == Command::REFRESH_ALL)
	{
		if (refreshAllReady())
		{
			Command *tempCommand = NULL;

			for (vector<Bank>::iterator currentBank = bank.begin(); currentBank != bank.end(); ++currentBank)
			{
				delete tempCommand;

				tempCommand = currentBank->pop();

				assert(tempCommand->isRefresh());
			}

			return tempCommand;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return bank[thisBank].pop();
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief update this rank to seem like time started at a nonzero time
/// @details used when the simulator has fast forwarded to a certain time and then
/// begun detailed simulation. this function will avoid giving too large/small values
/// after a simple->detailed switch
//////////////////////////////////////////////////////////////////////////
void Rank::resetToTime(const tick time)
{
	lastCASTime = time - 1000;
	lastCASWTime = time - 1000;
	lastPrechargeAnyBankTime = time;
	lastRefreshTime = time - timing.tRFC();
	lastCalculationTime = time;
	prechargeTime = 0;

	nextActivateTime = time;
	nextRefreshTime = time + timing.tRP();
	for (boost::circular_buffer<tick>::iterator i = lastActivateTimes.begin(); i != lastActivateTimes.end(); ++i)
		*i = time - timing.tFAW();
	for (vector<Bank>::iterator i = bank.begin(); i != bank.end(); ++i)
		i->resetToTime(time);

}

//////////////////////////////////////////////////////////////////////////
/// @brief get the total length of time that this rank had all banks precharged 
/// @returns cycles all banks are precharged
//////////////////////////////////////////////////////////////////////////
tick Rank::getTotalPrechargeTime(const tick currentTime) const
{	
	return totalPrechargeTime + ((banksPrecharged == bank.size()) ? max(currentTime - max(lastPrechargeAnyBankTime, lastCalculationTime), (tick)0) : 0);
}

//////////////////////////////////////////////////////////////////////////
/// @brief get the total length of time that this rank had all banks precharged since the last reset
/// @returns cycles all banks are precharged
//////////////////////////////////////////////////////////////////////////
tick Rank::getPrechargeTime(const tick currentTime) const
{
	return prechargeTime + ((banksPrecharged == bank.size()) ? max(currentTime - max(lastPrechargeAnyBankTime, lastCalculationTime), (tick)0) : 0);
}

//////////////////////////////////////////////////////////////////////
/// @brief is there a refresh command ready at each per bank queue
/// @details if there is a refresh all command at the head of each per
/// bank queue, then the next command for this rank is a refresh all banks
/// command\n
/// this is signified by inserting a refresh all command into the tail of each
/// queue and removing them when they all reach the front\n
/// this is done to ensure that a refresh command will never disturb an open
/// row that has a waiting CAS, as the refresh command will ruin the row
/// @author Joe Gross
/// @return true if all queue heads have refresh all commands
//////////////////////////////////////////////////////////////////////
bool Rank::refreshAllReady() const
{
	for (vector<Bank>::const_iterator currentBank = bank.begin(); currentBank != bank.end(); ++currentBank)
	{
		// if any queue is empty or the head of any queue isn't a refresh command, then the rank isn't ready for a refresh all command
		if (!currentBank->front() || !currentBank->front()->isRefresh())
		{
			return false;
		}
	}
	return true;
}

bool Rank::isEmpty() const
{
	for (vector<Bank>::const_iterator i = bank.begin(); i != bank.end(); ++i)
	{
		if (!i->hasNoReadWrite())
			return false;
	}
	return true;
}

Rank& Rank::operator =(const Rank& rhs)
{
	//::new(this)DRAMsimII::Rank(rhs.timing,rhs.bank);
	lastRefreshTime = rhs.lastRefreshTime;
	lastPrechargeAnyBankTime = rhs.lastPrechargeAnyBankTime;
	lastCASTime = rhs.lastCASTime;
	lastCASWTime = rhs.lastCASWTime;
	prechargeTime = rhs.prechargeTime;
	totalPrechargeTime = rhs.totalPrechargeTime;
	lastCASLength = rhs.lastCASLength;
	lastCASWLength = rhs.lastCASWLength;
	rankID = rhs.rankID;
	lastBankID = rhs.lastBankID;
	banksPrecharged = rhs.banksPrecharged;
	lastActivateTimes = rhs.lastActivateTimes;
	bank = rhs.bank;
	CASLength = rhs.CASLength;
	CASWLength = rhs.CASWLength;
	otherLastCASTime = rhs.otherLastCASTime;
	otherLastCASWTime = rhs.otherLastCASWTime;
	otherLastCASLength = rhs.otherLastCASLength;
	otherLastCASWLength = rhs.otherLastCASWLength;
	tags = rhs.tags;
	lastCalculationTime = rhs.lastCalculationTime;
	nextRefreshTime = rhs.nextRefreshTime;
	nextWriteTime = rhs.nextWriteTime;
	nextReadTime = rhs.nextReadTime;
	nextActivateTime = rhs.nextActivateTime;

	return *this;
}

bool Rank::operator==(const Rank& right) const
{
	return (timing == right.timing && lastRefreshTime == right.lastRefreshTime && lastPrechargeAnyBankTime == right.lastPrechargeAnyBankTime &&
		lastCASTime == right.lastCASTime && lastCASWTime == right.lastCASWTime && prechargeTime == right.prechargeTime && totalPrechargeTime == right.totalPrechargeTime &&
		lastCASLength == right.lastCASLength && lastCASWLength == right.lastCASWLength && rankID == right.rankID && lastBankID == right.lastBankID &&
		banksPrecharged == right.banksPrecharged && lastActivateTimes == right.lastActivateTimes && bank == right.bank && CASLength == right.CASLength &&
		CASWLength == right.CASWLength && otherLastCASTime == right.otherLastCASTime && otherLastCASWTime == right.otherLastCASWTime &&
		otherLastCASLength == right.otherLastCASLength && otherLastCASWLength == right.otherLastCASWLength && systemConfig == right.systemConfig);
}

std::ostream& DRAMsimII::operator<<(std::ostream &os, const Rank &r)
{
	os << r.lastRefreshTime << endl;
	os << r.lastPrechargeAnyBankTime << endl;
	os << r.lastCASTime << endl;	
	os << r.lastCASWTime << endl;	
	os << r.prechargeTime << endl;
	os << r.totalPrechargeTime << endl;
	os << r.lastCASLength << endl;	
	os << r.lastCASWLength << endl;	
	os << r.rankID << endl;			
	os << r.lastBankID << endl;		
	os << r.banksPrecharged << endl;	

	return os;
}
