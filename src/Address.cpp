// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
// 
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#include "Address.h"
#include "globals.h"

//#include "enumTypes.h"
#include "Settings.h"
#include "SystemConfiguration.h"

#include <iostream>
#include <iomanip>

using std::cerr;
using std::endl;
using std::setbase;
using std::setw;
using namespace DRAMsimII;

// static member declaration
unsigned Address::channelAddressDepth;
unsigned Address::rankAddressDepth;
unsigned Address::bankAddressDepth;
unsigned Address::rowAddressDepth;
unsigned Address::columnAddressDepth;
unsigned Address::columnSizeDepth;
unsigned Address::cacheLineSize;
Address::AddressMappingScheme Address::mappingScheme;

Address::Address():
virtualAddress(UINT_MAX),
physicalAddress(PHYSICAL_ADDRESS_MAX),
channel(UINT_MAX),
rank(UINT_MAX),
bank(UINT_MAX), 
row(UINT_MAX),
column(UINT_MAX)
{}

Address::Address(PHYSICAL_ADDRESS pA):
virtualAddress(0),
physicalAddress(pA),
channel(0),
rank(0),
bank(0),
row(0),
column(0)
{
	addressTranslation();
}

Address::Address(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column):
virtualAddress(0),
physicalAddress(0x00),
channel(channel),
rank(rank),
bank(bank),
row(row),
column(column)
{
	reverseAddressTranslation();
}

void Address::initialize(const Settings &dramSettings)
{
	channelAddressDepth = log2(dramSettings.channelCount);
	rankAddressDepth = log2(dramSettings.rankCount);
	bankAddressDepth = log2(dramSettings.bankCount);
	rowAddressDepth  = log2(dramSettings.rowCount);
	columnAddressDepth  = log2(dramSettings.columnCount);
	//FIXME: shouldn't this already be set appropriately?
	columnSizeDepth	= log2(dramSettings.dramType == DRDRAM ? 16 : dramSettings.columnSize);
	cacheLineSize = dramSettings.cacheLineSize;
	mappingScheme = dramSettings.addressMappingScheme;
}

void Address::initialize(const SystemConfiguration &systemConfig)
{
	channelAddressDepth = log2(systemConfig.getChannelCount());
	rankAddressDepth = log2(systemConfig.getRankCount());
	bankAddressDepth = log2(systemConfig.getBankCount());
	rowAddressDepth  = log2(systemConfig.getRowCount());
	columnAddressDepth  = log2(systemConfig.getColumnCount());
	//FIXME: shouldn't this already be set appropriately?
	columnSizeDepth	= log2(systemConfig.getDRAMType() == DRDRAM ? 16 : systemConfig.getColumnSize());
	cacheLineSize = systemConfig.getCachelineSize();
	mappingScheme = systemConfig.getAddressMappingScheme();
}

bool Address::reverseAddressTranslation()
{
	switch(mappingScheme)
	{
	case BURGER_BASE_MAP:
		break;
	case SDRAM_HIPERF_MAP:
	case OPEN_PAGE_BASELINE:

		break;
	case SDRAM_CLOSE_PAGE_MAP:
	case CLOSE_PAGE_BASELINE:
		{
			unsigned columnLowDepth = log2(cacheLineSize) - columnSizeDepth;
			unsigned columnHighDepth = columnAddressDepth - columnLowDepth;

			PHYSICAL_ADDRESS columnHigh = column >> columnLowDepth;
			PHYSICAL_ADDRESS columnLow = column & (columnHigh << columnLowDepth);

			physicalAddress = 0x00;
			unsigned shiftAmount = columnSizeDepth;
			physicalAddress |= columnLow << shiftAmount;
			shiftAmount += columnLowDepth;
			physicalAddress |= bank << shiftAmount;
			shiftAmount += bankAddressDepth;
			physicalAddress |= rank << shiftAmount;
			shiftAmount += rankAddressDepth;
			physicalAddress |= columnHigh << shiftAmount;
			shiftAmount += columnHighDepth;
			physicalAddress |= row << shiftAmount;

			break;
		}
	default:
		break;
	}

#if 0
#ifdef DEBUG
	PHYSICAL_ADDRESS backup = physicalAddress;
	addressTranslation();
	assert(physicalAddress == backup);
#endif
#endif

	return true;
}


