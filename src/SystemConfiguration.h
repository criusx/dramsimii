// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
//
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DRAMSYSTEMCONFIGURATION_H
#define DRAMSYSTEMCONFIGURATION_H
#pragma once

#include "globals.h"
#include "Settings.h"
#include "Address.h"
//#include "Channel.h"

#include <fstream>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

namespace DRAMsimII
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
		DRAMType dramType;
		RowBufferPolicy rowBufferManagementPolicy;						///< row buffer management policy? OPEN/CLOSE, etc
		Address::AddressMappingScheme addressMappingScheme;						///< addr mapping scheme for physical to DRAM addr
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
		unsigned decodeWindow;											///< how many transactions to consider when decoding to commands
		const unsigned epoch;											///< the amount of time between stats aggregation and reporting
		double shortBurstRatio;
		double readPercentage;											///< the percentage of transactions that are reads
		std::string sessionID;											///< a unique identifier for this run

	public:
		// constructors
		explicit SystemConfiguration(const Settings& settings);
		
		// accessors
		RowBufferPolicy getRowBufferManagementPolicy() const { return rowBufferManagementPolicy; }
		Address::AddressMappingScheme getAddressMappingScheme() const { return addressMappingScheme; }
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
		DRAMType getDRAMType() const { return dramType; }
		bool isAutoPrecharge() const { return autoPrecharge; }
		bool isReadWriteGrouping() const { return readWriteGrouping; }
		bool isPostedCAS() const { return postedCAS; }
		double getShortBurstRatio() const { return shortBurstRatio; }
		double getReadPercentage() const { return readPercentage; }
		double Frequency() const { return datarate; }
		const std::string &getSessionID() const { return sessionID; }
		unsigned getDecodeWindow() const { return decodeWindow; }

		// operator overloads
		SystemConfiguration& operator =(const SystemConfiguration &rs);
		bool operator ==(const SystemConfiguration &) const;

		// friends
		friend std::ostream &operator<<(std::ostream &, const System &);	
		friend std::ostream &operator<<(std::ostream &, const SystemConfiguration &);		
		

	private:
		friend class boost::serialization::access;
		
		template<class Archive>
		void serialize(Archive& ar, const unsigned version)
		{
			ar & commandOrderingAlgorithm & transactionOrderingAlgorithm & configType & refreshTime & refreshPolicy & columnSize & rowSize;
			ar & cachelineSize & seniorityAgeLimit & dramType & rowBufferManagementPolicy & addressMappingScheme & datarate & postedCAS;
			ar & readWriteGrouping & autoPrecharge & clockGranularity & cachelinesPerRow & channelCount & rankCount & bankCount & rowCount;
			ar & columnCount & shortBurstRatio & readPercentage & sessionID;
			//ar & epoch;
		}

		template <class Archive>
		friend inline void save_construct_data(Archive& ar, const DRAMsimII::SystemConfiguration *t, const unsigned version)
		{
			ar << t->epoch;
		}

		template <class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMsimII::SystemConfiguration *t, const unsigned version)
		{
			Settings s;
			ar >> s.epoch;			
			
			new(t)DRAMsimII::SystemConfiguration(s);
		}

	};
}
#endif
