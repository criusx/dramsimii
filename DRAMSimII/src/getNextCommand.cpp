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
	//dramChannel &channel= dramSystem::channel[chan_id];

	// look at the most recently retired command in this channel's history
	const command *lastCommand = get_most_recent_command();

	unsigned lastBankId = lastCommand ? lastCommand->getAddress().bank_id : rank[0].bank.size() - 1;
	unsigned lastRankId = lastCommand ? lastCommand->getAddress().rank_id : rank.size() - 1;
	const command_type_t lastCommandType = lastCommand ? lastCommand->getCommandType() : CAS_WRITE_AND_PRECHARGE_COMMAND;

	switch (system_config->getCommandOrderingAlgorithm())
	{
	case STRICT_ORDER: // look for oldest command, execute that
		{
			numeric_limits<tick_t> ll;
			tick_t oldest_command_time = ll.max();	
			vector<bank_c>::iterator oldest_bank_id;
			vector<rank_c>::iterator oldest_rank_id;

			// search each rank and each bank to find the oldest command
			for (vector<rank_c>::iterator rank_id = rank.begin(); rank_id != rank.end(); rank_id++)
			{
				bool notAllRefresh = false;

				for (vector<bank_c>::iterator bank_id = rank_id->bank.begin(); bank_id != rank_id->bank.end(); bank_id++)
				{
					command *temp_c = bank_id->per_bank_q.read_back();

					if (temp_c != NULL)
					{
						if (oldest_command_time > temp_c->getEnqueueTime())
						{
							// if it's a refresh_all command and
							// we haven't proved that all the queues aren't refresh_all commands, search
							if (temp_c->getCommandType() == REFRESH_ALL_COMMAND)
							{
								if (!notAllRefresh)
								{
									for (vector<bank_c>::iterator cur_bank = rank_id->bank.begin(); cur_bank != rank_id->bank.end(); cur_bank++)
									{
										if (cur_bank->per_bank_q.read_back()->getCommandType() != REFRESH_ALL_COMMAND)
										{
											notAllRefresh = true;
											break;
										}

									}
									if (!notAllRefresh)
									{
										oldest_command_time = temp_c->getEnqueueTime();
										oldest_bank_id = bank_id;
										oldest_rank_id = rank_id;
									}
								}
							}
							else
							{
								oldest_command_time = temp_c->getEnqueueTime();
								oldest_bank_id = bank_id;
							}
						}
					}
				}
			}

			// if there was a command found
			if (oldest_command_time < ll.max())
			{
				// if it was a refresh all command, then dequeue all n banks worth of commands
				if (oldest_bank_id->per_bank_q.read_back()->getCommandType() == REFRESH_ALL_COMMAND)
				{
					for (vector<bank_c>::iterator cur_bank = oldest_rank_id->bank.begin(); cur_bank != oldest_rank_id->bank.end();)
					{
						command *temp_com = cur_bank->per_bank_q.dequeue();
						cur_bank++;
						if (cur_bank == oldest_rank_id->bank.end())
							return temp_com;
						else
							delete temp_com;
					}

				}
				else
				{
					return oldest_bank_id->per_bank_q.dequeue();
				}
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
				command *temp_c =  rank[lastRankId].bank[lastBankId].per_bank_q.read(0);

				if ((temp_c != NULL) &&
					((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
				{
					return rank[lastRankId].bank[lastBankId].per_bank_q.dequeue();
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
				lastRankId = (lastRankId + 1) % rank.size(); // try the next rank

				if (lastRankId == 0)
				{
					lastBankId = (lastBankId + 1) % rank[lastRankId].bank.size(); // try the next bank

					if (lastBankId == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;						
					}
				}

				command *temp_c =  rank[lastRankId].bank[lastBankId].per_bank_q.read(0);

				if(temp_c != NULL)
				{
					if(system_config->isReadWriteGrouping() == false)
					{
						return rank[lastRankId].bank[lastBankId].per_bank_q.dequeue();
					}
					else // have to follow read_write grouping considerations 
					{
						command *next_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							return rank[lastRankId].bank[lastBankId].per_bank_q.dequeue();
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
			if (lastCommandType == RAS_COMMAND)
			{
				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);

				if((temp_c != NULL) &&
					((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND))){
						return rank[lastRankId].bank[lastBankId].per_bank_q.dequeue();
				}
				else
				{
					cerr << "Serious problems. RAS not followed by CAS." << endl;
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
				transaction_type = WRITE_TRANSACTION; // FIXME: added this to ensure no uninit vars
			}

			bool candidate_found = false;

			while (candidate_found == false)
			{
				lastBankId = (lastBankId + 1) % system_config->getBankCount();
				if (lastBankId == 0)
				{
					lastRankId = (lastRankId + 1) % rank.size();
					if (lastRankId  == 0)
					{
						if (transaction_type == WRITE_TRANSACTION)
						{
							transaction_type = READ_TRANSACTION;
						}
						else
						{
							transaction_type = WRITE_TRANSACTION;
						}
					}
				}

				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);

				if(temp_c != NULL)
				{	
					if(system_config->isReadWriteGrouping() == false)
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations
					{
						command *next_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION))){
								return rank[lastRankId].bank[lastBankId].per_bank_q.dequeue();
						}
					}
#ifdef DEBUG_FLAG_2
					cerr << "Looked in [" << temp_c->getAddress().rank_id << "] [" << temp_c->getAddress().bank_id << "] but wrong type, We want [" << transaction_type << "] Candidate command type [" << rank[lastRankId].bank[lastBankId].per_bank_q.read(1)->getCommandType() << "]" << endl;
#endif
				}
#ifdef DEBUG_FLAG_2
				cerr << "Looked in [" << lastRankId << "] [" << lastBankId << "] but Q empty" << endl;
#endif
			}
		}
		break;

		//	case WANG_RANK_HOP:	
		//		if(system_config->config_type == BASELINE_CONFIG)	/* baseline*/
		//		{
		//			bool candidate_found = false;
		//
		//			while(candidate_found == false)
		//			{
		//				command *ptr_c = algorithm.getWHCC().read(algorithm.WHCCOffset()[0]);
		//				algorithm.WHCCOffset()[0] = (algorithm.WHCCOffset()[0] + 1) % algorithm.getWHCC().get_count();
		//				
		//				command *candidate_c = channel.getRank(lastRankId).bank[lastBankId].per_bank_q.read(0);
		//
		//				if((candidate_c != NULL) && (candidate_c->getCommandType() == RAS_COMMAND))
		//				{
		//					if(ptr_c->getCommandType() == RAS_COMMAND)
		//					{
		//						if(lastBankId == 0)	// see if this rank needs a R/W switch around
		//						{
		//							algorithm.getTransactionType()[lastRankId] = channel.set_read_write_type(lastRankId,system_config->bank_count);
		//						}
		//
		//						command *next_c =  channel.getRank(lastRankId).bank[lastBankId].per_bank_q.read(1);
		//
		//						if(((algorithm.getTransactionType()[lastRankId] == READ_TRANSACTION) && (next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)) ||
		//							((algorithm.getTransactionType()[lastRankId] == WRITE_TRANSACTION) && (next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND)))
		//						{
		//							candidate_found = true;
		//							return  channel.getRank(lastRankId).bank[lastBankId].per_bank_q.dequeue();
		//						}
		//					}
		//				}
		//				else if((candidate_c != NULL) && (candidate_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND))
		//				{
		//					if((ptr_c->getCommandType() == CAS_COMMAND) && (algorithm.getTransactionType()[lastRankId] == READ_TRANSACTION))
		//					{
		//						candidate_found = true;
		//						return  channel.getRank(lastRankId).bank[lastBankId].per_bank_q.dequeue();
		//					}
		//				}
		//				else if ((candidate_c != NULL) && (candidate_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND))
		//				{
		//					if((ptr_c->getCommandType() == CAS_COMMAND) && (algorithm.getTransactionType()[lastRankId] == WRITE_TRANSACTION))
		//					{
		//						candidate_found = false;
		//						return  channel.getRank(lastRankId).bank[lastBankId].per_bank_q.dequeue();
		//					}
		//				}
		//				else
		//				{
		//#ifdef DEBUG_FLAG_1
		//					cerr << "some debug message" << endl;
		//#endif
		//				}
		//			}
		//		}
		//		break;

	case GREEDY:
		{
			command *candidate_command = NULL;

			int candidate_gap = INT_MAX;

			for (unsigned rank_id = 0; rank_id < rank.size(); ++rank_id)
			{
				for (unsigned bank_id = 0; bank_id < rank[rank_id].bank.size() ; ++bank_id)
				{
					command *challenger_command = rank[rank_id].bank[bank_id].per_bank_q.read(0);

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

			command *temp_c = rank[candidate_command->getAddress().rank_id].bank[candidate_command->getAddress().bank_id].per_bank_q.dequeue();

#ifdef DEBUG
			outStream << "R[" << candidate_command->getAddress().rank_id << "] B[" << candidate_command->getAddress().bank_id << "]\tWinner: " << *temp_c << "gap[" << candidate_gap << "] now[" << time << "]" << endl;
#endif

			return temp_c;
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


/// <summary>
/// Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned is not yet removed from the per_bank_command_queue
/// from which it was selected
/// </summary>
command *dramChannel::readNextCommand() const
{
	//const dramChannel &channel= dramSystem::channel[chan_id];

	// look at the most recently retired command in this channel's history

	const command *lastCommand = get_most_recent_command();

	unsigned lastBankId = lastCommand ? lastCommand->getAddress().bank_id : system_config->getBankCount() - 1;
	unsigned lastRankId = lastCommand ? lastCommand->getAddress().rank_id : system_config->getRankCount() - 1;
	const command_type_t lastCommandType = lastCommand ? lastCommand->getCommandType() : CAS_WRITE_AND_PRECHARGE_COMMAND;

	switch (system_config->getCommandOrderingAlgorithm())
	{
	case STRICT_ORDER: // look for oldest command, execute that
		{
			numeric_limits<tick_t> ll;
			tick_t oldest_command_time = ll.max();
			//bool foundSomething = false;			
			vector<bank_c>::const_iterator oldest_bank_id;
			vector<rank_c>::const_iterator oldest_rank_id;

			for (vector<rank_c>::const_iterator rank_id = rank.begin(); rank_id != rank.end(); rank_id++)
			{
				bool notAllRefresh = false;

				for (vector<bank_c>::const_iterator bank_id = rank_id->bank.begin(); bank_id != rank_id->bank.end(); bank_id++)
				{
					command *temp_c = bank_id->per_bank_q.read_back();

					if (temp_c != NULL)
					{
						if (oldest_command_time > temp_c->getEnqueueTime())
						{
							// if it's a refresh_all command and
							// we haven't proved that all the queues aren't refresh_all commands, search
							if (temp_c->getCommandType() == REFRESH_ALL_COMMAND)
							{
								if (!notAllRefresh)
								{
									for (vector<bank_c>::const_iterator currentBank = rank_id->bank.begin(); currentBank != rank_id->bank.end(); currentBank++)
									{
										if (currentBank->per_bank_q.read_back()->getCommandType() != REFRESH_ALL_COMMAND)
										{
											notAllRefresh = true;
											break;
										}

									}

									if (!notAllRefresh)
									{
										oldest_command_time = temp_c->getEnqueueTime();
										//foundSomething = true;
										oldest_bank_id = bank_id;
										oldest_rank_id = rank_id;
									}
								}
							}
							else
							{
								oldest_command_time = temp_c->getEnqueueTime();
								//foundSomething = true;
								oldest_bank_id = bank_id;
							}
						}
					}
				}
			}

			// if there was a command found
			if (oldest_command_time < ll.max())
			{
				assert(rank[oldest_bank_id->per_bank_q.read_back()->getAddress().rank_id].bank[oldest_bank_id->per_bank_q.read_back()->getAddress().rank_id].per_bank_q.read_back() == oldest_bank_id->per_bank_q.read_back());

				return oldest_bank_id->per_bank_q.read_back();				
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
				command *temp_c =  rank[lastRankId].bank[lastBankId].per_bank_q.read_back();

				if ((temp_c != NULL) &&
					((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
				{
					return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
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
				lastRankId = (lastRankId + 1) % system_config->getRankCount(); // try the next rank

				if (lastRankId == 0)
				{
					lastBankId = (lastBankId + 1) % system_config->getBankCount(); // try the next bank

					if (lastBankId == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;						
					}
				}

				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);

				if(temp_c != NULL)
				{
					if (system_config->isReadWriteGrouping() == false)
					{
						return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
					}
					else // have to follow read_write grouping considerations 
					{
						command *next_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
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
					command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);

					if((temp_c != NULL) &&
						((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
					{
						return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
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
				lastBankId = (lastBankId + 1) % system_config->getBankCount();
				if (lastBankId == 0)
				{
					lastRankId = (lastRankId + 1) % system_config->getRankCount();
					if (lastRankId == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
					}
				}

				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);

				if (temp_c != NULL)
				{	
					if(system_config->isReadWriteGrouping() == false)
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations
					{
						command *next_c =  rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankId].bank[lastBankId].per_bank_q.read_back()->getAddress().bank_id == lastBankId);
							assert(rank[lastRankId].bank[lastBankId].per_bank_q.read_back()->getAddress().rank_id == lastRankId);
							return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
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

			for (unsigned rank_id = 0; rank_id < system_config->getRankCount(); ++rank_id)
			{
				for (unsigned bank_id = 0; bank_id < system_config->getBankCount() ; ++bank_id)
				{
					command *challenger_command = rank[rank_id].bank[bank_id].per_bank_q.read_back();

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

			command *temp_c = rank[candidate_command->getAddress().rank_id].bank[candidate_command->getAddress().bank_id].per_bank_q.read_back();
			assert(temp_c == candidate_command);
			
#ifdef DEBUG
			outStream << "R[" << candidate_command->getAddress().rank_id << "] B[" << candidate_command->getAddress().bank_id << "]\tWinner: " << *temp_c << "gap[" << candidate_gap << "] now[" << time << "]" << endl;
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

//
///// <summary>
///// Chooses the command which should be executed next from the given channel
///// Choice is made based on command_ordering_algorithm from system config
///// Command returned is not yet removed from the per_bank_command_queue
///// from which it was selected
///// </summary>
//queue<command> *dramChannel::chooseNextCommand() const
//{
//	//const dramChannel &channel= dramSystem::channel[chan_id];
//
//	// look at the most recently retired command in this channel's history
//
//	const command *lastCommand = get_most_recent_command();
//
//	unsigned lastBankId = lastCommand ? lastCommand->getAddress().bank_id : system_config->getBankCount() - 1;
//	unsigned lastRankId = lastCommand ? lastCommand->getAddress().rank_id : system_config->getRankCount() - 1;
//	const command_type_t lastCommandType = lastCommand ? lastCommand->getCommandType() : CAS_WRITE_AND_PRECHARGE_COMMAND;
//
//	switch (system_config->getCommandOrderingAlgorithm())
//	{
//	case STRICT_ORDER: // look for oldest command, execute that
//		{
//			numeric_limits<tick_t> ll;
//			tick_t oldest_command_time = ll.max();
//			//bool foundSomething = false;			
//			vector<bank_c>::const_iterator oldest_bank_id;
//			vector<rank_c>::const_iterator oldest_rank_id;
//
//			for (vector<rank_c>::const_iterator rank_id = rank.begin(); rank_id != rank.end(); rank_id++)
//			{
//				bool notAllRefresh = false;
//
//				for (vector<bank_c>::const_iterator bank_id = rank_id->bank.begin(); bank_id != rank_id->bank.end(); bank_id++)
//				{
//					command *temp_c = bank_id->per_bank_q.read_back();
//
//					if (temp_c != NULL)
//					{
//						if (oldest_command_time > temp_c->getEnqueueTime())
//						{
//							// if it's a refresh_all command and
//							// we haven't proved that all the queues aren't refresh_all commands, search
//							if (temp_c->getCommandType() == REFRESH_ALL_COMMAND)
//							{
//								if (!notAllRefresh)
//								{
//									for (vector<bank_c>::const_iterator currentBank = rank_id->bank.begin(); currentBank != rank_id->bank.end(); currentBank++)
//									{
//										if (currentBank->per_bank_q.read_back()->getCommandType() != REFRESH_ALL_COMMAND)
//										{
//											notAllRefresh = true;
//											break;
//										}
//
//									}
//
//									if (!notAllRefresh)
//									{
//										oldest_command_time = temp_c->getEnqueueTime();
//										//foundSomething = true;
//										oldest_bank_id = bank_id;
//										oldest_rank_id = rank_id;
//									}
//								}
//							}
//							else
//							{
//								oldest_command_time = temp_c->getEnqueueTime();
//								//foundSomething = true;
//								oldest_bank_id = bank_id;
//							}
//						}
//					}
//				}
//			}
//
//			// if there was a command found
//			if (oldest_command_time < ll.max())
//			{
//				return &oldest_bank_id->per_bank_q;
//				// if it was a refresh all command, then dequeue all n banks worth of commands
//				/*if (oldest_bank_id->per_bank_q.read_back()->this_command == REFRESH_ALL_COMMAND)
//				{
//				for (vector<bank_c>::const_iterator cur_bank = oldest_rank_id->bank.begin(); cur_bank != oldest_rank_id->bank.end();)
//				{
//				command *temp_com = cur_bank->per_bank_q.read_back();
//				cur_bank++;
//				if (cur_bank == oldest_rank_id->bank.end())
//				return temp_com;
//				else
//				delete temp_com;
//				}
//
//				}
//				else
//				{
//				return oldest_bank_id->per_bank_q.dequeue();
//				} */
//			}
//			else
//				return NULL;
//		}
//		break;
//
//		// alternate ranks as we go down banks
//	case RANK_ROUND_ROBIN:
//		{
//			transaction_type_t transaction_type;
//
//			if (lastCommandType == RAS_COMMAND)
//			{
//				command *temp_c =  rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
//
//				if ((temp_c != NULL) &&
//					((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
//				{
//					return &rank[lastRankId].bank[lastBankId].per_bank_q;
//				}
//				else
//				{
//					cerr << "Serious problems. RAS not followed by CAS" << endl;
//					exit(2);
//				}
//			}
//			else if (lastCommandType == CAS_AND_PRECHARGE_COMMAND)
//			{
//				transaction_type = READ_TRANSACTION;
//			}
//			else if (lastCommandType == CAS_WRITE_AND_PRECHARGE_COMMAND)
//			{
//				transaction_type = WRITE_TRANSACTION;
//			}
//			else
//			{
//				transaction_type = READ_TRANSACTION;
//				cerr << "Did not find a CAS or RAS command when it was expected" << endl;
//			}
//
//			bool candidate_found = false;
//
//			while (candidate_found == false)
//			{
//				lastRankId = (lastRankId + 1) % system_config->getRankCount(); // try the next rank
//
//				if (lastRankId == 0)
//				{
//					lastBankId = (lastBankId + 1) % system_config->getBankCount(); // try the next bank
//
//					if (lastBankId == 0)
//					{
//						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;						
//					}
//				}
//
//				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);
//
//				if(temp_c != NULL)
//				{
//					if (system_config->isReadWriteGrouping() == false)
//					{
//						return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
//					}
//					else // have to follow read_write grouping considerations 
//					{
//						command *next_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */
//
//						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
//							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
//						{
//							return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
//						}
//					}
//
//#ifdef DEBUG_FLAG_2
//					cerr << "Looked in ["<< temp_c->getAddress().rank_id << "] [" << temp_c->getAddress().bank_id << "] but wrong type, We want [" << transaction_type << "]. Candidate command type ";
//					cerr << temp_c->getCommandType();
//					cerr << " followed by ";
//					cerr << rank[lastRankId].bank[lastBankId].per_bank_q.read(1)->getCommandType();
//					cerr << "count [" << rank[lastRankId].bank[lastBankId].per_bank_q.get_count() << "]" << endl;
//#endif
//
//				}
//
//#ifdef DEBUG_FLAG_2
//				cerr << "Looked in [" << lastRankId << "] [" << lastBankId << "] but Q empty" << endl;
//#endif
//
//			}
//		}
//		break;
//
//	case BANK_ROUND_ROBIN: // keep rank id as long as possible, go round robin down a given rank
//		{			
//			transaction_type_t transaction_type;
//			switch (lastCommandType)
//			{
//			case RAS_COMMAND:
//				{
//					command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);
//
//					if((temp_c != NULL) &&
//						((temp_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)))
//					{
//						return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
//					}
//					else
//					{
//						cerr << "Serious problems. RAS not followed by CAS." << endl;
//						exit(2);
//					}
//				}
//				break;
//			case CAS_AND_PRECHARGE_COMMAND:
//				transaction_type = READ_TRANSACTION;
//				break;
//			case CAS_WRITE_AND_PRECHARGE_COMMAND:
//				transaction_type = WRITE_TRANSACTION;
//				break;
//			default:
//				transaction_type = WRITE_TRANSACTION; // FIXME: added this to ensure no uninit vars
//				break;
//			}
//
//			bool candidate_found = false;
//
//			while (candidate_found == false)
//			{
//				lastBankId = (lastBankId + 1) % system_config->getBankCount();
//				if (lastBankId == 0)
//				{
//					lastRankId = (lastRankId + 1) % system_config->getRankCount();
//					if (lastRankId == 0)
//					{
//						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
//					}
//				}
//
//				command *temp_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(0);
//
//				if (temp_c != NULL)
//				{	
//					if(system_config->isReadWriteGrouping() == false)
//					{
//						return temp_c;
//					}
//					else // have to follow read_write grouping considerations
//					{
//						command *next_c = rank[lastRankId].bank[lastBankId].per_bank_q.read(1);	/* look at the second command */
//
//						if (((next_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
//							((next_c->getCommandType() == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION))){
//								return rank[lastRankId].bank[lastBankId].per_bank_q.read_back();
//						}
//					}
//#ifdef DEBUG_FLAG_2
//					cerr << "Looked in [" << temp_c->getAddress().rank_id << "] [" << temp_c->getAddress().bank_id << "] but wrong type, We want [" << transaction_type << "] Candidate command type [" << rank[lastRankId].bank[lastBankId].per_bank_q.read(1)->getCommandType() << "]" << endl;
//#endif
//				}
//#ifdef DEBUG_FLAG_2
//				cerr << "Looked in rank=[" << lastRankId << "] bank=[" << lastBankId << "] but Q empty" << endl;
//#endif
//			}
//		}
//		break;
//
//	case GREEDY:
//		{
//			command *candidate_command = NULL;
//
//			int candidate_gap = INT_MAX;
//
//			for (unsigned rank_id = 0; rank_id < system_config->getRankCount(); ++rank_id)
//			{
//				for (unsigned bank_id = 0; bank_id < system_config->getBankCount() ; ++bank_id)
//				{
//					command *challenger_command = rank[rank_id].bank[bank_id].per_bank_q.read(0);
//
//					if (challenger_command != NULL)
//					{
//						int challenger_gap = minProtocolGap(challenger_command);
//						if (challenger_gap < candidate_gap || (candidate_gap == challenger_gap && challenger_command->getEnqueueTime() < candidate_command->getEnqueueTime()))
//						{
//							candidate_gap = challenger_gap;
//							candidate_command = challenger_command;
//						}
//					}
//				}
//			}
//
//			command *temp_c = rank[candidate_command->getAddress().rank_id].bank[candidate_command->getAddress().bank_id].per_bank_q.read_back();
//
//#ifdef DEBUG
//			outStream << "R[" << candidate_command->getAddress().rank_id << "] B[" << candidate_command->getAddress().bank_id << "]\tWinner: " << *temp_c << "gap[" << candidate_gap << "] now[" << time << "]" << endl;
//#endif
//
//			return temp_c;
//		}
//		break;
//
//	default:
//		{
//			cerr << "This configuration and algorithm combination is not supported" << endl;
//			exit(0);
//		}
//		break;
//	}
//	return NULL;
//}