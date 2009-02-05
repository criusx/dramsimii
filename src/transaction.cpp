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

#include "transaction.h"

#include <iomanip>

using std::ostream;
using std::dec;
using std::hex;
using std::setw;
using namespace DRAMsimII;

// initialize the static members
Queue<Transaction> Transaction::freeTransactionPool(4*POOL_SIZE,true);

/// constructor to make a transaction with no values set
Transaction::Transaction():
Event(),
type(CONTROL_TRANSACTION),
length(0),
decodeTime(0),
PC(0),
threadNum(0),
originalTransaction(UINT_MAX)
{}


Transaction::Transaction(const TransactionType transType, const tick arrivalTime,const unsigned burstLength, const Address &address, PHYSICAL_ADDRESS programCounter, int threadNumber, const unsigned originalTrans):
Event(arrivalTime,address),
type(transType),
length(burstLength),
decodeTime(0),
PC(programCounter),
threadNum(threadNumber),
originalTransaction(originalTrans)
{}

Transaction::Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const Address &address, const unsigned originalTrans):
Event(arrivalTime,address),
type(transType),
length(burstLength),
decodeTime(0),
PC(0),
threadNum(0),
originalTransaction(originalTrans)
{}

/// copy constructor to duplicate a transaction
Transaction::Transaction(const Transaction &rs):
Event(rs),
type(rs.type),
length(rs.length),
decodeTime(rs.decodeTime),
PC(rs.PC),
threadNum(rs.threadNum),
originalTransaction(rs.originalTransaction)
{}

/// constructor to create a transaction with a certain size, enqueue time, attributes, and pointer to encapsulated external transaction
Transaction::Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const PHYSICAL_ADDRESS physicalAddress, PHYSICAL_ADDRESS programCounter, int threadNumber, const unsigned originalTrans):
Event(arrivalTime,physicalAddress),
type(transType),
length(burstLength),
decodeTime(0),
PC(programCounter),
threadNum(threadNumber),
originalTransaction(originalTrans)
{}

/// overrides the new operator to draw from the transaction pool instead
void *Transaction::operator new(size_t size)
{
	assert(size == sizeof(Transaction));
	//Transaction *newTrans = freeTransactionPool.acquireItem();
	return freeTransactionPool.acquireItem();
	//newTrans->getAddresses().setPhysicalAddress(0);
	//::new(newTrans)Transaction(0);
	//return newTrans;
}

/// override the delete operator to send transactions back to the pool
void Transaction::operator delete(void *mem)
{
	Transaction *trans = static_cast<Transaction*>(mem);
	freeTransactionPool.releaseItem(trans);
}

bool Transaction::operator==(const Transaction& right) const
{
	return (/*eventNumber == right.eventNumber &&*/ type == right.type && /*status == right.status && */length == right.length &&
		arrivalTime == right.arrivalTime && enqueueTime == right.enqueueTime && completionTime == right.completionTime &&
		decodeTime == right.decodeTime && this->Event::operator==(right) && originalTransaction == right.originalTransaction);
}

bool Transaction::operator !=(const Transaction& right) const
{
	return !(*this == right);
}

/// prints the key attributes of a transaction
ostream &DRAMsimII::operator<<(ostream &os, const Transaction& thisTransaction)
{	
	return os << thisTransaction.getType() << ((Event&)thisTransaction);
}

/// decodes and prints the transaction type
ostream &DRAMsimII::operator<<(ostream &os, const Transaction::TransactionType type)
{
	switch (type)
	{
	case Transaction::IFETCH_TRANSACTION:
		os << "FETCH  ";
		break;
	case Transaction::WRITE_TRANSACTION:
		os << "WRITE  ";
		break;
	case Transaction::READ_TRANSACTION:
		os << "READ   ";
		break;
	case Transaction::PREFETCH_TRANSACTION:
		os << "PREFET ";
		break;
	case Transaction::AUTO_REFRESH_TRANSACTION:
		os << "REFRSH ";
		break;
	case Transaction::PER_BANK_REFRESH_TRANSACTION:
		os << "BNKREF ";
		break;
	case Transaction::AUTO_PRECHARGE_TRANSACTION:
		os << "AUTOPR ";
		break;
	case Transaction::CONTROL_TRANSACTION:
		os << "CTRL   ";
		break;
	default:
		os << "UNKWN  ";
		break;
	}
	return os;
}
