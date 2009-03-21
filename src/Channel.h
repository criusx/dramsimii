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
		std::vector<tick> refreshCounter;				///< holds the next refresh command time for the rank
		const SystemConfiguration &systemConfig;		///< a pointer to common system config values
		Statistics &statistics;							///< backward pointer to the stats engine
		PowerConfig powerModel;							///< the power model for this channel, retains power stats
		Algorithm algorithm;							///< the algorithms used for transaction, command, etc. ordering
		unsigned channelID;								///< the ordinal value of this channel (0..n)
		bool dbReporting;								///< whether or not to report results to a db
		std::vector<Rank> rank;							///< vector of the array of ranks
		std::queue<std::pair<unsigned,tick> > finishedTransactions;		///< the transactions finished this time

		// functions
		void retireCommand(Command *);
		bool checkForAvailableCommandSlots(const Transaction *trans) const;	
		bool transaction2commands(Transaction *);
		Command *getNextCommand(const Command *useThisCommand = NULL);		
		tick nextTransactionDecodeTime() const;
		tick nextCommandExecuteTime() const;
		unsigned readAvailableTransaction() const;
		Transaction *getAvailableTransaction(unsigned useThis = UINT_MAX);
		void executeCommand(Command *thisCommand);

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
		void doPowerCalculation(const tick systemTime);
		virtual tick nextTick() const;
		void resetToTime(const tick time);
		unsigned pendingTransactionCount() const { return finishedTransactions.size(); }
		void getPendingTransactions(std::queue<std::pair<unsigned,tick> > &);

		virtual void moveToTime(const tick endTime);

		// accessors
		const TimingSpecification& getTimingSpecification() const { return timingSpecification; }	///< returns a reference to access the timing specification
		Rank& getRank(const unsigned rankNum) { return rank[rankNum]; }								///< get a reference to this channel's rank n
		const Rank& getRank(const unsigned rankNum) const { return rank[rankNum]; }					///< get a const reference to this channel's rank n
		std::vector<Rank>& getRank() { return rank; }												///< get a reference to this channel's ranks
		const std::vector<Rank>& getRank() const { return rank; }									///< get a const reference to this channel's ranks
		tick getTime() const { return time; }														///< get the time that this channel is at
		unsigned getLastRankID() const { return lastCommand ? lastCommand->getAddress().getRank() : systemConfig.getRankCount() - 1; }///< get the last rank id a command was issued to

		Transaction *getTransaction();																
		const Transaction *readTransaction(bool) const;												
		Transaction *createNextRefresh();
		const Transaction *readNextRefresh() const;
		tick nextRefresh() const;

		unsigned getTransactionQueueCount() const { return transactionQueue.size(); }				///< determine how many items are in the transaction completion queue
		unsigned getTransactionQueueDepth() const { return transactionQueue.depth(); }			///< determine how large the transaction completion queue is
		Rank& operator[](unsigned rank_num) { return rank[rank_num]; }

		// mutators
		void setTime(tick value) { time = value; }						///< update the time for this channel
		void setChannelID(const unsigned value) { channelID = value; }	///< set the channel ordinal
		Transaction::TransactionType setReadWriteType(const int,const int) const;	///< determine whether a read or write transaction should be generated

		// overloads
		Channel& operator =(const Channel& rs);
		bool operator==(const Channel& right) const;
		friend std::ostream& operator<<(std::ostream& , const Channel&);

	private:
		bool sendPower(double PsysRD, double PsysWR, std::vector<int> rankArray, std::vector<double> PsysACTSTBYArray, std::vector<double> PsysACTArray, const tick currentTime) const;

		// serialization
		explicit Channel(const Settings settings, const SystemConfiguration& sysConf, Statistics & stats, const PowerConfig &power,const std::vector<Rank> &rank, const TimingSpecification &timing);
		explicit Channel();

		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned int version)
		{
			ar & time & lastCommandIssueTime & transactionQueue
				& refreshCounter & algorithm & channelID & dbReporting;			
		}

		template<class Archive>
		friend inline void save_construct_data(Archive &ar, const DRAMsimII::Channel *t, const unsigned version)
		{			
			const DRAMsimII::SystemConfiguration* const sysC = &(t->systemConfig);
			ar << sysC;
			const DRAMsimII::Statistics* const stats = &(t->statistics);
			ar << stats;
			const DRAMsimII::PowerConfig* const power = &(t->powerModel);
			ar << power;
			const std::vector<DRAMsimII::Rank>* const rank = &(t->rank);
			ar << rank;
			const DRAMsimII::TimingSpecification* const timing = &(t->timingSpecification);
			ar << timing;
		}

		template<class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMsimII::Channel * t, const unsigned version)
		{
			DRAMsimII::SystemConfiguration* sysC;
			ar >> sysC;
			DRAMsimII::Statistics* stats;
			ar >> stats;
			DRAMsimII::PowerConfig* power;
			ar >> power;
			std::vector<DRAMsimII::Rank>* newRank;
			ar >> newRank;
			DRAMsimII::TimingSpecification* timing;
			ar >> timing;
			Settings settings;


			new(t)DRAMsimII::Channel(settings, *sysC, *stats, *power, *newRank, *timing);
		}
	};
}
#endif
