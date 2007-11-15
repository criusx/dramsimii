#ifndef FBDFRAME_H
#define FBDFRAME_H
#pragma once

#include "globals.h"
#include "command.h"
#include "enumTypes.h"


namespace DRAMSimII
{
	class fbdFrame
	{
	private:
		static queue<fbdFrame> freeFramePool; // frame objects are stored here to avoid allocating memory after initialization

		enum frame_type_t frameType;
		command *slots[3];
		tick_t arrivalTime;
		tick_t executeTime;			// the time that this frame should arrive at the every DIMM and the A command should be available

	public:
		// constructor and destructor
		explicit fbdFrame();
		~fbdFrame();

		// operator overloads
		void *operator new(size_t size);
		void operator delete(void *);

		// mutators
		void setSlotA(command *value) { slots[0] = value; }
		void setSlotB(command *value) { slots[1] = value; }
		void setSlotC(command *value) { slots[2] = value; }

		// accessors
		tick_t getExecuteTime() const { return executeTime; }

		// friends
		friend std::ostream &DRAMSimII::operator<<(std::ostream &, const DRAMSimII::fbdFrame &);	
	};
}
#endif