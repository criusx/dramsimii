#ifndef FBDSYSTEM_H
#define FBDSYSTEM_H
#pragma once

#include <vector>
#include <set>
#include "fbdChannel.h"

namespace DRAMSimII
{
	class fbdSystem
	{
	private:
		std::vector<fbdChannel> fbdChannels;	// the serial channels
		std::vector<dramChannel> dramChannels;	// used to keep track of the channels hanging off the AMBs
		std::multiset<
	public:
		fbdSystem(void);
		~fbdSystem(void);
	};
}

#endif