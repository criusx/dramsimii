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
//refreshRowIndex(0),
lastRefreshTime(0),
lastRankID(0),
timing_specification(settings),
transactionQueue(settings->transactionQueueDepth),
//refreshQueue(settings->rowCount * settings->rankCount,true),
refreshCounter(NULL),
historyQueue(settings->historyQueueDepth),
completionQueue(settings->completionQueueDepth),
systemConfig(NULL),
powerModel(settings),
algorithm(settings)
{
	// assign an id to each channel (normally done with commands)
	for (unsigned i = 0; i < settings->rankCount; i++)
	{
		rank[i].setRankID(i);
	}

	// initialize the refresh counters per rank
	if (settings->refreshPolicy != NO_REFRESH)
	{
		refreshCounter = new transaction *[settings->rankCount];

		// stagger the times that each rank will be refreshed so they don't all arrive in a burst
		unsigned step = settings->tREFI / settings->rankCount;

		for (unsigned j = 0; j < settings->rankCount; ++j)
		{
			transaction *newTrans = new transaction();
			newTrans->setType(AUTO_REFRESH_TRANSACTION);
			newTrans->getAddresses().rank = j;
			newTrans->setEnqueueTime(j * step);
			refreshCounter[j] = newTrans;

			//transaction *currentRefreshTrans = refreshQueue.pop();
			//currentRefreshTrans->setEnqueueTime((i + 1) * step);
			//assert(currentRefreshTrans->getType() == CONTROL_TRANSACTION);
			//currentRefreshTrans->setType(AUTO_REFRESH_TRANSACTION);
			//currentRefreshTrans->getAddresses().rank_id = j;
			//refreshQueue.push(currentRefreshTrans);
		}		
	}
}

dramChannel::dramChannel(const dramChannel &dc):
time(dc.time),
rank(dc.rank),
//refreshRowIndex(dc.refreshRowIndex),
lastRefreshTime(dc.lastRefreshTime),
lastRankID(dc.lastRankID),
timing_specification(dc.timing_specification),
transactionQueue(dc.transactionQueue),
//refreshQueue(dc.refreshQueue),
refreshCounter(dc.refreshCounter),
historyQueue(dc.historyQueue),
completionQueue(dc.completionQueue),
systemConfig(dc.systemConfig),
powerModel(dc.powerModel),
algorithm(dc.algorithm)
{}



