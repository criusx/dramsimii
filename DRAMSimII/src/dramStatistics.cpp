#include "dramStatistics.h"
#include "globals.h"
#include <iomanip>
#include <iostream>

using namespace DRAMSimII;
using namespace std;

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

ostream &DRAMSimII::operator<<(ostream &os, const dramStatistics &this_a)
{
	os << "RR[" << setw(6) << setprecision(6) << (double)this_a.end_time/max(1,this_a.bo4_count + this_a.bo8_count);
	os << "] BWE[" << setw(6) << setprecision(6) << ((double)this_a.bo8_count * 8.0 + this_a.bo4_count * 4.0) * 100.0 / max(this_a.end_time,(tick_t)1);
	os << "]";
	return os;

}
