#include "Address.h"
#include "enumTypes.h"
#include "globals.h"

#include <iostream>
#include <iomanip>

using std::cerr;
using std::endl;
using std::setbase;
using std::setw;
using namespace DRAMSimII;


// static member declaration
unsigned Address::chan_addr_depth;
unsigned Address::rank_addr_depth;
unsigned Address::bank_addr_depth;
unsigned Address::row_addr_depth;
unsigned Address::col_addr_depth;
unsigned Address::col_size_depth;
unsigned Address::cacheLineSize;
AddressMappingScheme Address::mappingScheme;

Address::Address():
virtualAddress(UINT_MAX),
physicalAddress(0),
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
{
	convertAddress();
}

Address::Address(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column):
virtualAddress(0),
physicalAddress(PHYSICAL_ADDRESS_MAX),
channel(channel),
rank(rank),
bank(bank),
row(row),
column(column)
{}

void Address::initialize(const Settings &dramSettings)
{
	chan_addr_depth = log2(dramSettings.channelCount);
	rank_addr_depth = log2(dramSettings.rankCount);
	bank_addr_depth = log2(dramSettings.bankCount);
	row_addr_depth  = log2(dramSettings.rowCount);
	col_addr_depth  = log2(dramSettings.columnCount);
	//FIXME: shouldn't this already be set appropriately?
	col_size_depth	= log2(dramSettings.dramType == DRDRAM ? 16 : dramSettings.columnSize);
	cacheLineSize = dramSettings.cacheLineSize;
	mappingScheme = dramSettings.addressMappingScheme;
}

