#ifndef DRAMSYSTEM_H
#define DRAMSYSTEM_H
#pragma once

#include <vector>
#include <ostream>
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
#include "gzstream/gzstream.h"

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
	command *readNextCommand(const int) const;
	enum input_status_t transaction2commands(transaction *);
	int minProtocolGap(const unsigned,const command *) const;
	int find_oldest_channel() const;
	void executeCommand(command *, const int);
	void update_system_time();
	
	enum input_status_t get_next_input_transaction(transaction *&);
	void get_next_random_request(transaction *);

public:	

	ogzstream outStream;
	bool checkForAvailableCommandSlots(const transaction *trans) const;	
	int convert_address(addresses &) const;
	const void *moveAllChannelsToTime(const tick_t endTime, tick_t *transFinishTime);
	const void *moveChannelToTime(const tick_t endTime, const int chan, tick_t *transFinishTime);
	bool enqueue(transaction* trans);
	void enqueueTimeShift(transaction* trans);
	input_status_t waitForTransactionToFinish(transaction *trans);
	double Frequency() const { return system_config.Frequency(); }
	tick_t nextTick() const;
	void run_simulations();
	void run_simulations2();

	// constructors
	explicit dramSystem(std::map<file_io_token_t,std::string> &);

	// friends
	friend std::ostream &operator<<(std::ostream &, const dramSystem &);	
};

#endif
