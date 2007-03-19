#define _CRT_RAND_S

#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <assert.h>

#include "dramSystem.h"


using namespace std;


// returns the time when the memory system next has an event
// the event may either be a conversion of a transaction into commands
// or it may be the next time a command may be issued
tick_t dramSystem::nextTick() const
{
	tick_t nextWake = INT_MAX;

	for (int j = 0;j != channel.size(); j++)
	{		
		// first look for transactions
		if (transaction *nextTrans = channel[j].read_transaction())
		{
			int tempGap = (nextTrans->enqueueTime - channel[j].get_time()) + timing_specification.t_buffer_delay + 1; // make sure it can finish
			tempGap = max(1,tempGap);
			assert(tempGap <= timing_specification.t_buffer_delay + 1);
			if ((tempGap + channel[j].get_time() < nextWake) && (checkForAvailableCommandSlots(nextTrans)))
			{
				nextWake = tempGap + channel[j].get_time();
			}
		}
		// then check to see when the next command occurs
		command *tempCommand = readNextCommand(j);
		if (tempCommand)
		{
			int tempGap = minProtocolGap(j,tempCommand);
			if (tempGap + channel[j].get_time() < nextWake)
			{
				nextWake = tempGap + channel[j].get_time();
			}
		}
	}

	if (nextWake < INT_MAX)
	{
		//outStream << "min gap is: " << std::dec << gap << endl;
		return nextWake;
	}
	else 
	{
		return 0;
	}
}

