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
	class fbdSystem: public dramSystem
	{
	private:
		// members
		std::vector<fbdChannel> channel;	// the serial channels
		
	public:
		// constructors
		explicit fbdSystem(const dramSettings& settings);		

		
	};
}

#endif