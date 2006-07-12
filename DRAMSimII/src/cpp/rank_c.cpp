#include "dramsim2.h"

rank_c::rank_c():
last_ras_times(4)
{
	last_refresh_time = 0;
	last_cas_time = -100;
	last_casw_time = -100;
}

void rank_c::init_ranks(int bank_count, int per_bank_queue_depth)
{
	this->bank_count = bank_count;
	last_bank_id = bank_count-1;
	bank = new bank_c[bank_count];
	for (int i=0;i<bank_count;i++)
		bank[i].init_banks(per_bank_queue_depth);
}

rank_c::~rank_c()
{
	delete[] bank;
}