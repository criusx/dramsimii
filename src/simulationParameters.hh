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

#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H
#pragma once

#include "globals.hh"
#include "Settings.hh"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>

namespace DRAMsimII
{
	/// @brief the parameters for the simulation, including where the requests come from and how many requests to simulate
	class SimulationParameters
	{
	protected:
		tick requestCount;		///< the number of requests to simulate
		InputStream::InputType inputType;		///< the input type

	public:
		// constructors
		SimulationParameters(const Settings& settings);

		// functions
		tick getRequestCount() {return requestCount;}

		// friends
		friend std::ostream& operator<<(std::ostream& , const SimulationParameters&);
		bool operator==(const SimulationParameters& right) const;

		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned int version)
		{
			if (version == 0)
			{
				ar & requestCount & inputType;
			}

		}

		template<class Archive>
		friend void inline load_construct_data(Archive& ar, DRAMsimII::SimulationParameters* t, const unsigned version)
		{
			if (version == 0)
			{
				assert(&ar);
				Settings settings;
				::new(t)DRAMsimII::SimulationParameters(settings);
			}

		}
	};
}
#endif
