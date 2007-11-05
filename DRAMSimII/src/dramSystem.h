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
	class dramSystem
	{
	protected:
		// members
		dramSystemConfiguration systemConfig;
		std::vector<dramChannel> channel;	
		simulationParameters simParameters;
		dramStatistics statistics;	
		inputStream input_stream;

		tick_t time;	// master clock	

		//functions
		unsigned findOldestChannel() const;	
		void updateSystemTime();
		transaction *getNextIncomingTransaction();
		void getNextRandomRequest(transaction *);
		bool convertAddress(addresses &) const;

	public:		
		
		// functions
		const void *moveAllChannelsToTime(const tick_t endTime, tick_t *transFinishTime);
		bool enqueue(transaction* trans); // enqueue this transaction into the proper per-channel queue		
		void enqueueTimeShift(transaction* trans);
		input_status_t waitForTransactionToFinish(transaction *trans);		
		virtual tick_t nextTick() const;		
		virtual void doPowerCalculation();
		virtual void printStatistics();
		void runSimulations();
		void runSimulations2();
		void runSimulations3();
		void runSimulations4();

		// accessors
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); }
		double Frequency() const { return systemConfig.Frequency(); }

		// constructors	
		explicit dramSystem(const dramSettings& settings);
		
		// friends
		friend std::ostream &operator<<(std::ostream &, const dramSystem &);	
	};
}
#endif
