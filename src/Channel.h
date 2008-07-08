#ifndef DRAMCHANNEL_H
#define DRAMCHANNEL_H
#pragma once

#include "globals.h"
#include "Rank.h"
#include "powerConfig.h"
#include "TimingSpecification.h"
#include "SystemConfiguration.h"
#include "Statistics.h"
#include "queue.h"
#include "powerConfig.h"
#include "transaction.h"
#include "command.h"
#include "Algorithm.h"

namespace DRAMSimII
{
	/// @brief represents a DRAM channel, has individual timing parameters, ranks, banks, clock, etc.
	class Channel
	{
		// members
	protected:
		tick time;										///< channel time, allow for channel concurrency			
		tick lastRefreshTime;							///< tells me when last refresh was done
		unsigned lastRankID;							///< id of the last accessed rank of this channel
		TimingSpecification timingSpecification;		///< the timing specs for this channel
		Queue<Transaction> transactionQueue;			///< transaction queue for the channel
		Transaction **refreshCounter;					///< holds the next refresh commands
		Queue<Command> historyQueue;					///< what were the last N commands to this channel?
		Queue<Transaction> completionQueue;				///< completed_q, can send status back to memory controller
		const SystemConfiguration& systemConfig;		///< a pointer to common system config values
		Statistics *statistics;							///< backward pointer to the stats engine
		PowerConfig powerModel;							///< the power model for this channel, retains power stats
		Algorithm algorithm;							///< the algorithms used for transaction, command, etc. ordering
		unsigned channelID;								///< the ordinal value of this channel (0..n)
		std::vector<Rank> rank;							///< vector of the array of ranks

	public:
		// functions
		bool enqueue(Transaction *in);
		bool isFull() const { return transactionQueue.freecount() == 0; }	///< determines whether there is room for more transactions
		void retireCommand(Command *);
		unsigned getChannelID() const { return channelID; }					///< return the ordinal of this channel
		bool checkForAvailableCommandSlots(const Transaction *trans) const;	
		bool transaction2commands(Transaction *);
		Command *getNextCommand();		
		void doPowerCalculation();
		//void executeCommand(Command *thisCommand,const int gap);
		void executeCommand(Command *thisCommand);
		tick nextTransactionDecodeTime() const;
		tick nextCommandExecuteTime() const;
		virtual tick nextTick() const;
		

		// functions that may differ for architectures that inherit this		
		virtual const Command *readNextCommand() const;
		virtual const void *moveChannelToTime(const tick endTime, tick& transFinishTime);
		virtual int minProtocolGap(const Command *thisCommand) const;
		virtual tick earliestExecuteTime(const Command *thisCommand) const;

		// accessors
		const TimingSpecification& getTimingSpecification() const { return timingSpecification; }	///< returns a reference to access the timing specification
		Rank& getRank(const unsigned rankNum) { return rank[rankNum]; }								///< get a reference to this channel's rank n
		const Rank& getRank(const unsigned rankNum) const { return rank[rankNum]; }					///< get a const reference to this channel's rank n
		std::vector<Rank>& getRank() { return rank; }												///< get a reference to this channel's ranks
		const std::vector<Rank>& getRank() const { return rank; }
		tick getTime() const { return time; }
		unsigned getLastRankID() const { return lastRankID; }
		Transaction *getTransaction();			// remove and return the oldest transaction
		const Transaction *readTransaction(bool) const;	// read the oldest transaction without affecting the queue
		const Transaction *readTransactionSimple() const { return transactionQueue.front(); }
		Transaction *getRefresh();
		const Transaction *readRefresh() const;
		Transaction *getOldestCompletedTransaction() { return completionQueue.pop(); }
		unsigned getTransactionQueueCount() const { return transactionQueue.size(); }
		unsigned getTransactionQueueDepth() const { return transactionQueue.get_depth(); }
		Rank& operator[](unsigned rank_num) { return rank[rank_num]; }

		// mutators
		void setStatistics(Statistics *value) { statistics = value; }		///< set the statistics pointer to a dramStatistics object
		void setTime(tick new_time) { time = new_time; }						///< update the time for this channel
		void setChannelID(const unsigned value) { channelID = value; }			///< set the channel ordinal
		TransactionType setReadWriteType(const int,const int) const;	///< determine whether a read or write transaction should be generated

		// constructors
		explicit Channel();	
		explicit Channel(const Settings& settings, const SystemConfiguration &sysConfig);
		Channel(const Channel&);
		virtual ~Channel();

		Channel& operator =(const Channel& rs);
	};
}

#endif
