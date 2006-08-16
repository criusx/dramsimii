#ifndef DRAMCHANNEL_H
#define DRAMCHANNEL_H
#pragma once

#include <vector>
#include "queue.h"
#include "transaction.h"
#include "command.h"
#include "rank_c.h"

class dramChannel
{
private:
	tick_t time; // channel time, allow for channel concurrency	
	std::vector<rank_c> rank;// vector of the array of ranks
	int refresh_row_index; // the row index to be refreshed
	tick_t last_refresh_time; // tells me when last refresh was done
	int last_rank_id; // id of the last accessed rank of this channel
	queue<transaction> transaction_q;// transaction queue for the channel
	queue<transaction> refreshQueue; // queue of refresh transactions
	queue<command> history_q; // what were the last N commands to this channel?
	queue<transaction> completion_q;// completed_q, can send status back to memory controller

public:
	// the get_ functions
	rank_c& get_rank(const unsigned rank_num) { return rank[rank_num]; }
	std::vector<rank_c>& get_rank() { return rank; }
	tick_t get_time() const { return time; }
	void set_time(tick_t new_time) { time = new_time; }
	int get_last_rank_id() const { return last_rank_id; }
	transaction *get_transaction() { return transaction_q.dequeue(); } // remove and return the oldest transaction
	transaction *read_transaction() { return transaction_q.read_back(); } // read the oldest transaction without affecting the queue
	transaction *get_refresh() { return refreshQueue.dequeue(); }
	transaction *read_refresh() { return refreshQueue.read_back(); }
	input_status_t enqueueRefresh(transaction *in) { return refreshQueue.enqueue(in); }
	input_status_t enqueue(transaction *in) { return transaction_q.enqueue(in); }
	input_status_t complete(transaction *in) { return completion_q.enqueue(in); }
	transaction *get_oldest_completed() { return completion_q.dequeue(); }
	command *get_most_recent_command() const { return history_q.newest(); } // get the most recent command from the history queue
	void record_command(command *, queue<command> &);
	void initRefreshQueue(const unsigned, const unsigned); // init the RefreshQueue using selected algorithm

	explicit dramChannel();
	explicit dramChannel(int, int, int, int, int, int, int);
	dramChannel(const dramChannel &);
	rank_c& operator[](unsigned rank_num) { return rank[rank_num]; }
	void init_controller(int, int, int, int, int, int, int);
	enum transaction_type_t set_read_write_type(const int,const int) const;
};

#endif
