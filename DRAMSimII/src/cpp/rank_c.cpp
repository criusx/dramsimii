#include "rank_c.h"

rank_c::rank_c():
last_ras_times(4)
{
	last_refresh_time = 0;
	last_cas_time = -100;
	last_casw_time = -100;	
}

rank_c::rank_c(const int bank_cnt, const int per_bank_queue_depth):
last_ras_times(4),
last_refresh_time(0),
last_cas_time(-100),
last_casw_time(-100),
bank_count(bank_cnt),
last_bank_id(bank_count-1),
bank(bank_cnt,bank_c(per_bank_queue_depth))
{
}

void rank_c::init_ranks(int bank_cnt, int per_bank_queue_depth)
{
	this->bank_count = bank_count;
	last_bank_id = bank_count-1;
	//bank = new bank_c[bank_count];
	for (int i=0;i<bank_count;i++)
		bank.push_back(bank_c(per_bank_queue_depth));
	//	bank[i].init_banks(per_bank_queue_depth);
}

rank_c::rank_c(const rank_c &r): 
bank(r.bank),
last_ras_times(r.last_ras_times),
bank_count(r.bank_count),
last_bank_id(r.last_bank_id),
last_refresh_time(r.last_refresh_time),
last_cas_time(r.last_cas_time),
last_casw_time(r.last_casw_time),
last_cas_length(r.last_cas_length),
last_casw_length(r.last_casw_length)
{
}