//////////////////////////////////////////////////////////////////////
/// @brief converts a given memory request from a physical address to a rank/bank/row/column representation
/// @details converts according to the address mapping scheme in systemConfig
/// @author Joe Gross
/// @param thisAddress the addresses representation to be decoded in-place
/// @return true if the conversion was successful, false if there was some problem
//////////////////////////////////////////////////////////////////////
bool Address::convertAddress()
{
	unsigned temp_a, temp_b;
	unsigned bit_15,bit_27,bits_26_to_16;

	// if there's a test involving specific ranks/banks and the mapping is predetermined
	//if (inputStream.getType() == MAPPED)
	//	return true;
	if (!physicalAddress)
		return false;

	// strip away the byte address portion
	unsigned input_a = physicalAddress >> col_size_depth;


	//unsigned cacheline_size;
	unsigned cacheline_size_depth;	/* address bit depth */
	unsigned col_id_lo;
	unsigned col_id_lo_depth;
	unsigned col_id_hi;
	unsigned col_id_hi_depth;

	switch (mappingScheme)
	{
	case BURGER_BASE_MAP:		/* Good for only Rambus memory really */

		// BURGER BASE :
		// |<-------------------------------->|<------>|<------>|<---------------->|<----------------->|<----------->|
		//                          row id     bank id   Rank id   Column id         Channel id          Byte Address
		//                                                         DRAM page size/   intlog2(chan. count)   within packet
		//                                                         Bus Width         used if chan. > 1
		//
		//               As applied to system (1 chan) using 256 Mbit RDRAM chips:
		//               512 rows X 32 banks X 128 columns X 16 bytes per column.
		//		 16 ranks gets us to 512 MByte.	
		//
		//    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		//             |<---------------------->| |<---------->| |<------->| |<---------------->|  |<------>|
		//                      row id                 bank         rank          Col id            16 byte
		//                      (512 rows)              id           id           2KB/16B            packet


		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		channel = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_addr_depth;
		temp_a  = input_a << col_addr_depth;
		column = temp_a ^ temp_b;     		/* strip out the column address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		rank = temp_a ^ temp_b;		/* this should strip out the rank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		bank = temp_a ^ temp_b;		/* this should strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		row = temp_a ^ temp_b;		/* this should strip out the row address */
		if (input_a != 0)				/* If there is still "stuff" left, the input address is out of range */
		{
			cerr << "Address out of range[" << std::hex << physicalAddress << "] of available physical memory" << endl;
		}
		break;

	case SDRAM_HIPERF_MAP:
	case OPEN_PAGE_BASELINE:	/* works for SDRAM, DDR, DDR2 and DDR3 ! */

		/*
		*               High performance SDRAM Mapping scheme
		*                                                                    5
		* |<-------------------->| |<->| |<->|  |<--------------->| |<---->| |<---------------->|  |<------------------->|
		*                row id    rank  bank       col_id(high)     chan_id   col_id(low)        	column size
		*                							intlog2(cacheline_size)	intlog2(channel_width)
		*									- intlog2(channel_width)	
		*  Rationale is as follows: From LSB to MSB
		*  min column size is the channel width, and individual byes within that "unit" is not addressable, so strip it out and throw it away.
		*  Then strip out a few bits of phys_addr address for the low order bits of col_id.  We basically want consecutive cachelines to
		*  map to different channels.
		*  Then strip out the bits for channel id.
		*  Then strip out the bits for the high order bits of the column id.
		*  Then strip out the bank_id.
		*  Then strip out the rank_id.
		*  What remains must be the row_id
		*
		*  As applied to system (1 dram channel, 64 bit wide. 4 ranks of 256 Mbit chips, each x16. 512 MB system)
		*
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*             |<------------------------------->| |<->| |<->|  |<------------------------>|  |<--->|
		*                       row id                    rank  bank       Column id                  (8B wide)
		*                                                  id    id        2KB * 4 / 8B               Byte Addr
		*
		*  As applied to system (2 dram channel, 64 bit wide each. 1 GB system)
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*          |<------------------------------->| |<->| |<->| |<---------------->|  ^  |<--->|  |<--->|
		*                    row id                    rank  bank      Column id high   chan col_id  (8B wide)
		*                                               id    id        2KB * 4 / 8B     id    low    Byte Addr
		*
		*  As applied to system (1 dram channel, 128 bit wide. 1 GB system)
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*          |<------------------------------->| |<->| |<->| |<------------------------->|  |<------>|
		*                       row id                  rank  bank       Column id                (16B wide)
		*                                               id    id        2KB * 4 / 8B               Byte Addr
		*
		*  As applied to system (2 dram channel, 128 bit wide each. 2 GB system)
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*       |<------------------------------->| |<->| |<->| |<------------------->|  ^  |<>|  |<------>|
		*                    row id                 rank  bank      Column id high     chan  col  (16B wide)
		*                                            id    id        2KB * 4 / 8B       id   idlo  Byte Addr
		*
		*/

		//

		//cacheline_size = systemConfig.getCachelineSize();
		cacheline_size_depth = log2(cacheLineSize);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_lo_depth;
		temp_a  = input_a << col_id_lo_depth;
		col_id_lo = temp_a ^ temp_b;     		/* strip out the column low address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		channel = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_hi_depth;
		temp_a  = input_a << col_id_hi_depth;
		col_id_hi = temp_a ^ temp_b;     		/* strip out the column hi address */

		column = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		bank = temp_a ^ temp_b;     		/* strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		rank = temp_a ^ temp_b;     		/* strip out the rank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		row = temp_a ^ temp_b;		/* this should strip out the row address */
		if (input_a != 0) // If there is still "stuff" left, the input address is out of range
		{
			cerr << "address out of range[" << physicalAddress << "] of available physical memory" << endl << this << endl;
			//print_addresses(this_a);
		}
		break;

	case SDRAM_BASE_MAP:		
		/* works for SDRAM and DDR SDRAM too! */

		/*
		*               Basic SDRAM Mapping scheme (As found on user-upgradable memory systems)
		*                                                                    5
		* |<---->| |<------------------->| |<->|  |<--------------->| |<---->| |<---------------->|  |<------------------->|
		*   rank             row id         bank       col_id(high)   chan_id   col_id(low)        	column size
		*                							intlog2(cacheline_size)	intlog2(channel_width)
		*									- intlog2(channel_width)	
		*  Rationale is as follows: From LSB to MSB
		*  min column size is the channel width, and individual byes within that "unit" is not addressable, so strip it out and throw it away.
		*  Then strip out a few bits of phys_addr address for the low order bits of col_id.  We basically want consecutive cachelines to
		*  map to different channels.
		*  Then strip out the bits for channel id.
		*  Then strip out the bits for the high order bits of the column id.
		*  Then strip out the bank_id.
		*  Then strip out the row_id
		*  What remains must be the rankid
		*
		*  As applied to system (2 dram channel, 64 bit wide each. 1 GB system)
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*          |<->| |<------------------------------->| |<->| |<---------------->|  ^  |<--->|  |<--->|
		*           rank         row id                       bank     Column id high   chan col_id  (8B wide)
		*           id                                        id       2KB * 4 / 8B     id    low    Byte Addr
		*
		*  As applied to system (1 dram channel, 128 bit wide. 1 GB system)
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*          |<->| |<------------------------------->| |<->| |<------------------------->|  |<------>|
		*           rank         row id                       bank     Column id                   (16B wide)
		*           id                                        id       2KB * 4 / 8B     id    low   Byte Addr
		*
		*/



		//cacheline_size = systemConfig.getCachelineSize();
		cacheline_size_depth = log2(cacheLineSize);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_lo_depth;
		temp_a  = input_a << col_id_lo_depth;
		col_id_lo = temp_a ^ temp_b;     		/* strip out the column low address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		channel = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_hi_depth;
		temp_a  = input_a << col_id_hi_depth;
		col_id_hi = temp_a ^ temp_b;     		/* strip out the column hi address */

		column = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		bank = temp_a ^ temp_b;     		/* strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		row = temp_a ^ temp_b;		/* this should strip out the row address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		rank = temp_a ^ temp_b;     		/* strip out the rank address */

		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			cerr << "address out of range[" << physicalAddress << "] of available physical memory" << endl;
		}
		break;

	case INTEL845G_MAP:

		/*  Data comes from Intel's 845G Datasheets.  Table 5-5
		*  DDR SDRAM mapping only.
		*
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*          |<->| |<---------------------------------->| |<->| |<------------------------->|  |<--->|
		*          rank               row id                    bank             Column id           (64 bit wide bus)
		*           id                                           id             2KB * 4 / 8B          Byte addr
		*     row id goes like this: addr[27:15:26-16]
		*     rank_id is addr[29:28]  This means that no device switching unless memory usage goes above 256MB grainularity
		*     No need to remap address with variable number of ranks.  Address just goes up to rank id, if there is more than XXX MB of memory.
		*     Draw back to this scheme is that we're not effectively using banks.
		*/
		input_a = physicalAddress >> 3;	/* strip away byte address */

		temp_b = input_a;				/* save away what is left of original address */
		input_a = input_a >> 10;
		temp_a  = input_a << 10;				/* 11-3 */
		column = temp_a ^ temp_b;     		/* strip out the column address */

		temp_b = input_a;				/* save away what is left of original address */
		input_a = input_a >> 2;
		temp_a  = input_a << 2;				/* 14:13 */
		bank = temp_a ^ temp_b;		/* strip out the bank address */

		temp_b = physicalAddress >> 15;
		input_a =  temp_b >> 1;
		temp_a  = input_a << 1;				/* 15 */
		bit_15 = temp_a ^ temp_b;			/* strip out bit 15, save for later  */

		temp_b = physicalAddress >> 16;
		input_a =  temp_b >> 11;
		temp_a  = input_a << 11;			/* 26:16 */
		bits_26_to_16 = temp_a ^ temp_b;		/* strip out bits 26:16, save for later  */

		temp_b = physicalAddress >> 27;
		input_a =  temp_b >> 1;
		temp_a  = input_a << 1;				/* 27 */
		bit_27 = temp_a ^ temp_b;			/* strip out bit 27 */

		row = (bit_27 << 13) | (bit_15 << 12) | bits_26_to_16 ;

		temp_b = physicalAddress >> 28;
		input_a = temp_b >> 2;
		temp_a  = input_a << 2;				/* 29:28 */
		rank = temp_a ^ temp_b;		/* strip out the rank id */

		channel = 0;				/* Intel 845G has only a single channel dram controller */
		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			cerr << "address out of range[" << physicalAddress << "] of available physical memory" << endl;
		}
		break;

	case CLOSE_PAGE_BASELINE:
	case SDRAM_CLOSE_PAGE_MAP:
		/*
		*               High performance closed page SDRAM Mapping scheme
		*                                                                    5
		* |<------------------>| |<------------>| |<---->|  |<---->| |<---->| |<----------------->| |<------------------->|
		*                row id    col_id(high)     rank      bank     chan      col_id(low)        	column size
		*                							intlog2(cacheline_size)	intlog2(channel_width)
		*									- intlog2(channel_width)	
		*									
		*  Rationale is as follows: From LSB to MSB
		*  min column size is the channel width, and individual byes within that "unit" is not addressable, so strip it out and throw it away.
		*  Then strip out a few bits of phys_addr address for the low order bits of col_id.  We basically want consecutive cachelines to
		*  map to different channels.
		*  Then strip out the bits for channel id.
		*  Then strip out the bank_id.
		*  Then strip out the rank_id.
		*  Then strip out the bits for the high order bits of the column id.
		*  What remains must be the row_id
		*
		*  As applied to system (1 dram channel, 64 bit wide each. 2 GB system)
		*    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		*       |<------------------------------------->| |<---------------->|  ^  |<--->|  |<--->|  |<--->|
		*                    row id                         Column id high     rank  bank    col_id  (8B wide)
		*                                                   1KB     / 8B        id    id      low    Byte Addr
		*/		

		cacheline_size_depth = log2(cacheLineSize);

		// this is really cacheline_size / col_size
		col_id_lo_depth = cacheline_size_depth - col_size_depth;

		// col_addr / col_id_lo
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a >>= col_id_lo_depth;

		// strip out the column low address
		col_id_lo = temp_b ^ (input_a << col_id_lo_depth); 

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		// strip out the channel address 
		channel = temp_b ^ (input_a << chan_addr_depth); 

		temp_b = input_a;				/* save away original address */
		input_a >>= bank_addr_depth;
		// strip out the bank address 
		bank = temp_b ^ (input_a << bank_addr_depth);

		temp_b = input_a;				/* save away original address */
		input_a >>= rank_addr_depth;
		// strip out the rank address 
		rank = temp_b ^ (input_a << rank_addr_depth);		

		temp_b = input_a;				/* save away original address */
		input_a >>= col_id_hi_depth;
		// strip out the column hi address
		col_id_hi = temp_b ^ (input_a << col_id_hi_depth);

		column = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a >>= row_addr_depth;
		// strip out the row address
		row = temp_b ^ (input_a << row_addr_depth);

		// If there is still "stuff" left, the input address is out of range
		if(input_a != 0)
		{
			cerr << "Mem address out of range[" << std::hex << physicalAddress << "] of available physical memory." << endl;
		}
		break;

	default:
		cerr << "Unknown address mapping scheme, mapping chan, rank, bank to zero: " << mappingScheme << endl;
		channel = 0;				/* don't know what this policy is.. Map everything to 0 */
		rank = 0;
		bank = 0;
		row  = 0;
		column  = 0;
		break;
	}

	return true;
}



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
