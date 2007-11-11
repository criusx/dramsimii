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
					const command *temp_c = bank_id->getPerBankQueue().front();

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
			transaction_type_t transactionType;

			switch (lastCommandType)
			{
				// if it was RAS before and you want to finish doing the read/write
			case RAS_COMMAND:
				{
					const command *temp_c =  rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

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
			case PRECHARGE_COMMAND:
				transactionType = READ_TRANSACTION;
				break;

			case REFRESH_ALL_COMMAND:
			case CAS_WRITE_COMMAND:
			case CAS_WRITE_AND_PRECHARGE_COMMAND:
				transactionType = WRITE_TRANSACTION;
				break;	
				
			default:
				transactionType = READ_TRANSACTION;
				cerr << "Did not find a CAS or RAS command when it was expected" << endl;
				break;
			}

			unsigned originalLastRankID = lastRankID;
			unsigned originalLastBankID = lastBankID;
			transaction_type_t originalTransactionType = transactionType;
			bool noPendingRefreshes = false;

			while (true)
			{
				// select the next rank
				lastRankID = (lastRankID + 1) % systemConfig.getRankCount();

				// select the next bank when all ranks at this bank have been checked
				if (lastRankID == originalLastRankID)
				{
					if (!noPendingRefreshes)
					{
						// before switching to the next bank, see if all the queues are refreshes in any rank
						for (vector<rank_c>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
						{
							bool notAllRefresh = false;
							for (vector<bank_c>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
							{
								// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
								if ((currentBank->getPerBankQueue().size() == 0) || ((currentBank->getPerBankQueue().front()) && (currentBank->getPerBankQueue().front()->getCommandType() != REFRESH_ALL_COMMAND)))
								{
									notAllRefresh = true;
									break;
								}
							}
							// are all the commands refreshes? if so then return this
							if (!notAllRefresh)
								return currentRank->bank[lastBankID].getPerBankQueue().front(); // which bank doesn't really matter
						}
						noPendingRefreshes = true;
					}

					lastBankID = (lastBankID + 1) % systemConfig.getBankCount();

					// when all ranks and all banks have been checked for a read/write, look for a write/read
					if (lastBankID == originalLastBankID)
					{
						
						transactionType = (transactionType == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
						// however, if this type has already been searched for, then there are no commands, so quit
						if (transactionType == originalTransactionType)
							break;
					}
				}

				const command *temp_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

				if (temp_c && temp_c->getCommandType() != REFRESH_ALL_COMMAND)
				{
					if (!systemConfig.isReadWriteGrouping())
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations 
					{
						// look at the second command
						command *next_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1);	

						if (next_c &&
							((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND || next_c->getCommandType() == CAS_COMMAND) && (transactionType == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND || next_c->getCommandType() == CAS_WRITE_COMMAND) && (transactionType == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankID].bank[lastBankID].getPerBankQueue().front()->getAddress().bank == lastBankID);
							assert(rank[lastRankID].bank[lastBankID].getPerBankQueue().front()->getAddress().rank == lastRankID);
							return rank[lastRankID].bank[lastBankID].getPerBankQueue().front();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< temp_c->getAddress().rank << "] [" << temp_c->getAddress().bank << "] but wrong type, We want [" << transactionType << "]. Candidate command type ";
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
			transaction_type_t transactionType;

			switch (lastCommandType)
			{
			case RAS_COMMAND:
				{
					const command *temp_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

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
						cerr << "Serious problems. RAS not followed by CAS." << endl;
						exit(2);
					}
				}
				break;
			case CAS_AND_PRECHARGE_COMMAND:
			case CAS_COMMAND:
			case PRECHARGE_COMMAND:
				transactionType = READ_TRANSACTION;
				break;

			case REFRESH_ALL_COMMAND:
			case CAS_WRITE_COMMAND:
			case CAS_WRITE_AND_PRECHARGE_COMMAND:
				transactionType = WRITE_TRANSACTION;
				break;
			default:
				transactionType = WRITE_TRANSACTION; // FIXME: added this to ensure no uninit vars
				cerr << "Did not find a CAS or RAS command when it was expected" << endl;
				break;
			}

			unsigned originalLastRankID = lastRankID;
			unsigned originalLastBankID = lastBankID;
			transaction_type_t originalTransactionType = transactionType;
			bool noPendingRefreshes = false;

			while (true)
			{
				// select the next bank
				lastBankID = (lastBankID + 1) % systemConfig.getBankCount();

				if (lastBankID == originalLastBankID)
				{
					if (!noPendingRefreshes)
					{
						// before switching to the next bank, see if all the queues are refreshes in any rank
						for (vector<rank_c>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
						{
							bool notAllRefresh = false;
							for (vector<bank_c>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
							{
								// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
								if ((currentBank->getPerBankQueue().size() == 0) || ((currentBank->getPerBankQueue().front()) && (currentBank->getPerBankQueue().front()->getCommandType() != REFRESH_ALL_COMMAND)))
								{
									notAllRefresh = true;
									break;
								}
							}
							// are all the commands refreshes? if so then return this
							if (!notAllRefresh)
								return currentRank->bank[lastBankID].getPerBankQueue().front(); // which bank doesn't really matter
						}
						noPendingRefreshes = true;
					}

					lastRankID = (lastRankID + 1) % systemConfig.getRankCount();

					if (lastRankID == originalLastRankID)
					{
						transactionType = (transactionType == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
						// however, if this type has already been searched for, then there are no commands, so quit
						if (transactionType == originalTransactionType)
							break;
					}
				}

				const command *temp_c = rank[lastRankID].bank[lastBankID].getPerBankQueue().front();

				if (temp_c && temp_c->getCommandType() != REFRESH_ALL_COMMAND)
				{	
					if(systemConfig.isReadWriteGrouping() == false)
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations
					{
						// look at the second command
						command *next_c =  rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1);

						if (next_c &&
							((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND || next_c->getCommandType() == CAS_COMMAND) && (transactionType == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND || next_c->getCommandType() == CAS_WRITE_COMMAND) && (transactionType == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankID].bank[lastBankID].getPerBankQueue().front()->getAddress().bank == lastBankID);
							assert(rank[lastRankID].bank[lastBankID].getPerBankQueue().front()->getAddress().rank == lastRankID);
							return rank[lastRankID].bank[lastBankID].getPerBankQueue().front();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< temp_c->getAddress().rank << "] [" << temp_c->getAddress().bank << "] but wrong type, We want [" << transactionType << "]. Candidate command type ";
					cerr << temp_c->getCommandType();
					cerr << " followed by ";
					cerr << rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1)->getCommandType();
					cerr << "count [" << rank[lastRankID].bank[lastBankID].getPerBankQueue().get_count() << "]" << endl;
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
			const command *candidateCommand = NULL;

			int candidateGap = INT_MAX;

			for (vector<rank_c>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				bool notAllRefresh = false;

				for (vector<bank_c>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
				{
					const command *challengerCommand = currentBank->getPerBankQueue().front();

					if (challengerCommand)
					{
						int challengerGap = minProtocolGap(challengerCommand);

						if (challengerGap < candidateGap || (candidateGap == challengerGap && challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))
						{
							if (challengerCommand->getCommandType() == REFRESH_ALL_COMMAND)
							{					
								// if it hasn't been proven to be all refreshes or not yet
								if (!notAllRefresh)
								{
									for (vector<bank_c>::const_iterator thisBank = currentRank->bank.begin(); thisBank != currentRank->bank.end(); thisBank++)
									{
										// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
										if ((thisBank->getPerBankQueue().size() == 0) || (thisBank->getPerBankQueue().front()->getCommandType() != REFRESH_ALL_COMMAND))
										{
											notAllRefresh = true;
											break;
										}
									}
									// are all the commands refreshes? if so then choose this
									if (!notAllRefresh)
									{
										candidateGap = challengerGap;
										candidateCommand = challengerCommand;
									}
								}
							}
							else
							{
								candidateGap = challengerGap;
								candidateCommand = challengerCommand;
							}							
						}
					}
				}
			}

			if (candidateCommand)
			{
				assert(candidateCommand->getCommandType() == REFRESH_ALL_COMMAND || rank[candidateCommand->getAddress().rank].bank[candidateCommand->getAddress().bank].getPerBankQueue().front() == candidateCommand);

#ifdef DEBUG_GREEDY
			timingOutStream << "R[" << candidateCommand->getAddress().rank << "] B[" << candidateCommand->getAddress().bank << "]\tWinner: " << *candidateCommand << "gap[" << candidateGap << "] now[" << time << "]" << endl;
#endif
			}

			return candidateCommand;
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
