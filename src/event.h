#ifndef EVENT_H
#define EVENT_H
#pragma once

#include "globals.h"
#include "queue.h"
#include "command.h"
#include "transaction.h"

namespace DRAMSimII
{
	/// @brief pending event queue
	class Event
	{
	private:
		static Queue<Event> freeEventPool;

	public:
		EventType eventType;	///< what kind of event is this?
		tick time;				///< the time that this event happens
		void *eventPointer;		///< point to a transaction or command to be enqueued or executed

		Event();
		void *operator new(size_t size);
		void operator delete(void *);
	};
}
#endif
