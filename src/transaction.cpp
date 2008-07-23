#include "transaction.h"

using std::ostream;
using std::dec;
using std::hex;
using std::setw;
using namespace DRAMSimII;

// initialize the static members
Queue<Transaction> Transaction::freeTransactionPool(4*COMMAND_QUEUE_SIZE,true);
unsigned Transaction::eventCounter(0);

/// constructor to make a transaction with no values set
Transaction::Transaction():
eventNumber(0),
type(CONTROL_TRANSACTION),
status(0),
length(0),
arrivalTime(0),
enqueueTime(0),
completionTime(0),
decodeTime(0),
addr(0),
originalTransaction(0)
{}


Transaction::Transaction(const TransactionType type, const tick arrivalTime,const unsigned burstLength, const Address &address, const void *originalTrans):
eventNumber(eventCounter++),
type(type),
status(0),
length(burstLength),
arrivalTime(arrivalTime),
enqueueTime(0),
completionTime(0),
decodeTime(0),
addr(address),
originalTransaction(originalTrans)
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
Transaction::Transaction(const TransactionType transType, const tick arrivalTime,const unsigned burstLength,const unsigned long long physicalAddress, const void *originalTrans):
eventNumber(eventCounter++),
type(transType),
status(0),
length(burstLength),
arrivalTime(arrivalTime),
enqueueTime(0),
completionTime(0),
decodeTime(0),
addr(physicalAddress),
originalTransaction(originalTrans)
{}

/// overrides the new operator to draw from the transaction pool instead
void * Transaction::operator new(size_t size)
{
	assert(size == sizeof(Transaction));
	Transaction *newTrans = freeTransactionPool.acquireItem();
	newTrans->getAddresses().setPhysicalAddress(0);
	return newTrans;
}

/// override the delete operator to send transactions back to the pool
void Transaction::operator delete(void *mem)
{
	Transaction *trans = static_cast<Transaction*>(mem);
	freeTransactionPool.releaseItem(trans);
}

/// prints the key attributes of a transaction
ostream &DRAMSimII::operator<<(ostream &os, const Transaction *this_t)
{	
	os << "Q[" << setw(8) << dec << this_t->getEnqueueTime() << "] ";
	os << "D[" << setw(8) << dec << this_t->getDecodeTime() << "] ";
	os << "E[" << setw(8) << dec << this_t->getCompletionTime() << "] ";
	os << this_t->getType();
	if (this_t->type == AUTO_REFRESH_TRANSACTION)
		os << " R[" << this_t->addr.rank << "] ";
	else
		os << "PA[0x" << hex << this_t->getAddresses().physicalAddress << "]";
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
