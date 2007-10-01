#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>
#include <vector>

#include "dramChannel.h"

using namespace std;
using namespace DRAMSimII;

dramChannel::dramChannel(const dramSettings *settings):
time(0),
rank(settings->rankCount, rank_c(settings)),
refreshRowIndex(0),
lastRefreshTime(0),
lastRankID(0),
timing_specification(settings),
transactionQueue(settings->transactionQueueDepth),
refreshQueue(settings->rowCount * settings->rankCount,true),
historyQueue(settings->historyQueueDepth),
completionQueue(settings->completionQueueDepth),
systemConfig(NULL),
powerModel(settings),
algorithm(settings)
{
	for (unsigned i = 0; i < settings->rankCount; i++)
	{
		rank[i].setRankID(i);
	}
}

dramChannel::dramChannel(const dramChannel &dc):
time(dc.time),
rank(dc.rank),
refreshRowIndex(dc.refreshRowIndex),
lastRefreshTime(dc.lastRefreshTime),
lastRankID(dc.lastRankID),
timing_specification(dc.timing_specification),
transactionQueue(dc.transactionQueue),
refreshQueue(dc.refreshQueue),
historyQueue(dc.historyQueue),
completionQueue(dc.completionQueue),
systemConfig(NULL),
powerModel(dc.powerModel),
algorithm(dc.algorithm)
{
}



/// Moves the specified channel to at least the time given
const void *dramChannel::moveChannelToTime(const tick_t endTime, tick_t *transFinishTime)
{
	while (time < endTime)
	{
		// attempt first to move transactions out of the transactions queue and
		// convert them into commands after a fixed amount of time		
		transaction *temp_t = readTransaction();


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
				if ((transactionQueue.size() > 0) && (time + timing_specification.t_buffer_delay <= endTime))
				{
					tick_t oldTime = time;
					time = timing_specification.t_buffer_delay + transactionQueue.front()->getEnqueueTime();
					assert(oldTime < time);
				}
				// no transactions to convert, no commands to issue, just go forward
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

					statistics->collectCommandStats(temp_c);	
#ifdef DEBUG_COMMAND
					outStream << "F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << min_gap << "] " << *temp_c << endl;
#endif


					// only get completed commands if they have finished TODO:
					transaction *completed_t = completionQueue.pop();

					if (completed_t != NULL)
					{
						statistics->collectTransactionStats(completed_t);
#ifdef DEBUG_TRANSACTION
						outStream << "CH[" << setw(2) << channelID << "] " << completed_t << endl;
#endif
						// reuse the refresh transactions
						if (completed_t->getType() == AUTO_REFRESH_TRANSACTION)
						{
							completed_t->setArrivalTime(completed_t->getArrivalTime() + systemConfig->getRefreshTime());

							enqueueRefresh(completed_t);
						}
						else // return what was pointed to
						{
							const void *origTrans = completed_t->getOriginalTransaction();


#ifdef M5
							if (!completed_t->originalTransaction)
								outStream << "transaction completed, not REFRESH, no orig trans" << endl;
#endif
							*transFinishTime = completed_t->getCompletionTime();

							
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
			transaction *completedTransaction = getTransaction();
			assert(temp_t == completedTransaction);
#ifdef DEBUG_TRANSACTION
			outStream << "T->C [" << time << "] Q[" << getTransactionQueueCount() << "]" << endl;
#endif
		}
	}
	assert(time <= endTime + timing_specification.t_cmd);
	*transFinishTime = endTime;
#ifdef M5DEBUG
	outStream << "ch[" << channelID << "] @ " << std::dec << time << endl;
#endif
	return NULL;
}


void dramChannel::initRefreshQueue(const unsigned rowCount,
								   const unsigned refreshTime,
								   const unsigned channel)
{
	unsigned step = refreshTime;
	step /= rowCount;
	step /= rank.size();
	int count = 0;

	for (int i = rowCount - 1; i >= 0; i--)
		for (int j = rank.size() - 1; j >= 0; j--)
		{
			refreshQueue.read(count)->setArrivalTime(count * step);
			refreshQueue.read(count)->setEnqueueTime(count * step);
			refreshQueue.read(count)->setType(AUTO_REFRESH_TRANSACTION);
			refreshQueue.read(count)->getAddresses().rank_id = j;
			refreshQueue.read(count)->getAddresses().row_id = i;
			refreshQueue.read(count)->getAddresses().chan_id = channel;
			count++;
		}

}

