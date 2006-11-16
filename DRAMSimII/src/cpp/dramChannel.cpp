#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>

#include "dramChannel.h"

using namespace std;

dramChannel::dramChannel(int transaction_queue_depth,
						   int history_queue_depth,
						   int completion_queue_depth,
						   int refresh_queue_depth,
						   int rank_count,
						   int bank_count,
						   int per_bank_queue_depth):
transaction_q(transaction_queue_depth),
history_q(history_queue_depth),
completion_q(completion_queue_depth),
refreshQueue(refresh_queue_depth,true),
rank(rank_count,rank_c(bank_count,per_bank_queue_depth))
{	
	time = 0;
	last_refresh_time = 0;
	refresh_row_index = 0;
	last_rank_id = 0;

	//transaction_q.init(transaction_queue_depth);
	//history_q.init(history_queue_depth);
	//completion_q.init(completion_queue_depth);



	//rank = new rank_c[rank_count]; // FIXME: consider converting this array to a vector
	// std::vector<rank_c> rank(rank_count, rank_c(bank_count,per_bank_queue_depth));
	//for (int i=0;i<rank_count;i++)
	//	rank.push_back(rank_c(bank_count,per_bank_queue_depth));
	//	rank[i].init_ranks(bank_count,per_bank_queue_depth);

	//refreshQueue.init(refresh_queue_depth,  true);
}

dramChannel::dramChannel(const dramChannel &dc): 
transaction_q(dc.transaction_q),
refreshQueue(dc.refreshQueue),
history_q(dc.history_q),
completion_q(dc.completion_q),
time(dc.time),
rank(dc.rank),
refresh_row_index(dc.refresh_row_index),
last_refresh_time(dc.last_refresh_time),
last_rank_id(dc.last_rank_id)
{
	//time = dc.time;
	//rank = dc.rank;
	//refresh_row_index = dc.refresh_row_index;
	//last_refresh_time = dc.last_refresh_time;
	//last_rank_id = dc.last_rank_id;
	//transaction_q = dc.transaction_q;
	//refreshQueue = dc.refreshQueue;
	//history_q = dc.history_q;
	//completion_q = dc.completion_q;
}

void dramChannel::init_controller(int transaction_queue_depth,
								  int history_queue_depth,
								  int completion_queue_depth,
								  int refresh_queue_depth,
								  int rank_count,
								  int bank_count,
								  int per_bank_queue_depth)
{
	transaction_q.init(transaction_queue_depth);
	history_q.init(history_queue_depth);
	completion_q.init(completion_queue_depth);
	

	time = 0;
	last_refresh_time = 0;
	refresh_row_index = 0;
	last_rank_id = 0;

	
	refreshQueue.init(refresh_queue_depth,  true);
}

void dramChannel::initRefreshQueue(const unsigned rowCount,
								   const unsigned refreshTime,
								   const unsigned channel)
{
	unsigned step = refreshTime;
	step /= rowCount;
	step /= rank.size();
	int count = 0;

	for (int i = rowCount - 1; i >= 0; i--)
		for (int j = rank.size() - 1; j >= 0; j--)
		{
			refreshQueue.read(count)->arrival_time = count * step;
			refreshQueue.read(count)->type = AUTO_REFRESH_TRANSACTION;
			refreshQueue.read(count)->addr.rank_id = j;
			refreshQueue.read(count)->addr.row_id = i;
			refreshQueue.read(count)->addr.chan_id = channel;
			count++;
		}

}

void dramChannel::record_command(command *latest_command)
{
	if (history_q.enqueue(latest_command) == FAILURE)
	{
		delete history_q.dequeue();
		history_q.enqueue(latest_command); //FIXME: assert that this is != FAILURE
	}

}


enum transaction_type_t	dramChannel::set_read_write_type(const int rank_id,const int bank_count) const
{
	int read_count = 0;
	int write_count = 0;
	int empty_count = 0;

	for(int i = 0; i < bank_count; ++i)
	{
		command *temp_c = rank[rank_id].bank[i].per_bank_q.read(1);

		if(temp_c != NULL)
		{
			if(temp_c->this_command == CAS_AND_PRECHARGE_COMMAND)
			{
				read_count++;
			}
			else
			{
				write_count++;
			}
		}
		else
		{
			empty_count++;
		}
	}
#ifdef DEBUG_FLAG
	cerr << "Rank[" << rank_id << "] Read[" << read_count << "] Write[" << write_count << "] Empty[" << empty_count << "]" << endl;
#endif

	if (read_count >= write_count)
		return READ_TRANSACTION;
	else
		return WRITE_TRANSACTION;
}
