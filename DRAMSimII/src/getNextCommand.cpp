#include <iostream>

#include "dramChannel.h"

using namespace std;
using namespace DRAMSimII;

/// <summary>
/// Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned has already been removed from the per_bank_command_queue
/// from which it was selected
/// </summary>
command *dramChannel::getNextCommand()
{
	const command *nextCommand = readNextCommand();

	if (nextCommand)
	{
		rank_c &currentRank = rank[nextCommand->getAddress().rank];
		
		// if it was a refresh all command, then dequeue all n banks worth of commands
		if (nextCommand->getCommandType() == REFRESH_ALL_COMMAND)
		{
			command *tempCommand = NULL;

			for (vector<bank_c>::iterator cur_bank = currentRank.bank.begin(); cur_bank != currentRank.bank.end();cur_bank++)
			{
				if (tempCommand)
					delete tempCommand;
				tempCommand = cur_bank->getPerBankQueue().pop();
				assert(tempCommand->getCommandType() == REFRESH_ALL_COMMAND);
			}

			return tempCommand;

		}
		else
		{
			//bank_c &currentBank = currentRank.bank[nextCommand->getAddress().bank_id];
			assert(currentRank.bank[nextCommand->getAddress().bank].getPerBankQueue().front() == nextCommand);
			return currentRank.bank[nextCommand->getAddress().bank].getPerBankQueue().pop();
		}
	}
	else
	{
		return NULL;
	}
}


