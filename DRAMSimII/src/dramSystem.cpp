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
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "dramSystem.h"

using namespace std;
using namespace DRAMSimII;

// returns the time when the memory system next has an event
// the event may either be a conversion of a transaction into commands
// or it may be the next time a command may be issued
// returns: TICK_T_MAX if there is no next event, or the time of the next event
tick_t dramSystem::nextTick() const
{
	tick_t nextWake = TICK_T_MAX;

	// find the next time to wake from among all the channels
	for (vector<dramChannel>::const_iterator currentChan = channel.begin(); currentChan != channel.end(); currentChan++)
	{
		// first look for transactions
		if (transaction *nextTrans = currentChan->read_transaction())
		{
			// make sure it can finish
			int tempGap = max(1,(int)(nextTrans->enqueueTime - currentChan->get_time()) + currentChan->getTimingSpecification().t_buffer_delay); 

			assert(nextTrans->enqueueTime <= currentChan->get_time());
			assert(tempGap <= currentChan->getTimingSpecification().t_buffer_delay );
			// whenever the next transaction is ready and there are available slots for the R/C/P commands
			if ((tempGap + currentChan->get_time() < nextWake) && (checkForAvailableCommandSlots(nextTrans)))
			{
				nextWake = tempGap + currentChan->get_time();
			}
		}

		// then check to see when the next command occurs
		if (command *tempCommand = currentChan->readNextCommand())
		{
			int tempGap = currentChan->minProtocolGap(tempCommand);

			if (tempGap + currentChan->get_time() < nextWake)
			{
				nextWake = tempGap + currentChan->get_time();
			}
		}
	}

	return nextWake;
}

dramSystem& dramSystem::operator =(const DRAMSimII::dramSystem &rs)
{
	if (this == &rs)
	{
		return *this;
	}
	system_config = rs.system_config;
	channel = rs.channel;
	sim_parameters = rs.sim_parameters;
	statistics = rs.statistics;
	//input_stream = rs.input_stream;
	time = rs.time;
	event_q = rs.event_q;

	return *this;
}

bool dramSystem::convert_address(addresses &this_a) const
{
	unsigned input_a;
	unsigned temp_a, temp_b;
	unsigned bit_15,bit_27,bits_26_to_16;

	// if there's a test involving specific ranks/banks and the mapping is predetermined
	if (input_stream.getType() == MAPPED)
		return true;
	//int	mapping_scheme;
	//int	chan_count, rank_count, bank_count, col_count, row_count;
	//int	chan_addr_depth, rank_addr_depth, bank_addr_depth, row_addr_depth, col_addr_depth;
	
	//, col_size_depth;

	//mapping_scheme = config->addr_mapping_scheme;

	

	//chan_count = config->chan_count;
	//rank_count = config->rank_count;
	//bank_count = config->bank_count;
	//row_count  = config->row_count;
	//col_count  = config->col_count;
	unsigned chan_addr_depth = log2(system_config.getChannelCount());
	unsigned rank_addr_depth = log2(system_config.getRankCount());
	unsigned bank_addr_depth = log2(system_config.getBankCount());
	unsigned row_addr_depth  = log2(system_config.getRowCount());
	unsigned col_addr_depth  = log2(system_config.getColumnCount());
	//FIXME: shouldn't this already be set appropriately?
	unsigned col_size_depth	= log2(system_config.getDRAMType() == DRDRAM ? 16 : system_config.getColumnSize());

	//input_a = this_a->phys_addr;
	// strip away the byte address portion
	input_a = this_a.phys_addr >> col_size_depth;

	//unsigned int address = static_cast<unsigned>(this_a.phys_addr);

	unsigned cacheline_size;
	unsigned cacheline_size_depth;	/* address bit depth */
	unsigned col_id_lo;
	unsigned col_id_lo_depth;
	unsigned col_id_hi;
	unsigned col_id_hi_depth;

	switch (system_config.getAddressMappingScheme())
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

		cacheline_size = system_config.getCachelineSize();
		cacheline_size_depth = log2(cacheline_size);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		cerr << "cacheline_size_depth " << cacheline_size_depth << " col_size_depth " << col_size_depth << " col_size " << system_config.getColumnSize() << endl;
		cerr << "system_config_addr " << &system_config << endl;
		//cerr << "cacheline_size " << cacheline_size << " cacheline_size_depth " << cacheline_size_depth << " col_id_lo_depth " << col_id_lo_depth << " col_id_hi_depth" << col_id_hi_depth << " chan_addr_depth " << chan_addr_depth << endl;
		
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



		cacheline_size = system_config.getCachelineSize();
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

		cacheline_size_depth = log2(system_config.getCachelineSize());

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
		cerr << "Unknown address mapping scheme, mapping chan, rank, bank to zero: " << system_config.getAddressMappingScheme() << endl;
		this_a.chan_id = 0;				/* don't know what this policy is.. Map everything to 0 */
		this_a.rank_id = 0;
		this_a.bank_id = 0;
		this_a.row_id  = 0;
		this_a.col_id  = 0;
		break;
	}
	cerr << this_a << endl;
	return true;
}

