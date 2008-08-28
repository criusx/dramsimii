#include <iostream>
#include "Rank.h"

using std::vector;
using std::max;
using std::endl;
using std::cerr;
using namespace DRAMSimII;

Rank::Rank(const Settings& settings, const TimingSpecification &timing, const SystemConfiguration &systemConfig):
timing(timing),
lastRefreshTime(-100),
lastPrechargeTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
prechargeTime(0),
totalPrechargeTime(0),
lastCASLength(0),
lastCASWLength(0),
rankID(UINT_MAX),
lastBankID(settings.bankCount - 1),
banksPrecharged(settings.bankCount),
lastActivateTimes(4, 4, -100), // make the queue hold four (tFAW)
bank(systemConfig.getBankCount(),Bank(settings, timing, systemConfig))
{}

Rank::Rank(const Rank &rhs):
timing(rhs.timing),
lastRefreshTime(rhs.lastRefreshTime),
lastPrechargeTime(rhs.lastPrechargeTime),
lastCASTime(rhs.lastCASTime),
lastCASWTime(rhs.lastCASWTime),
prechargeTime(rhs.prechargeTime),
totalPrechargeTime(rhs.totalPrechargeTime),
lastCASLength(rhs.lastCASLength),
lastCASWLength(rhs.lastCASWLength),
rankID(rhs.rankID),
lastBankID(rhs.lastBankID),
lastActivateTimes(rhs.lastActivateTimes),
banksPrecharged(rhs.banksPrecharged),
bank(rhs.bank)
{}

Rank::Rank(const Rank &rhs, const TimingSpecification &timing, const SystemConfiguration &systemConfig):
timing(timing),
lastRefreshTime(rhs.lastRefreshTime),
lastPrechargeTime(rhs.lastPrechargeTime),
lastCASTime(rhs.lastCASTime),
lastCASWTime(rhs.lastCASWTime),
prechargeTime(rhs.prechargeTime),
totalPrechargeTime(rhs.totalPrechargeTime),
lastCASLength(rhs.lastCASLength),
lastCASWLength(rhs.lastCASWLength),
rankID(rhs.rankID),
lastBankID(rhs.lastBankID),
lastActivateTimes(rhs.lastActivateTimes),
banksPrecharged(rhs.banksPrecharged),
bank((unsigned)systemConfig.getBankCount(), Bank(rhs.bank[0], timing, systemConfig))
{
	// TODO: copy over values in banks now that reference members are init	
	//for (unsigned i = 0; i < systemConfig.getBankCount(); i++)
	//{
	//	bank[i] = rhs.bank[i];
	//}
	bank = rhs.bank;
}

Rank::Rank(const TimingSpecification &timingSpec, const std::vector<Bank> & newBank):
timing(timingSpec),
bank(newBank),
lastActivateTimes(4)
{}

Rank& Rank::operator =(const Rank& rhs)
{
	//::new(this)DRAMSimII::Rank(rhs.timing,rhs.bank);
	lastRefreshTime = rhs.lastRefreshTime;
	lastPrechargeTime = rhs.lastPrechargeTime;
	lastCASTime = rhs.lastCASTime;
	lastCASWTime = rhs.lastCASWTime;
	prechargeTime = rhs.prechargeTime;
	totalPrechargeTime = rhs.totalPrechargeTime;
	lastCASLength = rhs.lastCASLength;
	lastCASWLength = rhs.lastCASWLength;
	rankID = rhs.rankID;
	lastBankID = rhs.lastBankID;
	banksPrecharged = rhs.banksPrecharged;
	//lastActivateTimes = rhs.lastActivateTimes;

	return *this;
}

bool Rank::operator==(const Rank& right) const
{
	return (timing == right.timing && lastRefreshTime == right.lastRefreshTime && lastPrechargeTime == right.lastPrechargeTime &&
		lastCASTime == right.lastCASTime && lastCASWTime == right.lastCASWTime && prechargeTime == right.prechargeTime && totalPrechargeTime == right.totalPrechargeTime &&
		lastCASLength == right.lastCASLength && lastCASWLength == right.lastCASWLength && rankID == right.rankID && lastBankID == right.lastBankID &&
		banksPrecharged == right.banksPrecharged && lastActivateTimes == right.lastActivateTimes && bank == right.bank);
}

