#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>
#include <vector>

#include "dramChannel.h"

using namespace std;
using namespace DRAMSimII;

dramChannel::dramChannel(const dramSettings *settings):
time(0),
rank(settings->rankCount, rank_c(settings)),
refresh_row_index(0),
last_refresh_time(0),
last_rank_id(0),
timing_specification(settings),
transaction_q(settings->transactionQueueDepth),
refreshQueue(settings->rowCount * settings->rankCount,true),
history_q(settings->historyQueueDepth),
completion_q(settings->completionQueueDepth),
system_config(NULL),
powerModel(settings),
algorithm(settings)
{}

dramChannel::dramChannel(const dramChannel &dc):
time(dc.time),
rank(dc.rank),
refresh_row_index(dc.refresh_row_index),
last_refresh_time(dc.last_refresh_time),
last_rank_id(dc.last_rank_id),
timing_specification(dc.timing_specification),
transaction_q(dc.transaction_q),
refreshQueue(dc.refreshQueue),
history_q(dc.history_q),
completion_q(dc.completion_q),
system_config(NULL),
powerModel(dc.powerModel),
algorithm(dc.algorithm)
{
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
	while (history_q.enqueue(latest_command) == FAILURE)
	{
		delete history_q.dequeue();
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
			if (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)
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

// calculate the power consumed by all channels during the last epoch
void dramChannel::doPowerCalculation()
{	
	for (std::vector<rank_c>::iterator k = rank.begin(); k != rank.end(); k++)
	{
		tick_t totalRAS = 1;
		for (std::vector<bank_c>::iterator l = k->bank.begin(); l != k->bank.end(); l++)
		{
			// Psys(ACT)
			totalRAS += (l->RASCount - l->previousRASCount);
			l->previousRASCount = l->RASCount;
		}
		tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS * powerModel.tBurst / 2;
		cerr << "Psys(ACT) ch[" << channelID << "] r[] " << setprecision(3) << powerModel.PdsACT * powerModel.tRC / tRRDsch * (powerModel.VDD / powerModel.VDDmax) * (powerModel.VDD / powerModel.VDDmax) <<
			"(" << totalRAS << ") tRRDsch(" << tRRDsch << ") lastCalc[" << powerModel.lastCalculation << "] time[" << time << "]" << endl;		
	}
	powerModel.lastCalculation = time;
}

dramChannel& dramChannel::operator =(const DRAMSimII::dramChannel &rs)
{
	if (this == &rs)
	{
		return *this;
	}
	time = rs.time;
	rank = rs.rank;
	refresh_row_index = rs.refresh_row_index;
	last_rank_id = rs.last_rank_id;
	timing_specification = rs.timing_specification;
	transaction_q = rs.transaction_q;
	refreshQueue = rs.refreshQueue;
	history_q = rs.history_q;
	completion_q = rs.completion_q;
	system_config = new dramSystemConfiguration(rs.system_config);
	powerModel = rs.powerModel;
	algorithm = rs.algorithm;
	return *this;
}
