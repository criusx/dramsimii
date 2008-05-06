#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H
#pragma once

#include <map>
#include "Settings.h"
#include "globals.h"

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
		SimulationParameters(std::map<FileIOToken,std::string> &);
		SimulationParameters(const Settings& settings);

		// functions
		unsigned getRequestCount() {return requestCount;}
	};
}
#endif
