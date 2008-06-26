
#include "transaction.h"

using namespace std;
using namespace DRAMSimII;

// initialize the static members
Queue<Transaction> Transaction::freeTransactionPool(4*COMMAND_QUEUE_SIZE,true);
unsigned Transaction::eventCounter(0);

/// constructor to make a transaction with no values set
Transaction::Transaction():
eventNumber(eventCounter++),
type(CONTROL_TRANSACTION),
status(0),
length(0),
enqueueTime(0),
completionTime(0),
decodeTime(0),
addr(),
originalTransaction(0)
{}

/// copy constructor to duplicate a transaction
Transaction::Transaction(const Transaction *rs):
eventNumber(rs->eventNumber),
type(rs->type),
status(rs->status),
length(rs->length),
enqueueTime(rs->enqueueTime),
completionTime(rs->completionTime),
decodeTime(rs->decodeTime),
addr(rs->addr),
originalTransaction(rs->originalTransaction)
{}

/// constructor to create a transaction with a certain size, enqueue time, attributes, and pointer to encapsulated external transaction
Transaction::Transaction(const int attribute,const tick enqueueTime,const int Size,const unsigned long long address, const void *originalTrans):
eventNumber(eventCounter++),
status(0),
length(Size),
enqueueTime(enqueueTime),
completionTime(0),
decodeTime(0),
addr(address),
originalTransaction(originalTrans)

{
	switch(attribute & 0x07)
	{
	case 1: type = READ_TRANSACTION; break;
	case 2: type = WRITE_TRANSACTION; break;
	case 3: type = PREFETCH_TRANSACTION; break;
	default: cerr << "unknown type of transaction" << endl; exit(-11);
	}
}

/// overrides the new operator to draw from the transaction pool instead
void * Transaction::operator new(size_t size)
{
	assert(size == sizeof(Transaction));
	Transaction *newTrans = freeTransactionPool.acquireItem();
	newTrans->getAddresses().physicalAddress = Address::ADDR_MAX;
	return newTrans;
}

/// override the delete operator to send transactions back to the pool
void Transaction::operator delete(void *mem)
{
	Transaction *trans = static_cast<Transaction*>(mem);
	freeTransactionPool.releaseItem(trans);
}

/// prints the key attributes of a transaction
std::ostream &DRAMSimII::operator<<(std::ostream &os, const Transaction *this_t)
{	
	os << "Q[" << std::setw(8) << std::dec << this_t->getEnqueueTime() << "] ";
	os << "D[" << std::setw(8) << std::dec << this_t->getDecodeTime() << "] ";
	os << "E[" << std::setw(8) << std::dec << this_t->getCompletionTime() << "] ";
	os << this_t->getType();
	if (this_t->type == AUTO_REFRESH_TRANSACTION)
		os << " R[" << this_t->addr.rank << "] ";
	else
		os << "PA[0x" << std::hex << this_t->getAddresses().physicalAddress << "]";
	return os;
}

/// decodes and prints the transaction type
ostream &DRAMSimII::operator<<(ostream &os, const TransactionType type)
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
