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

using std::ostream;
using std::dec;
using std::hex;
using std::setw;
using namespace DRAMsimII;

// initialize the static members
Queue<Transaction> Transaction::freeTransactionPool(4*COMMAND_QUEUE_SIZE,true);
unsigned Transaction::eventCounter(0);

/// constructor to make a transaction with no values set
Transaction::Transaction():
eventNumber(0xcdcdcdcd),
type(CONTROL_TRANSACTION),
status(0xcdcdcdcd),
length(0xcdcdcdcd),
arrivalTime(0xcdcdcdcd),
enqueueTime(0xcdcdcdcd),
completionTime(0xcdcdcdcd),
decodeTime(0xcdcdcdcd),
PC(0x00),
threadNum(0),
addr(0xcdcdcdcd),
originalTransaction(UINT_MAX)
{}


Transaction::Transaction(const TransactionType type, const tick arrivalTime,const unsigned burstLength, const Address &address, PHYSICAL_ADDRESS programCounter, int threadNumber, const unsigned originalTrans):
eventNumber(eventCounter++),
type(type),
status(0),
length(burstLength),
arrivalTime(arrivalTime),
enqueueTime(0xcdcdcdcd),
completionTime(0xcdcdcdcd),
decodeTime(0xcdcdcdcd),
PC(programCounter),
threadNum(threadNumber),
addr(address),
originalTransaction(originalTrans)
{}


/// copy constructor to duplicate a transaction
Transaction::Transaction(const Transaction &rs):
eventNumber(rs.eventNumber),
type(rs.type),
status(rs.status),
length(rs.length),
arrivalTime(rs.arrivalTime),
enqueueTime(rs.enqueueTime),
completionTime(rs.completionTime),
decodeTime(rs.decodeTime),
PC(rs.PC),
threadNum(rs.threadNum),
addr(rs.addr),
originalTransaction(rs.originalTransaction)
{}

/// constructor to create a transaction with a certain size, enqueue time, attributes, and pointer to encapsulated external transaction
Transaction::Transaction(const TransactionType transType, const tick arrivalTime, const unsigned burstLength, const PHYSICAL_ADDRESS physicalAddress, PHYSICAL_ADDRESS programCounter, int threadNumber, const unsigned originalTrans):
eventNumber(eventCounter++),
type(transType),
status(0),
length(burstLength),
arrivalTime(arrivalTime),
enqueueTime(0xcdcdcdcd),
completionTime(0xcdcdcdcd),
decodeTime(0xcdcdcdcd),
PC(programCounter),
threadNum(threadNumber),
addr(physicalAddress),
originalTransaction(originalTrans)
{}

/// overrides the new operator to draw from the transaction pool instead
void * Transaction::operator new(size_t size)
{
	assert(size == sizeof(Transaction));
	Transaction *newTrans = freeTransactionPool.acquireItem();
	//newTrans->getAddresses().setPhysicalAddress(0);
	//::new(newTrans)Transaction(0);
	return newTrans;
}

/// override the delete operator to send transactions back to the pool
void Transaction::operator delete(void *mem)
{
	Transaction *trans = static_cast<Transaction*>(mem);
	freeTransactionPool.releaseItem(trans);
}

bool Transaction::operator==(const Transaction& right) const
{
	return (/*eventNumber == right.eventNumber &&*/ type == right.type && status == right.status && length == right.length &&
		arrivalTime == right.arrivalTime && enqueueTime == right.enqueueTime && completionTime == right.completionTime &&
		decodeTime == right.decodeTime && addr == right.addr && originalTransaction == right.originalTransaction);
}

bool Transaction::operator !=(const Transaction& right) const
{
	return !(*this == right);
}

/// prints the key attributes of a transaction
ostream &DRAMsimII::operator<<(ostream &os, const Transaction& this_t)
{	
	os << "Q[" << setw(8) << dec << this_t.getEnqueueTime() << "] ";
	os << "D[" << setw(8) << dec << this_t.getDecodeTime() << "] ";
	os << "E[" << setw(8) << dec << this_t.getCompletionTime() << "] ";
	os << this_t.getType();
	if (this_t.type == AUTO_REFRESH_TRANSACTION)
		os << " R[" << this_t.addr.rank << "] ";
	else
		os << "PA[0x" << hex << this_t.getAddresses().physicalAddress << "]";
	return os;
}

/// decodes and prints the transaction type
ostream &DRAMsimII::operator<<(ostream &os, const TransactionType type)
{
	switch (type)
	{
	case IFETCH_TRANSACTION:
		os << "FETCH  ";
		break;
	case WRITE_TRANSACTION:
		os << "WRITE  ";
		break;
	case READ_TRANSACTION:
		os << "READ   ";
		break;
	case PREFETCH_TRANSACTION:
		os << "PREFET ";
		break;
	case AUTO_REFRESH_TRANSACTION:
		os << "REFRSH ";
		break;
	case PER_BANK_REFRESH_TRANSACTION:
		os << "BNKREF ";
		break;
	case AUTO_PRECHARGE_TRANSACTION:
		os << "AUTOPR ";
		break;
	case CONTROL_TRANSACTION:
		os << "CTRL   ";
		break;
	default:
		os << "UNKWN  ";
		break;
	}
	return os;
}
