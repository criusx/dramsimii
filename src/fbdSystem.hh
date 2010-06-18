// Copyright (C) 2010 University of Maryland.
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
#include "System.hh"
#include "fbdChannel.hh"
#include "SystemConfiguration.hh"
#include "TimingSpecification.hh"
#include "Address.hh"
#include "command.hh"
#include "simulationParameters.hh"
#include "Statistics.hh"
#include "InputStream.hh"
#include "event.hh"
#include "Channel.hh"
#include "enumTypes.hh"
#include "Rank.hh"
#include "Settings.hh"
#include "powerConfig.hh"
#include "globals.hh"
#include "SystemConfiguration.hh"


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
