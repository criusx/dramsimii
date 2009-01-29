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

#include <assert.h>
#include "command.h"

using std::cerr;
using std::endl;
using std::ostream;
using namespace DRAMsimII;

// initialize the static member
Queue<Command> Command::freeCommandPool(8*POOL_SIZE,true);

Command::Command():
commandType(RETIRE_COMMAND),
startTime(0),
enqueueTime(0),
completionTime(0),
addr(),
hostTransaction(NULL),
postedCAS(false),
length(0)
{}


Command::Command(const Command &rhs):
commandType(rhs.commandType),
startTime(rhs.startTime),
enqueueTime(rhs.enqueueTime),
completionTime(rhs.completionTime),
addr(rhs.addr),
hostTransaction(rhs.hostTransaction ? new Transaction(*rhs.hostTransaction) : NULL),
postedCAS(rhs.postedCAS),
length(rhs.length)
{
	assert(!hostTransaction ||
		(commandType == WRITE_AND_PRECHARGE && hostTransaction->getType() == WRITE_TRANSACTION) ||
		(commandType == READ_AND_PRECHARGE && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == READ && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == WRITE && hostTransaction->getType() == WRITE_TRANSACTION) ||		 
		(commandType == REFRESH_ALL && hostTransaction->getType() == AUTO_REFRESH_TRANSACTION) ||
		(commandType == ACTIVATE) || (commandType == PRECHARGE)
		);
}

Command::Command(Transaction& hostTransaction, const tick enqueueTime, const bool postedCAS, const bool autoPrecharge, const unsigned commandLength, const CommandType type):
startTime(-1),
enqueueTime(enqueueTime),
completionTime(-1),
addr(hostTransaction.getAddresses()),
hostTransaction(type == READ ? &hostTransaction : NULL), // this link is only needed for CAS commands
postedCAS(postedCAS),
length(commandLength)
{
	if (type == READ)
	{
		switch (hostTransaction.getType())
		{
		case AUTO_REFRESH_TRANSACTION:
			commandType = REFRESH_ALL;
			break;
		case WRITE_TRANSACTION:
			commandType = autoPrecharge ? WRITE_AND_PRECHARGE : WRITE;
			break;
		case READ_TRANSACTION:
		case IFETCH_TRANSACTION:
			commandType = autoPrecharge ? READ_AND_PRECHARGE : READ;
			break;
		default:
			cerr << "Unknown transaction type, quitting." << endl;
			exit(-21);
			break;
		}
	}
	else
	{
		assert(type == PRECHARGE || type == ACTIVATE);
		commandType = type;
	}

	assert((commandType == WRITE_AND_PRECHARGE && hostTransaction.getType() == WRITE_TRANSACTION) ||
		(commandType == READ_AND_PRECHARGE && hostTransaction.getType() == READ_TRANSACTION) ||
		(commandType == READ && hostTransaction.getType() == READ_TRANSACTION) ||
		(commandType == WRITE && hostTransaction.getType() == WRITE_TRANSACTION) ||
		(commandType == ACTIVATE) || (commandType == PRECHARGE) ||
		(commandType == REFRESH_ALL && hostTransaction.getType() == AUTO_REFRESH_TRANSACTION)
		);
}

Command::~Command()
{
	if (hostTransaction)
	{
		delete hostTransaction;
		// don't want to checkpoint this
		hostTransaction = NULL;
	}
}


/// @brief to convert CAS(W)+P <=> CAS(W)
/// @brief has no effect on non CAS commands
void Command::setAutoPrecharge(const bool autoPrecharge) const
{
	switch (commandType)
	{
	case READ_AND_PRECHARGE:
	case WRITE_AND_PRECHARGE:
		if (!autoPrecharge)
			commandType = (commandType == WRITE_AND_PRECHARGE) ? WRITE : READ;
		break;
	case READ:
	case WRITE:
		if (autoPrecharge)
			commandType = (commandType == WRITE) ? WRITE_AND_PRECHARGE : READ_AND_PRECHARGE;
		break;
	default:
		break;
	}

	assert((commandType == WRITE_AND_PRECHARGE && hostTransaction->getType() == WRITE_TRANSACTION) ||
		(commandType == READ_AND_PRECHARGE && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == READ && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == WRITE && hostTransaction->getType() == WRITE_TRANSACTION) ||
		(commandType == ACTIVATE) || (commandType == PRECHARGE) ||
		(commandType == REFRESH_ALL && hostTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		);
}


void *Command::operator new(size_t size)
{
	assert(size == sizeof(Command));
	return freeCommandPool.acquireItem();
}

bool Command::operator==(const Command& right) const
{
	if (commandType == right.commandType && startTime == right.startTime &&
		enqueueTime == right.enqueueTime && completionTime == right.completionTime && addr == right.addr && 
		postedCAS == right.postedCAS && length == right.length)
	{
		if ((hostTransaction && !right.hostTransaction) || 
			(!hostTransaction && right.hostTransaction))
			return false;
		else if (!hostTransaction && !right.hostTransaction)
			return true;
		else if (*hostTransaction == *right.hostTransaction)
			return true;
		else 
			return false;
	}
	else
		return false;
}


ostream &DRAMsimII::operator<<(ostream &os, const CommandType &command)
{
	switch(command)
	{
	case ACTIVATE:
		os << "RAS ";
		break;
	case READ:
		os << "CAS ";
		break;
	case READ_AND_PRECHARGE:
		os << "CAS+P ";
		break;
	case WRITE:
		os << "CASW ";
		break;
	case WRITE_AND_PRECHARGE:
		os << "CASW+P ";
		break;
	case RETIRE_COMMAND:
		os << "RETIRE ";
		break;
	case PRECHARGE:
		os << "PREC ";
		break;
	case PRECHARGE_ALL:
		os << "PREC_A ";
		break;
	case ACTIVATE_ALL:
		os << "RAS_A ";
		break;
	case DRIVE_COMMAND:
		os << "DRIVE ";
		break;
	case DATA_COMMAND:
		os << "DATA ";
		break;
	case CAS_WITH_DRIVE_COMMAND:
		os << "CAS+D ";
		break;
	case REFRESH_ALL:
		os << "REF   ";
		break;
	case SELF_REFRESH:
		os << "SELREF";
		break;
	case DESELECT:
		os << "DESLCT";
		break;
	case NOOP:
		os << "NOOP  ";
		break;
	case INVALID_COMMAND:
		os << "INVALD";
		break;
	}
	return os;
}

ostream &DRAMsimII::operator<<(ostream &os, const Command &currentCommand)
{
	os << currentCommand.commandType <<
		currentCommand.addr <<
		" S[" << std::dec << currentCommand.startTime << 
		"] Q[" << std::dec << currentCommand.enqueueTime << 
		"] E[" << std::dec << currentCommand.completionTime << 
		"] T[" << std::dec << currentCommand.completionTime - currentCommand.startTime << 
		"] DLY[" << std::dec << currentCommand.startTime - currentCommand.enqueueTime << "]";
	return os;
}

void Command::operator delete(void *mem)
{
	Command *cmd(static_cast<Command*>(mem));
	cmd->~Command();
	freeCommandPool.releaseItem(cmd);
}
