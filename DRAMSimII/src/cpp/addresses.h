#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

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
	explicit addresses(unsigned long long pA) { phys_addr = pA; }
};

#endif