int dramSystem::convert_address(addresses &this_a) const
{
	unsigned int input_a;
	unsigned int temp_a, temp_b;
	unsigned int bit_15,bit_27,bits_26_to_16;

	if (input_stream.type == MAPPED)
		return 1;
	//int	mapping_scheme;
	//int	chan_count, rank_count, bank_count, col_count, row_count;
	//int	chan_addr_depth, rank_addr_depth, bank_addr_depth, row_addr_depth, col_addr_depth;
	unsigned col_size;
	//, col_size_depth;

	//mapping_scheme = config->addr_mapping_scheme;

	if(system_config.dram_type == DRDRAM) //FIXME: shouldn't this already be set appropriately?
		col_size = 16;
	else
		col_size = system_config.col_size;

	//chan_count = config->chan_count;
	//rank_count = config->rank_count;
	//bank_count = config->bank_count;
	//row_count  = config->row_count;
	//col_count  = config->col_count;
	unsigned chan_addr_depth = log2(system_config.chan_count);
	unsigned rank_addr_depth = log2(system_config.rank_count);
	unsigned bank_addr_depth = log2(system_config.bank_count);
	unsigned row_addr_depth  = log2(system_config.row_count);
	unsigned col_addr_depth  = log2(system_config.col_count);
	unsigned col_size_depth	= log2(col_size);

	//input_a = this_a->phys_addr;
	// strip away the byte address portion
	input_a = this_a.phys_addr >> col_size_depth;

	unsigned int address = static_cast<unsigned>(this_a.phys_addr);

	unsigned cacheline_size;
	unsigned cacheline_size_depth;	/* address bit depth */
	unsigned col_id_lo;
	unsigned col_id_lo_depth;
	unsigned col_id_hi;
	unsigned col_id_hi_depth;
	
	switch (system_config.addr_mapping_scheme)
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
		this_a.chan_id = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_addr_depth;
		temp_a  = input_a << col_addr_depth;
		this_a.col_id = temp_a ^ temp_b;     		/* strip out the column address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		this_a.rank_id = temp_a ^ temp_b;		/* this should strip out the rank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		this_a.bank_id = temp_a ^ temp_b;		/* this should strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		this_a.row_id = temp_a ^ temp_b;		/* this should strip out the row address */
		if(input_a != 0)				/* If there is still "stuff" left, the input address is out of range */
		{
			cerr << "Address out of range[" << std::hex << this_a.phys_addr << "] of available physical memory" << endl;
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

		cacheline_size = system_config.cacheline_size;
		cacheline_size_depth = log2(cacheline_size);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_lo_depth;
		temp_a  = input_a << col_id_lo_depth;
		col_id_lo = temp_a ^ temp_b;     		/* strip out the column low address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		this_a.chan_id = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_hi_depth;
		temp_a  = input_a << col_id_hi_depth;
		col_id_hi = temp_a ^ temp_b;     		/* strip out the column hi address */

		this_a.col_id = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		this_a.bank_id = temp_a ^ temp_b;     		/* strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		this_a.rank_id = temp_a ^ temp_b;     		/* strip out the rank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		this_a.row_id = temp_a ^ temp_b;		/* this should strip out the row address */
		if (input_a != 0) // If there is still "stuff" left, the input address is out of range
		{
			cerr << "address out of range[" << this_a.phys_addr << "] of available physical memory" << endl << this_a << endl;
			//print_addresses(this_a);
		}
		break;

	case SDRAM_BASE_MAP:		/* works for SDRAM and DDR SDRAM too! */

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

		

		cacheline_size = system_config.cacheline_size;
		cacheline_size_depth = log2(cacheline_size);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_lo_depth;
		temp_a  = input_a << col_id_lo_depth;
		col_id_lo = temp_a ^ temp_b;     		/* strip out the column low address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		this_a.chan_id = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_hi_depth;
		temp_a  = input_a << col_id_hi_depth;
		col_id_hi = temp_a ^ temp_b;     		/* strip out the column hi address */

		this_a.col_id = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		this_a.bank_id = temp_a ^ temp_b;     		/* strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		this_a.row_id = temp_a ^ temp_b;		/* this should strip out the row address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		this_a.rank_id = temp_a ^ temp_b;     		/* strip out the rank address */

		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			cerr << "address out of range[" << this_a.phys_addr << "] of available physical memory" << endl;
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
		input_a = this_a.phys_addr >> 3;	/* strip away byte address */

		temp_b = input_a;				/* save away what is left of original address */
		input_a = input_a >> 10;
		temp_a  = input_a << 10;				/* 11-3 */
		this_a.col_id = temp_a ^ temp_b;     		/* strip out the column address */

		temp_b = input_a;				/* save away what is left of original address */
		input_a = input_a >> 2;
		temp_a  = input_a << 2;				/* 14:13 */
		this_a.bank_id = temp_a ^ temp_b;		/* strip out the bank address */

		temp_b = this_a.phys_addr >> 15;
		input_a =  temp_b >> 1;
		temp_a  = input_a << 1;				/* 15 */
		bit_15 = temp_a ^ temp_b;			/* strip out bit 15, save for later  */

		temp_b = this_a.phys_addr >> 16;
		input_a =  temp_b >> 11;
		temp_a  = input_a << 11;			/* 26:16 */
		bits_26_to_16 = temp_a ^ temp_b;		/* strip out bits 26:16, save for later  */

		temp_b = this_a.phys_addr >> 27;
		input_a =  temp_b >> 1;
		temp_a  = input_a << 1;				/* 27 */
		bit_27 = temp_a ^ temp_b;			/* strip out bit 27 */

		this_a.row_id = (bit_27 << 13) | (bit_15 << 12) | bits_26_to_16 ;

		temp_b = this_a.phys_addr >> 28;
		input_a = temp_b >> 2;
		temp_a  = input_a << 2;				/* 29:28 */
		this_a.rank_id = temp_a ^ temp_b;		/* strip out the rank id */

		this_a.chan_id = 0;				/* Intel 845G has only a single channel dram controller */
		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			cerr << "address out of range[" << this_a.phys_addr << "] of available physical memory" << endl;
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

		cacheline_size_depth = log2(system_config.cacheline_size);

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
		this_a.chan_id = temp_b ^ (input_a << chan_addr_depth); 

		temp_b = input_a;				/* save away original address */
		input_a >>= bank_addr_depth;
		// strip out the bank address 
		this_a.bank_id = temp_b ^ (input_a << bank_addr_depth);

		temp_b = input_a;				/* save away original address */
		input_a >>= rank_addr_depth;
		// strip out the rank address 
		this_a.rank_id = temp_b ^ (input_a << rank_addr_depth);

		temp_b = input_a;				/* save away original address */
		input_a >>= col_id_hi_depth;
		// strip out the column hi address
		col_id_hi = temp_b ^ (input_a << col_id_hi_depth);

		this_a.col_id = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a >>= row_addr_depth;
		// strip out the row address
		this_a.row_id = temp_b ^ (input_a << row_addr_depth);

		// If there is still "stuff" left, the input address is out of range
		if(input_a != 0)
		{
			cerr << "Mem address out of range[" << std::hex << this_a.phys_addr << "] of available physical memory." << endl;
		}
		break;

	default:
		cerr << "Unknown address mapping scheme, mapping chan, rank, bank to zero: " << system_config.addr_mapping_scheme << endl;
		this_a.chan_id = 0;				/* don't know what this policy is.. Map everything to 0 */
		this_a.rank_id = 0;
		this_a.bank_id = 0;
		this_a.row_id  = 0;
		this_a.col_id  = 0;
		break;
	}
	return 1;
}

