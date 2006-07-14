#ifndef DRAMSYSTEMCONFIGURATION_H
#define DRAMSYSTEMCONFIGURATION_H
#pragma once

#include <fstream>
#include "globals.h"
#include "enumTypes.h"


class dramSystemConfiguration
{ 

private:
	std::ifstream spd_file_ptr;
	ordering_algorithm_t command_ordering_algorithm;	// strict or round robin 
	int per_bank_queue_depth;							// command queue size
	system_configuration_type_t config_type;
	int refresh_time;									// loop time of refresh 
	refresh_policy_t refresh_policy;					// 
	int col_size;
	int row_size;										// bytes per row (across one rank) 
	int row_count;										// rows per bank
	int col_count;										// columns per row
	int cacheline_size;									// 32/64/128 etc 
	int history_queue_depth;							// keep track of per channel command history 
	int completion_queue_depth;							// keep track of per channel command history 
	int transaction_queue_depth;						// input transaction queue depth 
	int event_queue_depth;								// pending event queue depth 
	int refresh_queue_depth;							// loop time of refresh 
	int seniority_age_limit;
	dram_type_t dram_type; 
	row_buffer_policy_t row_buffer_management_policy;	// row buffer management policy? OPEN/CLOSE, etc 
	address_mapping_scheme_t addr_mapping_scheme;		// addr mapping scheme for physical to DRAM addr 
	double datarate;
	bool posted_cas;									// TRUE/FALSE, so the CAS command may be stored and run later
	bool read_write_grouping;
	bool auto_precharge;								// issue cas and prec separately or together? 
	int clock_granularity; 
	int cachelines_per_row;								// dependent variable 
	int chan_count;										// How many logical channels are there ? 
	int rank_count;										// How many ranks are there per channel ? 
	int bank_count;										// How many banks per device? 
	double short_burst_ratio;
	double read_percentage;

public:
	explicit dramSystemConfiguration(std::map<file_io_token_t,std::string> &);

	// friends
	friend dramSystem;
	friend std::ostream &operator<<(std::ostream &, const dramSystem &);
};

#endif