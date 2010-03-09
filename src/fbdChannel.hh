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

#ifndef FBDCHANNEL_H
#define FBDCHANNEL_H
#pragma once

#include "globals.h"
#include "fbdAMB.h"
#include "fbdFrame.h"
#include "Channel.h"
#include "queue.h"

namespace DRAMsimII
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
		tick minProtocolGap(const Command *this_c) const;
		unsigned moveToTime(const tick endTime, tick& transFinishTime);
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
