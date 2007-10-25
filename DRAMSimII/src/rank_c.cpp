#include "rank_c.h"

using namespace DRAMSimII;

rank_c::rank_c(const dramSettings *settings):
lastRefreshTime(0),
lastPrechargeTime(0),
lastCASTime(0),
lastCASWTime(0),
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
lastCASLength(r.lastCASLength),
lastCASWLength(r.lastCASWLength),
rankID(r.rankID),
lastBankID(r.lastBankID),
lastRASTimes(r.lastRASTimes),
banksPrecharged(r.banksPrecharged),
bank(r.bank)
{}
