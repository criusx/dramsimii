#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H
#pragma once

#include <map>
#include "dramSettings.h"
#include "globals.h"

namespace DRAMSimII
{
	class simulationParameters 
	{
	private:
		int request_count;
		int input_type;

	public:
		// constructors
		simulationParameters(std::map<file_io_token_t,std::string> &);
		simulationParameters(const dramSettings& settings);

		// functions
		int get_request_count() {return request_count;}
	};
}
#endif
