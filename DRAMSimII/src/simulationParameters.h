#ifndef SIMULATION_PARAMETERS_H
#define SIMULATION_PARAMETERS_H
#pragma once

class simulationParameters 
{
private:
	int request_count;
	int input_type;

public:
	simulationParameters(std::map<file_io_token_t,std::string> &);
	int get_request_count() {return request_count;}
};

#endif