/// <summary>
/// Updates the system time to be the same as that of the oldest channel
/// Although some channels may be farther along, selecting the oldest channel
/// when doing updates ensures that all channels stay reasonably close to one another
/// </summary>
void dramSystem::update_system_time()
{
	int oldest_chan_id = system_config.chan_count - 1;
	tick_t oldest_time = channel[oldest_chan_id].get_time();

	for (int chan_id = system_config.chan_count - 2; chan_id >= 0; chan_id--)
	{
		if (channel[chan_id].get_time() < oldest_time)
		{
			oldest_chan_id = chan_id;
			oldest_time = channel[chan_id].get_time();
		}
	}

	time = oldest_time;
}


void dramSystem::get_next_random_request(transaction *this_t)
{
	if (input_stream.type == RANDOM)
	{
		unsigned int j;

		rand_s(&j);

		// check against last transaction to see what the chan_id was, and whether we need to change channels or not
		if (input_stream.chan_locality * UINT_MAX < j)
		{
			this_t->addr.chan_id = (this_t->addr.chan_id + (j % (system_config.chan_count - 1))) % system_config.chan_count;
		}

		// check against the rank_id of the last transaction to the newly selected channel to see if we need to change the rank_id
		// or keep to this rank_id 

		int rank_id = channel[this_t->addr.chan_id].get_last_rank_id();

		rand_s(&j);
		if ((input_stream.rank_locality * UINT_MAX < j) && (system_config.rank_count > 1))
		{
			rank_id = this_t->addr.rank_id = 
				(rank_id + 1 + (j % (system_config.rank_count - 1))) % system_config.rank_count;
		}
		else
		{
			this_t->addr.rank_id = rank_id;
		}

		int bank_id = channel[this_t->addr.chan_id].getRank(rank_id).last_bank_id;

		rand_s(&j);

		if ((input_stream.bank_locality * UINT_MAX < j) && (system_config.bank_count > 1))
		{
			bank_id = this_t->addr.bank_id =
				(bank_id + 1 + (j % (system_config.bank_count - 1))) % system_config.bank_count;
		}
		else
		{
			this_t->addr.bank_id = bank_id;
		}

		int row_id = channel[this_t->addr.chan_id].getRank(rank_id).bank[bank_id].row_id;

		rand_s(&j);

		if (input_stream.row_locality * UINT_MAX < j)
		{
			this_t->addr.row_id = (row_id + 1 + (j % (system_config.row_count - 1))) % system_config.row_count;
			row_id = this_t->addr.row_id;
		}
		else
		{
			this_t->addr.row_id = row_id;
		}

		rand_s(&j);

		if (input_stream.read_percentage * UINT_MAX > j)
		{
			this_t->type = READ_TRANSACTION;
		}
		else
		{
			this_t->type = WRITE_TRANSACTION;
		}

		rand_s(&j);

		if (input_stream.short_burst_ratio * UINT_MAX > j)
		{
			this_t->length = 4;
		}
		else
		{
			this_t->length = 8;
		}

		int time_gap = 0;

		while (true)
		{
			rand_s(&j);

			if (j > input_stream.arrival_thresh_hold * UINT_MAX) /* interarrival probability function */
			{

				//gaussian distribution function
				if (input_stream.interarrival_distribution_model == GAUSSIAN_DISTRIBUTION)
				{
					input_stream.arrival_thresh_hold = 1.0 - (1.0 / input_stream.box_muller((double)input_stream.average_interarrival_cycle_count, 10));
				}
				//poisson distribution function
				else if (input_stream.interarrival_distribution_model == POISSON_DISTRIBUTION)
				{
					input_stream.arrival_thresh_hold = 1.0 - (1.0 / input_stream.poisson_rng((double)input_stream.average_interarrival_cycle_count));
				}
				break;
			}
			else
			{
				++input_stream.time;
			}
		}

		this_t->arrival_time = input_stream.time;
		this_t->addr.col_id = 0;
	}
}

