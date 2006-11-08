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
	queue<command> free_command_pool;			// command objects are stored here to avoid allocating memory after initialization
	queue<transaction> free_transaction_pool;	// transactions are stored here to avoid allocating memory after initialization
	queue<event> free_event_pool;				// same for events
	queue<event> event_q;						// pending event queue	

	void read_dram_config_from_file();

	void set_dram_timing_specification(enum dram_type_t);
	command *getNextCommand(const int);
	enum input_status_t transaction2commands(transaction *);
	int minProtocolGap(const unsigned,const command *);
	int find_oldest_channel() const;
	void executeCommand(command *, const int);
	void update_system_time();
	int convert_address(addresses &);
	enum input_status_t get_next_input_transaction(transaction *&);
	void get_next_random_request(transaction *);

public:

	explicit dramSystem(std::map<file_io_token_t,std::string> &);

#ifdef M5	

	struct Params//: PhysicalMemory::Params
	{
	//public:
		// parameters to be passed by the Python interface
		std::string outFilename;
		std::string dramType;
		std::string rowBufferManagmentPolicy;
		bool autoPrecharge;
		std::string addrMappingScheme;
		int datarate;
		int refreshTime;
		bool readWriteGrouping;
		std::string refreshPolicy;
		unsigned seniorityAgeLimit;
		bool postedCas;
		int clockGranularity;
		unsigned rowCount;
		unsigned cachelinesPerRow;
		unsigned colCount;
		unsigned colSize;
		unsigned rowSize;
		unsigned cachelineSize;
		unsigned historyQueueDepth;
		std::string completionQueueDepth;
		std::string transactionQueueDepth;
		std::string eventQueueDepth;
		std::string perBankQueueDepth;
		std::string orderingAlgorithm;
		std::string configType;
		std::string chanCount;
		std::string rankCount;
		std::string bankCount;
		std::string refreshQueueDepth;
		std::string tAL;
		std::string tBURST;
		std::string tCAS;
		std::string tFAW;
		std::string tRAS;
		std::string tRP;
		std::string tRCD;
		std::string tRFC;
		std::string tRRD;
		std::string tRTP;
		std::string tRTRS;
		std::string tWR;
		std::string tWTR;
	};

	explicit dramSystem(dramSystem::Params *);
#endif

	friend std::ostream &operator<<(std::ostream &, const dramSystem &);
	void run_simulations();
	void run_simulations2();
};

#endif