std::ostream& DRAMSimII::operator<<(std::ostream &os, const Rank &r)
{
	os << r.lastRefreshTime << endl;
	os << r.lastPrechargeTime << endl;
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

void Rank::issueRAS(const tick currentTime, const Command *currentCommand)
{
	// RAS time history queue, per rank
	const tick thisRASTime = currentTime;

	lastActivateTimes.push_front(thisRASTime);
	lastBankID = currentCommand->getAddress().getBank();

	// for power modeling, if all banks were precharged and now one is being activated, record the interval that one was precharged	
	if (banksPrecharged == bank.size())
	{
		prechargeTime += currentTime - lastPrechargeTime;
		totalPrechargeTime += currentTime - lastPrechargeTime;
	}
	if (banksPrecharged == bank.size())
		for (vector<Bank>::const_iterator curBnk = bank.begin(); curBnk != bank.end(); curBnk++)
			assert(!curBnk->isActivated());
	banksPrecharged--;
	assert(banksPrecharged >= 0 && banksPrecharged < bank.size());
	// update the bank to reflect this change also
	Bank &currentBank = bank[currentCommand->getAddress().getBank()];
	currentBank.issueRAS(currentTime, currentCommand);
}

void Rank::issuePRE(const tick currentTime, const Command *currentCommand)
{
	// update the bank to reflect this change also
	Bank &currentBank = bank[currentCommand->getAddress().getBank()];
	currentBank.issuePRE(currentTime, currentCommand);

	switch (currentCommand->getCommandType())
	{
	case CAS_AND_PRECHARGE_COMMAND:
		//lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCAS() + timing.tBurst() + timing.tRTP(), currentBank.getLastRASTime() + timing.tRAS());
		lastPrechargeTime = max(currentTime + (timing.tAL() - timing.tCCD() + timing.tBurst() + timing.tRTP()), currentBank.getLastRASTime() + timing.tRAS());
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		//lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR(), currentBank.getLastRASTime() + timing.tRAS());
		lastPrechargeTime = max(currentTime + (timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR()), currentBank.getLastRASTime() + timing.tRAS());
		break;
	case PRECHARGE_COMMAND:
		lastPrechargeTime = currentTime;
		break;
	default:
		cerr << "Unhandled CAS variant" << endl;
		break;
	}

	banksPrecharged++;
	assert(banksPrecharged > 0);
	assert(banksPrecharged <= bank.size());
}

void Rank::issueCAS(const tick currentTime, const Command *currentCommand)
{
	// update the bank to reflect this change also
	bank[currentCommand->getAddress().getBank()].issueCAS(currentTime, currentCommand);

	lastCASTime = currentTime;
	lastCASLength = currentCommand->getLength();

	lastBankID = currentCommand->getAddress().getBank();
}

void Rank::issueCASW(const tick currentTime, const Command *currentCommand)
{
	// update the bank to reflect this change also
	bank[currentCommand->getAddress().getBank()].issueCASW(currentTime, currentCommand);

	lastCASWTime = currentTime;
	lastCASWLength = currentCommand->getLength();

	lastBankID = currentCommand->getAddress().getBank();
}

void Rank::issueREF(const tick currentTime, const Command *currentCommand)
{
	// FIXME: should this not count as a RAS + PRE command to all banks?
	for (vector<Bank>::iterator currentBank = bank.begin(); currentBank != bank.end(); currentBank++)
		currentBank->issueREF(currentTime, currentCommand);
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
	if (bank[thisBank].nextCommandType() == REFRESH_ALL_COMMAND)
	{
		if (refreshAllReady())
		{
			Command *tempCommand = NULL;

			for (vector<Bank>::iterator currentBank = bank.begin(); currentBank != bank.end(); currentBank++)
			{
				if (tempCommand)
					delete tempCommand;

				tempCommand = currentBank->pop();

				assert(tempCommand->getCommandType() == REFRESH_ALL_COMMAND);
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
	for (vector<Bank>::const_iterator currentBank = bank.begin(); currentBank != bank.end(); currentBank++)
	{
		// if any queue is empty or the head of any queue isn't a refresh command, then the rank isn't ready for a refresh all command
		if (currentBank->nextCommandType() != REFRESH_ALL_COMMAND)
		{
			return false;
		}
	}
	return true;
}
