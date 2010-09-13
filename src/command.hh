// Copyright (C) 2010 University of Maryland.
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

#ifndef COMMAND_H
#define COMMAND_H
#pragma once

#include "globals.hh"
#include "Address.hh"
#include "queue.hh"
#include "transaction.hh"
#include "event.hh"

namespace DRAMsimII
{
	/// @brief represents a DRAM command from the memory controller to the DRAMs
	class Command: public Event
	{
	public:
		/// @brief available command types to send to DRAMs
		/// @details a basic DRAM command may be any of the following
		enum CommandType
		{
			ACTIVATE,							///< open this row
			READ,								///< read from the open row
			READ_AND_PRECHARGE,					///< read from the open row and then close it
			WRITE,								///< write this value to the open row
			WRITE_AND_PRECHARGE,				///< write this value and then close the row
			RETIRE_COMMAND,						///< ?
			PRECHARGE,							///< close this open row
			PRECHARGE_ALL,						///< precharge each bank in the rank, end of a refresh
			ACTIVATE_ALL,						///< activate a row in each bank in the rank, half of a refresh
			DRIVE_COMMAND,						///< ?
			DATA_COMMAND,						///< for FBD, holds data instead of a command
			CAS_WITH_DRIVE_COMMAND,				///< ?
			REFRESH_ALL,						///< refresh all banks in the rank
			SELF_REFRESH,						///< put the rank into self refresh mode
			DESELECT,							///< command inhibit, does not allow new commands to be executed
			NOOP,								///< the no operation command
			INVALID_COMMAND						///< no command, or not a valid command at this time
		};

	protected:
		static Queue<Command> freeCommandPool; ///< command objects are stored here to avoid allocating memory after initialization

		mutable CommandType commandType;///< what type of command this is
		Transaction *hostTransaction;	///< backward pointer to the original transaction
		unsigned length;				///< the burst length

		// assignment operator
		Command &operator=(const Command &rhs);

	public:

		// constructors
		explicit Command(const Command&);
		explicit Command();		
		explicit Command(Transaction *hostTransaction, const tick enqueueTime, const bool autoPrecharge, const unsigned commandLength, const CommandType commandType = READ);
		explicit Command(Transaction *hostTransaction, const Address &addr, const tick enqueueTime, const bool autoPrecharge, const unsigned commandLength, const CommandType commandType = READ);
		~Command();

		// accessors
		CommandType getCommandType() const { return commandType; }
		Transaction *getHost() const { return hostTransaction; }
		unsigned getLength() const { return length; }
		bool isActivate() const { return ((commandType == ACTIVATE) || (commandType == ACTIVATE_ALL)); }
		bool isRead() const { return ((commandType == READ) || (commandType == READ_AND_PRECHARGE)); }
		bool isWrite() const { return ((commandType == WRITE) || (commandType == WRITE_AND_PRECHARGE)); }
		bool isPrecharge() const { return ((commandType == READ_AND_PRECHARGE) || (commandType == WRITE_AND_PRECHARGE) || (commandType == PRECHARGE)); }
		bool isBasicPrecharge() const { return commandType == PRECHARGE; }
		bool isRefresh() const { return (commandType == REFRESH_ALL); }
		bool isReadOrWrite() const { return isRead() || isWrite(); }

		// mutators
		Transaction *removeHost() { Transaction* host = hostTransaction; hostTransaction = NULL; return host; }
		void setAutoPrecharge(const bool autoPrecharge) const;
		
		// friends
		friend std::ostream &DRAMsimII::operator<<(std::ostream &,const Command &);	

		// overloads
		bool operator==(const Command& right) const;
		bool operator!=(const Command& right) const;
		void *operator new(size_t size);
		void operator delete(void *);
	};	

	std::ostream& operator<<(std::ostream&, const DRAMsimII::Command::CommandType&);
}
#endif
