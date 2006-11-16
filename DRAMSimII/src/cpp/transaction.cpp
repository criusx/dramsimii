#include <assert.h>
#include "enumTypes.h"
#include "transaction.h"

using namespace std;

// initialize the static member
queue<transaction> transaction::freeTransactionPool(4*COMMAND_QUEUE_SIZE,true);

transaction::transaction()
{
	arrival_time = 0;
	completion_time = 0;
	status = 0;
	event_no = 0;

	type = CONTROL_TRANSACTION;
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