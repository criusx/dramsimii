#ifndef FBDFRAME_H
#define FBDFRAME_H
#pragma once

#include "globals.h"
#include "command.h"

namespace DRAMSimII
{
	/// @brief represents a fully buffered DIMM frame, containing 3x commands or 1x command + 1x data
	class fbdFrame
	{
	private:
		static Queue<fbdFrame> freeFramePool; // frame objects are stored here to avoid allocating memory after initialization

		enum FrameType frameType;
		const Command *slots[3];
		enum CommandType commandSlotType[3];
		tick arrivalTime;
		tick executeTime;			// the time that the A command of this frame should arrive at the every DIMM

	public:
		// constructor and destructor
		explicit fbdFrame(tick time = 0);
		~fbdFrame();

		// operator overloads
		void *operator new(size_t size);
		void operator delete(void *);

		// mutators
		void setCommandA(const Command *value)
		{
			slots[0] = value;
			if (!value)
				// can be empty or a command, but never data
				commandSlotType[0] = INVALID_COMMAND;
			else
				commandSlotType[0] = value->getCommandType();
		}
		void setCommandB(const Command *value)
		{
			slots[1] = value; 
			if (!value)
				// assume that an empty command is data
				commandSlotType[1] = DATA_COMMAND;
			else
				commandSlotType[1] = value->getCommandType();
		}
		void setCommandC(const Command *value)
		{
			slots[2] = value;
			if (!value)
				commandSlotType[2] = DATA_COMMAND;
			else
				commandSlotType[2] = value->getCommandType();
		}

		// accessors
		tick getExecuteTime() const { return executeTime; }
		
		Command* getCommandA() const { return (Command*)slots[0]; }
		Command* getCommandB() const { return (Command*)slots[1]; }
		Command* getCommandC() const { return (Command*)slots[2]; }

		const Command* readCommandA() const { return slots[0]; }
		const Command* readCommandB() const { return slots[1]; }
		const Command* readCommandC() const { return slots[2]; }

		enum CommandType getCommandAType() const { return commandSlotType[0]; }
		enum CommandType getCommandBType() const { return commandSlotType[1]; }
		enum CommandType getCommandCType() const { return commandSlotType[2]; }


		// friends
		friend std::ostream &DRAMSimII::operator<<(std::ostream &, const DRAMSimII::fbdFrame &);	
	};
}
#endif
