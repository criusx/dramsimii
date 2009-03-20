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

#include "Settings.h"
#include "SystemConfiguration.h"

#include <iostream>
#include <iomanip>

using std::cerr;
using std::endl;
using std::setbase;
using std::setw;
using std::hex;
using namespace DRAMsimII;

// static member declaration
unsigned Address::channelAddressDepth;
unsigned Address::rankAddressDepth;
unsigned Address::bankAddressDepth;
unsigned Address::rowAddressDepth;
unsigned Address::columnAddressDepth;
unsigned Address::columnSizeDepth;
unsigned Address::columnLowAddressDepth;
unsigned Address::columnHighAddressDepth;
unsigned Address::rowLowAddressDepth;
unsigned Address::rowHighAddressDepth;
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

Address::Address(PhysicalAddress pA):
virtualAddress(0),
physicalAddress(pA),
channel(0),
rank(0),
bank(0),
row(0),
column(0)
{
	addressTranslation();

#ifdef DEBUG
	unsigned oldCh = channel, oldRk = rank, oldBk = bank, oldRow = row, oldCol = column;
	reverseAddressTranslation();
	assert(oldCh == channel && oldRk == rank && oldBk == bank && oldRow == row && oldCol == column);
	assert((physicalAddress >> columnSizeDepth) == (pA >> columnSizeDepth));
#endif
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

#ifdef DEBUG
	PhysicalAddress pA = physicalAddress;
	addressTranslation();
	assert((physicalAddress >> columnSizeDepth) == (pA >> columnSizeDepth));
	assert(this->channel == channel && this->rank == rank && this->bank == bank && this->row == row && this->column == column);
#endif
}

PhysicalAddress Address::maxAddress()
{
	return (1LL << (channelAddressDepth + rankAddressDepth + bankAddressDepth + rowAddressDepth +
		columnAddressDepth + columnSizeDepth)) - 1;
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
	mappingScheme = dramSettings.addressMappingScheme;
	unsigned cachelineDepth = log2(dramSettings.cacheLineSize);
	assert(cachelineDepth > columnSizeDepth);
	columnLowAddressDepth = cachelineDepth - columnSizeDepth;
	columnHighAddressDepth = columnAddressDepth - columnLowAddressDepth;
	assert(rowAddressDepth > 3);
	rowLowAddressDepth = 3;
	rowHighAddressDepth = rowAddressDepth - 3;
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
	mappingScheme = systemConfig.getAddressMappingScheme();
	unsigned cachelineDepth = log2(systemConfig.getCachelineSize());
	assert(cachelineDepth > columnSizeDepth);
	columnLowAddressDepth = cachelineDepth - columnSizeDepth;
	columnHighAddressDepth = columnAddressDepth - columnLowAddressDepth;
	assert(rowAddressDepth > 3);
	rowLowAddressDepth = 3;
	rowHighAddressDepth = rowAddressDepth - 3;
}

