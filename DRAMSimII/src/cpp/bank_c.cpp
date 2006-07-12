#include "bank_c.h"

bank_c::bank_c(unsigned per_bank_queue_depth):
per_bank_q(per_bank_queue_depth)
{
	last_ras_time = -100;
	last_cas_time = -100;
	last_casw_time = -100;
	last_prec_time = -100;
	last_refresh_all_time = -100;
	row_id = 0;
	ras_count = 0;
	cas_count = 0;
}

//void bank_c::init_banks(int per_bank_queue_depth)
//{
//	per_bank_q.init(per_bank_queue_depth);
//}