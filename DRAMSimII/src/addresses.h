#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

namespace DRAMSimII
{
	class addresses
	{
	public:
		unsigned virt_addr;	// virtual address
		unsigned long long phys_addr; // physical address
		unsigned chan_id; // logical channel id
		unsigned rank_id; // device id
		unsigned bank_id;
		unsigned row_id;
		unsigned col_id; // column address

		// constructor
		addresses();
		explicit addresses(unsigned long long pA):
		virt_addr(0),
			phys_addr(pA),
			chan_id(0),
			rank_id(0),
			bank_id(0),
			row_id(0),
			col_id(0)
		{ }
	};
}
#endif
