#include <iostream>
#include <iomanip>
#include <cassert>

#include "dramSystem.h"

using namespace std;
using namespace DRAMSimII;

void dramSystem::run_simulations2()
{
	for (int i = sim_parameters.get_request_count(); i > 0; --i)
	{
		transaction *input_t;

		// see if it is time for the channel to arrive, if so, put in the channel
		// queue
		// if not, either make the channels wait and move time to the point where
		// the transaction should arrive or execute commands until that time happens
		// make sure not to overshoot the time by looking at when a transaction would end
		// so that executing one more command doesn't go too far forward
		// this only happens when there is the option to move time or execute commands
		if (getNextIncomingTransaction(input_t) == SUCCESS)
		{
			// record stats
			statistics.collect_transaction_stats(input_t);

			// first try to update the channel so that it is one command past this
			// transaction's start time
			tick_t finishTime;
			while (channel[input_t->addr.chan_id].moveChannelToTime(input_t->arrival_time,&finishTime)) {;}

			// attempt to enqueue, if there is no room, move time forward until there is
			enqueueTimeShift(input_t);
		}	
		else
			// EOF reached, quit the loop
			break;
	}

	statistics.set_end_time(time);
	statistics.set_valid_trans_count(sim_parameters.get_request_count());
}

void dramSystem::run_simulations3()
{
	transaction *input_t = NULL;
	tick_t nextArrival = 0;

	for (int i = sim_parameters.get_request_count(); i > 0; )
	{
		if (!input_t)
		{
			if (getNextIncomingTransaction(input_t) != SUCCESS)
				break;
			// if the previous transaction was delayed, thus making this arrival be in the past
			// prevent new arrivals from arriving in the past
			input_t->arrival_time = max(input_t->arrival_time,channel[input_t->addr.chan_id].get_time());
		}

		// in case this transaction tried to arrive while the queue was full
		

		tick_t nearFinish = 0;
		//const void *error;

		nextArrival = min(input_t->arrival_time,nextArrival);
		// as long as transactions keep happening prior to this time
		if (moveAllChannelsToTime(nextArrival,&nearFinish))
		{
			cerr << "not right, no host transactions here" << endl;
		}

		if (nearFinish >= input_t->arrival_time) 
		{
			if (enqueue(input_t))
			{
				input_t = NULL;
				i--;
			}
			else
				// figure that the cpu <=> mch bus runs at the mostly the same speed
				input_t->arrival_time += channel[0].getTimingSpecification().t_cmd;
		}
		nextArrival = nextTick();
	}
	statistics.set_end_time(time);
	statistics.set_valid_trans_count(sim_parameters.get_request_count());
}

/// Attempt to enqueue the command
/// Return true if there was room, else false
bool dramSystem::enqueue(transaction *trans)
{
	// map the PA of this transaction to this system
	convert_address(trans->addr);

	// attempt to insert the transaction into the per-channel transaction queue
	if (!channel[trans->addr.chan_id].enqueue(trans))
	{
#ifdef M5DEBUG
		outStream << "!+T(" << channel[trans->addr.chan_id].getTransactionQueueCount() << "/" << channel[trans->addr.chan_id].getTransactionQueueDepth() << ")" << endl;
#endif
		return false;
	}
	else
	{
#ifdef M5DEBUG
		outStream << "+T(" << trans->addr.chan_id << ")[" << channel[trans->addr.chan_id].getTransactionQueueCount() << "]" << endl;
#endif
		// if the transaction was successfully enqueued, set its enqueue time
		trans->enqueueTime = channel[trans->addr.chan_id].get_time();
		return true;
	}
}

/// Move time forward to ensure that the command was successfully enqueued
void dramSystem::enqueueTimeShift(transaction* trans)
{
	const unsigned chan = trans->addr.chan_id;

	// as long 
	while (!channel[chan].enqueue(trans))
	{
		transaction *temp_t = channel[chan].get_transaction();

		if(temp_t != NULL)
		{
			// if it can't fit in the transaction queue, drain the queue
			while (channel[temp_t->addr.chan_id].transaction2commands(temp_t))
			{
				command *temp_c = channel[chan].getNextCommand();

				int min_gap = channel[chan].minProtocolGap(temp_c);

				channel[chan].executeCommand(temp_c, min_gap);

#ifdef DEBUG_COMMAND
				outStream << "[" << std::hex << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

				update_system_time(); 

				transaction *completed_t = channel[chan].get_oldest_completed();

				if(completed_t != NULL)
				{
					if (completed_t->type == AUTO_REFRESH_TRANSACTION)
					{
						completed_t->arrival_time += 7 / 8 * system_config.refresh_time;
						channel[chan].enqueueRefresh(completed_t);
					}
					else
						delete completed_t;
#ifdef DEBUG_TRANSACTION
					outStream << "CH[" << setw(2) << chan << "] " << completed_t << endl;
#endif
				}

			}
		}
		else
			break;

		trans->arrival_time = channel[chan].get_time();
	}
}

