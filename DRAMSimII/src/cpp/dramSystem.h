#ifndef DRAMSYSTEM_H
#define DRAMSYSTEM_H
#pragma once

#include <vector>
#include <iostream>
#include "dramSystemConfiguration.h"
#include "dramTimingSpecification.h"
#include "addresses.h"
#include "command.h"
#include "simulationParameters.h"
#include "dramStatistics.h"
#include "dramAlgorithm.h"
#include "InputStream.h"
#include "event.h"
#include "dramChannel.h"
#include "enumTypes.h"
#include "rank_c.h"

class dramSystem
{
private:
	dramSystemConfiguration system_config;
	std::vector<dramChannel> channel;
	dramTimingSpecification timing_specification;
	simulationParameters sim_parameters;
	dramStatistics statistics;
	dramAlgorithm algorithm;
	inputStream input_stream;

	std::string output_filename;	

	tick_t time;								// master clock	
	
	queue<event> event_q;						// pending event queue	

	void read_dram_config_from_file();

	void set_dram_timing_specification(enum dram_type_t);
	command *getNextCommand(const int);
	enum input_status_t transaction2commands(transaction *);
	int minProtocolGap(const unsigned,const command *);
	int find_oldest_channel() const;
	void executeCommand(command *, const int);
	void update_system_time();
	
	enum input_status_t get_next_input_transaction(transaction *&);
	void get_next_random_request(transaction *);

public:

	explicit dramSystem(std::map<file_io_token_t,std::string> &);

	friend std::ostream &operator<<(std::ostream &, const dramSystem &);
	int convert_address(addresses &);
	void moveChannelToTime(const tick_t endTime, const int chan);
	void enqueue(transaction* trans);
	input_status_t waitForTransactionToFinish(transaction *trans);

	void run_simulations();
	void run_simulations2();
};

#endif
