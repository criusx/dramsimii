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
#include "event.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

namespace DRAMsimII
{
	/// @brief a request to read or write some portion of memory, atomically
	class Transaction: public Event
	{
	public:
		enum TransactionType
		{
			IFETCH_TRANSACTION,
			WRITE_TRANSACTION,
			READ_TRANSACTION,
			PREFETCH_TRANSACTION,
			AUTO_REFRESH_TRANSACTION,
			PER_BANK_REFRESH_TRANSACTION,
			AUTO_PRECHARGE_TRANSACTION,
			CONTROL_TRANSACTION
		};

	protected:
		static Queue<Transaction> freeTransactionPool;	///< transactions are stored here to avoid allocating memory after initialization

		const TransactionType type;				///< transaction type
		const unsigned length;					///< the number of words requested
		tick decodeTime;						///< when the transaction was split up into several commands
		PhysicalAddress PC;					///< the program counter associated with this transaction
		int threadNum;							///< the thread number associated with this transaction
		const unsigned originalTransaction;		///< utility counter id in the event that this transaction represents another version of a transaction

	public:

		// accessors
		tick getDecodeTime() const { return decodeTime; }							///< get its decode time
		tick getDecodeDelay() const { return decodeTime - enqueueTime; }			///< get the time it took before this transaction was decoded
		PhysicalAddress getProgramCounter() const { return PC; }					///< get the program counter for this transaction
		unsigned getLength() const { return length; }								///< get the number of bytes requested
		TransactionType getType() const { return type; }							///< get what type of transaction this is
		unsigned getOriginalTransaction() const { return originalTransaction; }		///< get the external transaction that this is a representation for
		bool isRead() const { return ((type == IFETCH_TRANSACTION) || (type == READ_TRANSACTION) || (type == PREFETCH_TRANSACTION)); }
		bool isWrite() const { return (type == WRITE_TRANSACTION); }
		bool isRefresh() const { return (type == AUTO_REFRESH_TRANSACTION); }

		// mutators
		void setDecodeTime(const tick value) { decodeTime = value; }

		// constructors
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const Address &address, PhysicalAddress PC, int threadNumber, const unsigned originalTrans = UINT_MAX);		
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const Address &address, const unsigned originalTrans = UINT_MAX);
		explicit Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const PhysicalAddress physicalAddress, PhysicalAddress PC, int threadNumber, const unsigned originalTrans = UINT_MAX);
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
		void serialize(Archive & ar, const unsigned version)
		{
			if (version == 0)
			{
				ar & boost::serialization::base_object<Event>(*this);
				ar & const_cast<TransactionType&>(type);
				ar & const_cast<unsigned&>(length);
				ar & decodeTime;
				ar & const_cast<unsigned&>(originalTransaction);
			}

		}
	};

	std::ostream& operator<<(std::ostream&, const DRAMsimII::Transaction::TransactionType);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::Transaction&);

}
#endif
