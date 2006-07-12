#ifndef DRAMSYSTEM_H
#define DRAMSYSTEM_H
#pragma once

#include <vector>
#include <iostream>
#include "addresses.h"
#include "globals.h"
#include "dramSystemConfiguration.h"
#include "dramTimingSpecification.h"
#include "simulationParameters.h"
#include "dramStatistics.h"
#include "dramAlgorithm.h"
#include "inputStream.h"
#include "event.h"
#include "dramChannel.h"
#include "enumTypes.h"

class dramSystem
{
private:
	dramSystemConfiguration system_config;
	dramTimingSpecification timing_specification;
	simulationParameters sim_parameters;
	dramStatistics statistics;
	dramAlgorithm algorithm;
	inputStream input_stream;

	std::string output_filename;	
	std::vector<dramChannel> channel;
	tick_t time; // master clock
	queue<command> free_command_pool;	// command objects are stored here to avoid allocating memory after initialization
	queue<transaction> free_transaction_pool;	// transactions are stored here to avoid allocating memory after initialization
	queue<event> free_event_pool;	// same for events
	queue<event> event_q;	// pending event queue	

	void read_dram_config_from_file();

	void set_dram_timing_specification(enum dram_type_t);
	command *getNextCommand(const int);
	enum input_status_t transaction2commands(transaction*);
	int minProtocolGap(const int,const command *) const;
	int find_oldest_channel() const;
	void executeCommand(command *, const int);
	void update_system_time();
	int convert_address(addresses &);
	enum input_status_t get_next_input_transaction(transaction *&);
	void get_next_random_request(transaction *);

public:
	explicit dramSystem(std::map<file_io_token_t,std::string> &);
	~dramSystem();
	friend std::ostream &operator<<(std::ostream &, const dramSystem &);
	void run_simulations();
	void run_simulations2();
};

#endif