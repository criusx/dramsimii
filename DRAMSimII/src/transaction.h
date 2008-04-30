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
	/// @brief a request to read or write some portion of memory, atomically
	class transaction
	{
	protected:
		static queue<transaction> freeTransactionPool;	///< transactions are stored here to avoid allocating memory after initialization

		unsigned event_no;					///< the nth event
		transaction_type_t type;			///< transaction type
		int status;							///< status of this transaction
		unsigned length;					///< the number of words requested
		tick_t enqueueTime;					///< time when the transaction enters the MC queue
		tick_t completionTime;				///< time when transaction has completed in DRAM ticks
		tick_t decodeTime;					///< when the transaction was split up into several commands
		addresses addr;						///< the physical, virtual, and mapped representations of the address for this transaction
		const void *originalTransaction;	///< utility pointer in the event that this transaction represents another version of a transaction

	public:
		
		// accessors		
		addresses &getAddresses() { return addr; }									///< get the address of this transaction
		const addresses &getAddresses() const { return addr; }						///< get the address of this transaction
		tick_t getEnqueueTime() const { return enqueueTime; }						///< get its enqueue time
		tick_t getDecodeTime() const { return decodeTime; }							///< get its decode time
		tick_t getCompletionTime() const { return completionTime; }					///< get the completion time
		unsigned getLength() const { return length; }								///< get the number of bytes requested
		transaction_type_t getType() const { return type; }							///< get what type of transaction this is
		unsigned getEventNumber() const { return event_no; }						///< which event number this is
		const void *getOriginalTransaction() const { return originalTransaction; }	///< get the external transaction that this is a representation for

		// mutators
		void setEnqueueTime(const tick_t value) { enqueueTime = value; }
		void setDecodeTime(const tick_t value) { decodeTime = value; }
		void setCompletionTime(const tick_t value) { completionTime = value; }
		void setType(const transaction_type_t value) { type = value; }
		void setLength(const unsigned value) { length = value; }
		void setEventNumber(const unsigned value) { event_no = value; }
		void setOriginalTransaction(const unsigned *value) { originalTransaction = value; }

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
