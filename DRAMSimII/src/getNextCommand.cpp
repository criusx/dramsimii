#include <iostream>
#include <climits>
#include <limits>

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
	command *nextCommand = readNextCommand();

	if (nextCommand)
	{
		rank_c &currentRank = rank[nextCommand->getAddress().rank_id];
		
		// if it was a refresh all command, then dequeue all n banks worth of commands
		if (nextCommand->getCommandType() == REFRESH_ALL_COMMAND)
		{
			command *tempCommand = NULL;

			for (vector<bank_c>::iterator cur_bank = currentRank.bank.begin(); cur_bank != currentRank.bank.end();cur_bank++)
			{
				if (tempCommand)
					delete tempCommand;
				tempCommand = cur_bank->per_bank_q.pop();
				assert(tempCommand->getCommandType() == REFRESH_ALL_COMMAND);
			}

			return tempCommand;

		}
		else
		{
			bank_c &currentBank = currentRank.bank[nextCommand->getAddress().bank_id];
			assert(rank[nextCommand->getAddress().rank_id].bank[nextCommand->getAddress().bank_id].per_bank_q.front() == nextCommand);
			return rank[nextCommand->getAddress().rank_id].bank[nextCommand->getAddress().bank_id].per_bank_q.pop();
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
command *dramChannel::readNextCommand() const
{
	// look at the most recently retired command in this channel's history

	const command *lastCommand = get_most_recent_command();

	unsigned lastBankId = lastCommand ? lastCommand->getAddress().bank_id : systemConfig->getBankCount() - 1;
	unsigned lastRankId = lastCommand ? lastCommand->getAddress().rank_id : systemConfig->getRankCount() - 1;
	const command_type_t lastCommandType = lastCommand ? lastCommand->getCommandType() : CAS_WRITE_AND_PRECHARGE_COMMAND;

	switch (systemConfig->getCommandOrderingAlgorithm())
	{
	case STRICT_ORDER: // look for oldest command, execute that
		{
			numeric_limits<tick_t> ll;
			tick_t oldestCommandTime = ll.max();	
			vector<bank_c>::const_iterator oldestBank;
			vector<rank_c>::const_iterator oldestRank;

			for (vector<rank_c>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				bool notAllRefresh = false;

				for (vector<bank_c>::const_iterator bank_id = currentRank->bank.begin(); bank_id != currentRank->bank.end(); bank_id++)
				{
					command *temp_c = bank_id->per_bank_q.front();

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
									if ((currentBank->per_bank_q.size() == 0) || ((currentBank->per_bank_q.front()) && (currentBank->per_bank_q.front()->getCommandType() != REFRESH_ALL_COMMAND)))
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
			if (oldestCommandTime < ll.max())
			{
				assert(oldestBank->per_bank_q.front()->getCommandType() == REFRESH_ALL_COMMAND || rank[oldestBank->per_bank_q.front()->getAddress().rank_id].bank[oldestBank->per_bank_q.front()->getAddress().bank_id].per_bank_q.front() == oldestBank->per_bank_q.front());

				return oldestBank->per_bank_q.front();
			}
			else
				return NULL;
		}
		break;

		// alternate ranks as we go down banks
	case RANK_ROUND_ROBIN:
		{
			transaction_type_t transaction_type;

			if (lastCommandType == RAS_COMMAND)
			{
				command *temp_c =  rank[lastRankId].bank[lastBankId].per_bank_q.front();

				if ((temp_c != NULL) &&
					((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
				{
					return rank[lastRankId].bank[lastBankId].per_bank_q.front();
				}
				else
				{
					cerr << "Serious problems. RAS not followed by CAS" << endl;
					exit(2);
				}
			}
			else if (lastCommandType == CAS_AND_PRECHARGE_COMMAND)
			{
				transaction_type = READ_TRANSACTION;
			}
			else if (lastCommandType == CAS_WRITE_AND_PRECHARGE_COMMAND)
			{
				transaction_type = WRITE_TRANSACTION;
			}
			else
			{
				transaction_type = READ_TRANSACTION;
				cerr << "Did not find a CAS or RAS command when it was expected" << endl;
			}

			bool candidate_found = false;

			while (candidate_found == false)
			{
				lastRankId = (lastRankId + 1) % systemConfig->getRankCount(); // try the next rank

				if (lastRankId == 0)
				{
					lastBankId = (lastBankId + 1) % systemConfig->getBankCount(); // try the next bank

					if (lastBankId == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;						
					}
				}

				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.front();

				if(temp_c != NULL)
				{
					if (systemConfig->isReadWriteGrouping() == false)
					{
						return rank[lastRankId].bank[lastBankId].per_bank_q.front();
					}
					else // have to follow read_write grouping considerations 
					{
						command *next_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							return rank[lastRankId].bank[lastBankId].per_bank_q.front();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< temp_c->getAddress().rank_id << "] [" << temp_c->getAddress().bank_id << "] but wrong type, We want [" << transaction_type << "]. Candidate command type ";
					cerr << temp_c->getCommandType();
					cerr << " followed by ";
					cerr << rank[lastRankId].bank[lastBankId].per_bank_q.read(1)->getCommandType();
					cerr << "count [" << rank[lastRankId].bank[lastBankId].per_bank_q.get_count() << "]" << endl;
#endif

				}

#ifdef DEBUG_FLAG_2
				cerr << "Looked in [" << lastRankId << "] [" << lastBankId << "] but Q empty" << endl;
#endif

			}
		}
		break;

	case BANK_ROUND_ROBIN: // keep rank id as long as possible, go round robin down a given rank
		{			
			transaction_type_t transaction_type;
			switch (lastCommandType)
			{
			case RAS_COMMAND:
				{
					command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.front();

					if((temp_c != NULL) &&
						((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
					{
						return rank[lastRankId].bank[lastBankId].per_bank_q.front();
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
				lastBankId = (lastBankId + 1) % systemConfig->getBankCount();
				if (lastBankId == 0)
				{
					lastRankId = (lastRankId + 1) % systemConfig->getRankCount();
					if (lastRankId == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
					}
				}

				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.front();

				if (temp_c != NULL)
				{	
					if(systemConfig->isReadWriteGrouping() == false)
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations
					{
						command *next_c =  rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankId].bank[lastBankId].per_bank_q.front()->getAddress().bank_id == lastBankId);
							assert(rank[lastRankId].bank[lastBankId].per_bank_q.front()->getAddress().rank_id == lastRankId);
							return rank[lastRankId].bank[lastBankId].per_bank_q.front();
						}
					}
#ifdef DEBUG_FLAG_2
					cerr << "Looked in [" << temp_c->getAddress().rank_id << "] [" << temp_c->getAddress().bank_id << "] but wrong type, We want [" << transaction_type << "] Candidate command type [" << rank[lastRankId].bank[lastBankId].per_bank_q.read(1)->getCommandType() << "]" << endl;
#endif
				}
#ifdef DEBUG_FLAG_2
				cerr << "Looked in rank=[" << lastRankId << "] bank=[" << lastBankId << "] but Q empty" << endl;
#endif
			}
		}
		break;

	case GREEDY:
		{
			command *candidate_command = NULL;

			int candidate_gap = INT_MAX;

			for (unsigned rank_id = 0; rank_id < systemConfig->getRankCount(); ++rank_id)
			{
				for (unsigned bank_id = 0; bank_id < systemConfig->getBankCount() ; ++bank_id)
				{
					command *challenger_command = rank[rank_id].bank[bank_id].per_bank_q.front();

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

			command *temp_c = rank[candidate_command->getAddress().rank_id].bank[candidate_command->getAddress().bank_id].per_bank_q.front();
			assert(temp_c == candidate_command);

#ifdef DEBUG
			timingOutStream << "R[" << candidate_command->getAddress().rank_id << "] B[" << candidate_command->getAddress().bank_id << "]\tWinner: " << *temp_c << "gap[" << candidate_gap << "] now[" << time << "]" << endl;
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
