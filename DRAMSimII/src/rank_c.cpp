#include <iostream>
#include "rank_c.h"

using namespace DRAMSimII;

rank_c::rank_c(const dramSettings *settings):
lastRefreshTime(0),
lastPrechargeTime(0),
lastCASTime(0),
lastCASWTime(0),
prechargeTime(0),
lastCASLength(0),
lastCASWLength(0),
rankID(UINT_MAX),
lastBankID(settings->bankCount - 1),
banksPrecharged(0),
lastRASTimes(4),
bank(settings->bankCount,bank_c(settings))
{}

rank_c::rank_c(const rank_c &r): 
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

rank_c::issueRAS(const tick_t currentTime, const command *currentCommand)
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
		prechargeTime += time - lastPrechargeTime;
	banksPrecharged--;
	assert(banksPrecharged > 0);
}

void rank_c::issuePRE(const tick_t currentTime, const command *currentCommand)
{
	switch (currentCommand->getCommandType())
	{
	case CAS_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + t_al + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtp, currentBank.lastRASTime + timing_specification.t_ras);
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr, currentBank.lastRASTime + timing_specification.t_ras);
		break;
	case PRECHARGE_COMMAND:
		lastPrechargeTime = time;
		break;
	default:
		cerr << "Unhandled precharge variant" << endl;
		break;
	}
	
	banksPrecharged++;
}

void rank_c::issueCAS(const tick_t currentTime, const command *currentCommand)
{
	lastCASTime = time;
	lastCASLength = currentCommand->getLength();
	lastBankID = currentCommand->getAddress().bank;
}

void rank_c::issueCASW(const tick_t currentTime, const command *currentCommand)
{
	lastCASWTime = time;

	lastCASWLength = currentCommand->getLength();
	lastBankID = currentCommand->getAddress().bank;
}