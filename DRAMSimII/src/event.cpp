#include <assert.h>
#include "event.h"

using namespace std;
using namespace DRAMSimII;

// initialize the static member
Queue<event> event::freeEventPool(4*COMMAND_QUEUE_SIZE,true);


event::event()
{
	eventType = 0;
	time = 0;
	event_ptr = NULL;
}


void * event::operator new(size_t size)
{
	assert(size == sizeof(Command));
	return freeEventPool.acquire_item();
}

void event::operator delete(void *mem)
{
	event *cmd = static_cast<event*>(mem);
	freeEventPool.release_item(cmd);
}
