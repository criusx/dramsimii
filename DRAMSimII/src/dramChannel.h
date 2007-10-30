#ifndef DRAMCHANNEL_H
#define DRAMCHANNEL_H
#pragma once

#include "globals.h"
#include "rank_c.h"
#include "powerConfig.h"
#include "dramTimingSpecification.h"
#include "dramSystemConfiguration.h"
#include "dramStatistics.h"
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
		tick_t time;							// channel time, allow for channel concurrency	
		std::vector<rank_c> rank;				// vector of the array of ranks
		tick_t lastRefreshTime;					// tells me when last refresh was done
		unsigned lastRankID;					// id of the last accessed rank of this channel
		dramTimingSpecification timing_specification; // the timing specs for this channel
		queue<transaction> transactionQueue;	// transaction queue for the channel
		transaction **refreshCounter;
		queue<command> historyQueue;			// what were the last N commands to this channel?
		queue<transaction> completionQueue;		// completed_q, can send status back to memory controller
		dramSystemConfiguration *systemConfig;	// a pointer to common system config values
		dramStatistics *statistics;				// backward pointer to the stats engine
		powerConfig powerModel;
		dramAlgorithm algorithm;
		unsigned channelID;						// the ordinal value of this channel (0..n)

	public:
		// functions
		bool checkForAvailableCommandSlots(const transaction *trans) const;	
		bool transaction2commands(transaction *);
		command *getNextCommand();
		const command *readNextCommand() const;
		
		const void *moveChannelToTime(const tick_t endTime, tick_t *transFinishTime);
		int minProtocolGap(const command *thisCommand) const;
		void executeCommand(command *, const int);
		void doPowerCalculation();


		// accessors and mutators
		void setSystemConfig(dramSystemConfiguration *_system_config) { systemConfig = _system_config; } // TODO: remove this and have parameters stored locally
		void setStatistics(dramStatistics *value) { statistics = value; }
		const dramTimingSpecification& getTimingSpecification() const { return timing_specification; }
		rank_c& getRank(const unsigned rankNum) { return rank[rankNum]; }
		const rank_c& getRank(const unsigned rankNum) const { return rank[rankNum]; }
		std::vector<rank_c>& getRank() { return rank; }
		const std::vector<rank_c>& getRank() const { return rank; }
		tick_t get_time() const { return time; }
		void setTime(tick_t new_time) { time = new_time; }
		unsigned get_last_rank_id() const { return lastRankID; }
		transaction *getTransaction();			// remove and return the oldest transaction
		const transaction *readTransaction() const;	// read the oldest transaction without affecting the queue
		transaction *readTransactionSimple() const { return transactionQueue.front(); }
		transaction *getRefresh();
		const transaction *readRefresh() const;
		//bool enqueueRefresh(transaction *in) { return refreshQueue.push(in); }
		bool enqueue(transaction *in);
		bool isFull() const { return transactionQueue.freecount() == 0; }
		bool complete(transaction *in) { return completionQueue.push(in); }
		transaction *get_oldest_completed() { return completionQueue.pop(); }
		command *get_most_recent_command() const { return historyQueue.newest(); } // get the most recent command from the history queue
		unsigned getTransactionQueueCount() const { return transactionQueue.size(); }
		unsigned getTransactionQueueDepth() const { return transactionQueue.get_depth(); }
		void recordCommand(command *);
		void setChannelID(const unsigned value) { channelID = value; }

		// constructors
		explicit dramChannel();	
		explicit dramChannel(const dramSettings *settings);
		dramChannel(const dramChannel &);

		// operator overloads
		dramChannel& operator =(const dramChannel &rs);


		rank_c& operator[](unsigned rank_num) { return rank[rank_num]; }
		enum transaction_type_t setReadWriteType(const int,const int) const;
	};
}

#endif
