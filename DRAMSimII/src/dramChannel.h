#ifndef DRAMCHANNEL_H
#define DRAMCHANNEL_H
#pragma once

#include "globals.h"
#include "rank_c.h"
#include "powerConfig.h"
#include "dramTimingSpecification.h"
#include "dramSystemConfiguration.h"
#include "queue.h"
#include "powerConfig.h"
#include "transaction.h"
#include "command.h"
#include "dramAlgorithm.h"

namespace DRAMSimII
{
	class dramChannel
	{
		// members
	private:
		tick_t time;						// channel time, allow for channel concurrency	
		std::vector<rank_c> rank;			// vector of the array of ranks
		unsigned refresh_row_index;			// the row index to be refreshed
		tick_t last_refresh_time;			// tells me when last refresh was done
		unsigned last_rank_id;				// id of the last accessed rank of this channel
		dramTimingSpecification timing_specification; // the timing specs for this channel
		queue<transaction> transaction_q;	// transaction queue for the channel
		queue<transaction> refreshQueue;	// queue of refresh transactions
		queue<command> history_q;			// what were the last N commands to this channel?
		queue<transaction> completion_q;	// completed_q, can send status back to memory controller
		dramSystemConfiguration *system_config; // a pointer to common system config values
		powerConfig powerModel;
		dramAlgorithm algorithm;
		unsigned channelID;					// the ordinal value of this channel (0..n)

	public:
		// functions
		bool transaction2commands(transaction *);
		command *getNextCommand();
		command *readNextCommand() const;
		const void *moveChannelToTime(const tick_t endTime, tick_t *transFinishTime);
		int minProtocolGap(const command *thisCommand) const;
		void executeCommand(command *, const int);
		void doPowerCalculation();


		// accessors and mutators
		void setSystemConfig(dramSystemConfiguration *_system_config) { system_config = _system_config; } // TODO: remove this and have parameters stored locally
		const dramTimingSpecification& getTimingSpecification() const { return timing_specification; }
		rank_c& getRank(const unsigned rankNum) { return rank[rankNum]; }
		const rank_c& getRank(const unsigned rankNum) const { return rank[rankNum]; }
		std::vector<rank_c>& getRank() { return rank; }
		const std::vector<rank_c>& getRank() const { return rank; }
		tick_t get_time() const { return time; }
		void set_time(tick_t new_time) { time = new_time; }
		unsigned get_last_rank_id() const { return last_rank_id; }
		transaction *get_transaction() { return transaction_q.dequeue(); } // remove and return the oldest transaction
		transaction *read_transaction() const { return transaction_q.read_back(); } // read the oldest transaction without affecting the queue
		transaction *get_refresh() { return refreshQueue.dequeue(); }
		transaction *read_refresh() { return refreshQueue.read_back(); }
		input_status_t enqueueRefresh(transaction *in) { return refreshQueue.enqueue(in); }
		input_status_t enqueue(transaction *in) { return transaction_q.enqueue(in); }
		input_status_t complete(transaction *in) { return completion_q.enqueue(in); }
		transaction *get_oldest_completed() { return completion_q.dequeue(); }
		command *get_most_recent_command() const { return history_q.newest(); } // get the most recent command from the history queue
		unsigned getTransactionQueueCount() const { return transaction_q.get_count(); }
		unsigned getTransactionQueueDepth() const { return transaction_q.get_depth(); }
		void record_command(command *);
		void initRefreshQueue(const unsigned, const unsigned, const unsigned); // init the RefreshQueue using selected algorithm
		void setChannelID(const unsigned value) { channelID = value; }

		// constructors
		explicit dramChannel();	
		explicit dramChannel(const dramSettings *settings);
		dramChannel(const dramChannel &);

		// operator overloads
		dramChannel& operator =(const dramChannel &rs);


		rank_c& operator[](unsigned rank_num) { return rank[rank_num]; }
		void init_controller(int, int, int, int, int, int, int);
		enum transaction_type_t set_read_write_type(const int,const int) const;
	};
}

#endif