bool Address::reverseAddressTranslation()
{
	unsigned columnLow = column & ((1 << columnLowAddressDepth) - 1);
	unsigned columnHigh = column >> columnLowAddressDepth;
	unsigned shift = columnSizeDepth;

	switch (mappingScheme)
	{
	case SDRAM_HIPERF_MAP:
	
		physicalAddress = (PhysicalAddress)columnLow << shift;
		shift += columnLowAddressDepth;
		physicalAddress |= (PhysicalAddress)channel << shift;
		shift += channelAddressDepth;
		physicalAddress |= (PhysicalAddress)columnHigh << shift;
		shift += columnHighAddressDepth;
		physicalAddress |= (PhysicalAddress)bank << shift;
		shift += bankAddressDepth;
		physicalAddress |= (PhysicalAddress)rank << shift;
		shift += rankAddressDepth;
		physicalAddress |= (PhysicalAddress)row << shift;

		break;
	case SDRAM_BASE_MAP:

		physicalAddress = (PhysicalAddress)columnLow << shift;
		shift += columnLowAddressDepth;
		physicalAddress |= (PhysicalAddress)channel << shift;
		shift += channelAddressDepth;
		physicalAddress |= (PhysicalAddress)columnHigh << shift;
		shift += columnHighAddressDepth;
		physicalAddress |= (PhysicalAddress)bank << shift;
		shift += bankAddressDepth;
		physicalAddress |= (PhysicalAddress)row << shift;
		shift += rowAddressDepth;
		physicalAddress |= (PhysicalAddress)rank << shift;

		break;
	case CLOSE_PAGE_BASELINE:

		physicalAddress = (PhysicalAddress)columnLow << shift;
		shift += columnLowAddressDepth;
		physicalAddress |= (PhysicalAddress)channel << shift;
		shift += channelAddressDepth;
		physicalAddress |= (PhysicalAddress)bank << shift;
		shift += bankAddressDepth;
		physicalAddress |= (PhysicalAddress)rank << shift;
		shift += rankAddressDepth;
		physicalAddress |= (PhysicalAddress)columnHigh << shift;
		shift += columnHighAddressDepth;
		physicalAddress |= (PhysicalAddress)row << shift;

		break;

	case CLOSE_PAGE_BASELINE_OPT:
		{
			unsigned rowLow = row & 0x07;
			unsigned rowHigh = row >> 3;
			physicalAddress = (PhysicalAddress)columnLow << shift;
			shift += columnLowAddressDepth;
			physicalAddress |= (PhysicalAddress)channel << shift;
			shift += channelAddressDepth;
			physicalAddress |= (PhysicalAddress)bank << shift;
			shift += bankAddressDepth;
			physicalAddress |= (PhysicalAddress)rowLow << shift;
			shift += 3;
			physicalAddress |= (PhysicalAddress)rank << shift;
			shift += rankAddressDepth;
			physicalAddress |= (PhysicalAddress)columnHigh << shift;
			shift += columnHighAddressDepth;
			physicalAddress |= (PhysicalAddress)rowHigh << shift;

			break;
		}
	case CLOSE_PAGE_LOW_LOCALITY:

		physicalAddress = (PhysicalAddress)channel << shift;
		shift += channelAddressDepth;
		physicalAddress |= (PhysicalAddress)rank << shift;
		shift += rankAddressDepth;
		physicalAddress |= (PhysicalAddress)bank << shift;
		shift += bankAddressDepth;
		physicalAddress |= (PhysicalAddress)columnLow << shift;
		shift += columnLowAddressDepth;
		physicalAddress |= (PhysicalAddress)row << shift;
		shift += rowAddressDepth;
		physicalAddress |= (PhysicalAddress)columnHigh << shift;

		break;

	case CLOSE_PAGE_HIGH_LOCALITY:

		physicalAddress = (PhysicalAddress)columnLow << shift;
		shift += columnLowAddressDepth;
		physicalAddress |= (PhysicalAddress)row << shift;
		shift += rowAddressDepth;
		physicalAddress |= (PhysicalAddress)columnHigh << shift;
		shift += columnHighAddressDepth;
		physicalAddress |= (PhysicalAddress)channel << shift;
		shift += channelAddressDepth;
		physicalAddress |= (PhysicalAddress)bank << shift;
		shift += bankAddressDepth;
		physicalAddress |= (PhysicalAddress)rank << shift;

		break;

	case BURGER_BASE_MAP:
		break;

	default:
		break;
	}

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
	PhysicalAddress buffer;

	if (!physicalAddress)
		return false;

	// strip away the byte address portion
	PhysicalAddress tempAddress = physicalAddress >> columnSizeDepth;

	unsigned columnLow;
	unsigned columnHigh;

	switch (mappingScheme)
	{
	case SDRAM_HIPERF_MAP:
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
		buffer = tempAddress;				
		tempAddress >>= columnLowAddressDepth;
		columnLow = tempAddress << columnLowAddressDepth ^ buffer;     	

		buffer = tempAddress;				
		tempAddress >>= channelAddressDepth;
		channel = (tempAddress << channelAddressDepth) ^ buffer;     	

		buffer = tempAddress;				
		tempAddress >>= columnHighAddressDepth;
		columnHigh = (tempAddress << columnHighAddressDepth) ^ buffer;

		column = (columnHigh << columnLowAddressDepth) | columnLow;

		buffer = tempAddress;			
		tempAddress >>= bankAddressDepth;
		bank = (tempAddress << bankAddressDepth) ^ buffer;     	

		buffer = tempAddress;			
		tempAddress >>= rankAddressDepth;
		rank = (tempAddress << rankAddressDepth) ^ buffer;     	

		buffer = tempAddress;			
		tempAddress >>= rowAddressDepth;
		row = (tempAddress << rowAddressDepth) ^ buffer;	

		break;

	case SDRAM_BASE_MAP:		
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
		buffer = tempAddress;
		tempAddress >>= columnLowAddressDepth;
		columnLow = (tempAddress << columnLowAddressDepth) ^ buffer;

		buffer = tempAddress;				
		tempAddress >>= channelAddressDepth;
		channel = (tempAddress << channelAddressDepth) ^ buffer;     	

		buffer = tempAddress;				
		tempAddress >>= columnHighAddressDepth;
		columnHigh = (tempAddress << columnHighAddressDepth) ^ buffer;     		

		column = (columnHigh << columnLowAddressDepth) | columnLow;

		buffer = tempAddress;				
		tempAddress >>= bankAddressDepth;
		bank = (tempAddress << bankAddressDepth) ^ buffer;     		

		buffer = tempAddress;				
		tempAddress >>= rowAddressDepth;
		row = (tempAddress << rowAddressDepth) ^ buffer;		

		buffer = tempAddress;				
		tempAddress >>= rankAddressDepth;
		rank = (tempAddress << rankAddressDepth) ^ buffer;     		

		break;

	case CLOSE_PAGE_BASELINE:
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
		buffer = tempAddress;
		tempAddress >>= columnLowAddressDepth;

		// strip out the column low address
		columnLow = buffer ^ (tempAddress << columnLowAddressDepth);

		buffer = tempAddress;				/* save away original address */
		tempAddress >>= channelAddressDepth;
		// strip out the channel address
		channel = buffer ^ (tempAddress << channelAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= bankAddressDepth;
		// strip out the bank address
		bank = buffer ^ (tempAddress << bankAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= rankAddressDepth;
		// strip out the rank address
		rank = buffer ^ (tempAddress << rankAddressDepth);		

		buffer = tempAddress;				
		tempAddress >>= columnHighAddressDepth;
		// strip out the column hi address
		columnHigh = buffer ^ (tempAddress << columnHighAddressDepth);

		column = (columnHigh << columnLowAddressDepth) | columnLow;

		buffer = tempAddress;				
		tempAddress >>= rowAddressDepth;
		// strip out the row address
		row = buffer ^ (tempAddress << rowAddressDepth);

		break;

	case CLOSE_PAGE_BASELINE_OPT:
		{
			/*
			*               High performance closed page SDRAM Mapping scheme
			*                                                                    5
			* |<----------->||<------------>||<------>||<----->||<---->||<---->||<----------------->||<------------------->|
			*     row_high     col_id(high)     rank     row_lo   bank    chan       col_id(low)           column size
			*
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
			buffer = tempAddress;
			tempAddress >>= columnLowAddressDepth;

			// strip out the column low address
			columnLow = buffer ^ (tempAddress << columnLowAddressDepth);

			buffer = tempAddress;			
			tempAddress >>= channelAddressDepth;
			// strip out the channel address
			channel = buffer ^ (tempAddress << channelAddressDepth);

			buffer = tempAddress;				
			tempAddress >>= bankAddressDepth;
			// strip out the bank address
			bank = buffer ^ (tempAddress << bankAddressDepth);

			buffer = tempAddress;
			tempAddress >>= 3;
			unsigned rowLow = buffer ^ (tempAddress << 3);

			buffer = tempAddress;				
			tempAddress >>= rankAddressDepth;
			// strip out the rank address
			rank = buffer ^ (tempAddress << rankAddressDepth);		

			buffer = tempAddress;				
			tempAddress >>= columnHighAddressDepth;
			// strip out the column hi address
			columnHigh = buffer ^ (tempAddress << columnHighAddressDepth);

			column = (columnHigh << columnLowAddressDepth) | columnLow;

			buffer = tempAddress;				
			tempAddress >>= (rowAddressDepth - 3);
			// strip out the row address
			unsigned rowHigh = buffer ^ (tempAddress << (rowAddressDepth - 3));

			row = (rowHigh << 3) | rowLow;

			break;
		}

	case CLOSE_PAGE_LOW_LOCALITY:
		/*
		*               High performance closed page SDRAM Mapping scheme for streams with low locality
		*                                                                    5
		* |<------------------>| |<------------>| |<---------->|  |<----->| |<----->| |<------>| |<------------------->|
		*       col_id(high)          row           col_id(low)      bank      rank      chan         column size
		*                							intlog2(cacheline_size)	intlog2(channel_width)
		*									- intlog2(channel_width)	
		*/
		buffer = tempAddress;				
		tempAddress >>= channelAddressDepth;
		// strip out the channel address
		channel = buffer ^ (tempAddress << channelAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= rankAddressDepth;
		// strip out the rank address
		rank = buffer ^ (tempAddress << rankAddressDepth);	

		buffer = tempAddress;				
		tempAddress >>= bankAddressDepth;
		// strip out the bank address
		bank = buffer ^ (tempAddress << bankAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= columnLowAddressDepth;
		// strip out the column low address
		columnLow = buffer ^ (tempAddress << columnLowAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= rowAddressDepth;
		// strip out the row address
		row = buffer ^ (tempAddress << rowAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= columnHighAddressDepth;
		// strip out the column hi address
		columnHigh = buffer ^ (tempAddress << columnHighAddressDepth);

		column = (columnHigh << columnLowAddressDepth) | columnLow;

		break;

	case CLOSE_PAGE_HIGH_LOCALITY:
		/*
		*               High performance closed page SDRAM Mapping scheme for streams with low locality
		*                                                                    5
		* |<------->| |<------->| |<----->|  |<-------------->| |<----->| |<--------------->| |<----------------->|
		*    rank        bank       chan         col_id(high)      row        col_id(low)          column size
		*                							intlog2(cacheline_size)	intlog2(channel_width)
		*									- intlog2(channel_width)	
		*/
		buffer = tempAddress;				
		tempAddress >>= columnLowAddressDepth;
		// strip out the column low address
		columnLow = buffer ^ (tempAddress << columnLowAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= rowAddressDepth;
		// strip out the row address
		row = buffer ^ (tempAddress << rowAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= columnHighAddressDepth;
		// strip out the column hi address
		columnHigh = buffer ^ (tempAddress << columnHighAddressDepth);

		column = (columnHigh << columnLowAddressDepth) | columnLow;	

		buffer = tempAddress;				
		tempAddress >>= channelAddressDepth;
		// strip out the channel address
		channel = buffer ^ (tempAddress << channelAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= bankAddressDepth;
		// strip out the bank address
		bank = buffer ^ (tempAddress << bankAddressDepth);

		buffer = tempAddress;				
		tempAddress >>= rankAddressDepth;
		// strip out the rank address
		rank = buffer ^ (tempAddress << rankAddressDepth);	

		break;

	case INTEL845G_MAP:
		{

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
			tempAddress = physicalAddress >> 3;

			buffer = tempAddress;			
			tempAddress >>= 10;
			// 11-3
			column = (tempAddress << 10) ^ buffer;

			buffer = tempAddress;			
			tempAddress >>= 2;
			// 14:13
			bank = (tempAddress << 2) ^ buffer;		

			buffer = physicalAddress >> 15;
			tempAddress =  buffer >> 1;
			// 15
			unsigned bit_15 = (tempAddress << 1) ^ buffer;

			buffer = physicalAddress >> 16;
			tempAddress =  buffer >> 11;
			// 26:16
			unsigned bits_26_to_16 = (tempAddress << 11) ^ buffer;		

			buffer = physicalAddress >> 27;
			tempAddress =  buffer >> 1;
			// 27
			unsigned bit_27 = (tempAddress << 1) ^ buffer;			

			row = (bit_27 << 13) | (bit_15 << 12) | bits_26_to_16;

			buffer = physicalAddress >> 28;
			tempAddress = buffer >> 2;
			// 29:28
			rank = (tempAddress << 2) ^ buffer;

			// Intel 845G has only a single channel dram controller
			channel = 0;				

		}
		break;

	case BURGER_BASE_MAP:		// Good for only Rambus memory really

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
		buffer = tempAddress;				
		tempAddress >>= channelAddressDepth;
		channel = (tempAddress << channelAddressDepth) ^ buffer;     	

		buffer = tempAddress;				
		tempAddress >>= columnAddressDepth;
		column = (tempAddress << columnAddressDepth) ^ buffer;     	

		buffer = tempAddress;				
		tempAddress >>= rankAddressDepth;
		rank = (tempAddress << rankAddressDepth) ^ buffer;		

		buffer = tempAddress;			
		tempAddress >>= bankAddressDepth;
		bank = (tempAddress << bankAddressDepth) ^ buffer;		

		buffer = tempAddress;			
		tempAddress >>= rowAddressDepth;
		row = (tempAddress << rowAddressDepth) ^ buffer;		

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

#ifdef DEBUG
	PhysicalAddress oldPA = physicalAddress;
	addressTranslation();
	assert((physicalAddress >> columnSizeDepth) == (oldPA >> columnSizeDepth));
#endif

}

std::ostream &DRAMsimII::operator <<(std::ostream &os, const Address& thisAddress)
{
	return os << "addr[0x" << hex << thisAddress.physicalAddress <<
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
		os << "SDHIPF";
		break;
	case Address::SDRAM_BASE_MAP:
		os << "SDBAS";
		break;
	case Address::CLOSE_PAGE_BASELINE:
		os << "CPBAS";
		break;
	case Address::CLOSE_PAGE_BASELINE_OPT:
		os << "CPBOPT";
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
	return (virtualAddress == right.virtualAddress && (physicalAddress >> columnSizeDepth) == (right.physicalAddress >> columnSizeDepth) && channel == right.channel &&
		rank == right.rank && bank == right.bank && row == right.row && column == right.column);

}
