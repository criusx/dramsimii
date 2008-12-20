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

#include <assert.h>
#include "event.h"

using namespace DRAMsimII;

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
