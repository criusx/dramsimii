#ifndef FBDCHANNEL_H
#define FBDCHANNEL_H
#pragma once

#include "queue.h"
#include "fbdAMB.h"
#include "fbdFrame.h"
#include "dramChannel.h"

namespace DRAMSimII
{

	class fbdChannel: dramChannel
	{
	private:
		//std::vector<fbdChannel> ranks;
		queue<fbdFrame> frameQueue;
	public:
		explicit fbdChannel(const dramSettings& settings, const dramSystemConfiguration &sysConfig);

	protected:
		// overridden virtual functions
		int minProtocolGap(const command *this_c) const;
		const void *moveChannelToTime(const tick_t endTime, tick_t *transFinishTime);
		const command *readNextCommand(const command *slotAFrame) const;

		// functions
		bool makeFrame(const tick_t currentTime);
		bool executeFrame(const tick_t currentTime);
	};
}

#endif
