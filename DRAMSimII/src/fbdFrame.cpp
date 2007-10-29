#include "fbdFrame.h"

using namespace std;
using namespace DRAMSimII;

// initialize the static member
queue<fbdFrame> fbdFrame::freeFramePool(4*COMMAND_QUEUE_SIZE,true);

fbdFrame::fbdFrame()
{}

fbdFrame::~fbdFrame()
{}

void *fbdFrame::operator new(size_t size)
{
	assert(size == sizeof(fbdFrame));
	return freeFramePool.acquire_item();
}

void fbdFrame::operator delete(void *mem)
{
	fbdFrame *frame(static_cast<fbdFrame*>(mem));
	freeFramePool.release_item(frame);
}

std::ostream &DRAMSimII::operator<<(std::ostream &os, const DRAMSimII::fbdFrame &frame)
{
	return os;
}
