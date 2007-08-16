#include "addresses.h"
#include "enumTypes.h"
#include "globals.h"

#include <iostream>
#include <iomanip>

using namespace DRAMSimII;
using namespace std;

addresses::addresses():
virt_addr(-1),
phys_addr(-1),
chan_id(-1),
rank_id(-1),
bank_id(-1),
row_id(-1),
col_id(-1)
{}

std::ostream &DRAMSimII::operator <<(std::ostream &os, const addresses &this_a)
{
	os << "addr[0x" << setbase(16) << this_a.phys_addr <<
		"] chan[" << setbase(16) << this_a.chan_id << "] rank[" <<
		this_a.rank_id << "] bank[" << setw(2) << setbase(16) << this_a.bank_id <<
		"] row[" << setw(4) << setbase(16) << this_a.row_id << "] col[" <<
		setbase(16) << this_a.col_id << "]";
	return os;
}

std::ostream &DRAMSimII::operator <<(std::ostream &os, const address_mapping_scheme_t &this_ams)
{
	switch (this_ams)
	{
	case BURGER_BASE_MAP:
		os << "Burger Baseline Map";
		break;
	case CLOSE_PAGE_BASELINE:
		os << "Close Page Baseline Map";
		break;
	case INTEL845G_MAP:
		os << "Intel 845G Map";
		break;
	case OPEN_PAGE_BASELINE:
		os << "Open Page Baseline Map";
		break;
	case SDRAM_BASE_MAP:
		os << "SDRAM Baseline Map";
		break;
	case SDRAM_CLOSE_PAGE_MAP:
		os << "SDRAM Close Page Map";
		break;
	case SDRAM_HIPERF_MAP:
		os << "SDRAM High Performance Map";
		break;
	}
	return os;
}

