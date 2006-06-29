#include <iostream>

#include "dramsim2.h"

/// <summary>
/// Updates the channel time to what it would be had this command been executed
/// Updates the rank and bank records of the most recent RAS, CAS, etc. times
/// Enqueues RAS times to allow t_faw to be determined later
/// Updates rank and bank records of CAS, RAS lengths for later calculations in 
/// min_protocol_gap()
/// </summary>
void dram_system::executeCommand(command *this_c,const int gap)
{
	tick_t *this_ras_time;

	unsigned chan_id = this_c->addr.chan_id;
	unsigned rank_id = this_c->addr.rank_id;
	unsigned bank_id = this_c->addr.bank_id;
	unsigned row_id = this_c->addr.row_id;

	dram_channel *channel= &(dram_system::channel[chan_id]);

	rank_c *this_r = channel->get_rank(rank_id);

	bank_c *this_b = &(this_r->bank[bank_id]);

	//transaction *host_t = NULL;
	//queue &history_q = channel.history_q;
	//  queue *complete_q;

	this_r->last_bank_id = bank_id;

	int t_al = this_c->posted_cas ? timing_specification.t_al : 0;

	// new
	this_c->start_time = max(channel->get_time(),this_c->start_time);


	// update the channel's idea of what time it is
	//channel->set_time(channel->get_time() + gap);
	channel->set_time(this_c->start_time + gap);

	// new
	this_c->completion_time = channel->get_time();

	switch(this_c->this_command)
	{
	case RAS_COMMAND:

		this_b->last_ras_time = channel->get_time();
		this_b->row_id = row_id;
		this_b->ras_count++;

		// RAS time history queue, per rank
		if(this_r->last_ras_times.freecount() == 0)	// FIXME: this is not very general
			this_ras_time = this_r->last_ras_times.dequeue();
		else
			this_ras_time = new tick_t;

		*this_ras_time = channel->get_time();
		this_r->last_ras_times.enqueue(this_ras_time);
		//this_c->completion_time = *this_ras_time + timing_specification.t_ras;
		break;

	case CAS_AND_PRECHARGE_COMMAND:

		this_b->last_prec_time = max(channel->get_time() + t_al + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtp, this_b->last_ras_time + timing_specification.t_ras);
		// lack of break is intentional

	case CAS_COMMAND:

		this_b->last_cas_time = channel->get_time();
		this_r->last_cas_time = channel->get_time();
		this_b->last_cas_length = this_c->length;
		this_r->last_cas_length = this_c->length;
		this_b->cas_count++;
		//host_t = this_c->host_t;
		this_c->host_t->completion_time	= channel->get_time() + timing_specification.t_cas;
		//this_c->completion_time = channel->get_time() + timing_specification.t_cas;
		break;

	case CAS_WRITE_AND_PRECHARGE_COMMAND:

		this_b->last_prec_time = max(channel->get_time() + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr, this_b->last_ras_time + timing_specification.t_ras);
		// missing break is intentional

	case CAS_WRITE_COMMAND:

		this_b->last_casw_time = channel->get_time();
		this_r->last_casw_time = channel->get_time();
		this_b->last_casw_length= this_c->length;
		this_r->last_casw_length= this_c->length;
		this_b->casw_count++;
		//host_t = this_c->host_t;
		this_c->host_t->completion_time	= channel->get_time();
		//this_c->completion_time = channel->get_time();
		break;

	case RETIRE_COMMAND:

		break;

	case PRECHARGE_COMMAND:

		this_b->last_prec_time = channel->get_time();
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
		this_b->last_refresh_all_time = channel->get_time();
		break;
	}

	// transaction complete? if so, put in completion queue
	// note that the host transaction should only be pointed to by a CAS command
	// since this is when a transaction is done from the standpoint of the requestor
	if (this_c->host_t != NULL) 
	{
		if(channel->complete(this_c->host_t) == FAILURE)
		{
			cerr << "Fatal error, cannot insert transaction into completion queue." << endl;
			cerr << "Increase execution q depth and resume. Should not occur. Check logic." << endl;
			_exit(2);
		}
	}

	// record command history. Check to see if this can be removed
	channel->record_command(this_c, free_command_pool);

	//if (channel.history_q.get_count() == system_config.history_queue_depth)
	//{		
	/*done with this command, release into pool */
	//	free_command_pool.release_item(channel.history_q.dequeue());
	//}
	//channel.history_q.enqueue(this_c);
}