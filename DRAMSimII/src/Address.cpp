#include "Address.h"
#include "enumTypes.h"
#include "globals.h"

#include <iostream>
#include <iomanip>

using namespace DRAMSimII;
using namespace std;

Address::Address():
virtualAddress(UINT_MAX),
physicalAddress(ULLONG_MAX),
channel(UINT_MAX),
rank(UINT_MAX),
bank(UINT_MAX),
row(UINT_MAX),
column(UINT_MAX)
{}

Address::Address(unsigned long long pA):
virtualAddress(0),
physicalAddress(pA),
channel(0),
rank(0),
bank(0),
row(0),
column(0)
{}

std::ostream &DRAMSimII::operator <<(std::ostream &os, const Address &this_a)
{
	os << "addr[0x" << setbase(16) << this_a.physicalAddress <<
		"] chan[" << setbase(16) << this_a.channel << "] rank[" <<
		this_a.rank << "] bank[" << setw(2) << setbase(16) << this_a.bank <<
		"] row[" << setw(4) << setbase(16) << this_a.row << "] col[" <<
		setbase(16) << this_a.column << "]";
	return os;
}

std::ostream &DRAMSimII::operator <<(std::ostream &os, const AddressMappingScheme &this_ams)
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
