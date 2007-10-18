#include <assert.h>
#include <iomanip>
#include "transaction.h"

using namespace std;
using namespace DRAMSimII;

// initialize the static member
queue<transaction> transaction::freeTransactionPool(4*COMMAND_QUEUE_SIZE,true);

transaction::transaction():
event_no(0),
type(CONTROL_TRANSACTION),
status(0),
length(0),
enqueueTime(0),
completionTime(0),
decodeTime(0),
addr(),
originalTransaction(0)
{}

transaction::transaction(const transaction *rs):
event_no(rs->event_no),
type(rs->type),
status(rs->status),
length(rs->length),
enqueueTime(rs->enqueueTime),
completionTime(rs->completionTime),
decodeTime(rs->decodeTime),
addr(rs->addr),
originalTransaction(rs->originalTransaction)
{}

transaction::transaction(const int attribute,const tick_t enqueueTime,const int Size,const unsigned long long address, const void *originalTrans):
event_no(0),
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

void * transaction::operator new(size_t size)
{
	assert(size == sizeof(transaction));
	return freeTransactionPool.acquire_item();
}

void transaction::operator delete(void *mem)
{
	transaction *trans = static_cast<transaction*>(mem);
	freeTransactionPool.release_item(trans);
}


std::ostream &DRAMSimII::operator<<(std::ostream &os, const transaction *this_t)
{	
	os << "Q[" << std::setw(8) << std::dec << this_t->getEnqueueTime() << "] ";
	os << "D[" << std::setw(8) << std::dec << this_t->getDecodeTime() << "] ";
	os << "E[" << std::setw(8) << std::dec << this_t->getCompletionTime() << "] ";
	os << this_t->getType();
	if (this_t->type == AUTO_REFRESH_TRANSACTION)
		os << " R[" << this_t->addr.rank_id << "] ";
	else
		os << "PA[0x" << std::hex << this_t->getAddresses().phys_addr << "]";
	return os;
}

ostream &DRAMSimII::operator<<(ostream &os, const transaction_type_t type)
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
