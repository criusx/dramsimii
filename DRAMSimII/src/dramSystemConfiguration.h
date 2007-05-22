#ifndef DRAMSYSTEMCONFIGURATION_H
#define DRAMSYSTEMCONFIGURATION_H
#pragma once

#include <fstream>
#include <map>
#include "enumTypes.h"
#include "globals.h"
#include "dramSettings.h"

class dramSystemConfiguration
{ 

private:
	command_ordering_algorithm_t command_ordering_algorithm;	// strict or round robin 
	unsigned per_bank_queue_depth;						// command queue size
	system_configuration_type_t config_type;
	unsigned refresh_time;								// loop time of refresh 
	refresh_policy_t refresh_policy;					// 
	unsigned col_size;
	unsigned row_size;									// bytes per row (across one rank) 
	unsigned row_count;									// rows per bank
	unsigned col_count;									// columns per row
	unsigned cacheline_size;							// 32/64/128 etc 
	unsigned history_queue_depth;						// keep track of per channel command history 
	unsigned completion_queue_depth;					// keep track of per channel command history 
	unsigned transaction_queue_depth;					// input transaction queue depth 
	unsigned event_queue_depth;							// pending event queue depth 
	unsigned refresh_queue_depth;						// loop time of refresh 
	unsigned seniority_age_limit;
	dram_type_t dram_type; 
	row_buffer_policy_t row_buffer_management_policy;	// row buffer management policy? OPEN/CLOSE, etc 
	address_mapping_scheme_t addr_mapping_scheme;		// addr mapping scheme for physical to DRAM addr 
	double datarate;
	bool posted_cas;									// TRUE/FALSE, so the CAS command may be stored and run later
	bool read_write_grouping;
	bool auto_precharge;								// issue cas and prec separately or together? 
	int clock_granularity; 
	unsigned cachelines_per_row;						// dependent variable 
	unsigned chan_count;								// How many logical channels are there ? 
	unsigned rank_count;								// How many ranks are there per channel ? 
	unsigned bank_count;								// How many banks per device? 
	double short_burst_ratio;
	double read_percentage;

public:
	// constructors
	explicit dramSystemConfiguration(std::map<file_io_token_t,std::string> &);
	explicit dramSystemConfiguration(const dramSettings *settings);
	
	// friends
	friend class dramSystem;
	friend std::ostream &operator<<(std::ostream &, const dramSystemConfiguration &);

	// accessors
	command_ordering_algorithm_t getCommandOrderingAlgorithm() const { return command_ordering_algorithm; }
	system_configuration_type_t getConfigType() const { return config_type; }
	unsigned getPerBankQueueDepth() const { return per_bank_queue_depth; }
	unsigned getRankCount() const { return rank_count; }
	unsigned getBankCount() const { return bank_count; }
	double getShortBurstRatio() const { return short_burst_ratio; }
	double getReadPercentage() const { return read_percentage; }
	double Frequency() const { return datarate; }

	// friends
	friend std::ostream &operator<<(std::ostream &, const dramSystem &);	
};

#endif