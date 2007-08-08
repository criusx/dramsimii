#ifndef TRANSACTION_H
#define TRANSACTION_H
#pragma once

#include <fstream>
#include "globals.h"
#include "addresses.h"
#include "enumTypes.h"
#include "queue.h"

namespace DRAMSimII
{
	class transaction
	{
	private:
		static queue<transaction> freeTransactionPool;	// transactions are stored here to avoid allocating memory after initialization

	public:
		int event_no;
		transaction_type_t type;	// transaction type
		int status;
		int length;					// how long?
		tick_t arrival_time;		// time when first seen by memory controller in DRAM ticks
		tick_t enqueueTime;			// time when the transaction enters the MC queue
		tick_t completion_time;		// time when transaction has completed in DRAM ticks

		addresses addr;
		const void *originalTransaction;	// utility pointer in the event that this transaction represents another version of a transaction

		// constructors
		explicit transaction();
		explicit transaction(const int,const  tick_t,const int,const unsigned long long, const void *);
		explicit transaction(const transaction *rs);
		friend std::ostream &operator<<(std::ostream &, const transaction *);

		void *operator new(size_t size);
		void operator delete(void *);
	};
}
#endif
