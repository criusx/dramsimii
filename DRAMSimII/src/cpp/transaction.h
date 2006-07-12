#ifndef TRANSACTION_H
#define TRANSACTION_H
#pragma once

#include <fstream>
#include "addresses.h"
#include "enumTypes.h"
#include "globals.h"


class transaction
{
public:
	int event_no;
	transaction_type_t type;	// transaction type
	int status;
	int length;					// how long?
	tick_t arrival_time;		// time when first seen by memory controller in DRAM ticks
	tick_t completion_time;		// time when transaction has completed in DRAM ticks
	addresses addr;
	transaction();
	friend std::ostream &operator<<(std::ostream &, const transaction *);
};

#endif