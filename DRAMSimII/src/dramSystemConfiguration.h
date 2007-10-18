#ifndef DRAMSYSTEMCONFIGURATION_H
#define DRAMSYSTEMCONFIGURATION_H
#pragma once

#include <fstream>

#include "enumTypes.h"
#include "globals.h"
#include "dramSettings.h"

namespace DRAMSimII
{
	class dramSystemConfiguration
	{ 
	private:
		command_ordering_algorithm_t command_ordering_algorithm;	// strict or round robin 
		transaction_ordering_algorithm_t transactionOrderingAlgorithm;
		//unsigned perBankQueueDepth;						// command queue size
		system_configuration_type_t config_type;
		unsigned refresh_time;								// loop time of refresh 
		refresh_policy_t refresh_policy;					// 
		unsigned col_size;
		unsigned row_size;									// bytes per row (across one rank) 
		
		unsigned cacheline_size;							// 32/64/128 etc 
		//unsigned history_queue_depth;						// keep track of per channel command history 
		//unsigned completion_queue_depth;					// keep track of per channel command history 
		//unsigned transaction_queue_depth;					// input transaction queue depth 
		//unsigned event_queue_depth;							// pending event queue depth 
		//unsigned refresh_queue_depth;						// loop time of refresh 
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
		unsigned channelCount;								// How many logical channels are there ? 
		unsigned rankCount;								// How many ranks are there per channel ? 
		unsigned bankCount;								// How many banks per device? 
		unsigned rowCount;									// rows per bank
		unsigned columnCount;									// columns per row
		double short_burst_ratio;
		double read_percentage;

	public:
		// constructors
		explicit dramSystemConfiguration(const dramSettings *settings);
		explicit dramSystemConfiguration(const dramSystemConfiguration *rhs);

		// accessors
		row_buffer_policy_t getRowBufferManagementPolicy() const { return row_buffer_management_policy; }
		address_mapping_scheme_t getAddressMappingScheme() const { return addr_mapping_scheme; }
		command_ordering_algorithm_t getCommandOrderingAlgorithm() const { return command_ordering_algorithm; }
		transaction_ordering_algorithm_t getTransactionOrderingAlgorithm() const { return transactionOrderingAlgorithm; }
		system_configuration_type_t getConfigType() const { return config_type; }
		//unsigned getPerBankQueueDepth() const { return perBankQueueDepth; }
		unsigned getRankCount() const { return rankCount; }
		unsigned getBankCount() const { return bankCount; }
		unsigned getChannelCount() const { return channelCount; }
		unsigned getRowCount() const { return rowCount; }
		unsigned getColumnCount() const { return columnCount; }
		unsigned getColumnSize() const { return col_size; }
		unsigned getCachelineSize() const { return cacheline_size; }
		unsigned getRefreshTime() const { return refresh_time; }
		unsigned getSeniorityAgeLimit() const { return seniority_age_limit; }
		refresh_policy_t getRefreshPolicy() const { return refresh_policy; }
		dram_type_t getDRAMType() const { return dram_type; }
		bool isAutoPrecharge() const { return auto_precharge; }
		bool isReadWriteGrouping() const { return read_write_grouping; }
		bool isPostedCAS() const { return posted_cas; }
		double getShortBurstRatio() const { return short_burst_ratio; }
		double getReadPercentage() const { return read_percentage; }
		double Frequency() const { return datarate; }

		// operator overloads
		dramSystemConfiguration& operator =(const dramSystemConfiguration &rs);

		// friends
		friend std::ostream &operator<<(std::ostream &, const dramSystem &);	
		friend std::ostream &operator<<(std::ostream &, const dramSystemConfiguration &);		
	};
}
#endif
