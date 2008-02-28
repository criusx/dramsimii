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
		const command *slots[3];
		enum command_type_t commandSlotType[3];
		tick_t arrivalTime;
		tick_t executeTime;			// the time that the A command of this frame should arrive at the every DIMM

	public:
		// constructor and destructor
		explicit fbdFrame(tick_t time = 0);
		~fbdFrame();

		// operator overloads
		void *operator new(size_t size);
		void operator delete(void *);

		// mutators
		void setCommandA(const command *value)
		{
			slots[0] = value;
			if (!value)
				// can be empty or a command, but never data
				commandSlotType[0] = EMPTY_COMMAND;
			else
				commandSlotType[0] = value->getCommandType();
		}
		void setCommandB(const command *value)
		{
			slots[1] = value; 
			if (!value)
				// assume that an empty command is data
				commandSlotType[1] = DATA_COMMAND;
			else
				commandSlotType[1] = value->getCommandType();
		}
		void setCommandC(const command *value)
		{
			slots[2] = value;
			if (!value)
				commandSlotType[2] = DATA_COMMAND;
			else
				commandSlotType[2] = value->getCommandType();
		}

		// accessors
		tick_t getExecuteTime() const { return executeTime; }
		
		command* getCommandA() const { return (command*)slots[0]; }
		command* getCommandB() const { return (command*)slots[1]; }
		command* getCommandC() const { return (command*)slots[2]; }

		const command* readCommandA() const { return slots[0]; }
		const command* readCommandB() const { return slots[1]; }
		const command* readCommandC() const { return slots[2]; }

		enum command_type_t getCommandAType() const { return commandSlotType[0]; }
		enum command_type_t getCommandBType() const { return commandSlotType[1]; }
		enum command_type_t getCommandCType() const { return commandSlotType[2]; }


		// friends
		friend std::ostream &DRAMSimII::operator<<(std::ostream &, const DRAMSimII::fbdFrame &);	
	};
}
#endif