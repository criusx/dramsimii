#ifndef TRANSACTION_H
#define TRANSACTION_H
#pragma once

#include "globals.h"
#include "Address.h"
#include "queue.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/is_abstract.hpp>

namespace DRAMSimII
{
	/// @brief a request to read or write some portion of memory, atomically
	//template<class OriginalTransactionType>
	class Transaction
	{
	protected:
		static Queue<Transaction> freeTransactionPool;	///< transactions are stored here to avoid allocating memory after initialization
		static unsigned eventCounter;		///< keeps track of how many transactions are created

		const unsigned eventNumber;				///< the nth event
		const TransactionType type;				///< transaction type
		int status;							///< status of this transaction
		const unsigned length;					///< the number of words requested
		tick arrivalTime;					///< the time when this transaction originally arrived
		tick enqueueTime;					///< time when the transaction enters the MC queue
		tick completionTime;				///< time when transaction has completed in DRAM ticks
		tick decodeTime;					///< when the transaction was split up into several commands
		const Address addr;						///< the physical, virtual, and mapped representations of the address for this transaction
		//const OriginalTransactionType *originalTransaction;	///< utility pointer in the event that this transaction represents another version of a transaction
		const void *originalTransaction;	///< utility pointer in the event that this transaction represents another version of a transaction

	public:
		
		// accessors		
		//Address &getAddresses() { return addr; }									///< get the address of this transaction
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
		//void setType(const TransactionType value) { type = value; }
		//void setLength(const unsigned value) { length = value; }
		//void setEventNumber(const unsigned value) { eventNumber = value; }
		void setOriginalTransaction(const unsigned *value) { originalTransaction = value; }

		// constructors
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const Address &address, const void *originalTrans);		
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const PHYSICAL_ADDRESS physicalAddress, const void *originalTrans);
		explicit Transaction(const Transaction &rhs);
		explicit Transaction();
	
	public:
		friend std::ostream &operator<<(std::ostream &, const Transaction &);

		// overloads
		void *operator new(size_t size);
		void operator delete(void *);
		bool operator==(const Transaction& right) const;
		bool operator!=(const Transaction& right) const;

	private:
		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned int version)
		{
			//ar & freeTransactionPool;
			//ar & eventCounter;
			ar & const_cast<unsigned&>(eventNumber) & const_cast<TransactionType&>(type) & status;
			ar & const_cast<unsigned&>(length) & arrivalTime & enqueueTime & completionTime;
			ar & decodeTime & const_cast<Address&>(addr);
			//ar & const_cast<OriginalTransactionType *>(originalTransaction);
		}
	};
}
#endif
