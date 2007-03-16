#include "bank_c.h"

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
ras_count(0),
cas_count(0),
casw_count(0)
{	
}

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
ras_count(b.ras_count),
cas_count(b.cas_count),
casw_count(b.casw_count)
{	
}
