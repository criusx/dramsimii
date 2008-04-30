#ifndef DRAMSYSTEM
#define DRAMSYSTEM
#pragma once

#include <vector>
#include <ostream>
#include <iostream>
#include "dramSystemConfiguration.h"
#include "dramTimingSpecification.h"
#include "addresses.h"
#include "command.h"
#include "simulationParameters.h"
#include "dramStatistics.h"
#include "dramAlgorithm.h"
#include "InputStream.h"
#include "event.h"
#include "dramChannel.h"
#include "enumTypes.h"
#include "rank_c.h"
#include "dramSettings.h"
#include "powerConfig.h"
#include "globals.h"
#include "dramSystemConfiguration.h"

namespace DRAMSimII
{
	/// @brief represents a DRAM system, the memory controller(s) and associated channels
	/// @details contains a representation for a DRAM system, with the memory controller(s), channels, ranks, banks
	/// statistics, ability to read input streams, handle asynchronous requests, return requests at certain times
	/// @author Joe Gross
	class dramSystem
	{
	protected:
		// members
		dramSystemConfiguration systemConfig;	///< stores the parameters for the DRAM system, including channel/rank/bank/row counts
		std::vector<dramChannel> channel;		///< represents the independent channels
		simulationParameters simParameters;		///< has all the necessary parameters for the simulation run
		dramStatistics statistics;				///< keeps running statistics about the simulation
		inputStream input_stream;				///< provides an interface to the input trace for the simulation

		tick_t time;							///< master clock, usually set to the oldest channel's time

		//functions
		unsigned findOldestChannel() const;	
		void updateSystemTime();
		transaction *getNextIncomingTransaction();
		transaction *getNextRandomRequest();
		bool convertAddress(addresses &) const;

	public:		
		
		// functions
		const void *moveAllChannelsToTime(const tick_t endTime, tick_t *transFinishTime);
		bool enqueue(transaction* trans);
		void enqueueTimeShift(transaction* trans);
		input_status_t waitForTransactionToFinish(transaction *trans);
		virtual tick_t nextTick() const;
		virtual void doPowerCalculation();
		virtual void printStatistics();
		//void runSimulations();
		//void runSimulations2();
		void runSimulations3();

		// accessors
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); } ///< returns true if this channel has no more room
		double Frequency() const { return systemConfig.Frequency(); }	///< accessor to get the frequency of the DRAM system

		// constructors	
		explicit dramSystem(const dramSettings& settings);
		
		// friends
		friend std::ostream &operator<<(std::ostream &, const dramSystem &);	
	};
}
#endif
