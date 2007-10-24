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
		command_ordering_algorithm_t commandOrderingAlgorithm;	// strict or round robin 
		transaction_ordering_algorithm_t transactionOrderingAlgorithm;
		system_configuration_type_t configType;
		unsigned refreshTime;								// loop time of refresh 
		refresh_policy_t refreshPolicy;					// 
		unsigned columnSize;
		unsigned rowSize;									// bytes per row (across one rank) 
		unsigned cachelineSize;							// 32/64/128 etc 
		unsigned seniorityAgeLimit;
		dram_type_t dram_type; 
		row_buffer_policy_t rowBufferManagementPolicy;	// row buffer management policy? OPEN/CLOSE, etc 
		address_mapping_scheme_t addressMappingScheme;		// addr mapping scheme for physical to DRAM addr 
		double datarate;
		bool postedCAS;									// TRUE/FALSE, so the CAS command may be stored and run later
		bool readWriteGrouping;
		bool autoPrecharge;								// is CAS+P an available command for close page
		int clockGranularity; 
		unsigned cachelinesPerRow;						// dependent variable 
		unsigned channelCount;								// How many logical channels are there ? 
		unsigned rankCount;								// How many ranks are there per channel ? 
		unsigned bankCount;								// How many banks per device? 
		unsigned rowCount;									// rows per bank
		unsigned columnCount;									// columns per row
		double shortBurstRatio;
		double readPercentage;

	public:
		// constructors
		explicit dramSystemConfiguration(const dramSettings *settings);
		explicit dramSystemConfiguration(const dramSystemConfiguration *rhs);

		// accessors
		row_buffer_policy_t getRowBufferManagementPolicy() const { return rowBufferManagementPolicy; }
		address_mapping_scheme_t getAddressMappingScheme() const { return addressMappingScheme; }
		command_ordering_algorithm_t getCommandOrderingAlgorithm() const { return commandOrderingAlgorithm; }
		transaction_ordering_algorithm_t getTransactionOrderingAlgorithm() const { return transactionOrderingAlgorithm; }
		system_configuration_type_t getConfigType() const { return configType; }
		unsigned getRankCount() const { return rankCount; }
		unsigned getBankCount() const { return bankCount; }
		unsigned getChannelCount() const { return channelCount; }
		unsigned getRowCount() const { return rowCount; }
		unsigned getColumnCount() const { return columnCount; }
		unsigned getColumnSize() const { return columnSize; }
		unsigned getCachelineSize() const { return cachelineSize; }
		unsigned getRefreshTime() const { return refreshTime; }
		unsigned getSeniorityAgeLimit() const { return seniorityAgeLimit; }
		refresh_policy_t getRefreshPolicy() const { return refreshPolicy; }
		dram_type_t getDRAMType() const { return dram_type; }
		bool isAutoPrecharge() const { return autoPrecharge; }
		bool isReadWriteGrouping() const { return readWriteGrouping; }
		bool isPostedCAS() const { return postedCAS; }
		double getShortBurstRatio() const { return shortBurstRatio; }
		double getReadPercentage() const { return readPercentage; }
		double Frequency() const { return datarate; }

		// operator overloads
		dramSystemConfiguration& operator =(const dramSystemConfiguration &rs);

		// friends
		friend std::ostream &operator<<(std::ostream &, const dramSystem &);	
		friend std::ostream &operator<<(std::ostream &, const dramSystemConfiguration &);		
	};
}
#endif
