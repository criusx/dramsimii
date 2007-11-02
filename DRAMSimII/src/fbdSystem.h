#ifndef FBDSYSTEM_H
#define FBDSYSTEM_H
#pragma once

#include <map>
#include <vector>
#include <ostream>
#include <iostream>
#include "dramSystem.h"
#include "fbdChannel.h"
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
	// a specialty type of dramSystem
	class fbdSystem: dramSystem
	{
	private:
		// members
		std::vector<fbdChannel> fbdChannels;	// the serial channels
		std::vector<dramChannel> dramChannels;	// used to keep track of the channels hanging off the AMBs
		//std::multimap<tick_t,fbdFrame> eventQueue;
	public:

		// functions
		const void *moveAllChannelsToTime(const tick_t endTime, tick_t *transFinishTime);
		bool enqueue(transaction* trans); // enqueue this transaction into the proper per-channel queue
		bool isFull(const unsigned channelNumber) const { return channel[channelNumber].isFull(); }
		void enqueueTimeShift(transaction* trans);
		input_status_t waitForTransactionToFinish(transaction *trans);
		double Frequency() const { return systemConfig.Frequency(); }
		tick_t nextTick() const;		
		void doPowerCalculation();
		void printStatistics();

		// constructors
		explicit fbdSystem(const dramSettings *settings);		
	};
}

#endif