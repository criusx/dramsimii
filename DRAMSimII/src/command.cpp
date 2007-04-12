#include <assert.h>
#include "command.h"

using namespace std;

// initialize the static member
queue<command> command::freeCommandPool(4*COMMAND_QUEUE_SIZE,true);

command::command()
{
	this_command = RETIRE_COMMAND;	/* which command is this? */
	start_time = 0;					/* when did the transaction start? */
	enqueue_time = 0;			/* when did it make it into the queues? */
	completion_time = 0;
	host_t = NULL;	/* backward pointer to the original transaction */
	/** Added list of completion times in order to clean up code */
	link_comm_tran_comp_time = 0;
	amb_proc_comp_time = 0;
	dimm_comm_tran_comp_time = 0;
	dram_proc_comp_time = 0;
	dimm_data_tran_comp_time = 0;
	amb_down_proc_comp_time = 0;
	link_data_tran_comp_time = 0;

	/* Variables added for the FB-DIMM */
	bundle_id = 0;              /* Bundle into which command is being sent - Do we need this ?? */
	tran_id = 0;                /*      The transaction id number */
	data_word = 0;              /* Which portion of data is returned i.e. entire cacheline or fragment thereof which portions are being sent*/
	data_word_position = 0;     /** Which part of the data transmission are we doing : postions include FIRST , MIDDLE, LAST **/
	refresh = 0;                /** This is used to determine if the ras/prec are part of refresh **/
	posted_cas = false;             /** This is used to determine if the ras + cas were in the same bundle **/
	length = 0;
}

command::command(const addresses address, const command_type_t commandType, const tick_t enqueueTime, transaction *hostTransaction, const bool postedCAS):
this_command(commandType),
start_time(-1),
enqueue_time(enqueueTime),
completion_time(-1),
addr(address),
host_t(hostTransaction),
link_comm_tran_comp_time(0),
amb_proc_comp_time(0),
dimm_comm_tran_comp_time(0),
dram_proc_comp_time(0),
dimm_data_tran_comp_time(0),
amb_down_proc_comp_time(0),
link_data_tran_comp_time(0),
bundle_id(0),
tran_id(0),
data_word(0),
data_word_position(0),
refresh(0),
posted_cas(postedCAS),
length(0)
{}

command::command(const addresses address, const command_type_t commandType, const tick_t enqueueTime, transaction *hostTransaction, const bool postedCAS, const int _length):
this_command(commandType),
start_time(-1),
enqueue_time(enqueueTime),
completion_time(-1),
addr(address),
host_t(hostTransaction),
link_comm_tran_comp_time(0),
amb_proc_comp_time(0),
dimm_comm_tran_comp_time(0),
dram_proc_comp_time(0),
dimm_data_tran_comp_time(0),
amb_down_proc_comp_time(0),
link_data_tran_comp_time(0),
bundle_id(0),
tran_id(0),
data_word(0),
data_word_position(0),
refresh(0),
posted_cas(postedCAS),
length(_length)
{}

ostream &operator<<(ostream &os, const command_type_t &command)
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
		os << "REF_A ";
		break;
	}
	return os;
}
ostream &operator<<(ostream &os, const command &this_c)
{
	os << this_c.this_command << this_c.addr << " S[" << std::dec << this_c.start_time << "] Q[" << std::dec << this_c.enqueue_time << "] E[" << std::dec << this_c.completion_time << std::dec << "] T[" << this_c.completion_time - this_c.start_time << "] DLY[" << std::dec << this_c.start_time - this_c.enqueue_time << "]";
	return os;
}

command::command(const command &rhs)
{
	this_command = rhs.this_command;
	start_time = rhs.start_time;
	enqueue_time = rhs.enqueue_time;
	completion_time = rhs.completion_time;
	addr = rhs.addr;
	host_t = rhs.host_t;		

	link_comm_tran_comp_time = rhs.link_comm_tran_comp_time;
	amb_proc_comp_time = rhs.amb_down_proc_comp_time;
	dimm_comm_tran_comp_time = rhs.dimm_comm_tran_comp_time;
	dram_proc_comp_time = rhs.dram_proc_comp_time;
	dimm_data_tran_comp_time = rhs.dimm_data_tran_comp_time;
	amb_down_proc_comp_time = rhs.amb_down_proc_comp_time;
	link_data_tran_comp_time = rhs.link_data_tran_comp_time;

	bundle_id = rhs.bundle_id;
	tran_id = rhs.tran_id;
	data_word = rhs.data_word;
	data_word_position = rhs.data_word_position;
	refresh = rhs.refresh;
	posted_cas = rhs.posted_cas;
	length = rhs.length;
}

void *command::operator new(size_t size)
{
	assert(size == sizeof(command));
	return freeCommandPool.acquire_item();
}

void command::operator delete(void *mem)
{
	command *cmd = static_cast<command*>(mem);
	freeCommandPool.release_item(cmd);
}
