#ifndef DRAMSYSTEMCONFIGURATION_H
#define DRAMSYSTEMCONFIGURATION_H
#pragma once

#include <fstream>

#include "enumTypes.h"
#include "globals.h"
#include "Settings.h"

namespace DRAMSimII
{
	/// @brief stores the system configuration options for a dramSystem
	class SystemConfiguration
	{ 
	protected:
		CommandOrderingAlgorithm commandOrderingAlgorithm;				///< describes how to place commands into the per bank command queues
		TransactionOrderingAlgorithm transactionOrderingAlgorithm;		///< the algorithm that describes how to place transactions into the queue
		SystemConfigurationType configType;								///< whether the system is standard or FBD
		unsigned refreshTime;											///< the frequency at which refresh commands are scheduled
		RefreshPolicy refreshPolicy;									///< determines how refreshes are handled
		unsigned columnSize;											///< the size of each column, in bytes
		unsigned rowSize;												///< bytes per row (across one rank) 
		unsigned cachelineSize;											///< 32/64/128 etc 
		unsigned seniorityAgeLimit;										///< the oldest a command may be before it takes top priority
		DRAMType dram_type; 
		RowBufferPolicy rowBufferManagementPolicy;						///< row buffer management policy? OPEN/CLOSE, etc 
		AddressMappingScheme addressMappingScheme;						///< addr mapping scheme for physical to DRAM addr 
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
		const unsigned epoch;											///< the amount of time between stats aggregation and reporting
		double shortBurstRatio;
		double readPercentage;											///< the percentage of transactions that are reads
		std::string sessionID;											///< a unique identifier for this run

	public:
		// constructors
		explicit SystemConfiguration(const Settings& settings);
		explicit SystemConfiguration(const SystemConfiguration *rhs);

		// accessors
		RowBufferPolicy getRowBufferManagementPolicy() const { return rowBufferManagementPolicy; }
		AddressMappingScheme getAddressMappingScheme() const { return addressMappingScheme; }
		CommandOrderingAlgorithm getCommandOrderingAlgorithm() const { return commandOrderingAlgorithm; }
		TransactionOrderingAlgorithm getTransactionOrderingAlgorithm() const { return transactionOrderingAlgorithm; }
		SystemConfigurationType getConfigType() const { return configType; }
		unsigned getRankCount() const { return rankCount; }
		unsigned getBankCount() const { return bankCount; }
		unsigned getChannelCount() const { return channelCount; }
		unsigned getRowCount() const { return rowCount; }
		unsigned getColumnCount() const { return columnCount; }
		unsigned getColumnSize() const { return columnSize; }
		unsigned getCachelineSize() const { return cachelineSize; }
		unsigned getRefreshTime() const { return refreshTime; }
		unsigned getSeniorityAgeLimit() const { return seniorityAgeLimit; }
		unsigned getEpoch() const { return epoch; }
		RefreshPolicy getRefreshPolicy() const { return refreshPolicy; }
		DRAMType getDRAMType() const { return dram_type; }
		bool isAutoPrecharge() const { return autoPrecharge; }
		bool isReadWriteGrouping() const { return readWriteGrouping; }
		bool isPostedCAS() const { return postedCAS; }
		double getShortBurstRatio() const { return shortBurstRatio; }
		double getReadPercentage() const { return readPercentage; }
		double Frequency() const { return datarate; }
		const std::string &getSessionID() const { return sessionID; }

		// operator overloads
		SystemConfiguration& operator =(const SystemConfiguration &rs);

		// friends
		friend std::ostream &operator<<(std::ostream &, const System &);	
		friend std::ostream &operator<<(std::ostream &, const SystemConfiguration &);		
	};
}
#endif
