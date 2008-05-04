#include <iostream>
#include "rank_c.h"

using namespace DRAMSimII;
using namespace std;

rank_c::rank_c(const Settings& settings, const TimingSpecification &timingVal):
timing(timingVal),
lastRefreshTime(0),
lastPrechargeTime(0),
lastCASTime(0),
lastCASWTime(0),
prechargeTime(0),
lastCASLength(0),
lastCASWLength(0),
rankID(UINT_MAX),
lastBankID(settings.bankCount - 1),
banksPrecharged(settings.bankCount),
lastRASTimes(4), // make the queue hold four (tFAW)
bank(settings.bankCount,Bank(settings, timingVal))
{}

rank_c::rank_c(const rank_c &r, const TimingSpecification &timingVal):
timing(timingVal),
lastRefreshTime(r.lastRefreshTime),
lastPrechargeTime(0),
lastCASTime(r.lastCASTime),
lastCASWTime(r.lastCASWTime),
prechargeTime(r.prechargeTime),
lastCASLength(r.lastCASLength),
lastCASWLength(r.lastCASWLength),
rankID(r.rankID),
lastBankID(r.lastBankID),
lastRASTimes(r.lastRASTimes),
banksPrecharged(r.banksPrecharged),
bank((unsigned)r.bank.size(), Bank(r.bank[0], timingVal))
{}

void rank_c::issueRAS(const tick currentTime, const Command *currentCommand)
{
	

	// RAS time history queue, per rank
	tick *this_ras_time = lastRASTimes.acquire_item();

	*this_ras_time = currentTime;

	lastRASTimes.push(this_ras_time);
	lastBankID = currentCommand->getAddress().bank;

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
	assert(banksPrecharged > 0);
	assert(banksPrecharged < bank.size());
	// update the bank to reflect this change also
	Bank &currentBank = bank[currentCommand->getAddress().bank];
	currentBank.issueRAS(currentTime, currentCommand);
}

void rank_c::issuePRE(const tick currentTime, const Command *currentCommand)
{
	// update the bank to reflect this change also
	Bank &currentBank = bank[currentCommand->getAddress().bank];
	currentBank.issuePRE(currentTime, currentCommand);

	switch (currentCommand->getCommandType())
	{
	case CAS_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCAS() + timing.tBurst() + timing.tRTP(), currentBank.getLastRASTime() + timing.tRAS());
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR(), currentBank.getLastRASTime() + timing.tRAS());
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

void rank_c::issueCAS(const tick currentTime, const Command *currentCommand)
{
	// update the bank to reflect this change also
	bank[currentCommand->getAddress().bank].issueCAS(currentTime, currentCommand);
	
	lastCASTime = currentTime;
	lastCASLength = currentCommand->getLength();

	lastBankID = currentCommand->getAddress().bank;
}

void rank_c::issueCASW(const tick currentTime, const Command *currentCommand)
{
	// update the bank to reflect this change also
	bank[currentCommand->getAddress().bank].issueCASW(currentTime, currentCommand);

	lastCASWTime = currentTime;
	lastCASWLength = currentCommand->getLength();

	lastBankID = currentCommand->getAddress().bank;
}

void rank_c::issueREF(const tick currentTime, const Command *currentCommand)
{
	// FIXME: should this not count as a RAS + PRE command to all banks?
	for (vector<Bank>::iterator currentBank = bank.begin(); currentBank != bank.end(); currentBank++)
		currentBank->issueREF(currentTime, currentCommand);
}