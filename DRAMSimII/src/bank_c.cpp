#include "bank_c.h"

using namespace DRAMSimII;
using namespace std;

bank_c::bank_c(const dramSettings& settings, const dramTimingSpecification &timingVal):
timing(timingVal),
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

bank_c::bank_c(const bank_c &b, const dramTimingSpecification &timingVal):
timing(timingVal),
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
void bank_c::issueRAS(const tick_t currentTime, const command *currentCommand)
{
	assert(activated == false);
	activated = true;
	lastRASTime = currentTime;
	openRowID = currentCommand->getAddress().row;
	RASCount++;
}

void bank_c::issuePRE(const tick_t currentTime, const command *currentCommand)
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
	assert(activated == true);
	activated = false;
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