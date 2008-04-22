#ifndef DRAMSYSTEMCONFIGURATION_H
#define DRAMSYSTEMCONFIGURATION_H
#pragma once

#include <fstream>

#include "enumTypes.h"
#include "globals.h"
#include "dramSettings.h"

namespace DRAMSimII
{
	/// @brief stores the system configuration options for a dramSystem
	class dramSystemConfiguration
	{ 
	protected:
		command_ordering_algorithm_t commandOrderingAlgorithm;			///< describes how to place commands into the per bank command queues
		transaction_ordering_algorithm_t transactionOrderingAlgorithm;	///< the algorithm that describes how to place transactions into the queue
		system_configuration_type_t configType;							///< whether the system is standard or FBD
		unsigned refreshTime;											///< the frequency at which refresh commands are scheduled
		refresh_policy_t refreshPolicy;									///< determines how refreshes are handled
		unsigned columnSize;											///< the size of each column, in bytes
		unsigned rowSize;												///< bytes per row (across one rank) 
		unsigned cachelineSize;											///< 32/64/128 etc 
		unsigned seniorityAgeLimit;										///< the oldest a command may be before it takes top priority
		dram_type_t dram_type; 
		row_buffer_policy_t rowBufferManagementPolicy;					///< row buffer management policy? OPEN/CLOSE, etc 
		address_mapping_scheme_t addressMappingScheme;					///< addr mapping scheme for physical to DRAM addr 
		double datarate;												///< the operating frequency of the system
		bool postedCAS;													///< TRUE/FALSE, so the CAS command may be stored and run later
		bool readWriteGrouping;											///< whether or not reads and writes should be grouped closely
		bool autoPrecharge;												///< is CAS+P an available command for close page
		int clockGranularity; 
		unsigned cachelinesPerRow;										///< dependent variable 
		unsigned channelCount;											///< How many logical channels are there ? 
		unsigned rankCount;												///< How many ranks are there per channel ? 
		unsigned bankCount;												///< How many banks per device? 
		unsigned rowCount;												///< rows per bank
		unsigned columnCount;											///< columns per row
		double shortBurstRatio;
		double readPercentage;											///< the percentage of transactions that are reads

	public:
		// constructors
		explicit dramSystemConfiguration(const dramSettings& settings);
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