/// <summary>
/// Updates the system time to be the same as that of the oldest channel
/// Although some channels may be farther along, selecting the oldest channel
/// when doing updates ensures that all channels stay reasonably close to one another
/// </summary>
void dramSystem::update_system_time()
{
	int oldest_chan_id = system_config.getChannelCount() - 1;
	tick_t oldest_time = channel[oldest_chan_id].get_time();

	for (int chan_id = system_config.getChannelCount() - 2; chan_id >= 0; chan_id--)
	{
		if (channel[chan_id].get_time() < oldest_time)
		{
			oldest_chan_id = chan_id;
			oldest_time = channel[chan_id].get_time();
		}
	}

	time = oldest_time;
}


void dramSystem::getNextRandomRequest(transaction *this_t)
{
	if (input_stream.getType() == RANDOM)
	{
		unsigned int j;

		rand_s(&j);

		// check against last transaction to see what the chan_id was, and whether we need to change channels or not
		if (input_stream.getChannelLocality() * UINT_MAX < j)
		{
			this_t->addr.chan_id = (this_t->addr.chan_id + (j % (system_config.getChannelCount() - 1))) % system_config.getChannelCount();
		}

		// check against the rank_id of the last transaction to the newly selected channel to see if we need to change the rank_id
		// or keep to this rank_id 

		int rank_id = channel[this_t->addr.chan_id].get_last_rank_id();

		rand_s(&j);
		if ((input_stream.getRankLocality() * UINT_MAX < j) && (system_config.getRankCount() > 1))
		{
			rank_id = this_t->addr.rank_id = 
				(rank_id + 1 + (j % (system_config.getRankCount() - 1))) % system_config.getRankCount();
		}
		else
		{
			this_t->addr.rank_id = rank_id;
		}

		int bank_id = channel[this_t->addr.chan_id].getRank(rank_id).last_bank_id;

		rand_s(&j);

		if ((input_stream.getBankLocality() * UINT_MAX < j) && (system_config.getBankCount() > 1))
		{
			bank_id = this_t->addr.bank_id =
				(bank_id + 1 + (j % (system_config.getBankCount() - 1))) % system_config.getBankCount();
		}
		else
		{
			this_t->addr.bank_id = bank_id;
		}

		int row_id = channel[this_t->addr.chan_id].getRank(rank_id).bank[bank_id].row_id;

		rand_s(&j);

		if (input_stream.getRowLocality() * UINT_MAX < j)
		{
			this_t->addr.row_id = (row_id + 1 + (j % (system_config.getRowCount() - 1))) % system_config.getRowCount();
			row_id = this_t->addr.row_id;
		}
		else
		{
			this_t->addr.row_id = row_id;
		}

		rand_s(&j);

		if (input_stream.getReadPercentage() * UINT_MAX > j)
		{
			this_t->type = READ_TRANSACTION;
		}
		else
		{
			this_t->type = WRITE_TRANSACTION;
		}

		rand_s(&j);

		if (input_stream.getShortBurstRatio() * UINT_MAX > j)
		{
			this_t->length = 4;
		}
		else
		{
			this_t->length = 8;
		}

		//int time_gap = 0;

		while (true)
		{
			rand_s(&j);

			if (j > input_stream.getArrivalThreshhold() * UINT_MAX) /* interarrival probability function */
			{

				// Gaussian distribution function
				if (input_stream.getInterarrivalDistributionModel() == GAUSSIAN_DISTRIBUTION)
				{
					input_stream.setArrivalThreshhold(1.0F - (1.0F / input_stream.box_muller(input_stream.getAverageInterarrivalCycleCount(), 10)));
				}
				// Poisson distribution function
				else if (input_stream.getInterarrivalDistributionModel() == POISSON_DISTRIBUTION)
				{
					input_stream.setArrivalThreshhold(1.0F - (1.0F / input_stream.poisson_rng(input_stream.getAverageInterarrivalCycleCount())));
				}
				break;
			}
			else
			{
				input_stream.setTime(input_stream.getTime() + 1);
			}
		}

		this_t->arrival_time = input_stream.getTime();
		this_t->addr.col_id = 0;
	}
}

