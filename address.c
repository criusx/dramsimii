/*
 * address.c - address conversion functions. takes a mapping policy and an
 * address and parses address data
 *
 */

#include "dramsim.h"

/* Find a cleaner way to code this int log2 function */

int intlog2(int input){
	int result ;
	result = INVALID;
	if(input == 1){
		result = 0;
	} else if (input == 2){ 		result = 1;
	} else if (input == 4){ 		result = 2;
	} else if (input == 8){ 		result = 3;
	} else if (input == 16){ 		result = 4;
	} else if (input == 32){ 		result = 5;
	} else if (input == 64){ 		result = 6;
	} else if (input == 128){ 		result = 7;
	} else if (input == 256){ 		result = 8;
	} else if (input == 512){ 		result = 9;
	} else if (input == 1024){ 		result = 10;
	} else if (input == 2048){ 		result = 11;
	} else if (input == 4096){ 		result = 12;
	} else if (input == 8192){ 		result = 13;
	} else if (input == 16384){ 		result = 14;
	} else if (input == 32768){ 		result = 15;
	} else if (input == 65536){ 		result = 16;
	} else if (input == 65536*2){ 		result = 17;
	} else if (input == 65536*4){ 		result = 18;
	} else if (input == 65536*8){ 		result = 19;
	} else if (input == 65536*16){ 		result = 20;
	} else if (input == 65536*32){ 		result = 21;
	} else if (input == 65536*64){ 		result = 22;
	} else if (input == 65536*128){ 	result = 23;
	} else if (input == 65536*256){	 	result = 24;
	} else if (input == 65536*512){ 	result = 25;
	} else if (input == 65536*1024){ 	result = 26;
	} else if (input == 65536*2048){ 	result = 27;
	} else if (input == 65536*4096){ 	result = 28;
	} else if (input == 65536*8192){ 	result = 29;
	} else {
		fprintf(stderr,"Error, %d is not a nice power of 2\n",input);
		_exit(2);
	}
	return result;
}