enum input_status_t dramSystem::get_next_input_transaction(transaction *&this_t)
{
	static transaction *temp_t;

	if (temp_t == NULL)
	{
		temp_t = new transaction;

		if (input_stream.type == RANDOM)
		{
			get_next_random_request(temp_t);
		}
		else if((input_stream.type == K6_TRACE) || (input_stream.type == MASE_TRACE) || (input_stream.type == MAPPED))
		{
			static busEvent this_e;

			if(input_stream.get_next_bus_event(this_e) == FAILURE)
			{
				/* EOF reached */
				return FAILURE;
			} 
			else
			{
				temp_t->addr = this_e.address;
				// FIXME: ignores return type
				convert_address(temp_t->addr);
				++(temp_t->event_no);
				temp_t->type = this_e.attributes;
				temp_t->length = 8;			// assume burst length of 8
				temp_t->arrival_time = this_e.timestamp;
				// need to adjust arrival time for K6 traces to cycles

			}
		}
	}

	if (system_config.refresh_policy == NO_REFRESH)
	{
		this_t = temp_t;
		temp_t = NULL;
	}
	else
	{
		// read but do not remove
		transaction *refresh_t = channel[temp_t->addr.chan_id].read_refresh();

		if (refresh_t->arrival_time < temp_t->arrival_time)
			this_t = channel[temp_t->addr.chan_id].get_refresh();
		else
		{
			this_t = temp_t;
			temp_t = NULL;
		}
	}
	return SUCCESS;
}

