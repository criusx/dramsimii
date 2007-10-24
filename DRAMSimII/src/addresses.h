#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

namespace DRAMSimII
{
	class addresses
	{
	public:
		unsigned virtualAddress;	// virtual address
		unsigned long long physicalAddress; // physical address
		unsigned channel; // logical channel id
		unsigned rank; // device id
		unsigned bank;
		unsigned row;
		unsigned column; // column address

		// constructor
		addresses();
		explicit addresses(unsigned long long pA);
	};
}
#endif
