#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H
#pragma once

#include "globals.h"
#include "Settings.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>

namespace DRAMSimII
{
	/// @brief the parameters for the simulation, including where the requests come from and how many requests to simulate
	class SimulationParameters 
	{
	protected:
		unsigned requestCount;		///< the number of requests to simulate
		InputType inputType;		///< the input type

	public:
		// constructors
		SimulationParameters(const Settings& settings);

		// functions
		unsigned getRequestCount() {return requestCount;}

		// friends
		friend std::ostream& operator<<(std::ostream& , const SimulationParameters& );
		bool operator==(const SimulationParameters& right) const;

		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned int version)
		{
			ar & requestCount & inputType;
		}

		template<class Archive>
		friend void inline load_construct_data(Archive& ar, DRAMSimII::SimulationParameters* t, const unsigned file_version)
		{
			Settings settings;
			::new(t)DRAMSimII::SimulationParameters(settings);
		}

		template <class Archive>
		friend inline void save_construct_data(Archive& ar, const DRAMSimII::SimulationParameters* t, const unsigned version)
		{
		}

	};
}
#endif
