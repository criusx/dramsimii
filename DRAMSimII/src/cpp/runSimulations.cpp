#include <iostream>
#include <iomanip>
#include <cassert>

#include "dramSystem.h"

using namespace std;

void dramSystem::run_simulations2()
{
	//bool EOF_reached = false;

	for(int i = sim_parameters.get_request_count(); i > 0; --i)
	{
		transaction *input_t;

		// see if it is time for the channel to arrive, if so, put in the channel
		// queue
		// if not, either make the channels wait and move time to the point where
		// the transaction should arrive or execute commands until that time happens
		// make sure not to overshoot the time by looking at when a transaction would end
		// so that executing one more command doesn't go too far forward
		// this only happens when there is the option to move time or execute commands
		if (get_next_input_transaction(input_t) == SUCCESS)
		{
			// record stats
			statistics.collect_transaction_stats(input_t);

			int chan = input_t->addr.chan_id;

			// first try to update the channel so that it is one command past this
			// transaction's start time
			while (channel[chan].get_time() < input_t->arrival_time)
			{
				// attempt first to move transactions out of the transactions queue and
				// convert them into commands
				transaction *temp_t = channel[chan].read_transaction();

				if ((temp_t == NULL) || (transaction2commands(temp_t) != SUCCESS))
				{
					// move time up by executing commands
					command *temp_c = getNextCommand(chan);

					if (temp_c != NULL)
					{
						int min_gap = minProtocolGap(input_t->addr.chan_id, temp_c);

						executeCommand(temp_c, min_gap);

#ifdef DEBUG_COMMAND
						cerr << "[" << std::hex << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

						update_system_time(); 

						transaction *completed_t = channel[chan].get_oldest_completed();

						if(completed_t != NULL)
						{
							if (completed_t->type == AUTO_REFRESH_TRANSACTION)
							{
								completed_t->arrival_time += 7 / 8 * system_config.refresh_time;
								channel[completed_t->addr.rank_id].enqueueRefresh(completed_t);
							}
							else
								free_transaction_pool.release_item(completed_t);
#ifdef DEBUG_TRANSACTION
							cerr << "CH[" << setw(2) << i << "] " << completed_t << endl;
#endif
						}
					}
					else
					{
						// the transaction queue and all the per bank queues are empty,
						// so just move time forward to the point where the transaction starts
						channel[chan].set_time(input_t->arrival_time);
					}
				}
				else // successfully converted to commands, dequeue
				{
					assert(temp_t == channel[chan].get_transaction());
				}
			}

			// attempt to enqueue, if there is no room, move time forward until there is
			while (channel[input_t->addr.chan_id].enqueue(input_t) == FAILURE)
			{
				transaction *temp_t = channel[chan].get_transaction();

				if(temp_t != NULL)
				{
					// if it can't fit in the transaction queue, drain the queue
					while (transaction2commands(temp_t) != SUCCESS)
					{
						command *temp_c = getNextCommand(chan);

						int min_gap = minProtocolGap(input_t->addr.chan_id, temp_c);

						executeCommand(temp_c, min_gap);

#ifdef DEBUG_COMMAND
						cerr << "[" << std::hex << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

						update_system_time(); 

						transaction *completed_t = channel[chan].get_oldest_completed();

						if(completed_t != NULL)
						{
							if (completed_t->type == AUTO_REFRESH_TRANSACTION)
							{
								completed_t->arrival_time += 7 / 8 * system_config.refresh_time;
								channel[completed_t->addr.rank_id].enqueueRefresh(completed_t);
							}
							else
								free_transaction_pool.release_item(completed_t);
#ifdef DEBUG_TRANSACTION
							cerr << "CH[" << setw(2) << i << "] " << completed_t << endl;
#endif
						}

					}
				}
				else
					break;

				input_t->arrival_time = channel[chan].get_time();
			}
		}	
		else
			// EOF reached, quit the loop
			break;
	}

	statistics.set_end_time(time);
	statistics.set_valid_trans_count(sim_parameters.get_request_count());
}


void dramSystem::run_simulations()
{
	for (int i = sim_parameters.get_request_count(); i > 0; --i)
	{
		transaction 	*input_t;
		if (get_next_input_transaction(input_t) == SUCCESS)
		{
			statistics.collect_transaction_stats(input_t);

			while (channel[input_t->addr.chan_id].enqueue(input_t) == FAILURE)
			{
				// tried to stuff req in channel queue, stalled, so try to drain channel queue first 
				// and drain it completely 
				// unfortunately, we may have to drain another channel first. 
				int oldest_chan_id = find_oldest_channel();
				transaction *temp_t = channel[oldest_chan_id].get_transaction();

#ifdef DEBUG_TRANSACTION
				cerr << "CH[" << setw(2) << oldest_chan_id << "] " << temp_t << endl;
#endif

				if(temp_t != NULL)
				{
					//rank_id = temp_t->addr.rank_id;
					//bank_id = temp_t->addr.bank_id;
					//bank_q = (((channel[oldest_chan_id]).rank[temp_t->addr.rank_id]).bank[bank_id]).per_bank_q;
					// if it can't fit in the transaction queue, drain the queue
					while(transaction2commands(temp_t) != SUCCESS)
					{
						command *temp_c = getNextCommand(oldest_chan_id);
						int min_gap = minProtocolGap(input_t->addr.chan_id, temp_c);

#ifdef DEBUG_COMMAND
						cerr << "[" << setbase(10) << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

						executeCommand(temp_c, min_gap);
						update_system_time(); 
						transaction *completed_t = channel[oldest_chan_id].get_oldest_completed();
						if(completed_t != NULL)
							free_transaction_pool.release_item(completed_t);
					}
				}
				/* randomness check
				for(rank_id = 0; rank_id < config->rank_count;rank_id++){
				for(bank_id = 0; bank_id < config->bank_count; bank_id++){
				fprintf(stderr,"[%d|%d|%d] ",
				(((channel[0]).rank[rank_id]).bank[bank_id]).cas_count,
				(((channel[0]).rank[rank_id]).bank[bank_id]).casw_count,
				(((channel[0]).rank[rank_id]).bank[bank_id]).cas_count +
				(((channel[0]).rank[rank_id]).bank[bank_id]).casw_count);
				}
				fprintf(stderr,"X");
				}
				fprintf(stderr,"\n");
				*/
			}
		}
		else
			break;
	}

	statistics.set_end_time(time);
	statistics.set_valid_trans_count(sim_parameters.get_request_count());
}
