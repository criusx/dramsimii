#include "bank_c.h"

using namespace DRAMSimII;

bank_c::bank_c(const dramSettings *settings):
perBankQueue(settings->perBankQueueDepth),
lastRASTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
lastPrechargeTime(-100),
lastRefreshAllTime(-100),
lastCASLength(8),
lastCASWLength(8),
openRowID(0),
isActivated(false),
RASCount(0),
previousRASCount(0),
CASCount(0),
previousCASCount(0),
CASWCount(0),
previousCASWCount(0)
{}

bank_c::bank_c(unsigned per_bank_queue_depth):
perBankQueue(per_bank_queue_depth),
lastRASTime(-100),
lastCASTime(-100),
lastCASWTime(-100),
lastPrechargeTime(-100),
lastRefreshAllTime(-100),
lastCASLength(8),
lastCASWLength(8),
openRowID(0),
isActivated(false),
RASCount(0),
previousRASCount(0),
CASCount(0),
previousCASCount(0),
CASWCount(0),
previousCASWCount(0)
{}

bank_c::bank_c(const bank_c &b):
perBankQueue(b.perBankQueue),
lastRASTime(b.lastRASTime),
lastCASTime(b.lastCASTime),
lastCASWTime(b.lastCASWTime),
lastPrechargeTime(b.lastPrechargeTime),
lastRefreshAllTime(b.lastRefreshAllTime),
lastCASLength(b.lastCASLength),
lastCASWLength(b.lastCASWLength),
openRowID(b.openRowID),
isActivated(b.isActivated),
RASCount(b.RASCount),
previousRASCount(b.previousRASCount),
CASCount(b.CASCount),
previousCASCount(b.previousCASCount),
CASWCount(b.CASWCount),
previousCASWCount(b.previousCASWCount)
{}

bank_c::issueRAS(const tick_t currentTime, const command *currentCommand)
{
	assert(isActivated == false);
	isActivated = true;
	lastRASTime = time;
	openRowID = currentCommand->getAddress().row;
	RASCount++;
}

void bank_c::issuePRE(const tick_t currentTime, const command *currentCommand)
{
	switch (currentCommand->getCommandType())
	{
	case CAS_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(time + t_al + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtp, currentBank.lastRASTime + timing_specification.t_ras);
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(time + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr, currentBank.lastRASTime + timing_specification.t_ras);
		break;
	case PRECHARGE_COMMAND:
		lastPrechargeTime = time;
		break;
	default:
		cerr << "Unhandled precharge variant" << endl;
		break;
	}
	assert(isActivated == true);
	isActivated = false;
}

void bank_c::issueCAS(const tick_t currentTime, const command *currentCommand)
{
	lastCASTime = time;
	lastCASLength = currentCommand->getLength();
	CASCount++;
}

void rank_c::issueCASW(const tick_t currentTime, const command *currentCommand)
{
	lastCASWTime = time;
	lastCASWLength = currentCommand->getLength();
	CASWCount++;
}