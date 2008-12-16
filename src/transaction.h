// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
// 
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#ifndef TRANSACTION_H
#define TRANSACTION_H
#pragma once

#include "globals.h"
#include "Address.h"
#include "queue.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

namespace DRAMSimII
{
	/// @brief a request to read or write some portion of memory, atomically
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
		PHYSICAL_ADDRESS PC;				///< the program counter associated with this transaction
		int threadNum;						///< the thread number associated with this transaction
		const Address addr;						///< the physical, virtual, and mapped representations of the address for this transaction
		const unsigned originalTransaction;	///< utility counter id in the event that this transaction represents another version of a transaction

	public:
		
		// accessors
		const Address &getAddresses() const { return addr; }						///< get the address of this transaction
		tick getArrivalTime() const { return arrivalTime; }							///< get its arrival time
		tick getEnqueueTime() const { return enqueueTime; }							///< get its enqueue time
		tick getDecodeTime() const { return decodeTime; }							///< get its decode time
		tick getCompletionTime() const { return completionTime; }					///< get the completion time
		PHYSICAL_ADDRESS getProgramCounter() const { return PC; }					///< get the program counter for this transaction
		unsigned getLength() const { return length; }								///< get the number of bytes requested
		TransactionType getType() const { return type; }							///< get what type of transaction this is
		unsigned getEventNumber() const { return eventNumber; }						///< which event number this is
		const unsigned getOriginalTransaction() const { return originalTransaction; }	///< get the external transaction that this is a representation for
		bool isRead() const { return ((type == IFETCH_TRANSACTION) || (type == READ_TRANSACTION) || (type == PREFETCH_TRANSACTION)); }
		bool isWrite() const { return (type == WRITE_TRANSACTION); }

		// mutators
		void setEnqueueTime(const tick value) { enqueueTime = value; }
		void setArrivalTime(const tick value) { arrivalTime = value; }
		void setDecodeTime(const tick value) { decodeTime = value; }
		void setCompletionTime(const tick value) { completionTime = value; }
		
		// constructors
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const Address &address, PHYSICAL_ADDRESS PC, int threadNumber, const unsigned originalTrans);		
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const PHYSICAL_ADDRESS physicalAddress, PHYSICAL_ADDRESS PC, int threadNumber, const unsigned originalTrans);
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
			ar & const_cast<unsigned&>(eventNumber) & const_cast<TransactionType&>(type) & status;
			ar & const_cast<unsigned&>(length) & arrivalTime & enqueueTime & completionTime;
			ar & decodeTime & const_cast<Address&>(addr) & const_cast<unsigned&>(originalTransaction);
		}
	};
}
#endif
