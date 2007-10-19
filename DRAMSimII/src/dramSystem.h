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
	private:
		// members
		dramSystemConfiguration systemConfig;
		std::vector<dramChannel> channel;	
		simulationParameters sim_parameters;
		dramStatistics statistics;	
		inputStream input_stream;

		tick_t time;	// master clock	

		queue<event> event_q;	// pending event queue	

		//functions
		int findOldestChannel() const;	
		void updateSystemTime();
		enum input_status_t getNextIncomingTransaction(transaction *&);
		void getNextRandomRequest(transaction *);

	public:

		
		bool convertAddress(addresses &) const;
		const void *moveAllChannelsToTime(const tick_t endTime, tick_t *transFinishTime);
		bool enqueue(transaction* trans); // enqueue this transaction into the proper per-channel queue
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); }
		void enqueueTimeShift(transaction* trans);
		input_status_t waitForTransactionToFinish(transaction *trans);
		double Frequency() const { return systemConfig.Frequency(); }
		tick_t nextTick() const;		
		void doPowerCalculation();
		void printStatistics();
		void runSimulations();
		void runSimulations2();
		void runSimulations3();

		// constructors	
		explicit dramSystem(const dramSettings *settings);

		// operator overloads
		dramSystem& operator =(const dramSystem &rs);

		// friends
		friend std::ostream &operator<<(std::ostream &, const dramSystem &);	
	};
}
#endif
