#ifndef FBDCHANNEL_H
#define FBDCHANNEL_H
#pragma once

#include <vector>
#include "fbdAMB.h"
#include "dramChannel.h"

namespace DRAMSimII
{

	class fbdChannel: dramChannel
	{
	private:
		std::vector<fbdAMB> ambs;
	public:
		explicit fbdChannel(const dramSettings *settings);
	};
}

#endif