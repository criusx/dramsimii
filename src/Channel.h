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

#include <vector>

#include <boost/circular_buffer.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>


namespace DRAMSimII
{
	/// @brief represents a DRAM channel, has individual timing parameters, ranks, banks, clock, etc.
	class Channel
	{
		// members
	protected:
		tick time;										///< channel time, allow for channel concurrency			
		tick lastRefreshTime;							///< tells me when last refresh was done
		tick lastCommandIssueTime;						///< the last time a command was executed on this channel
		unsigned lastRankID;							///< id of the last accessed rank of this channel
		TimingSpecification timingSpecification;		///< the timing specs for this channel
		Queue<Transaction> transactionQueue;			///< transaction queue for the channel
		std::vector<tick> refreshCounter;				///< holds the next refresh command time for the rank
		Queue<Command> historyQueue;					///< what were the last N commands to this channel?
		Queue<Transaction> completionQueue;				///< completed_q, can send status back to memory controller
		const SystemConfiguration &systemConfig;		///< a pointer to common system config values
		Statistics &statistics;							///< backward pointer to the stats engine
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
		void executeCommand(Command *thisCommand);
		tick nextTransactionDecodeTime() const;
		tick nextCommandExecuteTime() const;
		virtual tick nextTick() const;

	private:
		bool sendPower(float PsysRD, float PsysWR, std::vector<int> rankArray, std::vector<float> PsysACTSTBYArray, std::vector<float> PsysACTArray) const;

	public:
		// constructors
		explicit Channel(const Settings& settings, const SystemConfiguration& sysConfig, Statistics& stats);
		Channel(const Channel&);
		explicit Channel(const Channel& rhs, const SystemConfiguration& systemConfig, Statistics& stats);
		virtual ~Channel();

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
		Transaction *createNextRefresh();
		const Transaction *readNextRefresh() const;
		const tick nextRefresh() const;


		Transaction *getOldestCompletedTransaction() { return completionQueue.pop(); }
		unsigned getTransactionQueueCount() const { return transactionQueue.size(); }
		unsigned getTransactionQueueDepth() const { return transactionQueue.get_depth(); }
		Rank& operator[](unsigned rank_num) { return rank[rank_num]; }

		// mutators
		void setTime(tick value) { time = value; }						///< update the time for this channel
		void setChannelID(const unsigned value) { channelID = value; }			///< set the channel ordinal
		TransactionType setReadWriteType(const int,const int) const;	///< determine whether a read or write transaction should be generated
		
		// overloads
		Channel& operator =(const Channel& rs);
		bool operator==(const Channel& right) const;
		friend std::ostream& operator<<(std::ostream& , const Channel& );

		// serialization
	private:
		explicit Channel(const Settings settings, const SystemConfiguration& sysConf, Statistics & stats, const PowerConfig &power,const std::vector<Rank> &rank, const TimingSpecification &timing);
		explicit Channel();

		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned int version)
		{
			ar & time & lastRefreshTime & lastCommandIssueTime & lastRankID
				& transactionQueue & refreshCounter &
				historyQueue & completionQueue & algorithm & channelID;			
		}

		template<class Archive>
		friend inline void save_construct_data(Archive &ar, const DRAMSimII::Channel *t, const unsigned version)
		{			
			const DRAMSimII::SystemConfiguration* const sysC = &(t->systemConfig);
			ar << sysC;
			const DRAMSimII::Statistics* const stats = &(t->statistics);
			ar << stats;
			const DRAMSimII::PowerConfig* const power = &(t->powerModel);
			ar << power;
			const std::vector<DRAMSimII::Rank>* const rank = &(t->rank);
			ar << rank;
			const DRAMSimII::TimingSpecification* const timing = &(t->timingSpecification);
			ar << timing;
		}

		template<class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMSimII::Channel * t, const unsigned version)
		{
			DRAMSimII::SystemConfiguration* sysC;
			ar >> sysC;
			DRAMSimII::Statistics* stats;
			ar >> stats;
			DRAMSimII::PowerConfig* power;
			ar >> power;
			std::vector<DRAMSimII::Rank>* newRank;
			ar >> newRank;
			DRAMSimII::TimingSpecification* timing;
			ar >> timing;
			Settings settings;


			new(t)DRAMSimII::Channel(settings, *sysC, *stats, *power, *newRank, *timing);
		}
	};
}
#endif
