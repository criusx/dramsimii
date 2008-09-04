#include <iostream>
#include <iomanip>
#include <cassert>

#include "System.h"

using std::max;
using std::cerr;
using std::endl;
using namespace DRAMSimII;

//////////////////////////////////////////////////////////////////////
/// @brief automatically runs the simulations according to the set parameters
/// @details runs either until the trace file runs out or the request count reaches zero\n
/// will print power and general stats at regular intervals\n
/// pulls data from either a trace file or generates random requests according to parameters
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void System::runSimulations(const unsigned requestCount)
{
	Transaction *inputTransaction = NULL;

	tick newTime;

	for (unsigned i = requestCount > 0 ? requestCount : simParameters.getRequestCount(); i > 0; )
	{		
		if (!inputTransaction)
		{
			inputTransaction = inputStream.getNextIncomingTransaction();

			if (!inputTransaction)
				break;

			newTime = channel[inputTransaction->getAddresses().getChannel()].getTime();
			// if the previous transaction was delayed, thus making this arrival be in the past
			// prevent new arrivals from arriving in the past
			//inputTransaction->setEnqueueTime(max(inputTransaction->getEnqueueTime(),channel[inputTransaction->getAddresses().channel].getTime()));
		}

		// in case this transaction tried to arrive while the queue was full


		tick nearFinish = 0;

		// as long as transactions keep happening prior to this time
		//if (moveAllChannelsToTime(min(inputTransaction->getEnqueueTime(),nextTick()),nearFinish))
		if (moveAllChannelsToTime(max(newTime, inputTransaction->getArrivalTime()),nearFinish) < UINT_MAX)
		{
			cerr << "not right, no host transactions here" << endl;
		}

		// attempt to enqueue external transactions
		// as internal transactions (REFRESH) are enqueued automatically
		if (time >= inputTransaction->getArrivalTime()) 
		{
			if (enqueue(inputTransaction))
			{
				inputTransaction = NULL;
				i--;
			}
			else
			{
				// figure that the cpu <=> mch bus runs at the mostly the same speed
				//inputTransaction->setEnqueueTime(inputTransaction->getEnqueueTime() + channel[0].getTimingSpecification().tCMD());
				newTime = max(channel[inputTransaction->getAddresses().getChannel()].nextTick(), newTime);
			}
		}
	}
}


#if 0
/// Move time forward to ensure that the command was successfully enqueued
void System::enqueueTimeShift(Transaction* trans)
{
	const unsigned chan = trans->getAddresses().channel;

	// as long 
	while (!channel[chan].enqueue(trans))
	{
		Transaction *temp_t = channel[chan].getTransaction();

		if(temp_t != NULL)
		{
			// if it can't fit in the transaction queue, drain the queue
			while (channel[temp_t->getAddresses().channel].transaction2commands(temp_t))
			{
				Command *temp_c = channel[chan].getNextCommand();

				int min_gap = channel[chan].minProtocolGap(temp_c);

				channel[chan].executeCommand(temp_c, min_gap);

				statistics.collectCommandStats(temp_c);
#ifdef DEBUG_COMMAND				
				timingOutStream << "[" << std::hex << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

				updateSystemTime(); 

				Transaction *completed_t = channel[chan].getOldestCompletedTransaction();

				if(completed_t != NULL)
				{
					/*if (completed_t->getType() == AUTO_REFRESH_TRANSACTION)
					{
					completed_t->setEnqueueTime(completed_t->getEnqueueTime() + systemConfig.getRefreshTime());
					channel[chan].enqueueRefresh(completed_t);
					}
					else*/
					delete completed_t;

					statistics.collectTransactionStats(temp_t);
#ifdef DEBUG_TRANSACTION					
					timingOutStream << "CH[" << setw(2) << chan << "] " << completed_t << endl;
#endif
				}

			}
		}
		else
			break;

		trans->setEnqueueTime(channel[chan].getTime());
	}
}

input_status_t dramSystem::waitForTransactionToFinish(transaction *trans)
{
	const int chan = trans->getAddresses().channel;

	while (true)
	{
		// attempt first to move transactions out of the transactions queue and
		// convert them into commands
		// FIXME: no longer returns transactions
		const transaction *temp_t = channel[chan].readTransaction(true);

		// if there were no transactions left in the queue or there was not
		// enough room to split the transaction into commands
		if (!channel[temp_t->getAddresses().channel].checkForAvailableCommandSlots(temp_t))
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

				updateSystemTime(); 

				transaction *completed_t = channel[chan].getOldestCompletedTransaction();

				if (completed_t != NULL)
				{
					// reuse the refresh transactions
					//if (completed_t->getType() == AUTO_REFRESH_TRANSACTION)
					//{
					//	completed_t->setEnqueueTime(completed_t->getEnqueueTime() + systemConfig.getRefreshTime());
					//	//channel[completed_t->addr.chan_id].operator[](completed_t->addr.rank_id).enqueueRefresh(completed_t);
					//	channel[completed_t->getAddresses().chan_id].enqueueRefresh(completed_t);
					//}
					//else
					delete completed_t;

#ifdef DEBUG_TRANSACTION
					timingOutStream << "CH[" << setw(2) << chan << "] " << completed_t << endl;
#endif					
				}
#ifdef DEBUG_COMMAND
				timingOutStream << "F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif
				// if the CAS command was just executed, then this command is effectively done
				if (temp_c->getHost() == trans)
					return SUCCESS;
			}
		}
		else // successfully converted to commands, dequeue
		{
			transaction *completedTrans = channel[chan].getTransaction();
			assert(temp_t == completedTrans);
			channel[chan].transaction2commands(completedTrans);
		}
	}
}

void dramSystem::runSimulations()
{
	for (int i = simParameters.get_request_count(); i > 0; --i)
	{
		if (transaction *input_t = getNextIncomingTransaction())
		{
			statistics.collectTransactionStats(input_t);

			while (!channel[input_t->getAddresses().channel].enqueue(input_t))
			{
				// tried to stuff req in channel queue, stalled, so try to drain channel queue first 
				// and drain it completely 
				// unfortunately, we may have to drain another channel first. 
				int oldest_chan_id = findOldestChannel();
				transaction *temp_t = channel[oldest_chan_id].getTransaction();

#ifdef DEBUG_TRANSACTION
				timingOutStream << "CH[" << setw(2) << oldest_chan_id << "] " << temp_t << endl;
#endif

				if(temp_t != NULL)
				{
					//rank_id = temp_t->addr.rank_id;
					//bank_id = temp_t->addr.bank_id;
					//bank_q = (((channel[oldest_chan_id]).rank[temp_t->addr.rank_id]).bank[bank_id]).per_bank_q;
					// if it can't fit in the transaction queue, drain the queue
					while (!channel[temp_t->getAddresses().channel].transaction2commands(temp_t))
					{
						command *temp_c = channel[oldest_chan_id].getNextCommand();
						int min_gap = channel[input_t->getAddresses().channel].minProtocolGap(temp_c);

#ifdef DEBUG_COMMAND
						timingOutStream << "[" << setbase(10) << setw(8) << time << "] [" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif

						channel[temp_c->getAddress().channel].executeCommand(temp_c, min_gap);
						updateSystemTime(); 
						transaction *completed_t = channel[oldest_chan_id].getOldestCompletedTransaction();
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
}
#endif
