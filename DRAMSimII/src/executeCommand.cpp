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
	rank_c &currentRank = rank[this_command->getAddress().rank];

	bank_c &currentBank = currentRank.bank[this_command->getAddress().bank];

	currentRank.lastBankID = this_command->getAddress().bank;

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
			currentRank.issueRAS(time, this_command);
			
			// specific for RAS command
			this_command->setCompletionTime(this_command->getStartTime() + timing_specification.t_cmd + timing_specification.t_ras);
		}
		break;

	case CAS_AND_PRECHARGE_COMMAND:

		currentRank.issuePRE(time, this_command);
		// lack of break is intentional

	case CAS_COMMAND:

		currentRank.issueCAS(time, this_command);
		
		// specific for CAS command
		// should account for tAL buffering the CAS command until the right moment
		this_command->setCompletionTime(max(currentBank.lastRASTime + timing_specification.t_rcd + timing_specification.t_cas + timing_specification.t_burst, time + timing_specification.t_cmd + timing_specification.t_cas + timing_specification.t_burst));
		this_command->getHost()->setCompletionTime(this_command->getCompletionTime());
		break;

	case CAS_WRITE_AND_PRECHARGE_COMMAND:

		currentRank.issuePRE(time, this_command);		
		// missing break is intentional

	case CAS_WRITE_COMMAND:
		
		currentRank.issueCASW(time, this_command);
		
		// for the CAS write command
		this_command->getHost()->setCompletionTime(time);
		this_command->setCompletionTime(time + timing_specification.t_cmd + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr);
		break;

	case PRECHARGE_COMMAND:

		currentRank.issuePRE(time, this_command);

		this_command->setCompletionTime(this_command->getStartTime() + timing_specification.t_cmd + timing_specification.t_rp);

		break;

	case REFRESH_ALL_COMMAND:

		currentRank.issueREF(time, this_command);

		this_command->setCompletionTime(time + timing_specification.t_cmd + timing_specification.t_rfc);
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