void dramChannel::record_command(command *latest_command)
{
	while (historyQueue.push(latest_command) == false)
	{
		delete historyQueue.pop();
	}
}


enum transaction_type_t	dramChannel::set_read_write_type(const int rank_id,const int bank_count) const
{
	int read_count = 0;
	int write_count = 0;
	int empty_count = 0;

	for(int i = 0; i < bank_count; ++i)
	{
		command *temp_c = rank[rank_id].bank[i].per_bank_q.read(1);

		if(temp_c != NULL)
		{
			if (temp_c->getCommandType() == CAS_AND_PRECHARGE_COMMAND)
			{
				read_count++;
			}
			else
			{
				write_count++;
			}
		}
		else
		{
			empty_count++;
		}
	}
#ifdef DEBUG_FLAG
	cerr << "Rank[" << rank_id << "] Read[" << read_count << "] Write[" << write_count << "] Empty[" << empty_count << "]" << endl;
#endif

	if (read_count >= write_count)
		return READ_TRANSACTION;
	else
		return WRITE_TRANSACTION;
}

// calculate the power consumed by all channels during the last epoch
void dramChannel::doPowerCalculation()
{	
	for (std::vector<rank_c>::iterator k = rank.begin(); k != rank.end(); k++)
	{
		tick_t totalRAS = 1;
		for (std::vector<bank_c>::iterator l = k->bank.begin(); l != k->bank.end(); l++)
		{
			// Psys(ACT)
			totalRAS += (l->RASCount - l->previousRASCount);
			l->previousRASCount = l->RASCount;
		}
		//tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS * powerModel.tBurst / 2;
		tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS;
		cerr << "Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(3) << powerModel.PdsACT * powerModel.tRC / tRRDsch * (powerModel.VDD / powerModel.VDDmax) * (powerModel.VDD / powerModel.VDDmax) <<
			"(" << totalRAS << ") tRRDsch(" << tRRDsch / systemConfig->Frequency() / 1.0E-9 << "ns) lastCalc[" << powerModel.lastCalculation << "] time[" << 
			time << "]" << endl;		
	}
	powerModel.lastCalculation = time;
}

transaction *dramChannel::readTransaction() const
{
	transaction *temp_t = transactionQueue.front(); 
	transaction *refresh_t = refreshQueue.front();

	if (temp_t->getArrivalTime() < refresh_t->getArrivalTime() || systemConfig->getRefreshPolicy() == NO_REFRESH)
	{
		if ((temp_t) && (time - temp_t->getEnqueueTime() < timing_specification.t_buffer_delay))
		{
#ifdef M5DEBUG
			outStream << "resetting: ";
			outStream << time << " ";
			outStream << temp_t->getEnqueueTime() << " ";
			outStream << timing_specification.t_buffer_delay << endl;
#endif
			temp_t = NULL; // not enough time has passed		
		}
		return temp_t;
	}
	else
	{
		return refresh_t;
	}
}

// get the next transaction, whether a refresh transaction or a normal R/W transaction
transaction *dramChannel::getTransaction()
{
	if ((refreshQueue.front()->getEnqueueTime() < transactionQueue.front()->getEnqueueTime()) && (systemConfig->getRefreshPolicy() != NO_REFRESH))
	{
		return refreshQueue.pop();
	}
	else
	{
		return transactionQueue.pop();
	}
}

dramChannel& dramChannel::operator =(const DRAMSimII::dramChannel &rs)
{
	if (this == &rs)
	{
		return *this;
	}
	time = rs.time;
	rank = rs.rank;
	refreshRowIndex = rs.refreshRowIndex;
	lastRankID = rs.lastRankID;
	timing_specification = rs.timing_specification;
	transactionQueue = rs.transactionQueue;
	refreshQueue = rs.refreshQueue;
	historyQueue = rs.historyQueue;
	completionQueue = rs.completionQueue;
	systemConfig = new dramSystemConfiguration(rs.systemConfig);
	powerModel = rs.powerModel;
	algorithm = rs.algorithm;
	return *this;
}
