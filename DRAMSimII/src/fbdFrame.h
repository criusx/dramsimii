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
		enum command_type_t commandSlotType[3];
		tick_t arrivalTime;
		tick_t executeTime;			// the time that this frame should arrive at the every DIMM and the A command should be available

	public:
		// constructor and destructor
		explicit fbdFrame(tick_t time);
		~fbdFrame();

		// operator overloads
		void *operator new(size_t size);
		void operator delete(void *);

		// mutators
		void setSlotA(command *value)
		{
			slots[0] = value;
			if (!value)
				// can be empty or a command, but never data
				commandSlotType[0] = EMPTY_COMMAND;
			else
				commandSlotType[0] = value->getCommandType();
		}
		void setSlotB(command *value)
		{
			slots[1] = value; 
			if (!value)
				// assume that an empty command is data
				commandSlotType[1] = DATA_COMMAND;
			else
				commandSlotType[1] = value->getCommandType();
		}
		void setSlotC(command *value)
		{
			slots[2] = value;
			if (!value)
				commandSlotType[2] = DATA_COMMAND;
			else
				commandSlotType[2] = value->getCommandType();
		}

		// accessors
		tick_t getExecuteTime() const { return executeTime; }
		const command* getSlotA() const { return slots[0]; }
		const command* getSlotB() const { return slots[1]; }
		const command* getSlotC() const { return slots[2]; }

		// friends
		friend std::ostream &DRAMSimII::operator<<(std::ostream &, const DRAMSimII::fbdFrame &);	
	};
}
#endif