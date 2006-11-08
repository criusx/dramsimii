#include "dramStatistics.h"

dramStatistics::dramStatistics()
{
	//fout.o    = std::cout;
	start_no = 0;
	end_no   = 0;
	start_time = 0;
	end_time = 0;
	valid_transaction_count = 0;
	bo8_count = 0;
	bo4_count = 0;
}

void dramStatistics::collect_transaction_stats(transaction *this_t)
{
	if(this_t->length == 8)
	{
		++bo8_count;
	}
	else
	{
		++bo4_count;
	}
}
