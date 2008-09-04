#ifndef FBDCHANNEL_H
#define FBDCHANNEL_H
#pragma once

#include "globals.h"
#include "fbdAMB.h"
#include "fbdFrame.h"
#include "Channel.h"
#include "queue.h"

namespace DRAMSimII
{
	/// @brief a fully buffered DIMM channel
	class fbdChannel: Channel
	{
	private:
		//std::vector<fbdChannel> ranks;
		Queue<fbdFrame> frameQueue;	///< the queue of upcoming fbd frames
	public:
		explicit fbdChannel(const Settings& settings, const SystemConfiguration &sysConfig, Statistics &stats);

	protected:
		// overridden virtual functions
		int minProtocolGap(const Command *this_c) const;
		unsigned moveChannelToTime(const tick endTime, tick& transFinishTime);
		const Command *readNextCommand(const Command *slotAFrame, const Command *slotBFrame) const;
		Command *getNextCommand(const Command *slotAFrame, const Command *slotBFrame);
		fbdFrame *getNextFrame();
		tick nextFrameExecuteTime() const;

		// functions
		bool makeFrame(const tick currentTime);
		bool executeFrame(const tick currentTime);

		fbdChannel& operator=(const fbdChannel& rs);
	};
}

#endif