//////////////////////////////////////////////////////////////////////
/// @brief converts a given memory request from a physical address to a rank/bank/row/column representation
/// @details converts according to the address mapping scheme in systemConfig
/// @author Joe Gross
/// @param thisAddress the addresses representation to be decoded in-place
/// @return true if the conversion was successful, false if there was some problem
//////////////////////////////////////////////////////////////////////
bool Address::addressTranslation()
{
	PHYSICAL_ADDRESS bufferA, bufferB;
	unsigned bit_15, bit_27, bits_26_to_16;

	// if there's a test involving specific ranks/banks and the mapping is predetermined
	//if (inputStream.getType() == MAPPED)
	//	return true;
	if (!physicalAddress)
		return false;

	// strip away the byte address portion
	PHYSICAL_ADDRESS tempAddress = physicalAddress >> columnSizeDepth;
	//PHYSICAL_ADDRESS tempAddress = physicalAddress;


	//unsigned cacheline_size;
	unsigned cachelineDepth;	/* address bit depth */
	unsigned columnLow;
	unsigned columnIdDepth;
	unsigned columnHigh;
	unsigned columnIdHighDepth;

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


		bufferB = tempAddress;				
		tempAddress = tempAddress >> channelAddressDepth;
		bufferA  = tempAddress << channelAddressDepth;
		channel = bufferA ^ bufferB;     	

		bufferB = tempAddress;				
		tempAddress = tempAddress >> columnAddressDepth;
		bufferA  = tempAddress << columnAddressDepth;
		column = bufferA ^ bufferB;     	

		bufferB = tempAddress;				
		tempAddress = tempAddress >> rankAddressDepth;
		bufferA  = tempAddress << rankAddressDepth;
		rank = bufferA ^ bufferB;		

		bufferB = tempAddress;			
		tempAddress = tempAddress >> bankAddressDepth;
		bufferA  = tempAddress << bankAddressDepth;
		bank = bufferA ^ bufferB;		

		bufferB = tempAddress;			
		tempAddress = tempAddress >> rowAddressDepth;
		bufferA  = tempAddress << rowAddressDepth;
		row = bufferA ^ bufferB;		

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

		cachelineDepth = log2(cacheLineSize);

		columnIdDepth = cachelineDepth - columnSizeDepth;
		columnIdHighDepth = columnAddressDepth - columnIdDepth;

		bufferB = tempAddress;				
		tempAddress = tempAddress >> columnIdDepth;
		bufferA  = tempAddress << columnIdDepth;
		columnLow = bufferA ^ bufferB;     	

		bufferB = tempAddress;				
		tempAddress = tempAddress >> channelAddressDepth;
		bufferA  = tempAddress << channelAddressDepth;
		channel = bufferA ^ bufferB;     	

		bufferB = tempAddress;				
		tempAddress = tempAddress >> columnIdHighDepth;
		bufferA  = tempAddress << columnIdHighDepth;
		columnHigh = bufferA ^ bufferB;     

		column = (columnHigh << columnIdDepth) | columnLow;

		bufferB = tempAddress;			
		tempAddress = tempAddress >> bankAddressDepth;
		bufferA  = tempAddress << bankAddressDepth;
		bank = bufferA ^ bufferB;     	

		bufferB = tempAddress;			
		tempAddress = tempAddress >> rankAddressDepth;
		bufferA  = tempAddress << rankAddressDepth;
		rank = bufferA ^ bufferB;     	

		bufferB = tempAddress;			
		tempAddress = tempAddress >> rowAddressDepth;
		bufferA  = tempAddress << rowAddressDepth;
		row = bufferA ^ bufferB;	

		break;

	case SDRAM_BASE_MAP:		
		/* works for SDRAM and DDR SDRAM too! */

		/*
		*               Basic SDRAM Mapping scheme (As found on user-upgradeable memory systems)
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
		cachelineDepth = log2(cacheLineSize);

		columnIdDepth = cachelineDepth - columnSizeDepth;
		columnIdHighDepth = columnAddressDepth - columnIdDepth;

		bufferB = tempAddress;				/* save away original address */
		tempAddress = tempAddress >> columnIdDepth;
		bufferA  = tempAddress << columnIdDepth;
		columnLow = bufferA ^ bufferB;     		/* strip out the column low address */

		bufferB = tempAddress;				/* save away original address */
		tempAddress = tempAddress >> channelAddressDepth;
		bufferA  = tempAddress << channelAddressDepth;
		channel = bufferA ^ bufferB;     		/* strip out the channel address */

		bufferB = tempAddress;				/* save away original address */
		tempAddress = tempAddress >> columnIdHighDepth;
		bufferA  = tempAddress << columnIdHighDepth;
		columnHigh = bufferA ^ bufferB;     		/* strip out the column hi address */

		column = (columnHigh << columnIdDepth) | columnLow;

		bufferB = tempAddress;				/* save away original address */
		tempAddress = tempAddress >> bankAddressDepth;
		bufferA  = tempAddress << bankAddressDepth;
		bank = bufferA ^ bufferB;     		/* strip out the bank address */

		bufferB = tempAddress;				/* save away original address */
		tempAddress = tempAddress >> rowAddressDepth;
		bufferA  = tempAddress << rowAddressDepth;
		row = bufferA ^ bufferB;		/* this should strip out the row address */

		bufferB = tempAddress;				/* save away original address */
		tempAddress = tempAddress >> rankAddressDepth;
		bufferA  = tempAddress << rankAddressDepth;
		rank = bufferA ^ bufferB;     		/* strip out the rank address */

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
		tempAddress = physicalAddress >> 3;	/* strip away byte address */

		bufferB = tempAddress;				/* save away what is left of original address */
		tempAddress = tempAddress >> 10;
		bufferA  = tempAddress << 10;				/* 11-3 */
		column = bufferA ^ bufferB;     		/* strip out the column address */

		bufferB = tempAddress;				/* save away what is left of original address */
		tempAddress = tempAddress >> 2;
		bufferA  = tempAddress << 2;				/* 14:13 */
		bank = bufferA ^ bufferB;		/* strip out the bank address */

		bufferB = physicalAddress >> 15;
		tempAddress =  bufferB >> 1;
		bufferA  = tempAddress << 1;				/* 15 */
		bit_15 = bufferA ^ bufferB;			/* strip out bit 15, save for later  */

		bufferB = physicalAddress >> 16;
		tempAddress =  bufferB >> 11;
		bufferA  = tempAddress << 11;			/* 26:16 */
		bits_26_to_16 = bufferA ^ bufferB;		/* strip out bits 26:16, save for later  */

		bufferB = physicalAddress >> 27;
		tempAddress =  bufferB >> 1;
		bufferA  = tempAddress << 1;				/* 27 */
		bit_27 = bufferA ^ bufferB;			/* strip out bit 27 */

		row = (bit_27 << 13) | (bit_15 << 12) | bits_26_to_16 ;

		bufferB = physicalAddress >> 28;
		tempAddress = bufferB >> 2;
		bufferA  = tempAddress << 2;				/* 29:28 */
		rank = bufferA ^ bufferB;		/* strip out the rank id */

		channel = 0;				/* Intel 845G has only a single channel dram controller */

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

		cachelineDepth = log2(cacheLineSize);

		// this is really cacheline_size / col_size
		columnIdDepth = cachelineDepth - columnSizeDepth;

		// col_addr / columnLow
		columnIdHighDepth = columnAddressDepth - columnIdDepth;

		bufferB = tempAddress;				/* save away original address */
		tempAddress >>= columnIdDepth;

		// strip out the column low address
		columnLow = bufferB ^ (tempAddress << columnIdDepth); 

		bufferB = tempAddress;				/* save away original address */
		tempAddress >>= channelAddressDepth;
		// strip out the channel address 
		channel = bufferB ^ (tempAddress << channelAddressDepth); 

		bufferB = tempAddress;				/* save away original address */
		tempAddress >>= bankAddressDepth;
		// strip out the bank address 
		bank = bufferB ^ (tempAddress << bankAddressDepth);

		bufferB = tempAddress;				/* save away original address */
		tempAddress >>= rankAddressDepth;
		// strip out the rank address 
		rank = bufferB ^ (tempAddress << rankAddressDepth);		

		bufferB = tempAddress;				/* save away original address */
		tempAddress >>= columnIdHighDepth;
		// strip out the column hi address
		columnHigh = bufferB ^ (tempAddress << columnIdHighDepth);

		column = (columnHigh << columnIdDepth) | columnLow;

		bufferB = tempAddress;				/* save away original address */
		tempAddress >>= rowAddressDepth;
		// strip out the row address
		row = bufferB ^ (tempAddress << rowAddressDepth);

		break;

	case CLOSE_PAGE_LOW_LOCALITY:
		/*
		*               High performance closed page SDRAM Mapping scheme for streams with low locality
		*                                                                    5
		* |<------------------>| |<------------>| |<---------->|  |<----->| |<----->| |<----------------->| |<------------------->|
		*       col_id(high)          row           col_id(low)      bank      rank            chan              column size
		*                							intlog2(cacheline_size)	intlog2(channel_width)
		*									- intlog2(channel_width)	
		*/

		cachelineDepth = log2(cacheLineSize);

		// this is really cacheline_size / col_size
		columnIdDepth = cachelineDepth - columnSizeDepth;

		// col_addr / columnLow
		columnIdHighDepth = columnAddressDepth - columnIdDepth;

		bufferB = tempAddress;				
		tempAddress >>= channelAddressDepth;
		// strip out the channel address 
		channel = bufferB ^ (tempAddress << channelAddressDepth);

		bufferB = tempAddress;				
		tempAddress >>= rankAddressDepth;
		// strip out the rank address 
		rank = bufferB ^ (tempAddress << rankAddressDepth);	

		bufferB = tempAddress;				
		tempAddress >>= bankAddressDepth;
		// strip out the bank address 
		bank = bufferB ^ (tempAddress << bankAddressDepth);

		bufferB = tempAddress;				
		tempAddress >>= columnIdDepth;
		// strip out the column low address
		columnLow = bufferB ^ (tempAddress << columnIdDepth); 

		bufferB = tempAddress;				
		tempAddress >>= rowAddressDepth;
		// strip out the row address
		row = bufferB ^ (tempAddress << rowAddressDepth);

		bufferB = tempAddress;				
		tempAddress >>= columnIdHighDepth;
		// strip out the column hi address
		columnHigh = bufferB ^ (tempAddress << columnIdHighDepth);

		column = (columnHigh << columnIdDepth) | columnLow;

		break;

	case CLOSE_PAGE_HIGH_LOCALITY:
		/*
		*               High performance closed page SDRAM Mapping scheme for streams with low locality
		*                                                                    5
		* |<------->| |<------->| |<----->|  |<-------------->| |<----->| |<--------------->| |<----------------->|
		rank        bank       chan       col_id(high)       row        col_id(low)          column size
		*                							intlog2(cacheline_size)	intlog2(channel_width)
		*									- intlog2(channel_width)	
		*/

		cachelineDepth = log2(cacheLineSize);

		// this is really cacheline_size / col_size
		columnIdDepth = cachelineDepth - columnSizeDepth;

		// col_addr / columnLow
		columnIdHighDepth = columnAddressDepth - columnIdDepth;

		bufferB = tempAddress;				
		tempAddress >>= columnIdDepth;
		// strip out the column low address
		columnLow = bufferB ^ (tempAddress << columnIdDepth); 

		bufferB = tempAddress;				
		tempAddress >>= rowAddressDepth;
		// strip out the row address
		row = bufferB ^ (tempAddress << rowAddressDepth);

		bufferB = tempAddress;				
		tempAddress >>= columnIdHighDepth;
		// strip out the column hi address
		columnHigh = bufferB ^ (tempAddress << columnIdHighDepth);

		bufferB = tempAddress;				
		tempAddress >>= channelAddressDepth;
		// strip out the channel address 
		channel = bufferB ^ (tempAddress << channelAddressDepth);

		bufferB = tempAddress;				
		tempAddress >>= bankAddressDepth;
		// strip out the bank address 
		bank = bufferB ^ (tempAddress << bankAddressDepth);

		bufferB = tempAddress;				
		tempAddress >>= rankAddressDepth;
		// strip out the rank address 
		rank = bufferB ^ (tempAddress << rankAddressDepth);	

		column = (columnHigh << columnIdDepth) | columnLow;	

		break;

		// don't know what this policy is.. Map everything to 0 
	default:
		cerr << "Unknown address mapping scheme, mapping chan, rank, bank to zero: ";
		cerr << mappingScheme;
		cerr << endl;
		channel = rank = bank = row = column = 0;				
		return false;
		break;
	}

	// If there is still "stuff" left, the input address is out of range
	if (tempAddress)
	{
		cerr << "Memory address out of range[" << std::hex << physicalAddress << "] of available physical memory." << endl;
		return false;
	}

	return true;
}

