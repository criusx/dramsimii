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

#ifndef DRAMSYSTEM
#define DRAMSYSTEM
#pragma once

#include "globals.h"
#include "SystemConfiguration.h"
#include "TimingSpecification.h"
#include "Address.h"
#include "command.h"
#include "simulationParameters.h"
#include "Statistics.h"
#include "Algorithm.h"
#include "InputStream.h"
#include "event.h"
#include "Channel.h"
#include "Rank.h"
#include "Settings.h"
#include "powerConfig.h"
#include "SystemConfiguration.h"

#include <vector>
#include <ostream>
#include <iostream>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

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
		SystemConfiguration systemConfig;		///< stores the parameters for the DRAM system, including channel/rank/bank/row counts		
		SimulationParameters simParameters;		///< has all the necessary parameters for the simulation run
		Statistics statistics;					///< keeps running statistics about the simulation
		std::vector<Channel> channel;			///< represents the independent channels
		InputStream inputStream;				///< provides an interface to the input trace for the simulation

		tick time;								///< master clock, usually set to the oldest channel's time
		tick nextStats;							///< the next time at which stats should be collected

		//functions
		unsigned findOldestChannel() const;	
		void updateSystemTime();


		void checkStats();
		virtual void doPowerCalculation();
		virtual void printStatistics();	
		bool fileExists(std::stringstream& fileName) const;
		bool createNewFile(const std::string& fileName) const;

	public:		

		// functions
		void moveToTime(const tick endTime);
		bool enqueue(Transaction* trans);
		virtual tick nextTick() const;
		void runSimulations(const unsigned requestCount = 0);
		unsigned pendingTransactionCount() const;
		void getPendingTransactions(std::queue<std::pair<unsigned,tick> > &);

		// accessors
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); } ///< returns true if this channel has no more room
		double Frequency() const { return systemConfig.Frequency(); }	///< accessor to get the frequency of the DRAM system
		tick getTime() const { return time; }
		void resetToTime(const tick time);

		// constructors	
		explicit System(const Settings& settings);
		explicit System(const System& rhs);
		virtual ~System();

		// friends
		friend std::ostream &operator<<(std::ostream &, const System &);	
		bool operator==(const System &rhs) const;

	private:
		// serialization
		friend class boost::serialization::access;

		explicit System(Settings &settings, SystemConfiguration &systemConfig);

		// deserialization constructor
		explicit System(const SystemConfiguration &sysConfig, const std::vector<Channel> &chan, const SimulationParameters &simParams,
			const Statistics &stats, const InputStream &inputStr);

		template<class Archive>
		void serialize(Archive& ar, const unsigned version)
		{
			ar & time & nextStats;
			//ar & channel;
			//ar & simParameters;
			//ar & systemConfig;
		}

		template<class Archive>
		friend inline void save_construct_data(Archive &ar, const DRAMsimII::System *t, const unsigned version)
		{
			const SystemConfiguration *const sysConfig = &(t->systemConfig);
			ar << sysConfig;
			const std::vector<Channel> *const channel = &(t->channel);
			ar << channel;
			const SimulationParameters *const simParameters = &(t->simParameters);			
			ar << simParameters;
			const Statistics *const statistics = &(t->statistics);
			ar << statistics;
			const InputStream *const inputStream = &(t->inputStream);
			ar << inputStream;
		}

		template<class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMsimII::System * t, const unsigned version)
		{
			SystemConfiguration *sysConfig;
			ar >> sysConfig;
			std::vector<Channel> *channel;
			ar >> channel;
			SimulationParameters *simParameters;
			ar >> simParameters;
			Statistics *statistics;
			ar >> statistics;
			InputStream *inputStream;
			ar >> inputStream;

			new(t)DRAMsimII::System(*sysConfig, *channel, *simParameters, *statistics, *inputStream);
		}
	};
}
#endif
