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


ostream &DRAMSimII::operator<<(ostream &os, const CommandType &command)
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

ostream &DRAMSimII::operator<<(ostream &os, const Command &currentCommand)
{
	os << currentCommand.commandType << currentCommand.addr << " S[" << std::dec << currentCommand.startTime << "] Q[" << std::dec << currentCommand.enqueueTime << "] E[" << std::dec << currentCommand.completionTime << std::dec << "] T[" << currentCommand.completionTime - currentCommand.startTime << "] DLY[" << std::dec << currentCommand.startTime - currentCommand.enqueueTime << "]";
	return os;
}


void Command::operator delete(void *mem)
{
	Command *cmd(static_cast<Command*>(mem));
	cmd->~Command();
	freeCommandPool.releaseItem(cmd);
}
