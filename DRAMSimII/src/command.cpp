#include <assert.h>
#include "command.h"
#include "transaction.h"

using namespace std;
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

Command::Command(const Address address, const CommandType commandType, const tick enqueueTime, Transaction *hostTransaction, const bool postedCAS):
commandType(commandType),
startTime(-1),
enqueueTime(enqueueTime),
completionTime(-1),
addr(address),
hostTransaction(hostTransaction),
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
postedCAS(postedCAS),
length(0)
{}

Command::Command(const Address address, const CommandType commandType, const tick enqueueTime, Transaction *hostTransaction, const bool postedCAS, const int _length):
commandType(commandType),
startTime(-1),
enqueueTime(enqueueTime),
completionTime(-1),
addr(address),
hostTransaction(hostTransaction),
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
postedCAS(postedCAS),
length(_length)
{}

/// @brief to convert CAS(W)+P <=> CAS(W)
void Command::setAutoPrecharge(const bool autoPrecharge) const
{
	switch (commandType)
	{
	case CAS_AND_PRECHARGE_COMMAND:
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		if (!autoPrecharge)
			commandType = commandType == CAS_WRITE_AND_PRECHARGE_COMMAND ? CAS_WRITE_COMMAND : CAS_COMMAND;
		break;
	case CAS_COMMAND:
	case CAS_WRITE_COMMAND:
		if (autoPrecharge)
			commandType = commandType == CAS_WRITE_COMMAND ? CAS_WRITE_AND_PRECHARGE_COMMAND : CAS_AND_PRECHARGE_COMMAND;
	}
}

Command::Command(Transaction *hostTransaction, const tick enqueueTime, const bool postedCAS, const bool autoPrecharge):
startTime(-1),
enqueueTime(enqueueTime),
completionTime(-1),
addr(hostTransaction->getAddresses()),
hostTransaction(hostTransaction),
postedCAS(postedCAS)
{
	switch (hostTransaction->getType())
	{
	case WRITE_TRANSACTION:
		commandType = autoPrecharge ? CAS_WRITE_AND_PRECHARGE_COMMAND : CAS_WRITE_COMMAND;
		break;
	case READ_TRANSACTION:
	case IFETCH_TRANSACTION:
		commandType = autoPrecharge ? CAS_AND_PRECHARGE_COMMAND : CAS_COMMAND;
		break;
	}
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

ostream &DRAMSimII::operator<<(ostream &os, const Command &this_c)
{
	os << this_c.commandType << this_c.addr << " S[" << std::dec << this_c.startTime << "] Q[" << std::dec << this_c.enqueueTime << "] E[" << std::dec << this_c.completionTime << std::dec << "] T[" << this_c.completionTime - this_c.startTime << "] DLY[" << std::dec << this_c.startTime - this_c.enqueueTime << "]";
	return os;
}

Command::Command(const Command &rhs):
commandType(rhs.commandType),
startTime(rhs.startTime),
enqueueTime(rhs.enqueueTime),
completionTime(rhs.completionTime),
addr(rhs.addr),
hostTransaction(rhs.hostTransaction),
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
{}

void *Command::operator new(size_t size)
{
	assert(size == sizeof(Command));
	return freeCommandPool.acquireItem();
}

void Command::operator delete(void *mem)
{
	Command *cmd(static_cast<Command*>(mem));
	freeCommandPool.releaseItem(cmd);
}
