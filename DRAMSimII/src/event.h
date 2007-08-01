#ifndef EVENT_H
#define EVENT_H
#pragma once

#include "globals.h"
#include "queue.h"
#include "command.h"
#include "transaction.h"

namespace DRAMSimII
{
	// pending event queue
	class event
	{
	private:
		static queue<event> freeEventPool;

	public:
		int event_type; // what kind of event is this?
		tick_t time;
		void *event_ptr; // point to a transaction or command to be enqueued or executed

		event();
		void *operator new(size_t size);
		void operator delete(void *);
	};
}
#endif
