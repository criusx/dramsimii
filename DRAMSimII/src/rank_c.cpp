#include <iostream>
#include "rank_c.h"

using namespace DRAMSimII;
using namespace std;

rank_c::rank_c(const dramSettings& settings, const dramTimingSpecification &timingVal):
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
banksPrecharged(0),
lastRASTimes(4),
bank(settings.bankCount,bank_c(settings, timingVal))
{}

rank_c::rank_c(const rank_c &r):
timing(r.timing),
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
bank(r.bank)
{}

void rank_c::issueRAS(const tick_t currentTime, const command *currentCommand)
{
	// update the bank to reflect this change also
	bank_c &currentBank = bank[currentCommand->getAddress().bank];
	currentBank.issueRAS(currentTime, currentCommand);

	// RAS time history queue, per rank
	tick_t *this_ras_time = lastRASTimes.acquire_item();

	*this_ras_time = currentTime;

	lastRASTimes.push(this_ras_time);
	lastBankID = currentCommand->getAddress().bank;
	// for power modeling, if all banks were precharged and now one is being activated, record the interval that one was precharged
	if (banksPrecharged > bank.size())
		cerr << "counted too high " << banksPrecharged << "/" << bank.size() << endl;

	if (banksPrecharged >= bank.size())
		prechargeTime += currentTime - lastPrechargeTime;
	banksPrecharged--;
	assert(banksPrecharged > 0);
}

void rank_c::issuePRE(const tick_t currentTime, const command *currentCommand)
{
	// update the bank to reflect this change also
	bank_c &currentBank = bank[currentCommand->getAddress().bank];
	currentBank.issuePRE(currentTime, currentCommand);

	switch (currentCommand->getCommandType())
	{
	case CAS_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCAS() + timing.tBurst() + timing.tRTP(), currentBank.lastRASTime + timing.tRAS());
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing.tAL() + timing.tCWD() + timing.tBurst() + timing.tWR(), currentBank.lastRASTime + timing.tRAS());
		break;
	case PRECHARGE_COMMAND:
		lastPrechargeTime = currentTime;
		break;
	default:
		cerr << "Unhandled CAS variant" << endl;
		break;
	}
	
	banksPrecharged++;
}

void rank_c::issueCAS(const tick_t currentTime, const command *currentCommand)
{
	// update the bank to reflect this change also
	bank_c &currentBank = bank[currentCommand->getAddress().bank];
	currentBank.issueCAS(currentTime, currentCommand);

	lastCASTime = currentTime;
	lastCASLength = currentCommand->getLength();
	lastBankID = currentCommand->getAddress().bank;
}

void rank_c::issueCASW(const tick_t currentTime, const command *currentCommand)
{
	// update the bank to reflect this change also
	bank_c &currentBank = bank[currentCommand->getAddress().bank];
	currentBank.issueCASW(currentTime, currentCommand);

	lastCASWTime = currentTime;

	lastCASWLength = currentCommand->getLength();
	lastBankID = currentCommand->getAddress().bank;
}

void rank_c::issueREF(const tick_t currentTime, const command *currentCommand)
{
	// FIXME: should this not count as a RAS + PRE command to all banks?
	for (vector<bank_c>::iterator currentBank = bank.begin(); currentBank != bank.end(); currentBank++)
		currentBank->issueREF(currentTime, currentCommand);
}