/// <summary>
/// Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned is not yet removed from the per_bank_command_queue
/// from which it was selected
/// </summary>
const command *dramChannel::readNextCommand() const
{
	// look at the most recently retired command in this channel's history

	const command *lastCommand = historyQueue.back();

	unsigned lastBankID = lastCommand ? lastCommand->getAddress().bank : systemConfig.getBankCount() - 1;
	unsigned lastRankID = lastCommand ? lastCommand->getAddress().rank : systemConfig.getRankCount() - 1;
	const command_type_t lastCommandType = lastCommand ? lastCommand->getCommandType() : CAS_WRITE_AND_PRECHARGE_COMMAND;

	switch (systemConfig.getCommandOrderingAlgorithm())
	{
	case STRICT_ORDER: // look for oldest command, execute that
		{
			tick_t oldestCommandTime = TICK_T_MAX;	
			vector<bank_c>::const_iterator oldestBank;
			vector<rank_c>::const_iterator oldestRank;

			for (vector<rank_c>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				bool notAllRefresh = false;

				for (vector<bank_c>::const_iterator bank_id = currentRank->bank.begin(); bank_id != currentRank->bank.end(); bank_id++)
				{
					command *temp_c = bank_id->getPerBankQueue().front();

					if (temp_c && (oldestCommandTime > temp_c->getEnqueueTime()))
					{
						// if it's a refresh_all command and
						// we haven't proved that all the queues aren't refresh_all commands, search
						if (temp_c->getCommandType() == REFRESH_ALL_COMMAND)
						{
							if (!notAllRefresh)
							{
								// try to show that at the head of each queue isn't a refresh command
								for (vector<bank_c>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
								{
									// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
									if ((currentBank->getPerBankQueue().size() == 0) || ((currentBank->getPerBankQueue().front()) && (currentBank->getPerBankQueue().front()->getCommandType() != REFRESH_ALL_COMMAND)))
									{
										notAllRefresh = true;
										break;
									}

								}
								// if all are known now to be refresh commands
								if (!notAllRefresh)
								{
									oldestCommandTime = temp_c->getEnqueueTime();
									oldestBank = bank_id;
									oldestRank = currentRank;
								}
							}
						}
						else
						{
							oldestCommandTime = temp_c->getEnqueueTime();
							oldestBank = bank_id;
						}

					}
				}
			}

			// if there was a command found
			if (oldestCommandTime < TICK_T_MAX)
			{
				assert(oldestBank->getPerBankQueue().front()->getCommandType() == REFRESH_ALL_COMMAND || rank[oldestBank->getPerBankQueue().front()->getAddress().rank].bank[oldestBank->getPerBankQueue().front()->getAddress().bank].getPerBankQueue().front() == oldestBank->getPerBankQueue().front());

				return oldestBank->getPerBankQueue().front();
			}
			else
				return NULL;
		}
		break;

		// alternate ranks as we go down banks
	case RANK_ROUND_ROBIN:
		{
			transaction_type_t transaction_type;

			switch (lastCommandType)
			{
				// if it was RAS before and you want to finish doing the read/write
			case RAS_COMMAND:
				{
					command *temp_c =  rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

					if ((temp_c) &&
						((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) ||
						(temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) ||
						(temp_c->getCommandType() == CAS_COMMAND) ||
						(temp_c->getCommandType() == CAS_WRITE_COMMAND)))
					{
						return rank[lastRankID].bank[lastBankID].getPerBankQueue().front();
					}
					else
					{
						cerr << "Serious problems. RAS not followed by CAS" << endl;
						exit(2);
					}
				}
				break;
			case CAS_AND_PRECHARGE_COMMAND:
			case CAS_COMMAND:
				transaction_type = READ_TRANSACTION;
				break;
			case CAS_WRITE_COMMAND:
			case CAS_WRITE_AND_PRECHARGE_COMMAND:
				transaction_type = WRITE_TRANSACTION;
				break;
			default:
				transaction_type = READ_TRANSACTION;
				cerr << "Did not find a CAS or RAS command when it was expected" << endl;
				break;
			}

			bool candidate_found = false;
			unsigned originalLastRankID = lastRankID;
			unsigned originalLastBankID = lastBankID;

			while (candidate_found == false)
			{
				// select the next rank
				lastBankID = (lastBankID + 1) % systemConfig.getRankCount();

				// select the next bank
				if (lastRankID == 0)
				{
					lastBankID = (lastBankID + 1) % systemConfig.getBankCount();

					if (lastBankID == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;						
					}
				}

				command *temp_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

				if (temp_c)
				{
					if (systemConfig.isReadWriteGrouping() == false)
					{
						return rank[lastRankID].bank[lastBankID].getPerBankQueue().front();
					}
					else // have to follow read_write grouping considerations 
					{
						command *next_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							return rank[lastRankID].bank[lastBankID].getPerBankQueue().front();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< temp_c->getAddress().rank << "] [" << temp_c->getAddress().bank << "] but wrong type, We want [" << transaction_type << "]. Candidate command type ";
					cerr << temp_c->getCommandType();
					cerr << " followed by ";
					cerr << rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1)->getCommandType();
					cerr << "count [" << rank[lastRankID].bank[lastBankID].getPerBankQueue().get_count() << "]" << endl;
#endif

				}

#ifdef DEBUG_FLAG_2
				cerr << "Looked in [" << lastRankID << "] [" << lastBankID << "] but Q empty" << endl;
#endif

			}
		}
		break;

		// keep rank id as long as possible, go round robin down a given rank
	case BANK_ROUND_ROBIN: 
		{			
			transaction_type_t transaction_type;
			switch (lastCommandType)
			{
			case RAS_COMMAND:
				{
					command *temp_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

					if((temp_c != NULL) &&
						((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
					{
						return rank[lastRankID].bank[lastBankID].getPerBankQueue().front();
					}
					else
					{
						cerr << "Serious problems. RAS not followed by CAS." << endl;
						exit(2);
					}
				}
				break;
			case CAS_AND_PRECHARGE_COMMAND:
				transaction_type = READ_TRANSACTION;
				break;
			case CAS_WRITE_AND_PRECHARGE_COMMAND:
				transaction_type = WRITE_TRANSACTION;
				break;
			default:
				transaction_type = WRITE_TRANSACTION; // FIXME: added this to ensure no uninit vars
				break;
			}

			bool candidate_found = false;

			while (candidate_found == false)
			{
				lastBankID = (lastBankID + 1) % systemConfig.getBankCount();
				if (lastBankID == 0)
				{
					lastRankID = (lastRankID + 1) % systemConfig.getRankCount();
					if (lastRankID == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
					}
				}

				command *temp_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

				if (temp_c != NULL)
				{	
					if(systemConfig.isReadWriteGrouping() == false)
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations
					{
						command *next_c =  rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankID].bank[lastBankID].getPerBankQueue().front()->getAddress().bank == lastBankID);
							assert(rank[lastRankID].bank[lastBankID].getPerBankQueue().front()->getAddress().rank == lastRankID);
							return rank[lastRankID].bank[lastBankID].getPerBankQueue().front();
						}
					}
#ifdef DEBUG_FLAG_2
					cerr << "Looked in [" << temp_c->getAddress().rank << "] [" << temp_c->getAddress().bank << "] but wrong type, We want [" << transaction_type << "] Candidate command type [" << rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1)->getCommandType() << "]" << endl;
#endif
				}
#ifdef DEBUG_FLAG_2
				cerr << "Looked in rank=[" << lastRankID << "] bank=[" << lastBankID << "] but Q empty" << endl;
#endif
			}
		}
		break;

	case GREEDY:
		{
			command *candidate_command = NULL;

			int candidate_gap = INT_MAX;

			for (unsigned rank_id = 0; rank_id < systemConfig.getRankCount(); ++rank_id)
			{
				for (unsigned bank_id = 0; bank_id < systemConfig.getBankCount() ; ++bank_id)
				{
					command *challenger_command = rank[rank_id].bank[bank_id].getPerBankQueue().front();

					if (challenger_command != NULL)
					{
						int challenger_gap = minProtocolGap(challenger_command);
						if (challenger_gap < candidate_gap || (candidate_gap == challenger_gap && challenger_command->getEnqueueTime() < candidate_command->getEnqueueTime()))
						{
							candidate_gap = challenger_gap;
							candidate_command = challenger_command;
						}
					}
				}
			}

			assert(rank[candidate_command->getAddress().rank].bank[candidate_command->getAddress().bank].getPerBankQueue().front() == candidate_command);

#ifdef DEBUG
			timingOutStream << "R[" << candidate_command->getAddress().rank << "] B[" << candidate_command->getAddress().bank << "]\tWinner: " << *candidate_command << "gap[" << candidate_gap << "] now[" << time << "]" << endl;
#endif

			return candidate_command;
		}
		break;

	default:
		{
			cerr << "This configuration and algorithm combination is not supported" << endl;
			exit(0);
		}
		break;
	}
	return NULL;
}
