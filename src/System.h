#ifndef DRAMSYSTEM
#define DRAMSYSTEM
#pragma once

#include <vector>
#include <ostream>
#include <iostream>
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
#include "enumTypes.h"
#include "Rank.h"
#include "Settings.h"
#include "powerConfig.h"
#include "globals.h"
#include "SystemConfiguration.h"

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
		std::vector<Channel> channel;			///< represents the independent channels
		SimulationParameters simParameters;		///< has all the necessary parameters for the simulation run
		Statistics statistics;					///< keeps running statistics about the simulation
		InputStream input_stream;				///< provides an interface to the input trace for the simulation
		

		tick time;								///< master clock, usually set to the oldest channel's time
		tick nextStats;							///< the next time at which stats should be collected

		//functions
		unsigned findOldestChannel() const;	
		void updateSystemTime();
		Transaction *getNextIncomingTransaction();
		Transaction *getNextRandomRequest();
		bool convertAddress(Address &) const;
		void checkStats();
		virtual void doPowerCalculation();
		virtual void printStatistics();		

	public:		
		
		// functions
		const void *moveAllChannelsToTime(const tick endTime, tick *transFinishTime);
		bool enqueue(Transaction* trans);
		void enqueueTimeShift(Transaction* trans);
		InputStatus waitForTransactionToFinish(Transaction *trans);
		virtual tick nextTick() const;
		
		
		//void runSimulations();
		//void runSimulations2();
		void runSimulations3();

		// accessors
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); } ///< returns true if this channel has no more room
		double Frequency() const { return systemConfig.Frequency(); }	///< accessor to get the frequency of the DRAM system

		// constructors	
		explicit System(const Settings& settings);
		
		// friends
		friend std::ostream &operator<<(std::ostream &, const System &);	
	};
}
#endif
