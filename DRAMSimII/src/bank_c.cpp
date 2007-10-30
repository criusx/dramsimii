#include "bank_c.h"

using namespace DRAMSimII;
using namespace std;

const dramTimingSpecification* bank_c::timing;

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

void bank_c::issueRAS(const tick_t currentTime, const command *currentCommand)
{
	assert(isActivated == false);
	isActivated = true;
	lastRASTime = currentTime;
	openRowID = currentCommand->getAddress().row;
	RASCount++;
}

void bank_c::issuePRE(const tick_t currentTime, const command *currentCommand)
{
	switch (currentCommand->getCommandType())
	{
	case CAS_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing->tAL() + timing->tCAS() + timing->tBurst() + timing->tRTP(), lastRASTime + timing->tRAS());
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		lastPrechargeTime = max(currentTime + timing->tAL() + timing->tCWD() + timing->tBurst() + timing->tWR(), lastRASTime + timing->tRAS());
		break;
	case PRECHARGE_COMMAND:
		lastPrechargeTime = currentTime;
		break;
	default:
		cerr << "Unhandled CAS variant" << endl;
		break;
	}
	assert(isActivated == true);
	isActivated = false;
}

void bank_c::issueCAS(const tick_t currentTime, const command *currentCommand)
{
	lastCASTime = currentTime;
	lastCASLength = currentCommand->getLength();
	CASCount++;
}

void bank_c::issueCASW(const tick_t currentTime, const command *currentCommand)
{
	lastCASWTime = currentTime;
	lastCASWLength = currentCommand->getLength();
	CASWCount++;
}

void bank_c::issueREF(const tick_t currentTime, const command *currentCommand)
{

}