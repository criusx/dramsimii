#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H
#pragma once

#include <map>
#include "dramSettings.h"
#include "globals.h"

namespace DRAMSimII
{
	/// @brief the parameters for the simulation, including where the requests come from and how many requests to simulate
	class simulationParameters 
	{
	protected:
		int request_count;	///< the number of requests to simulate
		int input_type;		///< the input type

	public:
		// constructors
		simulationParameters(std::map<file_io_token_t,std::string> &);
		simulationParameters(const dramSettings& settings);

		// functions
		int get_request_count() {return request_count;}
	};
}
#endif