/// Moves the specified channel to at least the time given
const void *dramChannel::moveChannelToTime(const tick_t endTime, tick_t *transFinishTime)
{
	while (time < endTime)
	{
		// attempt first to move transactions out of the transactions queue and
		// convert them into commands after a fixed amount of time		
		const transaction *temp_t = readTransaction();


		// if there were no transactions left in the queue or there was not
		// enough room to split the transaction into commands
		if (!checkForAvailableCommandSlots(temp_t))
		{
#ifdef M5DEBUG
			if (temp_t)
				timingOutStream << "!T2C " << temp_t << endl;
#endif
			// move time up by executing commands
			const command *temp_c = readNextCommand();

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
				timingOutStream << "mg: " << min_gap << endl;
#endif

				// allow system to overrun so that it may send a command
				// FIXME: will this work?
				if ((min_gap + time <= endTime) || (min_gap + time <= endTime + timing_specification.t_cmd))
				{
					command *temp_com = getNextCommand();

					executeCommand(temp_com, min_gap);

					statistics->collectCommandStats(temp_com);	
#ifdef DEBUG_COMMAND
					timingOutStream << "C F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << min_gap << "] " << *temp_com << endl;
#endif


					// only get completed commands if they have finished TODO:
					transaction *completed_t = completionQueue.pop();

					if (completed_t != NULL)
					{
						statistics->collectTransactionStats(completed_t);
#ifdef DEBUG_TRANSACTION
						timingOutStream << "T CH[" << setw(2) << channelID << "] " << completed_t << endl;
#endif
						// reuse the refresh transactions
						if (completed_t->getType() == AUTO_REFRESH_TRANSACTION)
						{
							completed_t->setEnqueueTime(completed_t->getEnqueueTime() + systemConfig->getRefreshTime());

							assert(systemConfig->getRefreshPolicy() != NO_REFRESH);
							//refreshQueue.push(completed_t);
							delete completed_t;

							return NULL;
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
			// then break into commands
			assert(completedTransaction == temp_t);
			bool t2cResult = transaction2commands(completedTransaction);
			assert(t2cResult == true);
			// since reading vs dequeuing should yield the same result
			assert(temp_t == completedTransaction);
#ifdef DEBUG_TRANSACTION
			timingOutStream << "T->C [" << time << "] Q[" << getTransactionQueueCount() << "]" << temp_t << endl;
#endif
		}
	}
	assert(time <= endTime + timing_specification.t_cmd);
	*transFinishTime = endTime;
#ifdef M5DEBUG
	timingOutStream << "ch[" << channelID << "] @ " << std::dec << time << endl;
#endif
	return NULL;
}

void dramChannel::recordCommand(command *latest_command)
{
	while (historyQueue.push(latest_command) == false)
	{
		delete historyQueue.pop();
	}
}

bool dramChannel::enqueue(transaction *in)
{
	if (systemConfig->getTransactionOrderingAlgorithm() == STRICT)
		return transactionQueue.push(in);	
	// try to insert reads and fetches before writes
	// TODO: finish this
	else
	{
		exit(-1); // TODO
		assert(systemConfig->getTransactionOrderingAlgorithm() == RIFF);
	}
}


enum transaction_type_t	dramChannel::setReadWriteType(const int rank_id,const int bank_count) const
{
	int read_count = 0;
	int write_count = 0;
	int empty_count = 0;

	for(int i = 0; i < bank_count; ++i)
	{
		command *temp_c = rank[rank_id].bank[i].perBankQueue.read(1);

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
		cerr << "ch[" << channelID << "] %pre[" << k->prechargeTime / (time - powerModel.lastCalculation) * 100 << "]" << endl;
		k->prechargeTime = 0;
		// FIXME: assumes CKE is always high, so (1 - CKE_LOW_PRE%) = 1
		powerOutStream << "Psys(ACT_STBY) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(3) << 
			(powerModel.VDD / powerModel.VDDmax) * (powerModel.VDD / powerModel.VDDmax) * powerModel.frequency / 
			powerModel.frequencySpec * powerModel.IDD3N * powerModel.VDDmax * (1 - (k->prechargeTime / (time - powerModel.lastCalculation))) << endl; 
		//tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS * powerModel.tBurst / 2;
		tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS;
		powerOutStream << "Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(3) << powerModel.PdsACT * powerModel.tRC / tRRDsch * (powerModel.VDD / powerModel.VDDmax) * (powerModel.VDD / powerModel.VDDmax) <<
			"(" << totalRAS << ") tRRDsch(" << tRRDsch / systemConfig->Frequency() / 1.0E-9 << "ns) lastCalc[" << powerModel.lastCalculation << "] time[" << 
			time << "]" << endl;
		boost::iostreams::flush(powerOutStream);
	}
	powerModel.lastCalculation = time;
}

transaction *dramChannel::getRefresh()
{
	assert(rank.size() >= 1);

	transaction *earliestTransaction = refreshCounter[0];

	unsigned earliestRank = 0;

	for (unsigned i = 1; i < rank.size(); ++i)
	{
		assert(refreshCounter[i]);
		if (refreshCounter[i]->getEnqueueTime() < earliestTransaction->getEnqueueTime())
		{
			earliestTransaction = refreshCounter[i];
			earliestRank = i;
		}
	}

	refreshCounter[earliestRank] = new transaction();
	refreshCounter[earliestRank]->setEnqueueTime(earliestTransaction->getEnqueueTime() + timing_specification.t_refi);
	refreshCounter[earliestRank]->setType(AUTO_REFRESH_TRANSACTION);
	refreshCounter[earliestRank]->getAddresses().rank = earliestRank;

	return earliestTransaction;
}


const transaction *dramChannel::readRefresh() const
{
	assert(rank.size() >= 1);
	transaction *earliestTransaction = refreshCounter[0];
	for (unsigned i = 1; i < rank.size(); ++i)
	{
		assert(refreshCounter[i]);
		if (refreshCounter[i]->getEnqueueTime() < earliestTransaction->getEnqueueTime())
		{
			earliestTransaction = refreshCounter[i];
		}
	}
	return earliestTransaction;
}


// read the next available transaction for this channel without actually removing it from the queue
const transaction *dramChannel::readTransaction() const
{
	const transaction *tempTrans = transactionQueue.front(); 

	if (systemConfig->getRefreshPolicy() == NO_REFRESH)
	{
		if ((tempTrans) && (time - tempTrans->getEnqueueTime() < timing_specification.t_buffer_delay))
		{
#ifdef M5DEBUG
			timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timing_specification.t_buffer_delay << endl;			
#endif
			return NULL; // not enough time has passed		
		}
		return tempTrans;
	}
	else
	{
		const transaction *refreshTrans = readRefresh();
		assert(refreshTrans != NULL);

		// give an advantage to normal transactions, but prevent starvation for refresh operations
		if (tempTrans && (tempTrans->getEnqueueTime() < refreshTrans->getEnqueueTime() + systemConfig->getSeniorityAgeLimit()))
		{
			if (time - tempTrans->getEnqueueTime() < timing_specification.t_buffer_delay)
			{
#ifdef M5DEBUG
				timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timing_specification.t_buffer_delay << endl;			
#endif
				// if this refresh command has arrived
				if (refreshTrans->getEnqueueTime() < time) 
					return refreshTrans;
				else
					return NULL; // not enough time has passed and the newest refresh transaction hasn't arrived yet
			}
			return tempTrans;
		}
		else if (refreshTrans->getEnqueueTime() < time)
		{
			return refreshTrans;
		}
		else
		{
			return NULL;
		}
	}
}

// get the next transaction, whether a refresh transaction or a normal R/W transaction
transaction *dramChannel::getTransaction()
{
	transaction *tempTrans = transactionQueue.front(); 	

	if (systemConfig->getRefreshPolicy() == NO_REFRESH)
	{
		if ((tempTrans) && (time - tempTrans->getEnqueueTime() < timing_specification.t_buffer_delay))
		{
#ifdef M5DEBUG
			timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timing_specification.t_buffer_delay << endl;			
#endif
			return NULL; // not enough time has passed		
		}
		return transactionQueue.pop();
	}
	else
	{
		const transaction *refreshTrans = readRefresh();

		if (tempTrans && (tempTrans->getEnqueueTime() < refreshTrans->getEnqueueTime() + systemConfig->getSeniorityAgeLimit()))
		{
			if (time - tempTrans->getEnqueueTime() < timing_specification.t_buffer_delay)
			{
#ifdef M5DEBUG
				timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timing_specification.t_buffer_delay << endl;			
#endif
				// if this refresh command has arrived
				if (refreshTrans->getEnqueueTime() < time) 
					return getRefresh();
				else
					return NULL; // not enough time has passed and the newest refresh transaction hasn't arrived yet
			}
			return transactionQueue.pop();
		}
		else if (refreshTrans->getEnqueueTime() < time)
		{
			return getRefresh();
		}
		else
		{
			return NULL;
		}
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
	lastRankID = rs.lastRankID;
	timing_specification = rs.timing_specification;
	transactionQueue = rs.transactionQueue;
	refreshCounter = rs.refreshCounter;
	historyQueue = rs.historyQueue;
	completionQueue = rs.completionQueue;
	systemConfig = new dramSystemConfiguration(rs.systemConfig);
	powerModel = rs.powerModel;
	algorithm = rs.algorithm;
	cerr << "dramchannel is copied=" << endl; 
	return *this;
}
