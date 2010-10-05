// Copyright (C) 2010 University of Maryland.
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

#ifndef ADDRESS_HH
#define ADDRESS_HH

#include "globals.hh"

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
		enum AddressMappingPolicy
		{
			MUTIEN1,
			MUTIEN2,
			CLOSE_PAGE_BASELINE,			
			SDRAM_BASE_MAP,
			SDRAM_HIPERF_MAP,
			CLOSE_PAGE_BASELINE_OPT,
			CLOSE_PAGE_LOW_LOCALITY,
			CLOSE_PAGE_HIGH_LOCALITY,
			INTEL845G_MAP,
			BURGER_BASE_MAP
		};

	protected:
		static unsigned channelAddressDepth;	///< the number of bits to represent all channels
		static unsigned rankAddressDepth;		///< the number of bits to represent all ranks
		static unsigned bankAddressDepth;		///< the number of bits to represent all banks
		static unsigned rowAddressDepth;		///< the number of bits to represent all rows
		static unsigned columnAddressDepth;		///< the number of bits to represent all columns
		static unsigned columnSizeDepth;		///< the minimum block that can be addressed
		static unsigned rowLowAddressDepth;		///< the number of bits to represent the lower portion of the row
		static unsigned rowHighAddressDepth;	///< the number of bits to represent the upper portion of the row
		static unsigned columnLowAddressDepth;	///< the number of bits to represent the lower portion of a column
		static unsigned columnHighAddressDepth; ///< the number of bits to represent the upper portion of a column
		static unsigned rankCount;				///< the number of ranks per DIMM
		static AddressMappingPolicy mappingScheme;	///< the mapping scheme to convert physical to logical addresses

		unsigned virtualAddress;			///< the virtual address
		PhysicalAddress physicalAddress;	///< the physical address

		unsigned channel;					///< the enumerated channel id
		unsigned dimm;						///< the dimm id
		unsigned rank;						///< the rank id
		unsigned bank;						///< the bank id
		unsigned row;						///< the row id
		unsigned column;					///< the column id

		// functions
		bool addressTranslation();
		bool reverseAddressTranslation();
	public:

		// functions
		PhysicalAddress static highestAddress();

		// accessors
		PhysicalAddress getPhysicalAddress() const { return physicalAddress; }
		unsigned getChannel() const { return channel; }
		unsigned getDimm() const { return dimm; }
		unsigned getRank() const { return rank; }
		unsigned getBank() const { return bank; }
		unsigned getRow() const { return row; }
		unsigned getColumn() const { return column; }

		// mutators
		void setPhysicalAddress(PhysicalAddress pa) { physicalAddress = pa; addressTranslation(); }
		void setAddress(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column);
		void setAddress(const Address &rhs);
		void setChannel(const unsigned value) { channel = value; }
		void setRank(const unsigned value) { dimm = value / rankCount; rank = value; }
		void setBank(const unsigned value) { bank = value; }
		void setRow(const unsigned value) { row = value; }
		void setColumn(const unsigned value) { column = value; }

		// constructor
		Address();						///< the no-arg constructor
		explicit Address(PhysicalAddress pA);	///< the constructor based on a physical address
		explicit Address(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column);

		// initialize
		void static initialize(const Settings &dramSettings);
		void static initialize(const SystemConfiguration &systemConfig);
		PhysicalAddress static maxAddress();

		friend std::ostream &DRAMsimII::operator<<(std::ostream &os, const Address&);

		// overloads
		bool operator==(const Address& right) const;
		bool operator!=(const Address& right) const;

	};
	std::ostream& operator<<(std::ostream&, const Address::AddressMappingPolicy&);

}
#endif
