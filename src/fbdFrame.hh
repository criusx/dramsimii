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

#ifndef FBDFRAME_H
#define FBDFRAME_H
#pragma once

#include "globals.hh"
#include "command.hh"

namespace DRAMsimII
{
	/// @brief represents a fully buffered DIMM frame, containing 3x commands or 1x command + 1x data
	class fbdFrame
	{
	private:
		static Queue<fbdFrame> freeFramePool; // frame objects are stored here to avoid allocating memory after initialization

		FrameType frameType;
		const Command *slots[3];
		Command::CommandType commandSlotType[3];
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
				commandSlotType[0] = Command::INVALID_COMMAND;
			else
				commandSlotType[0] = value->getCommandType();
		}
		void setCommandB(const Command *value)
		{
			slots[1] = value; 
			if (!value)
				// assume that an empty command is data
				commandSlotType[1] = Command::DATA_COMMAND;
			else
				commandSlotType[1] = value->getCommandType();
		}
		void setCommandC(const Command *value)
		{
			slots[2] = value;
			if (!value)
				commandSlotType[2] = Command::DATA_COMMAND;
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

		Command::CommandType getCommandAType() const { return commandSlotType[0]; }
		Command::CommandType getCommandBType() const { return commandSlotType[1]; }
		Command::CommandType getCommandCType() const { return commandSlotType[2]; }


		// friends
		friend std::ostream &DRAMsimII::operator<<(std::ostream &, const DRAMsimII::fbdFrame &);	
	};
}
#endif
