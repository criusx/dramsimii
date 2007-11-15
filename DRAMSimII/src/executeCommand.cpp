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
void dramChannel::executeCommand(command *thisCommand,const int gap)
{
	rank_c &currentRank = rank[thisCommand->getAddress().rank];

	bank_c &currentBank = currentRank.bank[thisCommand->getAddress().bank];

	currentRank.lastBankID = thisCommand->getAddress().bank;

	//int t_al = this_command->isPostedCAS() ? timingSpecification.tAL() : 0;

	// update the channel's idea of what time it is and set the start time for the command
	// ensure that the command is never started before it is enqueued
	// this implies that if there was an idle period for the memory system, commands
	// will not be seen as executing during this time	
	thisCommand->setStartTime(max(time + gap, thisCommand->getEnqueueTime()));
	
	// set this channel's time to the start time of this command
	time = thisCommand->getStartTime();

	switch(thisCommand->getCommandType())
	{
	case RAS_COMMAND:
		{
			currentRank.issueRAS(time, thisCommand);
			
			// specific for RAS command
			thisCommand->setCompletionTime(thisCommand->getStartTime() + timingSpecification.tCMD() + timingSpecification.tRAS());
		}
		break;

	case CAS_AND_PRECHARGE_COMMAND:

		currentRank.issuePRE(time, thisCommand);
		// lack of break is intentional

	case CAS_COMMAND:

		currentRank.issueCAS(time, thisCommand);
		
		// specific for CAS command
		// should account for tAL buffering the CAS command until the right moment
		thisCommand->setCompletionTime(max(currentBank.getLastRASTime() + timingSpecification.tRCD() + timingSpecification.tCAS() + timingSpecification.tBurst(), time + timingSpecification.tCMD() + timingSpecification.tCAS() + timingSpecification.tBurst()));
		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());
		break;

	case CAS_WRITE_AND_PRECHARGE_COMMAND:

		currentRank.issuePRE(time, thisCommand);		
		// missing break is intentional

	case CAS_WRITE_COMMAND:
		
		currentRank.issueCASW(time, thisCommand);
		
		// for the CAS write command
		thisCommand->getHost()->setCompletionTime(time);
		thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());
		break;

	case PRECHARGE_COMMAND:

		currentRank.issuePRE(time, thisCommand);

		thisCommand->setCompletionTime(thisCommand->getStartTime() + timingSpecification.tCMD() + timingSpecification.tRP());

		break;

	case REFRESH_ALL_COMMAND:

		currentRank.issueREF(time, thisCommand);

		thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tRFC());
		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());
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
	if (thisCommand->getHost()) 
	{
		if (!completionQueue.push(thisCommand->getHost()))
		{
			cerr << "Fatal error, cannot insert transaction into completion queue." << endl;
			cerr << "Increase execution q depth and resume. Should not occur. Check logic." << endl;
			exit(2);
		}
	}

	// record command history.
	// inserts into a queue which dequeues into the command pool
	recordCommand(thisCommand);
}
