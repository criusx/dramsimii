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


namespace DRAMsimII
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
