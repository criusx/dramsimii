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
