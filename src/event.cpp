#include <assert.h>
#include "event.h"

using namespace DRAMSimII;

// initialize the static member
Queue<Event> Event::freeEventPool(4*COMMAND_QUEUE_SIZE,true);


Event::Event():
eventType(NO_EVENT),
time(0),
eventPointer(NULL)
{}


void * Event::operator new(size_t size)
{
	assert(size == sizeof(Command));
	return freeEventPool.acquireItem();
}

void Event::operator delete(void *mem)
{
	Event *cmd = static_cast<Event*>(mem);
	freeEventPool.releaseItem(cmd);
}
