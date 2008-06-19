#include "Bank.h"

using namespace DRAMSimII;
using namespace std;

Bank::Bank(const Settings& settings, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal):
timing(timingVal),
systemConfig(systemConfigVal),
perBankQueue(settings.perBankQueueDepth),
lastRASTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
lastPrechargeTime(-100),
lastRefreshAllTime(-100),
lastCASLength(8),
lastCASWLength(8),
openRowID(0),
activated(false),
RASCount(0),
totalRASCount(0),
CASCount(0),
totalCASCount(0),
CASWCount(0),
totalCASWCount(0)
{}

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
activated(false),
RASCount(b.RASCount),
totalRASCount(b.totalRASCount),
CASCount(b.CASCount),
totalCASCount(b.totalCASCount),
CASWCount(b.CASWCount),
totalCASWCount(b.totalCASWCount)
{}

/// this logically issues a RAS command and updates all variables to reflect this
void Bank::issueRAS(const tick currentTime, const Command *currentCommand)
{
	// make sure activates follow precharges
	assert(activated == false);

	activated = true;

	lastRASTime = currentTime;
	openRowID = currentCommand->getAddress().row;
	RASCount++;
}

void Bank::issuePRE(const tick currentTime, const Command *currentCommand)
{
	switch (currentCommand->getCommandType())
	{
	case CAS_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCAS() + timing.tBurst() + timing.tRTP(), lastRASTime + timing.tRAS());
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR(), lastRASTime + timing.tRAS());
		break;
	case PRECHARGE_COMMAND:
		lastPrechargeTime = currentTime;
		break;
	default:
		cerr << "Unhandled CAS variant" << endl;
		break;
	}

	// make sure precharges follow activates
	assert(activated == true);

	activated = false;
}

void Bank::issueCAS(const tick currentTime, const Command *currentCommand)
{
	lastCASTime = currentTime;
	lastCASLength = currentCommand->getLength();
	CASCount++;
}

void Bank::issueCASW(const tick currentTime, const Command *currentCommand)
{
	lastCASWTime = currentTime;
	lastCASWLength = currentCommand->getLength();
	CASWCount++;
}

void Bank::issueREF(const tick currentTime, const Command *currentCommand)
{

}

//////////////////////////////////////////////////////////////////////
/// @brief attempts to insert a transaction as a single CAS command, taking advantage of existing RAS-PRE commands
/// @details goes through the entire per bank queue to find a matching precharge command\n
/// if one is found, then the transaction is converted into the appropriate CAS command and inserted
/// @author Joe Gross
/// @param value the transaction to be inserted
/// @return true if the transaction was converted and inserted successfully, false otherwise
//////////////////////////////////////////////////////////////////////
bool Bank::openPageInsert(DRAMSimII::Transaction *value, tick time)
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
			else if ((currentCommand->getCommandType() == PRECHARGE_COMMAND) && (currentCommand->getAddress().row == value->getAddresses().row)) 
			{
				bool result = perBankQueue.insert(new Command(value, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge()), currentIndex);
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
			else if ((currentCommand->getCommandType() == PRECHARGE_COMMAND) && (currentCommand->getAddress().row == value->getAddresses().row)) 
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
		return false;
	}
	else
	{
		// no place to insert it
		return false;
	}
}