#define _CRT_RAND_S

#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>

#include "dramsim2.h"

int dram_system::convert_address(__inout addresses &this_a)
{
	unsigned int input_a;
	unsigned int temp_a, temp_b;
	unsigned int bit_15,bit_27,bits_26_to_16;
	//int	mapping_scheme;
	//int	chan_count, rank_count, bank_count, col_count, row_count;
	//int	chan_addr_depth, rank_addr_depth, bank_addr_depth, row_addr_depth, col_addr_depth;
	int	col_size;
	//, col_size_depth;

	//mapping_scheme = config->addr_mapping_scheme;

	if(system_config.dram_type == DRDRAM) //FIXME: shouldn't this already be set appropriately?
		col_size	= 16;
	else
		col_size 	= system_config.col_size;

	//chan_count = config->chan_count;
	//rank_count = config->rank_count;
	//bank_count = config->bank_count;
	//row_count  = config->row_count;
	//col_count  = config->col_count;
	int chan_addr_depth = log2(system_config.chan_count);
	int rank_addr_depth = log2(system_config.rank_count);
	int bank_addr_depth = log2(system_config.bank_count);
	int row_addr_depth  = log2(system_config.row_count);
	int col_addr_depth  = log2(system_config.col_count);
	int col_size_depth	= log2(col_size);

	//input_a = this_a->phys_addr;
	// strip away the byte address portion
	input_a = this_a.phys_addr >> col_size_depth;

	unsigned int address = this_a.phys_addr;

	int cacheline_size;
	int cacheline_size_depth;	/* address bit depth */
	int col_id_lo;
	int col_id_lo_depth;
	int col_id_hi;
	int col_id_hi_depth;
	
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
		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
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

command *dram_system::get_next_command(const int chan_id)
{
	dram_channel	*channel= &(dram_system::channel[chan_id]);
	//queue &history_q  = channel->history_q;

	command	*last_c;
	command	*temp_c;
	command	*candidate_c;
	command	*next_c;
	//command	*ptr_c;

	int offset;
	//int tail_offset;
	int oldest_rank_id;
	int oldest_bank_id;
	int rank_id;
	int bank_id;
	int section_id;
	transaction_type_t transaction_type;
	bool candidate_found = false;
	int start_bank_id;
	int count;
	int i,j,k;
	bool candidate_count;
	tick_t oldest_command_time;

	//tail_offset = history_q.get_count() - 1;

	// nothing in history q, start from rank 0, bank 0

	// look at the most recently retired command in this channel's history
	if((last_c = channel->get_most_recent_command()) == NULL)		
	{
		last_c = free_command_pool.acquire_item();
		last_c->addr.rank_id = system_config.rank_count - 1;
		last_c->addr.bank_id = system_config.bank_count - 1;
		last_c->this_command = CAS_WRITE_AND_PRECHARGE_COMMAND;
	}

	if(system_config.command_ordering_algorithm == STRICT_ORDER) /* look for oldest command, execute that */
	{
		oldest_command_time = -1;
		oldest_rank_id = -1;
		oldest_bank_id = -1;
		for(int rank_id = 0; rank_id< system_config.rank_count; ++rank_id)
		{
			for(int bank_id = 0; bank_id< system_config.rank_count; ++bank_id)
			{
				temp_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);
				if(temp_c != NULL)
				{
					if((oldest_command_time < 0) || (oldest_command_time > temp_c->enqueue_time))
					{
						oldest_command_time = temp_c->enqueue_time;
						oldest_rank_id 	= rank_id;
						oldest_bank_id 	= bank_id;
					}
				}
			}
		}
		return  channel->get_rank(oldest_rank_id)->bank[oldest_bank_id].per_bank_q.dequeue();
	}
	// alternate ranks as we go down banks
	else if(system_config.command_ordering_algorithm == RANK_ROUND_ROBIN)
	{
		bank_id 	= last_c->addr.bank_id;
		rank_id 	= last_c->addr.rank_id;
		if(last_c->this_command == RAS_COMMAND)
		{
			temp_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);
			if((temp_c != NULL) &&
				((temp_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->this_command == CAS_AND_PRECHARGE_COMMAND)))
			{
				return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
			}
			else
			{
				cerr << "Serious problems. RAS not followed by CAS" << endl;
				_exit(2);
			}
		}
		else if (last_c->this_command == CAS_AND_PRECHARGE_COMMAND)
		{
			transaction_type = READ_TRANSACTION;
		}
		else if (last_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND)
		{
			transaction_type = WRITE_TRANSACTION;
		}
		else
		{
			transaction_type = READ_TRANSACTION;
			cerr << "Did not find a CAS or RAS command when it was expected" << endl;
		}

		while(candidate_found == false)
		{
			rank_id = (rank_id + 1) % system_config.rank_count;
			if(rank_id == 0)
			{
				bank_id = (bank_id + 1) % system_config.bank_count;
				if(bank_id == 0)
				{
					if (transaction_type == WRITE_TRANSACTION)
					{
						transaction_type = READ_TRANSACTION;
					}
					else
					{
						transaction_type = WRITE_TRANSACTION;
					}
				}
			}
			temp_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);
			if(temp_c != NULL)
			{
				if(system_config.read_write_grouping == false)
				{
					return channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
				}
				else // have to follow read_write grouping considerations 
				{
					next_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(1);	/* look at the second command */
					if (((next_c->this_command == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
						((next_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
					{
						return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
					}
				}

#ifdef DEBUG_FLAG_2
				cerr << "Looked in ["<< temp_c->addr.rank_id << "] [" << temp_c->addr.bank_id << "] but wrong type, We want [" << transaction_type << "]. Candidate command type ";
				cerr << temp_c->this_command;
				cerr << " followed by ";
				cerr << next_c->this_command;
				cerr << "count [" << channel->get_rank(rank_id)->bank[bank_id].per_bank_q.get_count() << "]" << endl;
#endif

			}

#ifdef DEBUG_FLAG_2
			cerr << "Looked in [" << rank_id << "] [" << bank_id << "] but Q empty" << endl;
#endif

		}
	}
	// keep rank id as long as possible, go round robin down a given rank
	else if (system_config.command_ordering_algorithm == BANK_ROUND_ROBIN)	
	{
		bank_id = last_c->addr.bank_id;
		rank_id = last_c->addr.rank_id;
		if(last_c->this_command == RAS_COMMAND)
		{
			temp_c = channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);
			if((temp_c != NULL) &&
				((temp_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->this_command == CAS_AND_PRECHARGE_COMMAND))){
					return (command*) channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
			}
			else
			{
				cerr << "Serious problems. RAS not followed by CAS" << endl;
				_exit(2);
			}
		}
		else if (last_c->this_command == CAS_AND_PRECHARGE_COMMAND)
		{
			transaction_type = READ_TRANSACTION;
		}
		else if (last_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND)
		{
			transaction_type = WRITE_TRANSACTION;
		}
		while (candidate_found == false)
		{
			bank_id = (bank_id + 1) % system_config.bank_count;
			if (bank_id == 0)
			{
				rank_id = (rank_id + 1) % system_config.rank_count;
				if (rank_id == 0)
				{
					if (transaction_type == WRITE_TRANSACTION)
					{
						transaction_type = READ_TRANSACTION;
					}
					else
					{
						transaction_type = WRITE_TRANSACTION;
					}
				}
			}
			temp_c = channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);
			if(temp_c != NULL)
			{	
				if(system_config.read_write_grouping == false)
				{
					return temp_c;
				}
				else					/* have to follow read_write grouping considerations */
				{
					next_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(1);	/* look at the second command */
					if (((next_c->this_command == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
						((next_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION))){
							return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
					}
				}
#ifdef DEBUG_FLAG_2
				cerr << "Looked in [" << temp_c->addr.rank_id << "] [" << temp_c->addr.bank_id << "] but wrong type, We want [" << transaction_type << "] Candidate command type [" << next_c->this_command << "]" << endl;
#endif
			}
#ifdef DEBUG_FLAG_2
			cerr << "Looked in [" << rank_id << "] [" << bank_id << "] but Q empty" << endl;
#endif
		}
	}
	else if((system_config.command_ordering_algorithm == WANG_RANK_HOP) && (system_config.config_type == BASELINE_CONFIG))	/* baseline*/
	{
		while(candidate_found == false)
		{
			command *ptr_c = algorithm.WHCC.read(algorithm.WHCC_offset[0]);
			algorithm.WHCC_offset[0] = (algorithm.WHCC_offset[0] + 1) % algorithm.WHCC.get_count();
			rank_id 	= ptr_c->addr.rank_id;
			bank_id 	= ptr_c->addr.bank_id;
			candidate_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

			if((candidate_c != NULL) && (candidate_c->this_command == RAS_COMMAND))
			{
				if(ptr_c->this_command == RAS_COMMAND)
				{
					if(bank_id == 0)	// see if this rank needs a R/W switch around
					{
						algorithm.transaction_type[rank_id] = channel->set_read_write_type(rank_id,system_config.bank_count);
					}

					next_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(1);

					if(((algorithm.transaction_type[rank_id] == READ_TRANSACTION) && (next_c->this_command == CAS_AND_PRECHARGE_COMMAND)) ||
						((algorithm.transaction_type[rank_id] == WRITE_TRANSACTION) && (next_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND)))
					{
							candidate_found = true;
							return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
					}
				}
			}
			else if((candidate_c != NULL) && (candidate_c->this_command == CAS_AND_PRECHARGE_COMMAND))
			{
				if((ptr_c->this_command == CAS_COMMAND) && (algorithm.transaction_type[rank_id] == READ_TRANSACTION))
				{
					candidate_found = true;
					return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
				}
			}
			else if((candidate_c != NULL) && (candidate_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND))
			{
				if((ptr_c->this_command == CAS_COMMAND) && (algorithm.transaction_type[rank_id] == WRITE_TRANSACTION))
				{
					candidate_found = false;
					return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
				}
			}
			else
			{
#ifdef DEBUG_FLAG_1
				cerr << "some debug message" << endl;
#endif
			}
		}
	}
	else if (system_config.command_ordering_algorithm == GREEDY)
	{
		command *candidate_command = NULL;
		int candidate_gap = INT_MAX;
		for (rank_id = system_config.rank_count; rank_id >= 0; --rank_id)
		{
			for (bank_id = system_config.bank_count; bank_id >= 0; --bank_id)
			{
				command *challenger_command = channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);
				if (challenger_command != NULL)
				{
					int challenger_gap = min_protocol_gap(chan_id,challenger_command);
					if (challenger_gap < candidate_gap || (candidate_gap == challenger_gap && challenger_command->enqueue_time < candidate_command->enqueue_time))
					{
						candidate_gap = challenger_gap;
						candidate_command = challenger_command;
					}
				}
			}
		}
		temp_c = channel->get_rank(candidate_command->addr.rank_id)->bank[candidate_command->addr.bank_id].per_bank_q.dequeue();

#ifdef DEBUG
		cerr << "R[" << candidate_command->addr.rank_id << "] B[" << candidate_command->addr.bank_id << "]\tWinner: " << *temp_c << "gap[" << candidate_gap << "] now[" << channel->time << "]" << endl;
#endif

		return temp_c;
	}
	else
	{
		cerr << "This configuration and algorithm combination is not supported" << endl;
		_exit(0);
	}
}


dram_system::~dram_system()
{
	delete[] channel;
	//delete input_stream;
}

void DRAMSim2::dram_system::update_system_time()
{
	int oldest_chan_id = system_config.chan_count;
	tick_t oldest_time = channel[system_config.chan_count].get_time();

	for (int chan_id = system_config.chan_count - 1; chan_id > 0; --chan_id)
	{
		if (channel[chan_id].get_time() < oldest_time)
		{
			oldest_chan_id = chan_id;
			oldest_time = channel[chan_id].get_time();
		}
	}

	time = oldest_time;
}


void dram_system::get_next_random_request(transaction *this_t)
{
	if(input_stream.type == RANDOM)
	{
		unsigned int j;

		rand_s(&j);

		/* check against last transaction to see what the chan_id was, and whether we need to change channels or not */
		if (input_stream.chan_locality * UINT_MAX < j)
		{
			//rand_s(&j);
			this_t->addr.chan_id = (this_t->addr.chan_id + (j % (system_config.chan_count - 1))) % system_config.chan_count;
		}

		// check against the rank_id of the last transaction to the newly selected channel to see if we need to change the rank_id
		// or keep to this rank_id 

		int rank_id = channel[this_t->addr.chan_id].get_last_rank_id();

		rand_s(&j);
		if (input_stream.rank_locality * UINT_MAX < j)
		{
			//rand_s(&j);
			this_t->addr.rank_id = (rank_id + 1 + (j % (system_config.rank_count - 1))) % system_config.rank_count;
			rank_id = this_t->addr.rank_id ;
		}
		else
		{
			this_t->addr.rank_id = rank_id;
		}

		int bank_id = channel[this_t->addr.chan_id].get_rank(rank_id)->last_bank_id;

		rand_s(&j);

		if (input_stream.bank_locality * UINT_MAX < j)
		{
			//rand_s(&j);
			this_t->addr.bank_id = (bank_id + 1 + (j % (system_config.bank_count - 1))) % system_config.bank_count;
			bank_id = this_t->addr.bank_id ;
		}
		else
		{
			this_t->addr.bank_id = bank_id;
		}

		int row_id = channel[this_t->addr.chan_id].get_rank(rank_id)->bank[bank_id].row_id;

		rand_s(&j);

		if (input_stream.row_locality * UINT_MAX < j)
		{
			//rand_s(&j);
			this_t->addr.row_id = (row_id + 1 + (j % (system_config.row_count - 1))) % system_config.row_count;
			row_id = this_t->addr.row_id;
		}
		else
			this_t->addr.row_id = row_id;

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

		while(true)
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


void DRAMSim2::dram_system::execute_command(command *this_c,const int gap)
{
	tick_t *this_ras_time;

	
	unsigned int chan_id = this_c->addr.chan_id;
	unsigned int rank_id = this_c->addr.rank_id;
	unsigned int bank_id = this_c->addr.bank_id;
	unsigned int row_id = this_c->addr.row_id;

	dram_channel &channel= dram_system::channel[chan_id];
	tick_t now = channel.get_time() + gap;
	int t_al;
	//int history_q_count;
	rank_c *this_r = channel.get_rank(rank_id);
	bank_c &this_b = this_r->bank[bank_id];

	transaction *host_t = NULL;
	//queue &history_q = channel.history_q;
	//  queue *complete_q;

	this_r->last_bank_id = bank_id;

	t_al = this_c->posted_cas ? timing_specification.t_al : 0;

	channel.set_time(channel.get_time() + gap);

	switch(this_c->this_command)
	{
	case RAS_COMMAND:
		this_b.last_ras_time = now;
		this_b.row_id = row_id;
		++this_b.ras_count;

		if(this_r->last_ras_times.get_count() == 4)	/* ras time history queue, per rank */
			this_ras_time = (tick_t *) this_r->last_ras_times.dequeue();
		else
			this_ras_time = new tick_t;

		*this_ras_time = now;
		this_r->last_ras_times.enqueue(this_ras_time);
		break;

	case CAS_AND_PRECHARGE_COMMAND:

		this_b.last_prec_time = max(now + t_al + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtp, this_b.last_ras_time + timing_specification.t_ras);

	case CAS_COMMAND:

		this_b.last_cas_time = now;
		this_r->last_cas_time = now;
		this_b.last_cas_length = this_c->length;
		this_r->last_cas_length = this_c->length;
		++this_b.cas_count;
		host_t = this_c->host_t;
		host_t->completion_time	= now + timing_specification.t_cas;
		break;

	case CAS_WRITE_AND_PRECHARGE_COMMAND:

		this_b.last_prec_time = max(now + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr, this_b.last_ras_time + timing_specification.t_ras);

	case CAS_WRITE_COMMAND:

		this_b.last_casw_time = now;
		this_r->last_casw_time = now;
		this_b.last_casw_length= this_c->length;
		this_r->last_casw_length= this_c->length;
		this_b.casw_count++;
		host_t = this_c->host_t;
		host_t->completion_time	= now;
		break;

	case RETIRE_COMMAND:

		break;

	case PRECHARGE_COMMAND:

		this_b.last_prec_time = now;
		break;

	case PRECHARGE_ALL_COMMAND:

		break;

	case RAS_ALL_COMMAND:

		break;
	case DRIVE_COMMAND:

		break;

	case DATA_COMMAND:

		break;
	case CAS_WITH_DRIVE_COMMAND:

		break;
	case REFRESH_ALL_COMMAND:

		this_b.last_refresh_all_time = now;
		break;
	}
	/* transaction complete? if so, put in completion queue */
	if(host_t != NULL) 
	{
		if(channel.complete(host_t) == FAILURE)
		{
			cerr << "Fatal error, cannot insert transaction into completion queue." << endl
				<< "Increase execution q depth and resume. Should not occur. Check logic." << endl;
			_exit(2);
		}
	}

	/* record command history. Check to see if this can be removed */
	channel.record_command(this_c, free_command_pool);
	//if (channel.history_q.get_count() == system_config.history_queue_depth)
	//{		
		/*done with this command, release into pool */
	//	free_command_pool.release_item(channel.history_q.dequeue());
	//}
	//channel.history_q.enqueue(this_c);
}



enum input_status_t dram_system::get_next_input_transaction(transaction *&this_t)
{
	this_t = free_transaction_pool.acquire_item();

	if (input_stream.type == RANDOM)
	{
		get_next_random_request(this_t);
	}
	else if((input_stream.type == K6_TRACE) || (input_stream.type == MASE_TRACE))
	{
		static bus_event this_e;

		if(input_stream.get_next_bus_event(this_e) == FAILURE)
		{
			/* EOF reached */
			return FAILURE;
		} 
		else
		{
			this_t->addr.phys_addr = this_e.address;
			// FIXME: ignores return type
			convert_address(this_t->addr);
			++(this_t->event_no);
			this_t->type = this_e.attributes;
			this_t->length = 8;			/* assume burst length of 8 */
			this_t->arrival_time = this_e.timestamp;
			/* need to adjust arrival time for K6 traces to cycles */
		}
	}
	return SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////
/// Function:	dram_system::min_protocol_gap()
/// Arguments:	dram_channel *channel: the channel by which the gap to the next command
///                                      will be computed
///            this_c: the command that the gap will be calculated for
/// Purpose:		find the protocol gap between a command and current system state
///
////////////////////////////////////////////////////////////////////////////////////////
int DRAMSim2::dram_system::min_protocol_gap(const int channel_no,const command *this_c)
{ 
	//FIXME: some max() functions take uninit values
	int     min_gap;
	dram_channel &channel = dram_system::channel[channel_no];
	//int     rank_id;
	int     bank_id;
	int     row_id;
	const int this_rank_id = this_c->addr.rank_id;
	const int this_bank_id = this_c->addr.bank_id;
	rank_c	*this_r= channel.get_rank(this_rank_id);
	bank_c	&this_b= this_r->bank[this_bank_id];
	tick_t 	now = channel.get_time();
	int     t_cas_gap = 0;
	//int     t_faw_gap = 0;
	int     t_ras_gap = 0;
	int     t_rcd_gap = 0;
	int     t_rfc_gap = 0;
	int     t_rp_gap = 0;
	int     t_rrd_gap = 0;
	int     casw_to_rp_gap;
	int     cas_to_rp_gap;
	tick_t	*last_ras_time;		/* for t_rrd computations */
	//tick_t	*fourth_ras_time;	/* for t_faw computations */
	tick_t	temp_time;
	tick_t	this_r_last_cas_time;
	tick_t	this_r_last_casw_time;
	tick_t	other_r_last_cas_time;
	tick_t	other_r_last_casw_time;
	tick_t	last_cas_time;
	tick_t	last_casw_time;
	int     last_cas_length;
	int     last_casw_length;
	int     other_r_last_cas_length;
	int     other_r_last_casw_length;
	int     i,j;
	int	time_inserted;
	//int	t_al;
	int	ras_q_count;
	//int	t_burst; Removed 3/9/06 JG
	int cas_length;
	int casw_length;

	int t_al = this_c->posted_cas ? timing_specification.t_al : 0;
	//if(this_c->posted_cas == true)
	//	t_al 	= timing_specification.t_al;
	//else
	//	t_al	= 0;

	switch(this_c->this_command)
	{
	case RAS_COMMAND:
		/* respect t_rp of same bank*/
		t_rp_gap = max(0 , (int)(this_b.last_prec_time - now) + timing_specification.t_rp);

		/* respect t_rrd of all other banks of same rank*/
		ras_q_count	= this_r->last_ras_times.get_count();

		t_rrd_gap;
		if(ras_q_count == 0)
		{
			t_rrd_gap = 0;
		}
		else 
		{
			last_ras_time	= this_r->last_ras_times.read(ras_q_count - 1);	/* read tail end of ras history */
			t_rrd_gap = max(t_rrd_gap,(int)(*last_ras_time + timing_specification.t_rrd - now));
		}

		int t_faw_gap;
		if(ras_q_count < 4)
		{
			t_faw_gap	= 0;
		}
		else
		{
			tick_t *fourth_ras_time = this_r->last_ras_times.read(0); /// read head of ras history
			t_faw_gap = max(0,(int)(*fourth_ras_time - now) + timing_specification.t_faw );
		}

		min_gap = max(t_rp_gap , max(t_rrd_gap , t_faw_gap));

		break;

	case CAS_AND_PRECHARGE_COMMAND:
		/* Auto precharge will be issued as part of command,
		* but DRAM devices are intelligent enough to delay the prec command
		* until tRAS timing is met, so no need to check tRAS timing requirement here.
		*/
	case CAS_COMMAND:
		/* respect last ras of same rank */
		t_ras_gap	= max(0,(int)(this_b.last_ras_time + timing_specification.t_rcd - t_al - now));

		other_r_last_cas_time	= now - 1000;
		other_r_last_cas_length	= timing_specification.t_burst;
		other_r_last_casw_time	= now - 1000;
		other_r_last_casw_length= timing_specification.t_burst;
		for(int rank_id = 0;rank_id< system_config.rank_count;rank_id++)
		{
			if(rank_id != this_rank_id)
			{
				if(channel.get_rank(rank_id)->last_cas_time > other_r_last_cas_time)
				{
					other_r_last_cas_time   = channel.get_rank(rank_id)->last_cas_time;
					other_r_last_cas_length = channel.get_rank(rank_id)->last_cas_length;
				}
				if(channel.get_rank(rank_id)->last_casw_time > other_r_last_casw_time)
				{
					other_r_last_casw_time   = channel.get_rank(rank_id)->last_casw_time;
					other_r_last_casw_length = channel.get_rank(rank_id)->last_casw_length;
				}
			}
		}

		/* respect last cas of same rank */
		//  DW 3/9/2006  add these two lines
		//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
		//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
		//  DW 3/9/2006  replace the line after next with the next line
		//t_cas_gap	= max(0,(int)(this_r.last_cas_time + cas_length - now));
		t_cas_gap	= max(0,(int)(this_r->last_cas_time + timing_specification.t_burst - now));
		/* respect last cas write of same rank */
		//  DW 3/9/2006 replace the line after next with the next line
		//t_cas_gap	= max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
		t_cas_gap	= max(t_cas_gap,(int)(this_r->last_casw_time + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wtr - now));

		if(system_config.rank_count > 1) {
			/* respect most recent cas of different rank */
			t_cas_gap	= max(t_cas_gap,(int)(other_r_last_cas_time + other_r_last_cas_length + timing_specification.t_rtrs - now));
			/* respect timing of READ follow WRITE, different ranks.*/
			t_cas_gap	= max(t_cas_gap,(int)(other_r_last_casw_time + timing_specification.t_cwd + other_r_last_casw_length + timing_specification.t_rtrs - timing_specification.t_cas - now));
		}
		min_gap = max(t_ras_gap,t_cas_gap);
		/*
		fprintf(stderr,"   [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		*/
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		/// Auto precharge will be issued as part of command, so
		/// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		/// the precharge will met tRAS timing or not. So CAS_WRITE_AND_PRECHARGE_COMMAND
		/// has the exact same timing requirements as a simple CAS COMMAND.
	case CAS_WRITE_COMMAND:
		/* respect last ras of same rank */
		t_ras_gap	= max(0,(int)(this_b.last_ras_time + timing_specification.t_rcd - t_al - now));

		other_r_last_cas_time = now - 1000;
		other_r_last_cas_length = timing_specification.t_burst;
		other_r_last_casw_time = now - 1000;
		other_r_last_casw_length= timing_specification.t_burst;
		for(int rank_id = 0 ; rank_id < system_config.rank_count ; rank_id++)
		{
			if (rank_id != this_rank_id)
			{
				if (channel.get_rank(rank_id)->last_cas_time > other_r_last_cas_time)
				{
					other_r_last_cas_time   = channel.get_rank(rank_id)->last_cas_time;
					other_r_last_cas_length = channel.get_rank(rank_id)->last_cas_length;
				}
				if( channel.get_rank(rank_id)->last_casw_time > other_r_last_casw_time)
				{
					other_r_last_casw_time   = channel.get_rank(rank_id)->last_casw_time;
					other_r_last_casw_length = channel.get_rank(rank_id)->last_casw_length;
				}
			}
		}
		//  DW 3/9/2006  add these two lines
		//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
		//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
		/* respect last cas to same rank */
		//  DW 3/9/2006  replace the line after next with the next line
		//t_cas_gap	= max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
		t_cas_gap	= max(0,(int)(this_r->last_cas_time + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtrs - timing_specification.t_cwd - now));
		/* respect last cas to different ranks */
		t_cas_gap	= max(t_cas_gap,(int)(other_r_last_cas_time + timing_specification.t_cas + other_r_last_cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
		/* respect last cas write to same rank */
		//  DW 3/9/2006  replace the line after next with the next line
		//t_cas_gap	= max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
		t_cas_gap	= max(t_cas_gap,(int)(this_r->last_casw_time + timing_specification.t_burst - now));
		/* respect last cas write to different ranks */
		t_cas_gap	= max(t_cas_gap,(int)(other_r_last_casw_time + other_r_last_casw_length - now));
		min_gap		= max(t_ras_gap,t_cas_gap);
		break;
	case RETIRE_COMMAND:
		break;
	case PRECHARGE_COMMAND:

		/* respect t_ras of same bank */
		t_ras_gap	= max(0,(int)(this_b.last_ras_time + timing_specification.t_ras - now));

		/* respect t_cas of same bank */
		t_cas_gap	= max(0,(int)(this_b.last_cas_time + t_al + timing_specification.t_cas + timing_specification.t_burst + max(0,timing_specification.t_rtp - timing_specification.t_cmd)- now));

		/* respect t_casw of same bank */
		t_cas_gap	= max(t_cas_gap,(int)(this_b.last_casw_time + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr - now));

		min_gap		= max(t_ras_gap,t_cas_gap);

		break;
	case PRECHARGE_ALL_COMMAND:
		break;
	case RAS_ALL_COMMAND:
		break;
	case DRIVE_COMMAND:
		break;
	case DATA_COMMAND:
		break;
	case CAS_WITH_DRIVE_COMMAND:
		break;
	case REFRESH_ALL_COMMAND:
		break;
	}
#ifdef DEBUG_FLAG_1
	cerr << this_c->this_command;
	cerr << "       ras[" << setw(2) << t_ras_gap << "] rrd["  << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]" << endl;
#endif
	return max(min_gap,timing_specification.t_cmd);
}


enum input_status_t DRAMSim2::dram_system::transaction2commands(transaction *this_t) 
{
	queue<command> *bank_q = &(channel[this_t->addr.chan_id].get_rank(this_t->addr.rank_id)->bank[this_t->addr.bank_id].per_bank_q);
	
	if (system_config.row_buffer_management_policy == CLOSE_PAGE)
	{
		int empty_command_slot_count = bank_q->freecount();

		if(empty_command_slot_count < 2)
		{
			return FAILURE;
		}
		else if((system_config.auto_precharge == false) && (empty_command_slot_count < 3))
		{
			return FAILURE;
		}
		else
		{
			command *free_c = free_command_pool.acquire_item();

			// command one
			free_c->this_command = RAS_COMMAND;
			free_c->start_time = this_t->arrival_time;
			free_c->enqueue_time = time;
			free_c->addr = this_t->addr; /// copy the addr stuff over
			free_c->host_t = NULL;

			bank_q->enqueue(free_c);

			
			if(system_config.auto_precharge == false)
			{
				free_c = free_command_pool.acquire_item();

				// command two
				switch (this_t->type)
				{
				case WRITE_TRANSACTION:
					free_c->this_command = CAS_WRITE_COMMAND;
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					free_c->this_command = CAS_COMMAND;
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->type;
					exit(-8);
				}

				free_c->start_time = this_t->arrival_time;
				free_c->enqueue_time = time;
				free_c->addr = this_t->addr;
				free_c->posted_cas = system_config.posted_cas;
				free_c->host_t = this_t;				
				free_c->length = this_t->length;
				bank_q->enqueue(free_c);

				// command three
				free_c = free_command_pool.acquire_item();
				free_c->this_command = PRECHARGE_COMMAND;
				free_c->start_time = this_t->arrival_time;
				free_c->enqueue_time = time;
				free_c->addr = this_t->addr;
				free_c->host_t = NULL; // only one of these commands has a pointer to the original transaction
				bank_q->enqueue(free_c);
			}
			else // precharge is implied, only need two commands
			{
				free_c = free_command_pool.acquire_item();

				// command two
				switch(this_t->type)
				{
				case WRITE_TRANSACTION:
					free_c->this_command = CAS_WRITE_AND_PRECHARGE_COMMAND;
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					free_c->this_command = CAS_AND_PRECHARGE_COMMAND;
					break;
				case PER_BANK_REFRESH_TRANSACTION:
					free_c->this_command = PRECHARGE_COMMAND;
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->type;
					exit(-8);
				}
				free_c->start_time = this_t->arrival_time;
				free_c->enqueue_time = time;
				free_c->addr = this_t->addr;		/* copy the addr stuff over */
				free_c->posted_cas = system_config.posted_cas;
				free_c->host_t = this_t;				
				free_c->length = this_t->length;

				bank_q->enqueue(free_c);
			}
		}
	}
	else if(system_config.row_buffer_management_policy == OPEN_PAGE)
	{
		int queued_command_count = bank_q->get_count();
		int empty_command_slot_count = bank_q->freecount();
		/// look in the bank_q and see if there's a precharge for this row
		bool bypass_allowed 	= true;
		/// go from tail to head
		for(int tail_offset = queued_command_count -1 ;(tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
		{	
			command *temp_c = bank_q->read(tail_offset);

			if(temp_c->this_command == PRECHARGE_COMMAND)	/// found a precharge command
			{
				if(temp_c->addr.row_id == this_t->addr.row_id)	/// same row, insert here 
				{
					if(empty_command_slot_count < 1)
						return FAILURE;

					command *free_c = free_command_pool.acquire_item();

					if(this_t->type == WRITE_TRANSACTION)
						free_c->this_command = CAS_WRITE_COMMAND;
					else if (this_t->type == READ_TRANSACTION)
						free_c->this_command = CAS_COMMAND;
					free_c->start_time = this_t->arrival_time;
					free_c->enqueue_time = time;
					free_c->addr = this_t->addr; /// copy the addr stuff over
					free_c->host_t = this_t;

					free_c->length = this_t->length;

					bank_q->insert(free_c, tail_offset);	/* insert at this location */
					return SUCCESS;
				}
			}

			/// even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge
			/// command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND

			if((system_config.command_ordering_algorithm == STRICT_ORDER) || ((int)(time-temp_c->enqueue_time) > system_config.seniority_age_limit))
			{
				bypass_allowed = false;
			}
		}
		if(empty_command_slot_count < 3)
		{
			return FAILURE;
		}
		command *free_c = free_command_pool.acquire_item();

		free_c->this_command = RAS_COMMAND;
		free_c->start_time = this_t->arrival_time;
		free_c->enqueue_time = time;
		free_c->addr = this_t->addr;		/* copy the addr stuff over */
		free_c->host_t = NULL;

		bank_q->enqueue(free_c);

		free_c = free_command_pool.acquire_item();
		if(this_t->type == WRITE_TRANSACTION)
		{
			free_c->this_command = CAS_WRITE_COMMAND;
		}
		else if (this_t->type == READ_TRANSACTION)
		{
			free_c->this_command = CAS_COMMAND;
		}
		else
		{
			cerr << "Unhandled transaction type: " << this_t->type;
			exit(-8);
		}

		free_c->start_time = this_t->arrival_time;
		free_c->enqueue_time = time;
		free_c->posted_cas = system_config.posted_cas;
		free_c->addr = this_t->addr;		/* copy the addr stuff over */
		free_c->length = this_t->length;
		// FIXME: not initializing the host_t value?
		bank_q->enqueue(free_c);

		free_c = free_command_pool.acquire_item();
		free_c->this_command = PRECHARGE_COMMAND;
		free_c->start_time = this_t->arrival_time;
		free_c->enqueue_time = time;
		free_c->addr = this_t->addr;		/* copy the addr stuff over */
		free_c->host_t = NULL;
		bank_q->enqueue(free_c);
	}
	else
	{
		cerr << "Unhandled row buffer management policy" << endl;
		return FAILURE;
	}
	return SUCCESS;
}


void dram_system::set_dram_timing_specification(enum dram_type_t dram_type)
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

	if(dram_type == SDRAM){						/* @ 100 MHz */
		t_al = 0;		/* no such thing as posted CAS in SDRAM */
		t_burst = 8;		/* depending on system config! can be 1, 2, 4, or 8 */
		t_cas = 2;
		t_cmd = 1;		/* protocol specific, cannot be changed */
		t_cwd = 0;		/* no such thing in SDRAM */
		t_int_burst = 1;		/* prefetch length is 1 */
		t_faw = 0;		/* no such thing in SDRAM */
		t_ras = 5;		/* */
		t_rc = 7;		
		t_rcd = 2;
		t_rfc = 7;		/* same as t_rc */
		t_rp = 2;		/* 12 ns @ 1.25ns per cycle = 9.6 cycles */
		t_rrd = 0;		/* no such thing in SDRAM */
		t_rtp = 1;
		t_rtrs = 0;		/* no such thing in SDRAM */
		t_wr = 2;		

	} else if (dram_type == DDR){					/* @ 200 MHz (400 Mbps) */
		t_al = 0;		/* no such thing in DDR */
		t_burst = 8;		/* depending on system config! can be 2, 4, or 8 */
		t_cas = 6;
		t_cmd = 2;		/* protocol specific, cannot be changed */
		t_cwd = 2;		/* protocol specific, cannot be changed */
		t_int_burst = 2;		/* protocol specific, cannot be changed */
		t_faw = 0;		/* no such thing in DDR */
		t_ras = 16;		/* 40 ns @ 2.5 ns per beat == 16 beats */
		t_rc = 22;		/* 55 ns t_rc */
		t_rcd = 6;
		t_rfc = 28;		/* 70 ns @ 2.5 ns per beat == 28 beats */
		t_rp = 6;		/* 15 ns @ 2.5ns per beat = 6 beats */
		t_rrd = 0;
		t_rtp = 2;
		t_rtrs = 2;
		t_wr = 6;		/* 15 ns @ 2.5 ns per beat = 6 beats */
		t_wtr = 4;

	} else if (dram_type == DDR2){					/* @ 800 Mbps */
		t_al = 0;
		t_burst = 8;		/* depending on system config! can be 4, or 8 */
		t_cas = 10;
		t_cmd = 2;		/* protocol specific, cannot be changed */
		t_cwd = t_cas - 2;	/* protocol specific, cannot be changed */
		t_int_burst = 4;		/* protocol specific, cannot be changed */
		t_faw = 30;
		t_ras = 36;		/* 45 ns @ 1.25ns per beat = 36 beats */
		t_rc = 46;		/* 57 ns @ 1.25ns per beat = 45.6 beats */
		t_rcd = 10;
		t_rfc = 102;		/* 128 ns @ 1.25ns per beat ~= 102 beats */
		t_rp = 10;		/* 12 ns @ 1.25ns per beat = 9.6 beats */
		t_rrd = 6;		/* 7.5 ns */
		t_rtp = 6;
		t_rtrs = 2;
		t_wr = 12;
		t_wtr = 6;

	} else if (dram_type == DDR3){					/* @ 1.33 Gbps = 0.75 ns per beat */
		t_al = 0;
		t_burst = 8;		/* protocol specific, cannot be changed */
		t_cas = 10;
		t_cmd = 2;		/* protocol specific, cannot be changed */
		t_cwd = t_cas - 2;
		t_int_burst = 8;		/* protocol specific, cannot be changed */
		t_faw = 30;
		t_ras = 36;		/* 27 ns @ 0.75ns per beat = 36 beats */
		t_rc = 48;		/* 36 ns @ 0.75ns per beat = 48 beats */
		t_rcd = 12;
		t_rfc = 280;
		t_rp = 12;		/* 9 ns @ 0.75ns per beat = 12 beats */
		t_rrd = 8;
		t_rtrs = 2;
		t_rtp = 8;
		t_wr = 12;
		t_wtr = 8;

	} else if (dram_type == DRDRAM){					/* FIXME */
		/* not currently supported */
		cerr << "Not yet supported" << endl;
	}
}


void DRAMSim2::dram_system::run_simulations()
{
	bool EOF_reached = false;

	for(int i = 0; (i < sim_parameters.get_request_count()) && (EOF_reached == false); ++i)
	{
		transaction 	*input_t;
		if (get_next_input_transaction(input_t) == SUCCESS)
		{
			statistics.collect_transaction_stats(input_t);

			while (channel[input_t->addr.chan_id].enqueue(input_t) == FAILURE)
			{
				/* tried to stuff req in channel queue, stalled, so try to drain channel queue first */
				/* and drain it completely */
				/* unfortunately, we may have to drain another channel first. */
				int oldest_chan_id = find_oldest_channel();
				transaction *temp_t = channel[oldest_chan_id].get_transaction();

#ifdef DEBUG_FLAG
				cerr << "CH[" << setw(2) << oldest_chan_id << "] " << temp_t << endl;
#endif

				if(temp_t != NULL)
				{
					//rank_id = temp_t->addr.rank_id;
					//bank_id = temp_t->addr.bank_id;
					//bank_q = (((channel[oldest_chan_id]).rank[temp_t->addr.rank_id]).bank[bank_id]).per_bank_q;
					// if it can't fit in the transaction queue, drain the queue
					while(transaction2commands(temp_t) != SUCCESS)
					{
						command *temp_c = get_next_command(oldest_chan_id);
						int min_gap = min_protocol_gap(input_t->addr.chan_id, temp_c);

#ifdef DEBUG_FLAG
						cerr << "[" << setbase(10) << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

						execute_command(temp_c, min_gap);
						update_system_time(); 
						transaction *completed_t = channel[oldest_chan_id].complete();
						if(completed_t != NULL)
							free_transaction_pool.release_item(completed_t);
					}
				}
				/* randomness check
				for(rank_id = 0; rank_id < config->rank_count;rank_id++){
				for(bank_id = 0; bank_id < config->bank_count; bank_id++){
				fprintf(stderr,"[%d|%d|%d] ",
				(((channel[0]).rank[rank_id]).bank[bank_id]).cas_count,
				(((channel[0]).rank[rank_id]).bank[bank_id]).casw_count,
				(((channel[0]).rank[rank_id]).bank[bank_id]).cas_count +
				(((channel[0]).rank[rank_id]).bank[bank_id]).casw_count);
				}
				fprintf(stderr,"X");
				}
				fprintf(stderr,"\n");
				*/
			}
		}
		else
			EOF_reached = true;
	}

	statistics.set_end_time(time);
	statistics.set_valid_trans_count(sim_parameters.get_request_count());
}





dram_system::dram_system(map<file_io_token_t,string> &parameter):
system_config(parameter),
timing_specification(parameter),
sim_parameters(parameter),
free_command_pool(4*COMMAND_QUEUE_SIZE,true), /// place to temporarily dump unused command structures */
free_transaction_pool(4*COMMAND_QUEUE_SIZE,true), /// ditto, but for transactions, avoid system calls during runtime
free_event_pool(COMMAND_QUEUE_SIZE,true), /// create enough events, transactions and commands ahead of time
event_q(COMMAND_QUEUE_SIZE),
input_stream(parameter),
time(0) /// start the clock

{
	//time = 0;                  /* start the clock */

	map<file_io_token_t, string>::iterator temp;
	stringstream temp2;

	if ((temp=parameter.find(output_file_token))!=parameter.end())
		output_filename = parameter[output_file_token];


	// create as many channels as were specified, all of the same type
	// now that the parameters for each have been set
	channel =  new dram_channel[system_config.chan_count];
	for (int i = 0; i < system_config.chan_count; ++i)
		channel[i].init_controller(system_config.transaction_queue_depth,
		system_config.history_queue_depth,
		system_config.completion_queue_depth,
		system_config.refresh_queue_depth,
		system_config.rank_count,
		system_config.bank_count,
		system_config.per_bank_queue_depth);
	algorithm.init(free_command_pool, system_config.rank_count, system_config.bank_count, system_config.config_type);

}



int DRAMSim2::dram_system::find_oldest_channel() const
{
	int oldest_chan_id = 0;
	tick_t oldest_time = channel[0].get_time();

	for (int chan_id = 1; chan_id < system_config.chan_count ; ++chan_id)
	{
		if(channel[chan_id].get_time() < oldest_time)
		{
			oldest_chan_id = chan_id;
			oldest_time = channel[chan_id].get_time();
		}
	}
	return oldest_chan_id;
}


void DRAMSim2::dram_system::run_simulations2()
{
	bool EOF_reached = false;

	for(int i = 0; (i < sim_parameters.get_request_count()) ; ++i)
	{
		transaction 	*input_t;

		// see if it is time for the channel to arrive, if so, put in the channel
		// queue
		// if not, either make the channels wait and move time to the point where
		// the transaction should arrive or execute commands until that time happens
		// make sure not to overshoot the time by looking at when a transaction would end
		// so that executing one more command doesn't go too far forward
		// this only happens when there is the option to move time or execute commands
		if (get_next_input_transaction(input_t) == SUCCESS)
		{
			// record stats
			statistics.collect_transaction_stats(input_t);

			// decide whether to move time ahead by waiting or executing
			int oldest_chan_id = find_oldest_channel();
			transaction *temp_t = channel[oldest_chan_id].get_transaction();
			command *temp_c = get_next_command(oldest_chan_id);
			// min gap is how long until the next command finishes
			int min_gap = min_protocol_gap(input_t->addr.chan_id, temp_c);

			while (channel[input_t->addr.chan_id].enqueue(input_t) == FAILURE)
			{
				/* tried to stuff req in channel queue, stalled, so try to drain channel queue first */
				/* and drain it completely */
				/* unfortunately, we may have to drain another channel first. */
				
				int oldest_chan_id = find_oldest_channel();
				transaction *temp_t = channel[oldest_chan_id].get_transaction();

#ifdef DEBUG_FLAG
				cerr << "CH[" << setw(2) << oldest_chan_id << "] " << temp_t << endl;
#endif

				if(temp_t != NULL)
				{
					// if it can't fit in the transaction queue, drain the queue
					while(transaction2commands(temp_t) != SUCCESS)
					{
						command *temp_c = get_next_command(oldest_chan_id);
						int min_gap = min_protocol_gap(input_t->addr.chan_id, temp_c);

#ifdef DEBUG_FLAG
						cerr << "[" << setbase(10) << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

						execute_command(temp_c, min_gap);
						update_system_time(); 
						transaction *completed_t = channel[oldest_chan_id].complete();
						if(completed_t != NULL)
							free_transaction_pool.release_item(completed_t);
					}
				}
			}
		}
		else
			// EOF reached, quit the loop
			break;
	}

	statistics.set_end_time(time);
	statistics.set_valid_trans_count(sim_parameters.get_request_count());
}




