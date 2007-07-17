#include "bank_c.h"


bank_c::bank_c(const dramSettings *settings):
per_bank_q(settings->perBankQueueDepth),
last_ras_time(-100),
last_cas_time(-100),
last_casw_time(-100),
last_prec_time(-100),
last_refresh_all_time(-100),
last_cas_length(8),
last_casw_length(8),
row_id(0),
isActivated(false),
RASCount(0),
previousRASCount(0),
CASCount(0),
previousCASCount(0),
CASWCount(0),
previousCASWCount(0)
{}

bank_c::bank_c(unsigned per_bank_queue_depth):
per_bank_q(per_bank_queue_depth),
last_ras_time(-100),
last_cas_time(-100),
last_casw_time(-100),
last_prec_time(-100),
last_refresh_all_time(-100),
last_cas_length(8),
last_casw_length(8),
row_id(0),
isActivated(false),
RASCount(0),
previousRASCount(0),
CASCount(0),
previousCASCount(0),
CASWCount(0),
previousCASWCount(0)
{}

bank_c::bank_c(const bank_c &b):
per_bank_q(b.per_bank_q),
last_ras_time(b.last_ras_time),
last_cas_time(b.last_cas_time),
last_casw_time(b.last_casw_time),
last_prec_time(b.last_prec_time),
last_refresh_all_time(b.last_refresh_all_time),
last_cas_length(b.last_cas_length),
last_casw_length(b.last_casw_length),
row_id(b.row_id),
isActivated(b.isActivated),
RASCount(b.RASCount),
previousRASCount(b.previousRASCount),
CASCount(b.CASCount),
previousCASCount(b.previousCASCount),
CASWCount(b.CASWCount),
previousCASWCount(b.previousCASWCount)
{}
