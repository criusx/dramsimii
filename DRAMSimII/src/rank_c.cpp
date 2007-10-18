#include "rank_c.h"

using namespace DRAMSimII;

rank_c::rank_c(const dramSettings *settings):
last_refresh_time(0),
last_prec_time(0),
last_cas_time(-100),
last_casw_time(-100),
last_cas_length(0),
last_casw_length(0),
rankID(-1),
last_bank_id(settings->bankCount - 1),
last_ras_times(4),
bank(settings->bankCount,bank_c(settings))
{}

//rank_c::rank_c():
//last_bank_id(0),
//last_refresh_time(0),
//last_prec_time(0),
//last_cas_time(0),
//last_casw_time(0),
//last_cas_length(0),
//last_casw_length(0),
//last_ras_times(4),
//rankID(-1)
//{}

//rank_c::rank_c(const unsigned bankCount, const unsigned per_bank_queue_depth):
//last_bank_id(bankCount-1),
//last_refresh_time(0),
//last_prec_time(0),
//last_cas_time(-100),
//last_casw_time(-100),
//last_cas_length(0),
//last_casw_length(0),
//last_ras_times(4),
//bank(bankCount,bank_c(per_bank_queue_depth))
//{}

rank_c::rank_c(const rank_c &r): 
last_refresh_time(r.last_refresh_time),
last_prec_time(0),
last_cas_time(r.last_cas_time),
last_casw_time(r.last_casw_time),
last_cas_length(r.last_cas_length),
last_casw_length(r.last_casw_length),
rankID(r.rankID),
last_bank_id(r.last_bank_id),
last_ras_times(r.last_ras_times),
bank(r.bank)
{}