int convert_address(dram_system_configuration_t *config, addresses_t     *this_a){
	unsigned int input_a;
	unsigned int temp_a, temp_b;
	unsigned int bit_15,bit_27,bits_26_to_16;
	int	mapping_scheme;
	int	chan_count, rank_count, bank_count, col_count, row_count;
	int	chan_addr_depth, rank_addr_depth, bank_addr_depth, row_addr_depth, col_addr_depth; 
	int	col_size, col_size_depth;
	
	mapping_scheme = config->addr_mapping_scheme;

	if(config->dram_type == DRDRAM){
		col_size	= 16;
	} else {
		col_size 	= config->col_size;
	}
	chan_count = config->chan_count;
	rank_count = config->rank_count;
	bank_count = config->bank_count;
	row_count  = config->row_count;
	col_count  = config->col_count;
	chan_addr_depth = intlog2(chan_count);
	rank_addr_depth = intlog2(rank_count);
	bank_addr_depth = intlog2(bank_count);
	row_addr_depth  = intlog2(row_count);
	col_addr_depth  = intlog2(col_count);
	col_size_depth	= intlog2(col_size);

	input_a = this_a->phys_addr;
	input_a = input_a >> col_size_depth;

	if(mapping_scheme == BURGER_BASE_MAP){		/* Good for only Rambus memory really */
		/* BURGER BASE : 
		 * |<-------------------------------->|<------>|<------>|<---------------->|<----------------->|<----------->|
		 *                           row id     bank id   Rank id   Column id         Channel id          Byte Address 
		 *                                                          DRAM page size/   intlog2(chan. count)   within packet
		 *                                                          Bus Width         used if chan. > 1   
		 *     
		 *               As applied to system (1 chan) using 256 Mbit RDRAM chips:
		 *               512 rows X 32 banks X 128 columns X 16 bytes per column.
		 *		 16 ranks gets us to 512 MByte.	
		 *
		 *    31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		 *             |<---------------------->| |<---------->| |<------->| |<---------------->|  |<------>|
		 *                      row id                 bank         rank          Col id            16 byte 
		 *                      (512 rows)              id           id           2KB/16B            packet
		 */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		this_a->chan_id = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_addr_depth;
		temp_a  = input_a << col_addr_depth;
		this_a->col_id = temp_a ^ temp_b;     		/* strip out the column address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		this_a->rank_id = temp_a ^ temp_b;		/* this should strip out the rank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		this_a->bank_id = temp_a ^ temp_b;		/* this should strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		this_a->row_id = temp_a ^ temp_b;		/* this should strip out the row address */
		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			fprintf(stderr,"address out of range[%X] of available physical memory\n",this_a->phys_addr);
		}
	} else if((mapping_scheme == SDRAM_HIPERF_MAP) || (mapping_scheme == OPEN_PAGE_BASELINE)){	/* works for SDRAM, DDR, DDR2 and DDR3 ! */
		
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

		int cacheline_size;
		int cacheline_size_depth;	/* address bit depth */
		int col_id_lo;
		int col_id_lo_depth;
		int col_id_hi;
		int col_id_hi_depth;

		cacheline_size = config->cacheline_size;
		cacheline_size_depth = intlog2(cacheline_size);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_lo_depth;
		temp_a  = input_a << col_id_lo_depth;
		col_id_lo = temp_a ^ temp_b;     		/* strip out the column low address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		this_a->chan_id = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_hi_depth;
		temp_a  = input_a << col_id_hi_depth;
		col_id_hi = temp_a ^ temp_b;     		/* strip out the column hi address */

		this_a->col_id = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		this_a->bank_id = temp_a ^ temp_b;     		/* strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		this_a->rank_id = temp_a ^ temp_b;     		/* strip out the rank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		this_a->row_id = temp_a ^ temp_b;		/* this should strip out the row address */
		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			fprintf(stderr,"address out of range[%X] of available physical memory\n",this_a->phys_addr);
			print_addresses(this_a);
		}
	} else if(mapping_scheme == SDRAM_BASE_MAP){		/* works for SDRAM and DDR SDRAM too! */
		
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

		int cacheline_size;
		int cacheline_size_depth;	/* address bit depth */
		int col_id_lo;
		int col_id_lo_depth;
		int col_id_hi;
		int col_id_hi_depth;

		cacheline_size = config->cacheline_size;
		cacheline_size_depth = intlog2(cacheline_size);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_lo_depth;
		temp_a  = input_a << col_id_lo_depth;
		col_id_lo = temp_a ^ temp_b;     		/* strip out the column low address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		this_a->chan_id = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_hi_depth;
		temp_a  = input_a << col_id_hi_depth;
		col_id_hi = temp_a ^ temp_b;     		/* strip out the column hi address */

		this_a->col_id = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		this_a->bank_id = temp_a ^ temp_b;     		/* strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		this_a->row_id = temp_a ^ temp_b;		/* this should strip out the row address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		this_a->rank_id = temp_a ^ temp_b;     		/* strip out the rank address */

		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			fprintf(stderr,"address out of range[%X] of available physical memory\n",this_a->phys_addr);
		}
	} else if(mapping_scheme == INTEL845G_MAP){

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
		input_a = this_a->phys_addr >> 3;	/* strip away byte address */

		temp_b = input_a;				/* save away what is left of original address */
		input_a = input_a >> 10;
		temp_a  = input_a << 10;				/* 11-3 */
		this_a->col_id = temp_a ^ temp_b;     		/* strip out the column address */

		temp_b = input_a;				/* save away what is left of original address */
		input_a = input_a >> 2;
		temp_a  = input_a << 2;				/* 14:13 */
		this_a->bank_id = temp_a ^ temp_b;		/* strip out the bank address */

		temp_b = this_a->phys_addr >> 15;
		input_a =  temp_b >> 1;
		temp_a  = input_a << 1;				/* 15 */
		bit_15 = temp_a ^ temp_b;			/* strip out bit 15, save for later  */

		temp_b = this_a->phys_addr >> 16;
		input_a =  temp_b >> 11;
		temp_a  = input_a << 11;			/* 26:16 */
		bits_26_to_16 = temp_a ^ temp_b;		/* strip out bits 26:16, save for later  */

		temp_b = this_a->phys_addr >> 27;
		input_a =  temp_b >> 1;
		temp_a  = input_a << 1;				/* 27 */
		bit_27 = temp_a ^ temp_b;			/* strip out bit 27 */

		this_a->row_id = (bit_27 << 13) | (bit_15 << 12) | bits_26_to_16 ;

		temp_b = this_a->phys_addr >> 28;
		input_a = temp_b >> 2;
		temp_a  = input_a << 2;				/* 29:28 */
		this_a->rank_id = temp_a ^ temp_b;		/* strip out the rank id */

		this_a->chan_id = 0;				/* Intel 845G has only a single channel dram controller */
		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			fprintf(stderr,"address out of range[%X] of available physical memory\n",this_a->phys_addr);
		}

	} else if(mapping_scheme == SDRAM_CLOSE_PAGE_MAP){	
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
		int cacheline_size;
		int cacheline_size_depth;	/* address bit depth */
		int col_id_lo;
		int col_id_lo_depth;
		int col_id_hi;
		int col_id_hi_depth;

		cacheline_size = config->cacheline_size;
		cacheline_size_depth = intlog2(cacheline_size);

		col_id_lo_depth = cacheline_size_depth - col_size_depth;
		col_id_hi_depth = col_addr_depth - col_id_lo_depth;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_lo_depth;
		temp_a  = input_a << col_id_lo_depth;
		col_id_lo = temp_a ^ temp_b;     		/* strip out the column low address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> chan_addr_depth;
		temp_a  = input_a << chan_addr_depth;
		this_a->chan_id = temp_a ^ temp_b;     		/* strip out the channel address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> bank_addr_depth;
		temp_a  = input_a << bank_addr_depth;
		this_a->bank_id = temp_a ^ temp_b;     		/* strip out the bank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> rank_addr_depth;
		temp_a  = input_a << rank_addr_depth;
		this_a->rank_id = temp_a ^ temp_b;     		/* strip out the rank address */

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> col_id_hi_depth;
		temp_a  = input_a << col_id_hi_depth;
		col_id_hi = temp_a ^ temp_b;     		/* strip out the column hi address */

		this_a->col_id = (col_id_hi << col_id_lo_depth) | col_id_lo;

		temp_b = input_a;				/* save away original address */
		input_a = input_a >> row_addr_depth;
		temp_a  = input_a << row_addr_depth;
		this_a->row_id = temp_a ^ temp_b;		/* this should strip out the row address */
		if(input_a != 0){				/* If there is still "stuff" left, the input address is out of range */
			fprintf(stderr,"address out of range[%X] of available physical memory\n",this_a->phys_addr);
		}
	} else {
		fprintf(stderr,"%d\n",mapping_scheme);
		this_a->chan_id = 0;				/* don't know what this policy is.. Map everything to 0 */
		this_a->rank_id = 0;
		this_a->bank_id = 0;
		this_a->row_id  = 0;
		this_a->col_id  = 0;
	}
	return 1;
}

void print_addresses(	addresses_t     *this_a){
	fprintf(stderr,"addr[0x%X] chan[%X] rank[%X] bank[%2X] row[%4X] col[%X]\n",
				this_a->phys_addr,
				this_a->chan_id,
				this_a->rank_id,
				this_a->bank_id,
				this_a->row_id,
				this_a->col_id);
}