void dramSystem::set_dram_timing_specification(enum dram_type_t dram_type)
{
	/// references to make reading this function easier
	int &t_al = timing_specification.t_al;
	int &t_burst = timing_specification.t_burst;
	int &t_cas = timing_specification.t_cas;
	int &t_cmd = timing_specification.t_cmd;
	int &t_cwd = timing_specification.t_cwd;
	int &t_int_burst = timing_specification.t_int_burst;
	int &t_faw = timing_specification.t_faw;
	int &t_ras = timing_specification.t_ras;
	int &t_rc = timing_specification.t_rc;
	int &t_rcd = timing_specification.t_rcd;
	int &t_rfc = timing_specification.t_rfc;
	int &t_rp = timing_specification.t_rp;
	int &t_rrd = timing_specification.t_rrd;
	int &t_rtp = timing_specification.t_rtp;
	int &t_rtrs = timing_specification.t_rtrs;
	int &t_wr = timing_specification.t_wr;
	int &t_wtr = timing_specification.t_wtr;

	switch (dram_type)
	{

	case SDRAM:	// @ 100 MHz
		t_al = 0;			// no such thing as posted CAS in SDRAM
		t_burst = 8;		// depending on system config! can be 1, 2, 4, or 8
		t_cas = 2;
		t_cmd = 1;			// protocol specific, cannot be changed
		t_cwd = 0;			// no such thing in SDRAM
		t_int_burst = 1;	// prefetch length is 1
		t_faw = 0;			// no such thing in SDRAM
		t_ras = 5;			// */
		t_rc = 7;		
		t_rcd = 2;
		t_rfc = 7;			// same as t_rc
		t_rp = 2;			// 12 ns @ 1.25ns per cycle = 9.6 cycles
		t_rrd = 0;			// no such thing in SDRAM
		t_rtp = 1;
		t_rtrs = 0;			// no such thing in SDRAM
		t_wr = 2;		
		break;

	case DDR:				// @ 200 MHz (400 Mbps)
		t_al = 0;			// no such thing in DDR
		t_burst = 8;		// depending on system config! can be 2, 4, or 8
		t_cas = 6;
		t_cmd = 2;			// protocol specific, cannot be changed
		t_cwd = 2;			// protocol specific, cannot be changed
		t_int_burst = 2;	// protocol specific, cannot be changed
		t_faw = 0;			// no such thing in DDR
		t_ras = 16;			// 40 ns @ 2.5 ns per beat == 16 beats
		t_rc = 22;			// 55 ns t_rc
		t_rcd = 6;
		t_rfc = 28;			// 70 ns @ 2.5 ns per beat == 28 beats
		t_rp = 6;			// 15 ns @ 2.5ns per beat = 6 beats
		t_rrd = 0;
		t_rtp = 2;
		t_rtrs = 2;
		t_wr = 6;			// 15 ns @ 2.5 ns per beat = 6 beats
		t_wtr = 4;
		break;

	case DDR2:				// @ 800 Mbps
		t_al = 0;
		t_burst = 8;		// depending on system config! can be 4, or 8
		t_cas = 10;
		t_cmd = 2;			// protocol specific, cannot be changed
		t_cwd = t_cas - 2;	// protocol specific, cannot be changed
		t_int_burst = 4;	// protocol specific, cannot be changed
		t_faw = 30;
		t_ras = 36;			// 45 ns @ 1.25ns per beat = 36 beats
		t_rc = 46;			// 57 ns @ 1.25ns per beat = 45.6 beats
		t_rcd = 10;
		t_rfc = 102;		// 128 ns @ 1.25ns per beat ~= 102 beats
		t_rp = 10;			// 12 ns @ 1.25ns per beat = 9.6 beats
		t_rrd = 6;			// 7.5 ns
		t_rtp = 6;
		t_rtrs = 2;
		t_wr = 12;
		t_wtr = 6;
		break;

	case DDR3:				// @ 1.33 Gbps = 0.75 ns per beat
		t_al = 0;
		t_burst = 8;		// protocol specific, cannot be changed
		t_cas = 10;
		t_cmd = 2;			// protocol specific, cannot be changed
		t_cwd = t_cas - 2;
		t_int_burst = 8;	// protocol specific, cannot be changed
		t_faw = 30;
		t_ras = 36;			// 27 ns @ 0.75ns per beat = 36 beats
		t_rc = 48;			// 36 ns @ 0.75ns per beat = 48 beats
		t_rcd = 12;
		t_rfc = 280;
		t_rp = 12;			// 9 ns @ 0.75ns per beat = 12 beats
		t_rrd = 8;
		t_rtrs = 2;
		t_rtp = 8;
		t_wr = 12;
		t_wtr = 8;
		break;

	case DRDRAM:	// FIXME
		// not currently supported
		cerr << "Not yet supported" << endl;
		break;

	default:
		cerr << "Not supported" << endl;
		break;
	}
}

dramSystem::dramSystem(const dramSettings *settings): 
system_config(settings),
channel(system_config.chan_count,
		dramChannel(settings)),
