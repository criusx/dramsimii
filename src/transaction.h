#ifndef TRANSACTION_H
#define TRANSACTION_H
#pragma once

#include <fstream>
#include <assert.h>
#include <iomanip>
#include "globals.h"
#include "Address.h"
#include "enumTypes.h"
#include "queue.h"

namespace DRAMSimII
{
	/// @brief a request to read or write some portion of memory, atomically
	class Transaction
	{
	protected:
		static Queue<Transaction> freeTransactionPool;	///< transactions are stored here to avoid allocating memory after initialization
		static unsigned eventCounter;		///< keeps track of how many transactions are created

		unsigned eventNumber;				///< the nth event
		TransactionType type;				///< transaction type
		int status;							///< status of this transaction
		unsigned length;					///< the number of words requested
		tick arrivalTime;					///< the time when this transaction originally arrived
		tick enqueueTime;					///< time when the transaction enters the MC queue
		tick completionTime;				///< time when transaction has completed in DRAM ticks
		tick decodeTime;					///< when the transaction was split up into several commands
		Address addr;						///< the physical, virtual, and mapped representations of the address for this transaction
		const void *originalTransaction;	///< utility pointer in the event that this transaction represents another version of a transaction

	public:
		
		// accessors		
		Address &getAddresses() { return addr; }									///< get the address of this transaction
		const Address &getAddresses() const { return addr; }						///< get the address of this transaction
		tick getArrivalTime() const { return arrivalTime; }							///< get its arrival time
		tick getEnqueueTime() const { return enqueueTime; }							///< get its enqueue time
		tick getDecodeTime() const { return decodeTime; }							///< get its decode time
		tick getCompletionTime() const { return completionTime; }					///< get the completion time
		unsigned getLength() const { return length; }								///< get the number of bytes requested
		TransactionType getType() const { return type; }							///< get what type of transaction this is
		unsigned getEventNumber() const { return eventNumber; }						///< which event number this is
		const void *getOriginalTransaction() const { return originalTransaction; }	///< get the external transaction that this is a representation for

		// mutators
		void setEnqueueTime(const tick value) { enqueueTime = value; }
		void setArrivalTime(const tick value) { arrivalTime = value; }
		void setDecodeTime(const tick value) { decodeTime = value; }
		void setCompletionTime(const tick value) { completionTime = value; }
		void setType(const TransactionType value) { type = value; }
		void setLength(const unsigned value) { length = value; }
		void setEventNumber(const unsigned value) { eventNumber = value; }
		void setOriginalTransaction(const unsigned *value) { originalTransaction = value; }

		// constructors
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const Address &address, const void *originalTrans);		
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const unsigned long long physicalAddress, const void *originalTrans);
		explicit Transaction(const Transaction *rs);
		explicit Transaction();
	
	public:
		friend std::ostream &operator<<(std::ostream &, const Transaction *);

		void *operator new(size_t size);
		void operator delete(void *);
	};
}
#endif
