// Copyright (C) 2010 University of Maryland.
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

#ifndef SYSTEM_HH
#define SYSTEM_HH

#include "globals.hh"
#include "SystemConfiguration.hh"
#include "TimingSpecification.hh"
#include "Address.hh"
#include "command.hh"
#include "Statistics.hh"
#include "event.hh"
#include "Channel.hh"
#include "Rank.hh"
#include "Settings.hh"
#include "powerConfig.hh"
#include "SystemConfiguration.hh"

#include <vector>
#include <ostream>
#include <iostream>

namespace DRAMsimII
{
	/// @brief represents a DRAM system, the memory controller(s) and associated channels
	/// @details contains a representation for a DRAM system, with the memory controller(s), channels, ranks, banks
	/// statistics, ability to read input streams, handle asynchronous requests, return requests at certain times
	/// @author Joe Gross
	class System
	{
	protected:
		// members
		const SystemConfiguration systemConfig;		///< stores the parameters for the DRAM system, including channel/rank/bank/row counts		
		Statistics statistics;					///< keeps running statistics about the simulation
		std::vector<Channel> channel;			///< represents the independent channels
	
		tick time;								///< master clock, usually set to the oldest channel's time
		tick lastStatsTime;						///< the time at which stats were last printed
		tick nextStats;							///< the next time at which stats should be collected

		//functions
		unsigned findOldestChannel() const;	
		void updateSystemTime();

		void checkStats();
		void printStats();
		virtual void doPowerCalculation();
		virtual void printStatistics();			
		
	public:		

		// functions
		bool moveToTime(const tick endTime);
		bool enqueue(Transaction* trans);
		virtual tick nextTick() const;
		unsigned pendingTransactionCount() const;
		void getPendingTransactions(std::queue<std::pair<unsigned,tick> > &);

		// accessors
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); } ///< returns true if this channel has no more room
		double Frequency() const { return systemConfig.Frequency(); }	///< accessor to get the frequency of the DRAM system
		tick getTime() const { return time; }
		void resetToTime(const tick time);
		bool isEmpty() const;

		// constructors	
		explicit System(const Settings& settings);
		explicit System(const System& rhs);
		~System();
		
		// friends
		friend std::ostream &operator<<(std::ostream &, const System &);	
		bool operator==(const System &rhs) const;

	};
}
#endif
