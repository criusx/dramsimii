#ifndef FBDFRAME_H
#define FBDFRAME_H
#pragma once

#include "globals.h"
#include "command.h"


namespace DRAMSimII
{
	class fbdFrame
	{
	private:
		static queue<fbdFrame> freeFramePool; // frame objects are stored here to avoid allocating memory after initialization

		command slots[3];
		tick_t arrivalTime;

	public:
		// constructor and destructor
		explicit fbdFrame();
		~fbdFrame();

		// operator overloads
		void *operator new(size_t size);
		void operator delete(void *);


		// friends
		friend std::ostream &DRAMSimII::operator<<(std::ostream &, const DRAMSimII::fbdFrame &);	
	};
}
#endif