#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

#include <limits.h>

namespace DRAMSimII
{
	/// @brief This class logically represents several interpretations of a memory address
	/// @details A class to store various representations of an address
	/// The same address may be represented as a virtual address, physical address, 
	/// or as channel, rank, bank, column and row identifiers
	class Address
	{
	public:
		static const unsigned long long ADDR_MAX = ULLONG_MAX;	///< the largest possible address value
		unsigned virtualAddress;			///< the virtual address
		unsigned long long physicalAddress; ///< the physical address
		unsigned channel;					///< the enumerated channel id
		unsigned rank;						///< the rank id
		unsigned bank;						///< the bank id
		unsigned row;						///< the row id
		unsigned column;					///< the column id

		// constructor
		Address();						///< the no-arg constructor
		explicit Address(unsigned long long pA);	///< the constructor based on a physical address
	};
}
#endif
