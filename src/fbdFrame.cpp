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

#include "fbdFrame.h"

using namespace std;
using namespace DRAMSimII;

// initialize the static member
Queue<fbdFrame> fbdFrame::freeFramePool(4*COMMAND_QUEUE_SIZE,true);

fbdFrame::fbdFrame(tick time): 
executeTime(time)
{
	commandSlotType[0] = commandSlotType[1] = commandSlotType[2] = INVALID_COMMAND;
}

fbdFrame::~fbdFrame()
{}

void *fbdFrame::operator new(size_t size)
{
	assert(size == sizeof(fbdFrame));
	return freeFramePool.acquireItem();
}

void fbdFrame::operator delete(void *mem)
{
	fbdFrame *frame(static_cast<fbdFrame*>(mem));
	freeFramePool.releaseItem(frame);
}

std::ostream &DRAMSimII::operator<<(std::ostream &os, const DRAMSimII::fbdFrame &frame)
{
	return os;
}
