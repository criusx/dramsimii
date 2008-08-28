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
#include <boost/serialization/is_abstract.hpp>



namespace DRAMSimII
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

	public:		

		// functions
		const void *moveAllChannelsToTime(const tick endTime, tick& transFinishTime);
		bool enqueue(Transaction* trans);
		void enqueueTimeShift(Transaction* trans);
		InputStatus waitForTransactionToFinish(Transaction *trans);
		virtual tick nextTick() const;
		void runSimulations(const unsigned requestCount = 0);

	public:
		// accessors
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); } ///< returns true if this channel has no more room
		double Frequency() const { return systemConfig.Frequency(); }	///< accessor to get the frequency of the DRAM system
		tick getTime() const { return time; }

		// constructors	
		explicit System(const Settings& settings);
		explicit System(const System& rhs);

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
		friend inline void save_construct_data(Archive &ar, const DRAMSimII::System *t, const unsigned version)
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
		friend inline void load_construct_data(Archive & ar, DRAMSimII::System * t, const unsigned version)
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

			new(t)DRAMSimII::System(*sysConfig, *channel, *simParameters, *statistics, *inputStream);
		}
	};
}
#endif
