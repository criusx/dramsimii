#include <iostream>

#include "dramChannel.h"

using namespace std;
using namespace DRAMSimII;

/// <summary>
/// Updates the channel time to what it would be had this command been executed
/// Updates the rank and bank records of the most recent RAS, CAS, etc. times
/// Enqueues RAS times to allow t_faw to be determined later
/// Updates rank and bank records of CAS, RAS lengths for later calculations in 
/// min_protocol_gap()
/// </summary>
/// <param name="this_command">The command to execute against the current state</param>
/// <param name="gap">The minimum amount of time to delay before executing the command</param>
void dramChannel::executeCommand(command *this_command,const int gap)
{
	//dramChannel &channel= dramSystem::channel[this_command->getAddress().chan_id];

	// do power calculations
	//powerModel.recordCommand(this_command, channel, timing_specification);

	rank_c &this_rank = rank[this_command->getAddress().rank_id];

	bank_c &this_bank = this_rank.bank[this_command->getAddress().bank_id];

	this_rank.last_bank_id = this_command->getAddress().bank_id;

	int t_al = this_command->isPostedCAS() ? timing_specification.t_al : 0;

	// update the channel's idea of what time it is and set the start time for the command
	// ensure that the command is never started before it is enqueued
	// this implies that if there was an idle period for the memory system, commands
	// will not be seen as executing during this time	
	this_command->setStartTime(max(time + gap, this_command->getEnqueueTime()));
	
	// set this channel's time to the start time of this command
	time = this_command->getStartTime();

	switch(this_command->getCommandType())
	{
	case RAS_COMMAND:
		{
			this_bank.isActivated = true;

			// RAS time history queue, per rank
			tick_t *this_ras_time = this_rank.last_ras_times.acquire_item();

			*this_ras_time = this_bank.last_ras_time = time;
			this_bank.row_id = this_command->getAddress().row_id;
			this_bank.RASCount++;

			this_rank.last_ras_times.push(this_ras_time);
			this_rank.last_bank_id = this_command->getAddress().bank_id;

			// specific for RAS command
			this_command->setCompletionTime(this_command->getStartTime() + timing_specification.t_cmd + timing_specification.t_ras);
		}
		break;

	case CAS_AND_PRECHARGE_COMMAND:

		this_bank.isActivated = false;
		this_rank.last_prec_time = this_bank.last_prec_time = max(time + t_al + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtp, this_bank.last_ras_time + timing_specification.t_ras);
		// lack of break is intentional

	case CAS_COMMAND:

		this_bank.last_cas_time = time;
		this_rank.last_cas_time = time;
		this_bank.last_cas_length = this_command->getLength();
		this_rank.last_cas_length = this_command->getLength();
		this_rank.last_bank_id = this_command->getAddress().bank_id;
		this_bank.CASCount++;
		//this_command->getHost()->completion_time = time + timing_specification.t_cas;
		
		// specific for CAS command
		// should account for tAL buffering the CAS command until the right moment
		this_command->setCompletionTime(max(this_bank.last_ras_time + timing_specification.t_rcd + timing_specification.t_cas + timing_specification.t_burst, this_command->getStartTime() + timing_specification.t_cmd + timing_specification.t_cas + timing_specification.t_burst));
		this_command->getHost()->setCompletionTime(this_command->getCompletionTime());
		break;

	case CAS_WRITE_AND_PRECHARGE_COMMAND:

		this_bank.isActivated = false;
		this_rank.last_prec_time = this_bank.last_prec_time = max(time + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr, this_bank.last_ras_time + timing_specification.t_ras);
		// missing break is intentional

	case CAS_WRITE_COMMAND:

		this_bank.last_casw_time = time;
		this_rank.last_casw_time = time;
		this_bank.last_casw_length = this_command->getLength();
		this_rank.last_casw_length = this_command->getLength();
		this_rank.last_bank_id = this_command->getAddress().bank_id;
		this_bank.CASWCount++;
		this_command->getHost()->setCompletionTime(time);
		
		// for the CAS write command
		this_command->setCompletionTime(this_command->getStartTime() + timing_specification.t_cmd + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr);
		break;

	case PRECHARGE_COMMAND:

		this_bank.isActivated = false;
		this_rank.last_prec_time = this_bank.last_prec_time = time;
		this_command->setCompletionTime(this_command->getStartTime() + timing_specification.t_cmd + timing_specification.t_rp);
		break;

	case REFRESH_ALL_COMMAND:

		// FIXME: should this not count as a RAS + PRE command to all banks?
		for (vector<bank_c>::iterator currentBank = this_rank.bank.begin(); currentBank != this_rank.bank.end(); currentBank++)
			currentBank->last_refresh_all_time = time;
		this_command->setCompletionTime(this_command->getStartTime() + timing_specification.t_cmd + timing_specification.t_rfc);
		this_command->getHost()->setCompletionTime(this_command->getCompletionTime());
		break;

	case RETIRE_COMMAND:
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
	}

	// transaction complete? if so, put in completion queue
	// note that the host transaction should only be pointed to by a CAS command
	// since this is when a transaction is done from the standpoint of the requester
	if (this_command->getHost()) 
	{
		if (!complete(this_command->getHost()))
		{
			cerr << "Fatal error, cannot insert transaction into completion queue." << endl;
			cerr << "Increase execution q depth and resume. Should not occur. Check logic." << endl;
			exit(2);
		}
	}

	// record command history.
	// inserts into a queue which dequeues into the command pool
	recordCommand(this_command);
}