/// Moves all channels to the specified time
/// If a transaction completes, then it is returned without completing the movement
const void *dramSystem::moveAllChannelsToTime(const tick_t endTime, tick_t *transFinishTime)
{
#if M5DEBUG
	outStream << "move forward until: " << endTime << endl;
#endif
	for (vector<dramChannel>::iterator i = channel.begin(); i != channel.end(); i++)
	{
		const void *finishedTrans = i->moveChannelToTime(endTime, transFinishTime);
		if (finishedTrans)
			return finishedTrans;
	}
	update_system_time();
	return NULL;
}

/// Moves the specified channel to at least the time given
const void *dramChannel::moveChannelToTime(const tick_t endTime, tick_t *transFinishTime)
{
	bool processingTransaction = false;

	while (time < endTime)
	{
		// attempt first to move transactions out of the transactions queue and
		// convert them into commands after a fixed amount of time		
		transaction *temp_t = read_transaction();		

		if ((temp_t) && (time - temp_t->enqueueTime < timing_specification.t_buffer_delay))
		{
#ifdef M5DEBUG
			outStream << "resetting: ";
			outStream << time << " ";
			outStream << temp_t->enqueueTime << " ";
			outStream << timing_specification.t_buffer_delay << endl;
#endif
			temp_t = NULL; // not enough time has passed
			// make sure that it is known that there may be time to divide up a command before the end of the epoch
			processingTransaction = true;
		}


		// if there were no transactions left in the queue or there was not
		// enough room to split the transaction into commands
		if (!transaction2commands(temp_t))
		{
#ifdef M5DEBUG
			if (temp_t)
			outStream << "t2c fails" << temp_t << endl;
#endif
			// move time up by executing commands
			command *temp_c = readNextCommand();

			if (temp_c == NULL)
			{
				// the transaction queue and all the per bank queues are empty,
				// so just move time forward to the point where the transaction starts
				// or move time forward until the transaction is ready to be decoded
				if ((processingTransaction) && (time + timing_specification.t_buffer_delay <= endTime))
				{
					processingTransaction = false;
					// FIXME: this should move time forward until the trans can be successfully converted
					time = time + timing_specification.t_buffer_delay;
				}
				else
				{
					time = endTime;
				}
			}
			else
			{
				int min_gap = minProtocolGap(temp_c);
#ifdef M5DEBUG
				outStream << "mg: " << min_gap << endl;
#endif

				// allow system to overrun so that it may send a command
				// FIXME: will this work?
				if ((min_gap + time <= endTime) || (min_gap + time <= endTime + timing_specification.t_cmd))
				{
					temp_c = getNextCommand();

					executeCommand(temp_c, min_gap);

#ifdef DEBUG_COMMAND
					outStream << "F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

					

					transaction *completed_t = completion_q.dequeue();

					if (completed_t != NULL)
					{
#ifdef DEBUG_TRANSACTION
						outStream << "CH[" << setw(2) << channelID << "] " << completed_t << endl;
#endif
						// reuse the refresh transactions
						if (completed_t->type == AUTO_REFRESH_TRANSACTION)
						{
							completed_t->arrival_time += 7 / 8 * system_config->refresh_time;
							//channel[completed_t->addr.chan_id].operator[](completed_t->addr.rank_id).enqueueRefresh(completed_t);
							enqueueRefresh(completed_t);
						}
						else // return what was pointed to
						{
							const void *origTrans = completed_t->originalTransaction;


#ifdef M5
							if (!completed_t->originalTransaction)
								outStream << "transaction completed, not REFRESH, no orig trans" << endl;
#endif
							*transFinishTime = completed_t->completion_time;

							delete completed_t;							

							return origTrans;
						}
					}
				}
				else
				{
					time = endTime;
				}
			}
		}
		else // successfully converted to commands, dequeue
		{
			//channel[chan].set_time(min(endTime,channel[chan].get_time() + timing_specification.t_buffer_delay));
			//update_system_time(); 

			// actually remove it from the queue now
			transaction *completedTransaction = get_transaction();
			assert(temp_t == completedTransaction);
#ifdef DEBUG_TRANSACTION
			outStream << "T->C [" << time << "] Q[" << getTransactionQueueCount() << "]" << endl;
#endif
		}
	}
	assert(time == endTime);
	*transFinishTime = endTime;
#ifdef M5DEBUG
	outStream << "ch[" << channelID << "] @ " << std::dec << time << endl;
#endif
	return NULL;
}

