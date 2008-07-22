#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

#include <limits.h>
#include "Settings.h"

namespace DRAMSimII
{
	/// @brief This class logically represents several interpretations of a memory address
	/// @details A class to store various representations of an address
	/// The same address may be represented as a virtual address, physical address, 
	/// or as channel, rank, bank, column and row identifiers
	class Address
	{
	private:
		static unsigned chan_addr_depth;
		static unsigned rank_addr_depth;
		static unsigned bank_addr_depth;
		static unsigned row_addr_depth;
		static unsigned col_addr_depth;
		//FIXME: shouldn't this already be set appropriately?
		static unsigned col_size_depth;
		static unsigned cacheLineSize;
		static AddressMappingScheme mappingScheme;

	public:
		unsigned virtualAddress;			///< the virtual address
	private:
		PHYSICAL_ADDRESS physicalAddress; ///< the physical address
	public:
		unsigned channel;					///< the enumerated channel id
		unsigned rank;						///< the rank id
		unsigned bank;						///< the bank id
		unsigned row;						///< the row id
		unsigned column;					///< the column id

		// functions
		bool convertAddress();
		void setPhysicalAddress(PHYSICAL_ADDRESS pa) { physicalAddress = pa; convertAddress(); }
		unsigned long long getPhysicalAddress() const { return physicalAddress; }

		// constructor
		Address();						///< the no-arg constructor
		explicit Address(PHYSICAL_ADDRESS pA);	///< the constructor based on a physical address
		explicit Address(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column);

		// initialize
		void static initialize(const Settings &dramSettings);

		// friend
		friend std::ostream &DRAMSimII::operator<<(std::ostream &os, const Address &this_a);
		friend std::ostream &DRAMSimII::operator<<(std::ostream &os, const Transaction *this_t);
	};
}
#endif
