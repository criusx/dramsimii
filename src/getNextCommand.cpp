#include <iostream>

#include "Channel.h"

using namespace std;
using namespace DRAMSimII;

//////////////////////////////////////////////////////////////////////
/// @brief chooses and dequeues the next command to execute
/// @details Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned has already been removed from the per_bank_command_queue
/// from which it was selected
/// @author Joe Gross
/// @return a pointer to the next command
//////////////////////////////////////////////////////////////////////
Command *Channel::getNextCommand()
{
	if (const Command *nextCommand = readNextCommand())
	{
		Rank &currentRank = rank[nextCommand->getAddress().rank];
		
		// if it was a refresh all command, then dequeue all n banks worth of commands
		if (nextCommand->getCommandType() == REFRESH_ALL_COMMAND)
		{
			Command *tempCommand = NULL;

			for (vector<Bank>::iterator currentBank = currentRank.bank.begin(); currentBank != currentRank.bank.end();currentBank++)
			{
				if (tempCommand)
					delete tempCommand;

				tempCommand = currentBank->pop();

				assert(tempCommand->getCommandType() == REFRESH_ALL_COMMAND);
			}

			return tempCommand;

		}
		else
		{			
			assert(currentRank.bank[nextCommand->getAddress().bank].front() == nextCommand);
			return currentRank.bank[nextCommand->getAddress().bank].pop();
		}
	}
	else
	{
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////
/// @brief returns a pointer to the command which would be chosen to be executed next
/// @details chooses a command according to the command ordering algorithm, tries to choose a command which can execute
/// command is not actually removed from the per bank queues
/// @author Joe Gross
/// @return a const pointer to the next available command
//////////////////////////////////////////////////////////////////////
const Command *Channel::readNextCommand() const
{
	// look at the most recently retired command in this channel's history

	const Command *lastCommand = historyQueue.back();

	const unsigned lastBankID = lastCommand ? lastCommand->getAddress().bank : systemConfig.getBankCount() - 1;
	const unsigned lastRankID = lastCommand ? lastCommand->getAddress().rank : systemConfig.getRankCount() - 1;
	const CommandType lastCommandType = lastCommand ? lastCommand->getCommandType() : CAS_WRITE_AND_PRECHARGE_COMMAND;

	switch (systemConfig.getCommandOrderingAlgorithm())
	{
		// this strategy attempts to find the oldest command and returns that to be executed
		// however, if the oldest command cannot be issued, the oldest command that can be executed immediately
		// will be returned instead
	case STRICT_ORDER:
		{
			tick oldestCommandTime = TICK_MAX;
			tick oldestExecutableCommandTime = TICK_MAX;
			vector<Bank>::const_iterator oldestBank;
			vector<Bank>::const_iterator oldestExecutableBank;

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				bool notAllRefresh = false;

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
				{
					if (const Command *challengerCommand = currentBank->front())
					{
						if ((challengerCommand->getEnqueueTime() < oldestExecutableCommandTime) && (earliestExecuteTime(challengerCommand) <= time + timingSpecification.tCMD()))
						{
							assert(earliestExecuteTime(challengerCommand) == time + minProtocolGap(challengerCommand));

							// if it's a refresh_all command and we haven't proved that all the queues aren't refresh_all commands, search
							if (challengerCommand->getCommandType() == REFRESH_ALL_COMMAND)
							{
								if (!notAllRefresh)
								{
									// try to show that at the head of each queue isn't a refresh command
									for (vector<Bank>::const_iterator thisBank = currentRank->bank.begin(); thisBank != currentRank->bank.end(); thisBank++)
									{
										// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
										if (thisBank->nextCommandType() != REFRESH_ALL_COMMAND)
										{
											notAllRefresh = true;
											break;
										}

									}
									// if all are known now to be refresh commands
									if (!notAllRefresh)
									{
										oldestExecutableCommandTime = challengerCommand->getEnqueueTime();
										oldestExecutableBank = currentBank;
									}
								}
							}
						else if (challengerCommand->getEnqueueTime() < oldestCommandTime)
						{
							// if it's a refresh_all command and
							// we haven't proved that all the queues aren't refresh_all commands, search
							if (challengerCommand->getCommandType() == REFRESH_ALL_COMMAND)
							{
								if (!notAllRefresh)
								{
									// try to show that at the head of each queue isn't a refresh command
									for (vector<Bank>::const_iterator thisBank = currentRank->bank.begin(); thisBank != currentRank->bank.end(); thisBank++)
									{
										// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
										if (thisBank->nextCommandType() != REFRESH_ALL_COMMAND)
										{
											notAllRefresh = true;
											break;
										}

									}
									// if all are known now to be refresh commands
									if (!notAllRefresh)
									{
										oldestCommandTime = challengerCommand->getEnqueueTime();
										oldestBank = currentBank;
									}
								}
							}
							else
							{
								oldestCommandTime = challengerCommand->getEnqueueTime();
								oldestBank = currentBank;
							}

						}
						
							else
							{
								oldestExecutableCommandTime = challengerCommand->getEnqueueTime();
								oldestExecutableBank = currentBank;
							}
						}
					}
				}
			}

			// if any executable command was found, prioritize it over those which must wait
			if (oldestExecutableCommandTime < TICK_MAX)
			{
				assert(oldestExecutableBank->nextCommandType() == REFRESH_ALL_COMMAND || rank[oldestExecutableBank->front()->getAddress().rank].bank[oldestExecutableBank->front()->getAddress().bank].front() == oldestExecutableBank->front());

				return oldestExecutableBank->front();
			}
			// if there was a command found
			else if (oldestCommandTime < TICK_MAX)
			{
				assert(oldestBank->front()->getCommandType() == REFRESH_ALL_COMMAND || rank[oldestBank->front()->getAddress().rank].bank[oldestBank->front()->getAddress().bank].front() == oldestBank->front());

				return oldestBank->front();
			}
			else
				return NULL;
		}
		break;

		// alternate ranks as we go down banks
	case RANK_ROUND_ROBIN:
		{
			TransactionType transactionType;

			// attempt to group RAS/CAS pairs together
			switch (lastCommandType)
			{
				// if it was RAS before and you want to finish doing the read/write
			case RAS_COMMAND:
				{
					// look at the command just after the RAS, it should be some sort of CAS
					const Command *nextCommand = rank[lastRankID].bank[lastBankID].front();

					if ((nextCommand) &&
						((nextCommand->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) ||
						(nextCommand->getCommandType() == CAS_AND_PRECHARGE_COMMAND) ||
						(nextCommand->getCommandType() == CAS_COMMAND) ||
						(nextCommand->getCommandType() == CAS_WRITE_COMMAND)))
					{
						return rank[lastRankID].bank[lastBankID].front();
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

			unsigned currentRankID = lastRankID;
			unsigned currentBankID = lastBankID;
			TransactionType originalTransactionType = transactionType;
			bool noPendingRefreshes = false;
			bool allowNotReadyCommands = false;

			while (true)
			{
				// select the next rank
				currentRankID = (currentRankID + 1) % systemConfig.getRankCount();

				// select the next bank when all ranks at this bank have been checked
				if (lastRankID == currentRankID)
				{
					if (!noPendingRefreshes)
					{
						// before switching to the next bank, see if all the queues are refreshes in any rank
						for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
						{
							bool notAllRefresh = false;

							for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
							{
								// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
								if (currentBank->nextCommandType() != REFRESH_ALL_COMMAND)
								{
									notAllRefresh = true;
									break;
								}
							}

							// are all the commands refreshes? if so then return this
							if (!notAllRefresh)
								return currentRank->bank[currentBankID].front(); // which bank doesn't really matter
						}
						noPendingRefreshes = true;
					}

					// then switch to the next bank
					currentBankID = (currentBankID + 1) % systemConfig.getBankCount();

					// when all ranks and all banks have been checked for a read/write, look for a write/read
					if (currentBankID == currentRankID)
					{						
						transactionType = (transactionType == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;

						// however, if this type has already been searched for, then there are no commands, so quit
						if (transactionType == originalTransactionType)
						{
							if (allowNotReadyCommands)
								return NULL;
							else
								allowNotReadyCommands = true;
						}
					}
				}

				const Command *potentialCommand = rank[currentRankID].bank[currentBankID].front();

				// refresh commands are considered elsewhere
				if (potentialCommand && potentialCommand->getCommandType() != REFRESH_ALL_COMMAND && (allowNotReadyCommands || (earliestExecuteTime(potentialCommand) <= time + timingSpecification.tCMD() )))
				{
					if (!systemConfig.isReadWriteGrouping())
					{
						return potentialCommand;
					}
					else // have to follow read/write grouping considerations 
					{
						// look at the second command
						const Command *next_c = rank[lastRankID].bank[lastBankID].read(1);	

						if (next_c &&
							((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND || next_c->getCommandType() == CAS_COMMAND) && (transactionType == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND || next_c->getCommandType() == CAS_WRITE_COMMAND) && (transactionType == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().bank == lastBankID);
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().rank == lastRankID);
							return rank[lastRankID].bank[lastBankID].front();
						}
					}

				}
			}
		}
		break;

		// keep rank id as long as possible, go round robin down a given rank
	case BANK_ROUND_ROBIN: 
		{			
			TransactionType transactionType;

			switch (lastCommandType)
			{
			case RAS_COMMAND:
				{
					const Command *temp_c = rank[lastRankID].bank[lastBankID].front();

					if ((temp_c) &&
						((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) ||
						(temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) ||
						(temp_c->getCommandType() == CAS_COMMAND) ||
						(temp_c->getCommandType() == CAS_WRITE_COMMAND)))
					{
						return rank[lastRankID].bank[lastBankID].front();
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

			unsigned currentRankID = lastRankID;
			unsigned currentBankID = lastBankID;
			TransactionType originalTransactionType = transactionType;
			bool noPendingRefreshes = false;

			while (true)
			{
				// select the next bank
				currentBankID = (currentBankID + 1) % systemConfig.getBankCount();

				if (lastBankID == currentBankID)
				{
					if (!noPendingRefreshes)
					{
						// before switching to the next bank, see if all the queues are refreshes in any rank
						for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
						{
							bool notAllRefresh = false;

							for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
							{
								// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
								if (currentBank->nextCommandType() != REFRESH_ALL_COMMAND)
								{
									notAllRefresh = true;
									break;
								}
							}
							// are all the commands refreshes? if so then return this
							if (!notAllRefresh)
								return currentRank->bank[lastBankID].front(); // which bank doesn't really matter
						}
						noPendingRefreshes = true;
					}

					currentRankID = (currentRankID + 1) % systemConfig.getRankCount();

					if (lastRankID == currentRankID)
					{
						transactionType = (transactionType == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
						// however, if this type has already been searched for, then there are no commands, so quit
						if (transactionType == originalTransactionType)
							break;
					}
				}

				const Command *potentialCommand = rank[lastRankID].bank[lastBankID].front();

				if (potentialCommand && potentialCommand->getCommandType() != REFRESH_ALL_COMMAND)
				{	
					if(systemConfig.isReadWriteGrouping() == false)
					{
						return potentialCommand;
					}
					else // have to follow read_write grouping considerations
					{
						// look at the second command
						const Command *nextCommand =  rank[lastRankID].bank[lastBankID].read(1);

						if (nextCommand &&
							((nextCommand->getCommandType() == CAS_AND_PRECHARGE_COMMAND || nextCommand->getCommandType() == CAS_COMMAND) && (transactionType == READ_TRANSACTION)) ||
							((nextCommand->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND || nextCommand->getCommandType() == CAS_WRITE_COMMAND) && (transactionType == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().bank == lastBankID);
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().rank == lastRankID);
							return rank[lastRankID].bank[lastBankID].front();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< potentialCommand->getAddress().rank << "] [" << potentialCommand->getAddress().bank << "] but wrong type, We want [" << transactionType << "]. Candidate command type ";
					cerr << potentialCommand->getCommandType();
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
			const Command *candidateCommand = NULL;

			tick candidateExecuteTime = TICK_MAX;

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				bool notAllRefresh = false;

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
				{
					const Command *challengerCommand = currentBank->front();

					if (challengerCommand)
					{
						tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
						assert(time + minProtocolGap(challengerCommand) == challengerExecuteTime);

						// set a new candidate if the challenger can be executed sooner or execution times are the same but the challenger is older
						if (challengerExecuteTime < candidateExecuteTime || (candidateExecuteTime == challengerExecuteTime && challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))
						{
							if (challengerCommand->getCommandType() == REFRESH_ALL_COMMAND)
							{					
								// if it hasn't been proven to be all refreshes or not yet
								if (!notAllRefresh)
								{
									for (vector<Bank>::const_iterator thisBank = currentRank->bank.begin(); thisBank != currentRank->bank.end(); thisBank++)
									{
										// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
										if (thisBank->nextCommandType() != REFRESH_ALL_COMMAND)
										{
											notAllRefresh = true;
											break;
										}
									}
									// are all the commands refreshes? if so then choose this
									if (!notAllRefresh)
									{
										candidateExecuteTime = challengerExecuteTime;
										candidateCommand = challengerCommand;
									}
								}
							}
							else
							{
								candidateExecuteTime = challengerExecuteTime;
								candidateCommand = challengerCommand;
							}							
						}
					}
				}
			}

			if (candidateCommand)
			{
				assert(candidateCommand->getCommandType() == REFRESH_ALL_COMMAND || rank[candidateCommand->getAddress().rank].bank[candidateCommand->getAddress().bank].front() == candidateCommand);

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
