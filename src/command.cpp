#include <assert.h>
#include "command.h"
#include "transaction.h"

using std::cerr;
using std::endl;
using std::ostream;
using namespace DRAMSimII;

// initialize the static member
Queue<Command> Command::freeCommandPool(4*COMMAND_QUEUE_SIZE,true);

Command::Command():
commandType(RETIRE_COMMAND),
startTime(0),
enqueueTime(0),
completionTime(0),
addr(),
hostTransaction(NULL),
//link_comm_tran_comp_time(0),
//amb_proc_comp_time(0),
//dimm_comm_tran_comp_time(0),
//dram_proc_comp_time(0),
//dimm_data_tran_comp_time(0),
//amb_down_proc_comp_time(0),
//link_data_tran_comp_time(0),
//bundle_id(0),
//tran_id(0),
//data_word(0),
//data_word_position(0),
//refresh(0),
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
//hostTransaction(rhs.hostTransaction), // assume that this is managed elsewhere
//link_comm_tran_comp_time(rhs.link_comm_tran_comp_time),
//amb_proc_comp_time(rhs.amb_down_proc_comp_time),
//dimm_comm_tran_comp_time(rhs.dimm_comm_tran_comp_time),
//dram_proc_comp_time(rhs.dram_proc_comp_time),
//dimm_data_tran_comp_time(rhs.dimm_data_tran_comp_time),
//amb_down_proc_comp_time(rhs.amb_down_proc_comp_time),
//link_data_tran_comp_time(rhs.link_data_tran_comp_time),
//bundle_id(rhs.bundle_id),
//tran_id(rhs.tran_id),
//data_word(rhs.data_word),
//data_word_position(rhs.data_word_position),
//refresh(rhs.refresh),
postedCAS(rhs.postedCAS),
length(rhs.length)
{
	assert(!hostTransaction ||
		(commandType == CAS_WRITE_AND_PRECHARGE_COMMAND && hostTransaction->getType() == WRITE_TRANSACTION) ||
		(commandType == CAS_AND_PRECHARGE_COMMAND && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == CAS_COMMAND && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == CAS_WRITE_COMMAND && hostTransaction->getType() == WRITE_TRANSACTION) ||		 
		(commandType == REFRESH_ALL_COMMAND && hostTransaction->getType() == AUTO_REFRESH_TRANSACTION) ||
		(commandType == RAS_COMMAND) || (commandType == PRECHARGE_COMMAND)
		);
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


Command::Command(Transaction& hostTransaction, const tick enqueueTime, const bool postedCAS, const bool autoPrecharge, const CommandType type):
startTime(-1),
enqueueTime(enqueueTime),
completionTime(-1),
addr(hostTransaction.getAddresses()),
hostTransaction(type == CAS_COMMAND ? &hostTransaction : NULL), // this link is only needed for CAS commands
postedCAS(postedCAS)
{
	if (type == CAS_COMMAND)
	{
		switch (hostTransaction.getType())
		{
		case AUTO_REFRESH_TRANSACTION:
			commandType = REFRESH_ALL_COMMAND;
			break;
		case WRITE_TRANSACTION:
			commandType = autoPrecharge ? CAS_WRITE_AND_PRECHARGE_COMMAND : CAS_WRITE_COMMAND;
			break;
		case READ_TRANSACTION:
		case IFETCH_TRANSACTION:
			commandType = autoPrecharge ? CAS_AND_PRECHARGE_COMMAND : CAS_COMMAND;
			break;
		default:
			cerr << "Unknown transaction type, quitting." << endl;
			exit(-21);
			break;
		}
	}
	else
	{
		assert(type == PRECHARGE_COMMAND || type == RAS_COMMAND);
		commandType = type;
	}

	assert((commandType == CAS_WRITE_AND_PRECHARGE_COMMAND && hostTransaction.getType() == WRITE_TRANSACTION) ||
		(commandType == CAS_AND_PRECHARGE_COMMAND && hostTransaction.getType() == READ_TRANSACTION) ||
		(commandType == CAS_COMMAND && hostTransaction.getType() == READ_TRANSACTION) ||
		(commandType == CAS_WRITE_COMMAND && hostTransaction.getType() == WRITE_TRANSACTION) ||
		(commandType == RAS_COMMAND) || (commandType == PRECHARGE_COMMAND) ||
		(commandType == REFRESH_ALL_COMMAND && hostTransaction.getType() == AUTO_REFRESH_TRANSACTION)
		);
}

Command::~Command()
{
	if (hostTransaction)
	{
		delete hostTransaction;
		hostTransaction = NULL;
	}
}


/// @brief to convert CAS(W)+P <=> CAS(W)
/// @brief has no effect on non CAS commands
void Command::setAutoPrecharge(const bool autoPrecharge) const
{
	switch (commandType)
	{
	case CAS_AND_PRECHARGE_COMMAND:
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		if (!autoPrecharge)
			commandType = (commandType == CAS_WRITE_AND_PRECHARGE_COMMAND) ? CAS_WRITE_COMMAND : CAS_COMMAND;
		break;
	case CAS_COMMAND:
	case CAS_WRITE_COMMAND:
		if (autoPrecharge)
			commandType = (commandType == CAS_WRITE_COMMAND) ? CAS_WRITE_AND_PRECHARGE_COMMAND : CAS_AND_PRECHARGE_COMMAND;
		break;
	default:
		break;
	}

	assert((commandType == CAS_WRITE_AND_PRECHARGE_COMMAND && hostTransaction->getType() == WRITE_TRANSACTION) ||
		(commandType == CAS_AND_PRECHARGE_COMMAND && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == CAS_COMMAND && hostTransaction->getType() == READ_TRANSACTION) ||
		(commandType == CAS_WRITE_COMMAND && hostTransaction->getType() == WRITE_TRANSACTION) ||
		(commandType == RAS_COMMAND) || (commandType == PRECHARGE_COMMAND) ||
		(commandType == REFRESH_ALL_COMMAND && hostTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		);
}


ostream &DRAMSimII::operator<<(ostream &os, const CommandType &command)
{
	switch(command)
	{
	case RAS_COMMAND:
		os << "RAS ";
		break;
	case CAS_COMMAND:
		os << "CAS ";
		break;
	case CAS_AND_PRECHARGE_COMMAND:
		os << "CAS+P ";
		break;
	case CAS_WRITE_COMMAND:
		os << "CASW ";
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		os << "CASW+P ";
		break;
	case RETIRE_COMMAND:
		os << "RETIRE ";
		break;
	case PRECHARGE_COMMAND:
		os << "PREC ";
		break;
	case PRECHARGE_ALL_COMMAND:
		os << "PREC_A ";
		break;
	case RAS_ALL_COMMAND:
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
	case REFRESH_ALL_COMMAND:
		os << "REF   ";
		break;
	}
	return os;
}

ostream &DRAMSimII::operator<<(ostream &os, const Command &currentCommand)
{
	os << currentCommand.commandType << currentCommand.addr << " S[" << std::dec << currentCommand.startTime << "] Q[" << std::dec << currentCommand.enqueueTime << "] E[" << std::dec << currentCommand.completionTime << std::dec << "] T[" << currentCommand.completionTime - currentCommand.startTime << "] DLY[" << std::dec << currentCommand.startTime - currentCommand.enqueueTime << "]";
	return os;
}


void *Command::operator new(size_t size)
{
	assert(size == sizeof(Command));
	return freeCommandPool.acquireItem();
}

void Command::operator delete(void *mem)
{
	Command *cmd(static_cast<Command*>(mem));
	cmd->~Command();
	freeCommandPool.releaseItem(cmd);
}
