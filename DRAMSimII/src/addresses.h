#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

namespace DRAMSimII
{
	/// @brief This class logically represents several interpretations of a memory address
	/// @details A class to store various representations of an address
	/// The same address may be represented as a virtual address, physical address, 
	/// or as channel, rank, bank, column and row identifiers
	class addresses
	{
	public:
		unsigned virtualAddress;			///< the virtual address
		unsigned long long physicalAddress; ///< the physical address
		unsigned channel;					///< the enumerated channel id
		unsigned rank;						///< the rank id
		unsigned bank;						///< the bank id
		unsigned row;						///< the row id
		unsigned column;					///< the column id

		// constructor
		addresses();						///< the no-arg constructor
		explicit addresses(unsigned long long pA);	///< the constructor based on a physical address
	};
}
#endif
