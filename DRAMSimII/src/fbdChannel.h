#ifndef FBDCHANNEL_H
#define FBDCHANNEL_H
#pragma once

#include <vector>
#include "fbdAMB.h"

namespace DRAMSimII
{

	class fbdChannel
	{
	private:
		std::vector<fbdAMB> ambs;
	public:
		fbdChannel(void);
		~fbdChannel(void);
	};
}

#endif