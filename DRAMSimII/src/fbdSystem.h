#ifndef FBDSYSTEM_H
#define FBDSYSTEM_H
#pragma once

#include <map>
#include <vector>
#include <ostream>
#include <iostream>
#include "System.h"
#include "fbdChannel.h"
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
	/// @brief a specialty type of dramSystem
	class fbdSystem: public System
	{
	private:
		// members
		std::vector<fbdChannel> channel;	// the serial channels
		
	public:
		// constructors
		explicit fbdSystem(const Settings& settings);		

		
	};
}

#endif
