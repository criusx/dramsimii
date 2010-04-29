// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
//
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CHANNEL_HH
#define CHANNEL_HH
#pragma once

#include "globals.hh"
#include "Rank.hh"
#include "powerConfig.hh"
#include "TimingSpecification.hh"
#include "SystemConfiguration.hh"
#include "Statistics.hh"
#include "queue.hh"
#include "powerConfig.hh"
#include "transaction.hh"
#include "command.hh"

#include <vector>
#include <queue>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>


namespace DRAMsimII
{
	/// @brief represents a DRAM channel, has individual timing parameters, ranks, banks, clock, etc.
	class Channel
	{
		// members
	protected:

		tick time;										///< channel time, allow for channel concurrency			
		tick lastCommandIssueTime;						///< the last time a command was executed on this channel
		const Command *lastCommand;						///< id of the last accessed rank of this channel
		TimingSpecification timingSpecification;		///< the timing specs for this channel
		Queue<Transaction> transactionQueue;			///< transaction queue for the channel
		std::vector<Transaction *> refreshCounter;		///< holds the next refresh command time for the rank
		const SystemConfiguration &systemConfig;		///< a pointer to common system config values
		Statistics &statistics;							///< backward pointer to the stats engine
		PowerConfig powerModel;							///< the power model for this channel, retains power stats
		unsigned channelID;								///< the ordinal value of this channel (0..n)
		std::vector<Rank> rank;							///< vector of the array of ranks
		std::queue<std::pair<unsigned,tick> > finishedTransactions;		///< the transactions finished this time
		std::vector<Cache> cache;						///< the dimm caches
	
		// functions
		void retireCommand(Command *, const bool isHit);
		bool checkForAvailableCommandSlots(const Transaction *trans) const;	
		bool transaction2commands(Transaction *);
		Command *getNextCommand(const Command *useThisCommand = NULL);
		void getNextCPRHValues(unsigned &, unsigned &, const bool) const;

		Transaction *getTransaction();																
		Transaction *getAvailableTransaction(unsigned useThis = UINT_MAX);

		const Transaction *readTransaction(bool) const;												
		unsigned readAvailableTransaction(bool) const;

		Transaction *createNextRefresh();
		const Transaction *readNextRefresh() const;

		tick nextRefreshTime() const;
		tick nextTransactionDecodeTime() const;
		tick nextCommandExecuteTime() const;
		void executeCommand(Command *thisCommand);
		bool canIssue(const Command *thisCommand) const { return earliestExecuteTime(thisCommand) <= time; }
		void printVerilogCommand(const Command *thisCommand);
		
		// functions that may differ for architectures that inherit this		
		virtual const Command *readNextCommand() const;
		virtual tick minProtocolGap(const Command *thisCommand) const;
		virtual tick earliestExecuteTime(const Command *thisCommand) const;
		virtual tick earliestExecuteTimeLog(const Command *thisCommand) const;

	public:
		// constructors
		explicit Channel(const Settings& settings, const SystemConfiguration& sysConfig, Statistics& stats);
		Channel(const Channel&);
		explicit Channel(const Channel& rhs, const SystemConfiguration& systemConfig, Statistics& stats);
		virtual ~Channel();

		// functions
		bool enqueue(Transaction *in);
		bool isFull() const { return transactionQueue.isFull(); }	///< determines whether there is room for more transactions
		unsigned getChannelID() const { return channelID; }			///< return the ordinal of this channel
		void doPowerCalculation(std::ostream &os);
		virtual tick nextTick() const;
		void resetToTime(const tick time);
		std::queue<std::pair<unsigned,tick> >::size_type pendingTransactionCount() const { return finishedTransactions.size(); }
		void getPendingTransactions(std::queue<std::pair<unsigned,tick> > &);
		const std::vector<Cache> &getDimmCache() const { return cache; }
		void resetStats();

		virtual void moveToTime(const tick currentTime);

		// accessors
		const TimingSpecification& getTimingSpecification() const { return timingSpecification; }	///< returns a reference to access the timing specification
		Rank& getRank(const unsigned rankNum) { return rank[rankNum]; }								///< get a reference to this channel's rank n
		const Rank& getRank(const unsigned rankNum) const { return rank[rankNum]; }					///< get a const reference to this channel's rank n
		std::vector<Rank>& getRank() { return rank; }												///< get a reference to this channel's ranks
		const std::vector<Rank>& getRank() const { return rank; }									///< get a const reference to this channel's ranks
		tick getTime() const { return time; }														///< get the time that this channel is at
		unsigned getLastRankID() const { return lastCommand ? lastCommand->getAddress().getRank() : systemConfig.getRankCount() - 1; }///< get the last rank id a command was issued to
		bool isEmpty() const;


		unsigned getTransactionQueueCount() const { return transactionQueue.size(); }				///< determine how many items are in the transaction completion queue
		unsigned getTransactionQueueDepth() const { return transactionQueue.depth(); }			///< determine how large the transaction completion queue is
		Rank& operator[](unsigned rank_num) { return rank[rank_num]; }

		// mutators
		void setTime(tick value) { time = value; }						///< update the time for this channel
		void setChannelID(const unsigned value);
		Transaction::TransactionType setReadWriteType(const int) const;	

		// overloads
		Channel& operator =(const Channel& rs);
		bool operator==(const Channel& right) const;
		friend std::ostream& operator<<(std::ostream& , const Channel&);

	private:
		bool sendPower(double PsysRD, double PsysWR, std::vector<int> rankArray, std::vector<double> PsysACTSTBYArray, std::vector<double> PsysACTArray, const tick currentTime) const;

		// serialization
		explicit Channel(const Settings& settings, const SystemConfiguration& sysConf, Statistics& stats, const PowerConfig& power,const std::vector<Rank>& rank, const TimingSpecification& timing);
		explicit Channel();

		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned int version)
		{
			if (version == 0)
			{
				ar & time & lastCommandIssueTime & transactionQueue & refreshCounter & channelID;
			}

		}

		template<class Archive>
		friend inline void save_construct_data(Archive &ar, const Channel *t, const unsigned version)
		{			
			if (version == 0)
			{
				const SystemConfiguration* const sysC = &(t->systemConfig);
				ar << sysC;
				const Statistics* const stats = &(t->statistics);
				ar << stats;
				const PowerConfig* const power = &(t->powerModel);
				ar << power;
				const std::vector<Rank>* const rank = &(t->rank);
				ar << rank;
				const TimingSpecification* const timing = &(t->timingSpecification);
				ar << timing;
				
				ar << t->lastCommand;
			}

		}

		template<class Archive>
		friend inline void load_construct_data(Archive &ar, Channel *t, const unsigned version)
		{
			if (version == 0)
			{
				SystemConfiguration* sysC;
				ar >> sysC;
				Statistics* stats;
				ar >> stats;
				PowerConfig* power;
				ar >> power;
				std::vector<Rank>* newRank;
				ar >> newRank;
				TimingSpecification* timing;
				ar >> timing;
				Settings settings;


				new(t)Channel(settings, *sysC, *stats, *power, *newRank, *timing);

				Command *lastCmd;
				ar >> lastCmd;
				t->lastCommand = lastCmd;
			}

		}
	};
}
#endif
