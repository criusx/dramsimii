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

#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

#include "globals.h"
//#include "Settings.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

namespace DRAMsimII
{
	
	/// @brief This class logically represents several interpretations of a memory address
	/// @details A class to store various representations of an address
	/// The same address may be represented as a virtual address, physical address, 
	/// or as channel, rank, bank, column and row identifiers
	class Address
	{
	public:
		// This section defines the address mapping scheme
		// The scheme dictates how a memory address is converted
		// to rank, bank, row, col, byte
		enum AddressMappingScheme
		{
			BURGER_BASE_MAP,
			CLOSE_PAGE_BASELINE,
			INTEL845G_MAP,
			OPEN_PAGE_BASELINE,
			SDRAM_BASE_MAP,
			SDRAM_CLOSE_PAGE_MAP,
			SDRAM_HIPERF_MAP,
			CLOSE_PAGE_LOW_LOCALITY,
			CLOSE_PAGE_HIGH_LOCALITY
		};

	protected:
		static unsigned channelAddressDepth;
		static unsigned rankAddressDepth;
		static unsigned bankAddressDepth;
		static unsigned rowAddressDepth;
		static unsigned columnAddressDepth;
		static unsigned columnSizeDepth;
		static unsigned cacheLineSize;
		static AddressMappingScheme mappingScheme;

		unsigned virtualAddress;			///< the virtual address
		PHYSICAL_ADDRESS physicalAddress;	///< the physical address

		unsigned channel;					///< the enumerated channel id
		unsigned rank;						///< the rank id
		unsigned bank;						///< the bank id
		unsigned row;						///< the row id
		unsigned column;					///< the column id

		// functions
		bool addressTranslation();
		bool reverseAddressTranslation();
	public:

		// functions
		PHYSICAL_ADDRESS static highestAddress();

		// accessors
		PHYSICAL_ADDRESS getPhysicalAddress() const { return physicalAddress; }
		unsigned getChannel() const { return channel; }
		unsigned getRank() const { return rank; }
		unsigned getBank() const { return bank; }
		unsigned getRow() const { return row; }
		unsigned getColumn() const { return column; }

		// mutators
		void setPhysicalAddress(PHYSICAL_ADDRESS pa) { physicalAddress = pa; addressTranslation(); }
		void setAddress(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column);

		// constructor
		Address();						///< the no-arg constructor
		explicit Address(PHYSICAL_ADDRESS pA);	///< the constructor based on a physical address
		explicit Address(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column);

		// initialize
		void static initialize(const Settings &dramSettings);
		void static initialize(const SystemConfiguration &systemConfig);

		// friend		
		friend class boost::serialization::access;
		friend std::ostream &DRAMsimII::operator<<(std::ostream &os, const Address&);

		// overloads
		bool operator==(const Address& right) const;

	private:
		template<class Archive>
		void serialize(Archive & ar, const unsigned version )
		{
			ar & channelAddressDepth & rankAddressDepth & bankAddressDepth & rowAddressDepth & columnAddressDepth & columnSizeDepth & cacheLineSize;
			ar & mappingScheme & virtualAddress & physicalAddress & channel & rank & bank & row & column;
		}
	};
std::ostream& operator<<(std::ostream&, const Address::AddressMappingScheme&);
	
}
#endif
