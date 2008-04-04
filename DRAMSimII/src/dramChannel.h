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
	/// represents a DRAM channel, has individual timing parameters, ranks, banks, clock, etc.
	class dramChannel
	{
		// members
	protected:
		tick_t time;									///< channel time, allow for channel concurrency			
		tick_t lastRefreshTime;							///< tells me when last refresh was done
		unsigned lastRankID;							///< id of the last accessed rank of this channel
		dramTimingSpecification timingSpecification;	///< the timing specs for this channel
		queue<transaction> transactionQueue;			///< transaction queue for the channel
		transaction **refreshCounter;					///< holds the next refresh commands
		queue<command> historyQueue;					///< what were the last N commands to this channel?
		queue<transaction> completionQueue;				///< completed_q, can send status back to memory controller
		const dramSystemConfiguration& systemConfig;	///< a pointer to common system config values
		dramStatistics *statistics;					///< backward pointer to the stats engine
		powerConfig powerModel;						///< the power model for this channel, retains power stats
		dramAlgorithm algorithm;					///< the algorithms used for transaction, command, etc. ordering
		unsigned channelID;							///< the ordinal value of this channel (0..n)
		std::vector<rank_c> rank;					///< vector of the array of ranks

	public:
		// functions
		bool enqueue(transaction *in);
		bool isFull() const { return transactionQueue.freecount() == 0; }
		void recordCommand(command *);
		unsigned getChannelID() const { return channelID; }
		bool checkForAvailableCommandSlots(const transaction *trans) const;	
		bool transaction2commands(transaction *);
		command *getNextCommand();		
		void doPowerCalculation();
		void executeCommand(command *thisCommand,const int gap);
		void dramChannel::executeCommand(command *thisCommand);
		tick_t nextTransactionDecodeTime() const;
		virtual tick_t nextTick() const;
		

		// functions that may differ for architectures that inherit this		
		virtual const command *readNextCommand() const;
		virtual const void *moveChannelToTime(const tick_t endTime, tick_t *transFinishTime);
		virtual int minProtocolGap(const command *thisCommand) const;
		virtual tick_t earliestExecuteTime(const command *thisCommand) const;

		// accessors
		const dramTimingSpecification& getTimingSpecification() const { return timingSpecification; }
		rank_c& getRank(const unsigned rankNum) { return rank[rankNum]; }
		const rank_c& getRank(const unsigned rankNum) const { return rank[rankNum]; }
		std::vector<rank_c>& getRank() { return rank; }
		const std::vector<rank_c>& getRank() const { return rank; }
		tick_t getTime() const { return time; }
		unsigned getLastRankID() const { return lastRankID; }
		transaction *getTransaction();			// remove and return the oldest transaction
		const transaction *readTransaction(bool) const;	// read the oldest transaction without affecting the queue
		const transaction *readTransactionSimple() const { return transactionQueue.front(); }
		transaction *getRefresh();
		const transaction *readRefresh() const;
		transaction *getOldestCompletedTransaction() { return completionQueue.pop(); }
		unsigned getTransactionQueueCount() const { return transactionQueue.size(); }
		unsigned getTransactionQueueDepth() const { return transactionQueue.get_depth(); }
		rank_c& operator[](unsigned rank_num) { return rank[rank_num]; }

		// mutators
		void setStatistics(dramStatistics *value) { statistics = value; }
		void setTime(tick_t new_time) { time = new_time; }
		void setChannelID(const unsigned value) { channelID = value; }
		enum transaction_type_t setReadWriteType(const int,const int) const;

		// constructors
		explicit dramChannel();	
		explicit dramChannel(const dramSettings& settings, const dramSystemConfiguration &sysConfig);
		dramChannel(const dramChannel &);
	};
}

#endif
