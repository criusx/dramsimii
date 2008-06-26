#include "fbdFrame.h"

using namespace std;
using namespace DRAMSimII;

// initialize the static member
Queue<fbdFrame> fbdFrame::freeFramePool(4*COMMAND_QUEUE_SIZE,true);

fbdFrame::fbdFrame(tick time): 
executeTime(time)
{
	commandSlotType[0] = commandSlotType[1] = commandSlotType[2] = NO_COMMAND;
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