enum input_status_t dramSystem::getNextIncomingTransaction(transaction *&this_t)
{
	static transaction *temp_t;

	if (temp_t == NULL)
	{
		temp_t = new transaction;

		switch (input_stream.getType())
		{
		case RANDOM:
			getNextRandomRequest(temp_t);
			break;
		case K6_TRACE:
		case MASE_TRACE:
		case MAPPED:
			{
				static busEvent this_e;

				if(input_stream.getNextBusEvent(this_e) == FAILURE)
				{
					/* EOF reached */
					delete temp_t;
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
			break;
		default:
			cerr << "Unknown input trace format" << endl;
			exit(-20);
			break;
		}
	}

	if (system_config.getRefreshPolicy() == NO_REFRESH)
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

dramSystem::dramSystem(const dramSettings *settings): 
system_config(settings),
channel(system_config.getChannelCount(),
		dramChannel(settings)),
		sim_parameters(settings),
		statistics(),
		input_stream(settings),
		time(0),
		event_q(COMMAND_QUEUE_SIZE)
{
	if (settings->outFileType == BZ)
	{	
		outStream.push(boost::iostreams::bzip2_compressor());
		outStream.push(boost::iostreams::file_sink((settings->outFile + ".bz").c_str()));

		if (!outStream.good())
		{
			cerr << "Error opening file \"" << settings->outFile << "\" for writing" << endl;
			exit(-12);
		}
	}
	else if (settings->outFileType == GZ)
	{
		outStream.push(boost::iostreams::gzip_compressor());
		outStream.push(boost::iostreams::file_sink((settings->outFile + ".gz").c_str()));

		if (!outStream.good())
		{
			cerr << "Error opening file \"" << settings->outFile << "\" for writing" << endl;
			exit(-12);
		}
	}
	else if (settings->outFileType == UNCOMPRESSED)
	{
		outStream.push(boost::iostreams::file_sink(settings->outFile.c_str()));

		if (!outStream.good())
		{
			cerr << "Error opening file \"" << settings->outFile << "\" for writing" << endl;
			exit(-12);
		}
	}
	else if (settings->outFileType == COUT) 
	{
		outStream.push(std::cout);
	}


	// don't init the refresh queues if there's no need
	if (system_config.getRefreshPolicy() != NO_REFRESH)
	{
		// init the refresh queue for each channel and set the system_config pointers
		unsigned cnt = 0;
		for (vector<dramChannel>::iterator i = channel.begin(); i != channel.end(); i++)
		{
			i->initRefreshQueue(settings->rowCount, settings->refreshTime, cnt++);			
		}
	}
	for (vector<dramChannel>::iterator i = channel.begin(); i != channel.end(); i++)
	{
		i->setSystemConfig(&system_config);
	}
	// set the channelID so that each channel may know its ordinal value
	for (unsigned i = 0; i < settings->channelCount; i++)
	{
		channel[i].setChannelID(i);
	}
}

int dramSystem::findOldestChannel() const
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

ostream &DRAMSimII::operator<<(ostream &os, const dramSystem &this_a)
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

	os << this_a.statistics;
	os << this_a.system_config;

	return os;
}


// do the power calculation on all the channels
void dramSystem::doPowerCalculation()
{
	for (vector<dramChannel>::iterator currentChannel = channel.begin(); currentChannel != channel.end(); currentChannel++)
	{
		currentChannel->doPowerCalculation();
	}
}

