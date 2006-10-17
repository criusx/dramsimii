#include <iostream>

#include "dramSystem.h"

using namespace std;

/// <summary>
/// Updates the channel time to what it would be had this command been executed
/// Updates the rank and bank records of the most recent RAS, CAS, etc. times
/// Enqueues RAS times to allow t_faw to be determined later
/// Updates rank and bank records of CAS, RAS lengths for later calculations in 
/// min_protocol_gap()
/// </summary>
void dramSystem::executeCommand(command *this_command,const int gap)
{
	dramChannel &channel= dramSystem::channel[this_command->addr.chan_id];

	rank_c &this_rank = channel.get_rank(this_command->addr.rank_id);

	bank_c *this_bank = &this_rank.bank[this_command->addr.bank_id];

	this_rank.last_bank_id = this_command->addr.bank_id;

	int t_al = this_command->posted_cas ? timing_specification.t_al : 0;

	// new
	//this_command->start_time = max(channel.get_time(),this_command->start_time);
	//this_command->completion_time = channel.get_time();

	// update the channel's idea of what time it is
	channel.set_time(this_command->start_time = (this_command->start_time + gap));

	// new, this is not right, this only shows how long until the next command could execute
	// this_command->completion_time = channel.get_time();

	switch(this_command->this_command)
	{
	case RAS_COMMAND:

		this_bank->last_ras_time = channel.get_time();
		this_bank->row_id = this_command->addr.row_id;
		this_bank->ras_count++;

		// RAS time history queue, per rank
		tick_t *this_ras_time;

		if(this_rank.last_ras_times.freecount() == 0)	// FIXME: this is not very general
			this_ras_time = this_rank.last_ras_times.dequeue();
		else
			this_ras_time = new tick_t;

		*this_ras_time = channel.get_time();
		this_rank.last_ras_times.enqueue(this_ras_time);

		// specific for RAS command
		this_command->completion_time = this_command->start_time + timing_specification.t_ras;
		break;

	case CAS_AND_PRECHARGE_COMMAND:

		this_bank->last_prec_time = max(channel.get_time() + t_al + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtp, this_bank->last_ras_time + timing_specification.t_ras);
		// lack of break is intentional

	case CAS_COMMAND:

		this_bank->last_cas_time = channel.get_time();
		this_rank.last_cas_time = channel.get_time();
		this_bank->last_cas_length = this_command->length;
		this_rank.last_cas_length = this_command->length;
		this_bank->cas_count++;
		this_command->host_t->completion_time = channel.get_time() + timing_specification.t_cas;
		
		// specific for CAS command
		this_command->completion_time = this_command->start_time + timing_specification.t_cas;
		break;

	case CAS_WRITE_AND_PRECHARGE_COMMAND:

		this_bank->last_prec_time = max(channel.get_time() + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr, this_bank->last_ras_time + timing_specification.t_ras);
		// missing break is intentional

	case CAS_WRITE_COMMAND:

		this_bank->last_casw_time = channel.get_time();
		this_rank.last_casw_time = channel.get_time();
		this_bank->last_casw_length = this_command->length;
		this_rank.last_casw_length = this_command->length;
		this_bank->casw_count++;
		this_command->host_t->completion_time = channel.get_time();
		
		// for the CAS write command
		this_command->completion_time = this_command->start_time + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr;
		break;

	case RETIRE_COMMAND:
		break;

	case PRECHARGE_COMMAND:
		this_bank->last_prec_time = channel.get_time();
		this_command->completion_time = this_command->start_time + timing_specification.t_cmd + timing_specification.t_rp;
		break;

	case PRECHARGE_ALL_COMMAND:
		break;

	case RAS_ALL_COMMAND:
		break;

	case DRIVE_COMMAND:
		break;

	case DATA_COMMAND:
		break;

	case CAS_WITH_DRIVE_COMMAND:
		break;

	case REFRESH_ALL_COMMAND:
		this_bank->last_refresh_all_time = channel.get_time();
		this_command->completion_time = this_command->start_time + timing_specification.t_rfc;
		this_command->host_t->completion_time = this_command->completion_time;
		break;
	}

	// transaction complete? if so, put in completion queue
	// note that the host transaction should only be pointed to by a CAS command
	// since this is when a transaction is done from the standpoint of the requestor
	if (this_command->host_t != NULL) 
	{
		if (channel.complete(this_command->host_t) == FAILURE)
		{
			cerr << "Fatal error, cannot insert transaction into completion queue." << endl;
			cerr << "Increase execution q depth and resume. Should not occur. Check logic." << endl;
			_exit(2);
		}
	}

	// record command history. Check to see if this can be removed
	channel.record_command(this_command, free_command_pool);

	

	//if (channel.history_q.get_count() == system_config.history_queue_depth)
	//{		
	/*done with this command, release into pool */
	//	free_command_pool.release_item(channel.history_q.dequeue());
	//}
	//channel.history_q.enqueue(this_c);
}