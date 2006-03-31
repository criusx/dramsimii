#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>

#include "dramsim2.h"


dram_channel::dram_channel()
{
	//int chan_count = this_c->chan_count;
	//this_dc = (dram_channel_t *)calloc(chan_count,sizeof(dram_channel_t));
	/* initialize all controllers */
	//for(int i=0;i<chan_count;i++){
	//temp_dc = &(this_dc[i]);
	time 			= 0;
	last_refresh_time	= 0;
	refresh_row_index	= 0;
	last_rank_id 		= 0;
	//rank = new rank_c[rank_count];
	//for (int i=0;i<rank_count;i++)
	//  rank[i].init_ranks(bank_count,per_bank_queue_depth);
	//(bank_count,per_bank_queue_depth);
	//rank = init_ranks(this_c);
	//channel_q 	= init_q(transaction_queue_depth);
	//history_q 	= init_q(history_queue_depth);
	//completion_q 	= init_q(completion_queue_depth);
	//refresh_q 	= init_q(refresh_queue_depth);
	//}
}

void dram_channel::init_controller(int transaction_queue_depth,
								   int history_queue_depth,
								   int completion_queue_depth,
								   int refresh_queue_depth,
								   int rank_count,
								   int bank_count,
								   int per_bank_queue_depth)
{
	channel_q.init(transaction_queue_depth);
	history_q.init(history_queue_depth);
	completion_q.init(completion_queue_depth);
	refresh_q.init(refresh_queue_depth);

	time = 0;
	last_refresh_time	= 0;
	refresh_row_index	= 0;
	last_rank_id = 0;
	rank = new rank_c[rank_count];
	for (int i=0;i<rank_count;i++)
		rank[i].init_ranks(bank_count,per_bank_queue_depth);
}



enum transaction_type_t	dram_channel::set_read_write_type(int rank_id,int bank_count)
{
	int read_count = 0;
	int write_count = 0;
	int empty_count = 0;

	for(int i = 0; i < bank_count; ++i)
	{
		command *temp_c =  rank[rank_id].bank[i].per_bank_q.read(1);

		if(temp_c != NULL)
		{
			if(temp_c->this_command == CAS_AND_PRECHARGE_COMMAND)
			{
				++read_count;
			}
			else
			{
				++write_count;
			}
		}
		else
		{
			++empty_count;
		}
	}
#ifdef DEBUG_FLAG
	cerr << "Rank [" << rank_id << "] Read[" << read_count << "] Write[" << write_count << "]" << endl;
#endif
	if(read_count >= write_count)
		return READ_TRANSACTION;
	else
		return WRITE_TRANSACTION;
}

dram_channel::~dram_channel()
{
	delete[] rank;
}
