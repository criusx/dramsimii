#include <assert.h>
#include "event.h"

using namespace std;

// initialize the static member
queue<event> event::freeEventPool(4*COMMAND_QUEUE_SIZE,true);


event::event()
{
	event_type = 0;
	time = 0;
	event_ptr = NULL;
}


void * event::operator new(size_t size)
{
	assert(size == sizeof(command));
	return freeEventPool.acquire_item();
}

void event::operator delete(void *mem)
{
	event *cmd = static_cast<event*>(mem);
	freeEventPool.release_item(cmd);
}
