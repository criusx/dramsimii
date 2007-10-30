#ifndef FBDSYSTEM_H
#define FBDSYSTEM_H
#pragma once

#include <vector>

namespace DRAMSimII
{

	class fbdSystem
	{
	private:
		std::vector<fbdChannel> channels;
	public:
		fbdSystem(void);
		~fbdSystem(void);
	};
}

#endif