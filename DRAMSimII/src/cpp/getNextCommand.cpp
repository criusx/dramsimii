#include <iostream>
#include "dramsim2.h"

/// <summary>
/// Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned has already been removed from the per_bank_command_queue
/// from which it was selected
/// </summary>
command *dram_system::getNextCommand(const int chan_id)
{
	dram_channel *channel= &(dram_system::channel[chan_id]);

	// look at the most recently retired command in this channel's history

	command	*last_c = channel->get_most_recent_command();

	if (last_c == NULL)		
	{
		last_c = free_command_pool.acquire_item();
		last_c->addr.rank_id = system_config.rank_count - 1;
		last_c->addr.bank_id = system_config.bank_count - 1;
		last_c->this_command = CAS_WRITE_AND_PRECHARGE_COMMAND;
	}


	switch (system_config.command_ordering_algorithm)
	{
	case STRICT_ORDER: // look for oldest command, execute that
		{
			tick_t oldest_command_time = -1;
			int oldest_rank_id = -1;
			int oldest_bank_id = -1;

			for (int rank_id = system_config.rank_count - 1; rank_id > 0; rank_id--)
			{
				for (int bank_id = system_config.bank_count - 1; bank_id > 0; bank_id--)
				{
					command *temp_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

					if (temp_c != NULL)
					{
						if ((oldest_command_time < 0) || (oldest_command_time > temp_c->enqueue_time))
						{
							oldest_command_time = temp_c->enqueue_time;
							oldest_rank_id = rank_id;
							oldest_bank_id = bank_id;
						}
					}
				}
			}

			// if there were no commands found
			if (oldest_rank_id == -1)
				return NULL;
			else
				return channel->get_rank(oldest_rank_id)->bank[oldest_bank_id].per_bank_q.dequeue();
		}
		break;

		// alternate ranks as we go down banks
	case RANK_ROUND_ROBIN:
		{
			unsigned bank_id = last_c->addr.bank_id;
			unsigned rank_id = last_c->addr.rank_id;
			transaction_type_t transaction_type;

			if(last_c->this_command == RAS_COMMAND)
			{
				command *temp_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

				if ((temp_c != NULL) &&
					((temp_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->this_command == CAS_AND_PRECHARGE_COMMAND)))
				{
					return channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
				}
				else
				{
					cerr << "Serious problems. RAS not followed by CAS" << endl;
					_exit(2);
				}
			}
			else if (last_c->this_command == CAS_AND_PRECHARGE_COMMAND)
			{
				transaction_type = READ_TRANSACTION;
			}
			else if (last_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND)
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
				rank_id = (rank_id + 1) % system_config.rank_count; // try the next rank

				if (rank_id == 0)
				{
					bank_id = (bank_id + 1) % system_config.bank_count; // try the next bank

					if (bank_id == 0)
					{
						transaction_type = (transaction_type == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
						//if (transaction_type == WRITE_TRANSACTION)
						//{
						//	transaction_type = READ_TRANSACTION;
						//}
						//else
						//{
						//	transaction_type = WRITE_TRANSACTION;
						//}
					}
				}

				command *temp_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

				if(temp_c != NULL)
				{
					if(system_config.read_write_grouping == false)
					{
						return channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
					}
					else // have to follow read_write grouping considerations 
					{
						command *next_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->this_command == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION)))
						{
							return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< temp_c->addr.rank_id << "] [" << temp_c->addr.bank_id << "] but wrong type, We want [" << transaction_type << "]. Candidate command type ";
					cerr << temp_c->this_command;
					cerr << " followed by ";
					cerr << next_c->this_command;
					cerr << "count [" << channel->get_rank(rank_id)->bank[bank_id].per_bank_q.get_count() << "]" << endl;
#endif

				}

#ifdef DEBUG_FLAG_2
				cerr << "Looked in [" << rank_id << "] [" << bank_id << "] but Q empty" << endl;
#endif

			}
		}
		break;

	case BANK_ROUND_ROBIN: // keep rank id as long as possible, go round robin down a given rank
		{
			unsigned bank_id = last_c->addr.bank_id;
			unsigned rank_id = last_c->addr.rank_id;
			transaction_type_t transaction_type;
			if(last_c->this_command == RAS_COMMAND)
			{
				command *temp_c = channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

				if((temp_c != NULL) &&
					((temp_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->this_command == CAS_AND_PRECHARGE_COMMAND))){
						return channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
				}
				else
				{
					cerr << "Serious problems. RAS not followed by CAS." << endl;
					_exit(2);
				}
			}
			else if (last_c->this_command == CAS_AND_PRECHARGE_COMMAND)
			{
				transaction_type = READ_TRANSACTION;
			}
			else if (last_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND)
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
				bank_id = (bank_id + 1) % system_config.bank_count;
				if (bank_id == 0)
				{
					rank_id = (rank_id + 1) % system_config.rank_count;
					if (rank_id == 0)
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

				command *temp_c = channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

				if(temp_c != NULL)
				{	
					if(system_config.read_write_grouping == false)
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations
					{
						command *next_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(1);	/* look at the second command */

						if (((next_c->this_command == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
							((next_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION))){
								return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
						}
					}
#ifdef DEBUG_FLAG_2
					cerr << "Looked in [" << temp_c->addr.rank_id << "] [" << temp_c->addr.bank_id << "] but wrong type, We want [" << transaction_type << "] Candidate command type [" << next_c->this_command << "]" << endl;
#endif
				}
#ifdef DEBUG_FLAG_2
				cerr << "Looked in [" << rank_id << "] [" << bank_id << "] but Q empty" << endl;
#endif
			}
		}
		break;

	case WANG_RANK_HOP:	
		if(system_config.config_type == BASELINE_CONFIG)	/* baseline*/
		{
			bool candidate_found = false;

			while(candidate_found == false)
			{
				command *ptr_c = algorithm.WHCC.read(algorithm.WHCC_offset[0]);
				algorithm.WHCC_offset[0] = (algorithm.WHCC_offset[0] + 1) % algorithm.WHCC.get_count();
				unsigned rank_id = ptr_c->addr.rank_id;
				unsigned bank_id = ptr_c->addr.bank_id;
				command *candidate_c = channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

				if((candidate_c != NULL) && (candidate_c->this_command == RAS_COMMAND))
				{
					if(ptr_c->this_command == RAS_COMMAND)
					{
						if(bank_id == 0)	// see if this rank needs a R/W switch around
						{
							algorithm.transaction_type[rank_id] = channel->set_read_write_type(rank_id,system_config.bank_count);
						}

						command *next_c =  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(1);

						if(((algorithm.transaction_type[rank_id] == READ_TRANSACTION) && (next_c->this_command == CAS_AND_PRECHARGE_COMMAND)) ||
							((algorithm.transaction_type[rank_id] == WRITE_TRANSACTION) && (next_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND)))
						{
							candidate_found = true;
							return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
						}
					}
				}
				else if((candidate_c != NULL) && (candidate_c->this_command == CAS_AND_PRECHARGE_COMMAND))
				{
					if((ptr_c->this_command == CAS_COMMAND) && (algorithm.transaction_type[rank_id] == READ_TRANSACTION))
					{
						candidate_found = true;
						return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
					}
				}
				else if ((candidate_c != NULL) && (candidate_c->this_command == CAS_WRITE_AND_PRECHARGE_COMMAND))
				{
					if((ptr_c->this_command == CAS_COMMAND) && (algorithm.transaction_type[rank_id] == WRITE_TRANSACTION))
					{
						candidate_found = false;
						return  channel->get_rank(rank_id)->bank[bank_id].per_bank_q.dequeue();
					}
				}
				else
				{
#ifdef DEBUG_FLAG_1
					cerr << "some debug message" << endl;
#endif
				}
			}
		}
		break;

	case GREEDY:
		{
			command *candidate_command = NULL;

			int candidate_gap = INT_MAX;

			for (unsigned rank_id = system_config.rank_count; rank_id >= 0; --rank_id)
			{
				for (unsigned bank_id = system_config.bank_count; bank_id >= 0; --bank_id)
				{
					command *challenger_command = channel->get_rank(rank_id)->bank[bank_id].per_bank_q.read(0);

					if (challenger_command != NULL)
					{
						int challenger_gap = min_protocol_gap(chan_id,challenger_command);
						if (challenger_gap < candidate_gap || (candidate_gap == challenger_gap && challenger_command->enqueue_time < candidate_command->enqueue_time))
						{
							candidate_gap = challenger_gap;
							candidate_command = challenger_command;
						}
					}
				}
			}

			command *temp_c = channel->get_rank(candidate_command->addr.rank_id)->bank[candidate_command->addr.bank_id].per_bank_q.dequeue();

#ifdef DEBUG
			cerr << "R[" << candidate_command->addr.rank_id << "] B[" << candidate_command->addr.bank_id << "]\tWinner: " << *temp_c << "gap[" << candidate_gap << "] now[" << channel->time << "]" << endl;
#endif

			return temp_c;
		}
		break;

	default:
		{
			cerr << "This configuration and algorithm combination is not supported" << endl;
			_exit(0);
		}
		break;
	}
}