timing_specification(settings),
sim_parameters(settings),
statistics(),
algorithm(settings),
input_stream(settings),
time(0),
event_q(COMMAND_QUEUE_SIZE)
{
	if (settings->outFile.length() > 1)
	{
		outStream.open(settings->outFile.c_str());	
		if (!outStream.good())
		{
			cerr << "Error opening file \"" << settings->outFile << "\" for writing" << endl;
			exit(-12);
		}
	}
	// init the refresh queue for each channel
	unsigned cnt = 0;
	for (vector<dramChannel>::iterator i = channel.begin(); i != channel.end(); i++)
	{
		i->initRefreshQueue(settings->rowCount, settings->refreshTime, cnt++);
	}
}


dramSystem::dramSystem(map<file_io_token_t,string> &parameter):
system_config(parameter),
channel(system_config.chan_count,
		dramChannel(system_config.transaction_queue_depth,
		system_config.history_queue_depth,
		system_config.completion_queue_depth,
		system_config.refresh_queue_depth,
		system_config.rank_count,
		system_config.bank_count,
		system_config.per_bank_queue_depth)),
timing_specification(parameter),
sim_parameters(parameter),
algorithm(system_config.rank_count,
		  system_config.bank_count,
		  system_config.config_type),
input_stream(parameter),
time(0), // start the clock
//free_event_pool(COMMAND_QUEUE_SIZE,true), // create enough events, transactions and commands ahead of time
event_q(COMMAND_QUEUE_SIZE)
{
	map<file_io_token_t, string>::iterator temp;
	stringstream temp2;

	if ((temp=parameter.find(output_file_token))!=parameter.end())
	{
		//output_filename = parameter[output_file_token];
		if (temp->second.length() > 1)
		{
			outStream.open(temp->second.c_str());	
			if (!outStream.good())
			{
				cerr << "Error opening file \"" << temp->second << "\" for writing" << endl;
				exit(-12);
			}
		}
	}

	// init the refresh queue for each channel
	unsigned cnt = 0;
	for (vector<dramChannel>::iterator i = channel.begin(); i != channel.end(); i++)
	{
		i->initRefreshQueue(system_config.row_count, system_config.refresh_time, cnt++);
	}
}


int dramSystem::find_oldest_channel() const
{
	int oldest_chan_id = 0;
	tick_t oldest_time = channel[0].get_time();

	for (unsigned chan_id = 1; chan_id < channel.size() ; ++chan_id)
	{
		if (channel[chan_id].get_time() < oldest_time)
		{
			oldest_chan_id = chan_id;
			oldest_time = channel[chan_id].get_time();
		}
	}
	return oldest_chan_id;
}

ostream &operator<<(ostream &os, const dramSystem &this_a)
{
	os << "SYS[";
	switch(this_a.system_config.getConfigType())
	{
	case BASELINE_CONFIG:
		os << "BASE] ";
		break;
	default:
		os << "UNKN] ";
		break;
	}
	os << "RC[" << this_a.system_config.getRankCount() << "] ";
	os << "BC[" << this_a.system_config.getBankCount() << "] ";
	os << "ALG[";
	switch(this_a.system_config.getCommandOrderingAlgorithm())
	{
	case STRICT_ORDER:
		os << "STR ] ";
		break;
	case RANK_ROUND_ROBIN:
		os << "RRR ] ";
		break;
	case BANK_ROUND_ROBIN:
		os << "BRR ] ";
		break;
	case WANG_RANK_HOP:
		os << "WANG] ";
		break;
	case GREEDY:
		os << "GRDY] ";
		break;
	default:
		os << "UNKN] ";
		break;
	}
	os << "BQD[" << this_a.system_config.getPerBankQueueDepth() << "] ";
	os << "BLR[" << setprecision(0) << floor(100*(this_a.system_config.getShortBurstRatio() + 0.0001) + .5) << "] ";
	os << "RP[" << (int)(100*this_a.system_config.getReadPercentage()) << "] ";

	os << this_a.timing_specification;
	os << this_a.statistics;

	return os;
}