input_status_t dramSystem::waitForTransactionToFinish(transaction *trans)
{
	const int chan = trans->addr.chan_id;

	while (true)
	{
		// attempt first to move transactions out of the transactions queue and
		// convert them into commands
		transaction *temp_t = channel[chan].read_transaction();

		// if there were no transactions left in the queue or there was not
		// enough room to split the transaction into commands
		if (!channel[temp_t->addr.chan_id].transaction2commands(temp_t))
		{
			// move time up by executing commands
			command *temp_c = channel[chan].getNextCommand();

			if (temp_c == NULL)
			{
				// the transaction queue and all the per bank queues are empty,
				// so just move time forward to the point where the transaction starts
				//channel[chan].set_time(endTime);
				if (temp_t == NULL)
					return FAILURE;
			}
			else
			{
				int min_gap = channel[chan].minProtocolGap(temp_c);

				channel[chan].executeCommand(temp_c, min_gap);

				update_system_time(); 

				transaction *completed_t = channel[chan].get_oldest_completed();

				if (completed_t != NULL)
				{
					// reuse the refresh transactions
					if (completed_t->type == AUTO_REFRESH_TRANSACTION)
					{
						completed_t->arrival_time += 7 / 8 * system_config.refresh_time;
						//channel[completed_t->addr.chan_id].operator[](completed_t->addr.rank_id).enqueueRefresh(completed_t);
						channel[completed_t->addr.chan_id].enqueueRefresh(completed_t);
					}
					else
						delete completed_t;

#ifdef DEBUG_TRANSACTION
					outStream << "CH[" << setw(2) << chan << "] " << completed_t << endl;
#endif					
				}
#ifdef DEBUG_COMMAND
				outStream << "F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif
				// if the CAS command was just executed, then this command is effectively done
				if (temp_c->getHost() == trans)
					return SUCCESS;
			}
		}
		else // successfully converted to commands, dequeue
		{
			transaction *completedTrans = channel[chan].get_transaction();
			assert(temp_t == completedTrans);
		}
	}
}

void dramSystem::run_simulations()
{
	for (int i = sim_parameters.get_request_count(); i > 0; --i)
	{
		transaction* input_t;
		if (getNextIncomingTransaction(input_t) == SUCCESS)
		{
			statistics.collect_transaction_stats(input_t);

			while (!channel[input_t->addr.chan_id].enqueue(input_t))
			{
				// tried to stuff req in channel queue, stalled, so try to drain channel queue first 
				// and drain it completely 
				// unfortunately, we may have to drain another channel first. 
				int oldest_chan_id = findOldestChannel();
				transaction *temp_t = channel[oldest_chan_id].get_transaction();

#ifdef DEBUG_TRANSACTION
				outStream << "CH[" << setw(2) << oldest_chan_id << "] " << temp_t << endl;
#endif

				if(temp_t != NULL)
				{
					//rank_id = temp_t->addr.rank_id;
					//bank_id = temp_t->addr.bank_id;
					//bank_q = (((channel[oldest_chan_id]).rank[temp_t->addr.rank_id]).bank[bank_id]).per_bank_q;
					// if it can't fit in the transaction queue, drain the queue
					while (!channel[temp_t->addr.chan_id].transaction2commands(temp_t))
					{
						command *temp_c = channel[oldest_chan_id].getNextCommand();
						int min_gap = channel[input_t->addr.chan_id].minProtocolGap(temp_c);

#ifdef DEBUG_COMMAND
						outStream << "[" << setbase(10) << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

						channel[temp_c->getAddress().chan_id].executeCommand(temp_c, min_gap);
						update_system_time(); 
						transaction *completed_t = channel[oldest_chan_id].get_oldest_completed();
						if(completed_t != NULL)
							delete completed_t;
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