void Address::setAddress(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column)
{
	this->channel = channel;
	this->rank = rank;
	this->bank = bank;
	this->column = column;
	this->row = row;

	reverseAddressTranslation();
}

std::ostream &DRAMsimII::operator <<(std::ostream &os, const Address& thisAddress)
{
	return os << "addr[0x" << setbase(16) << thisAddress.physicalAddress <<
		"] chan[" << setbase(16) << thisAddress.channel << "] rank[" <<
		thisAddress.rank << "] bank[" << setbase(16) << thisAddress.bank <<
		"] row[" << setbase(16) << thisAddress.row << "] col[" <<
		setbase(16) << thisAddress.column << "]";
}

std::ostream &DRAMsimII::operator <<(std::ostream &os, const Address::AddressMappingScheme &mappingScheme)
{	
	switch (mappingScheme)
	{
	case Address::BURGER_BASE_MAP:
		os << "BBM";
		break;
	case Address::SDRAM_HIPERF_MAP:
	case Address::OPEN_PAGE_BASELINE:
		os << "OPBAS";
		break;
	case Address::SDRAM_BASE_MAP:
		os << "SDBAS";
		break;
	case Address::CLOSE_PAGE_BASELINE:
	case Address::SDRAM_CLOSE_PAGE_MAP:
		os << "CPBAS";
		break;
	case Address::INTEL845G_MAP:
		os << "845G";
		break;
	case Address::CLOSE_PAGE_LOW_LOCALITY:
		os << "LOLOC";
		break;
	case Address::CLOSE_PAGE_HIGH_LOCALITY:
		os << "HILOC";
		break;
	default:
		os << "UNKWN";
		break;
	}
	return os;
}

// overloads
bool Address::operator==(const Address& right) const
{
	return (virtualAddress == right.virtualAddress && physicalAddress == right.physicalAddress && channel == right.channel &&
		rank == right.rank && bank == right.bank && row == right.row && column == right.column);

}
