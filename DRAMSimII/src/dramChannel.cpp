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

dramChannel::dramChannel(const dramSettings& settings):
time(0),
lastRefreshTime(0),
lastRankID(0),
timingSpecification(settings),
transactionQueue(settings.transactionQueueDepth),
refreshCounter(NULL),
historyQueue(settings.historyQueueDepth),
completionQueue(settings.completionQueueDepth),
systemConfig(NULL),
powerModel(settings),
algorithm(settings),
rank(settings.rankCount, rank_c(settings, timingSpecification))
{
	// assign an id to each channel (normally done with commands)
	for (unsigned i = 0; i < settings.rankCount; i++)
	{
		rank[i].setRankID(i);
	}

	// initialize the refresh counters per rank
	if (settings.refreshPolicy != NO_REFRESH)
	{
		refreshCounter = new transaction *[settings.rankCount];

		// stagger the times that each rank will be refreshed so they don't all arrive in a burst
		unsigned step = settings.tREFI / settings.rankCount;

		for (unsigned j = 0; j < settings.rankCount; ++j)
		{
			transaction *newTrans = new transaction();
			newTrans->setType(AUTO_REFRESH_TRANSACTION);
			newTrans->getAddresses().rank = j;
			newTrans->setEnqueueTime(j * step);
			refreshCounter[j] = newTrans;			
		}		
	}
}

dramChannel::dramChannel(const dramChannel &dc):
time(dc.time),
lastRefreshTime(dc.lastRefreshTime),
lastRankID(dc.lastRankID),
timingSpecification(dc.timingSpecification),
transactionQueue(dc.transactionQueue),
refreshCounter(dc.refreshCounter),
historyQueue(dc.historyQueue),
completionQueue(dc.completionQueue),
systemConfig(dc.systemConfig),
powerModel(dc.powerModel),
algorithm(dc.algorithm),
rank((unsigned)dc.rank.size(), rank_c(dc.rank[0],timingSpecification))
{
	// assign an id to each channel (normally done with commands)
	for (unsigned i = 0; i < rank.size(); i++)
	{
		rank[i].setRankID(i);
	}
}



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
		if (checkForAvailableCommandSlots(temp_t))
		{
			// has room to decode this transaction
		
			//channel[chan].set_time(min(endTime,channel[chan].get_time() + timing_specification.t_buffer_delay));
			//update_system_time(); 

			// actually remove it from the queue now
			transaction *completedTransaction = getTransaction();
			assert(completedTransaction == temp_t);
			
			// then break into commands and insert into per bank command queues
			bool t2cResult = transaction2commands(completedTransaction);
			assert(t2cResult == true);

			// since reading vs dequeuing should yield the same result
			assert(temp_t == completedTransaction);

#ifdef DEBUG_TRANSACTION
			timingOutStream << "T->C [" << time << "] Q[" << getTransactionQueueCount() << "]" << temp_t << endl;
#endif
		}
		else
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
				if ((transactionQueue.size() > 0) && (time + timingSpecification.tBufferDelay() <= endTime))
				{
					tick_t oldTime = time;
					time = timingSpecification.tBufferDelay() + transactionQueue.front()->getEnqueueTime();
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
				if (min_gap + time <= endTime + timingSpecification.tCMD())
				{
					command *temp_com = getNextCommand();

					executeCommand(temp_com, min_gap);

					statistics->collectCommandStats(temp_com);	

#ifdef DEBUG_COMMAND
					timingOutStream << "C F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << min_gap << "] " << *temp_com << endl;
#endif

					// only get completed commands if they have finished TODO:
					transaction *completed_t = completionQueue.pop();

					if (completed_t)
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

							delete completed_t;

							return NULL;
						}
						else // return what was pointed to
						{
							const void *origTrans = completed_t->getOriginalTransaction();

							assert(completed_t->originalTransaction);

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
	}

	assert(time <= endTime + timingSpecification.tCMD());

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
		command *temp_c = rank[rank_id].bank[i].getPerBankQueue().read(1);

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
	float factorA = (powerModel.VDD / powerModel.VDDmax) * (powerModel.VDD / powerModel.VDDmax);
	float factorB = powerModel.frequency / powerModel.frequencySpec;

	unsigned totalRAS = 1;
	unsigned totalCAS = 1;
	unsigned totalCASW = 1; // ensure no div/0

	float PsysACT = 0;

	for (std::vector<rank_c>::iterator k = rank.begin(); k != rank.end(); k++)
	{
		unsigned perRankRASCount = 1;

		for (std::vector<bank_c>::iterator l = k->bank.begin(); l != k->bank.end(); l++)
		{
			totalRAS += l->getRASCount();
			perRankRASCount += l->getRASCount();
			totalCAS += l->getCASCount();
			totalCASW += l->getCASWCount();
			l->accumulateAndResetCounts();
		}
		//unsigned i = k->prechargeTime;
		//cerr << "ch[" << channelID << "] %pre[" << k->prechargeTime / (time - powerModel.lastCalculation) * 100 << "] " << k->prechargeTime << endl;
		
		// FIXME: assumes CKE is always high, so (1 - CKE_LOW_PRE%) = 1
		float percentActive = max(0.0F,1.0F - (k->prechargeTime / (float)(time - powerModel.lastCalculation)));

		
		//assert(RDschPct + WRschPct < 1.0F);
		

		powerOutStream << "Psys(ACT_STBY) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(5) << 
			factorA * factorB * powerModel.IDD3N * powerModel.VDDmax * percentActive << " mW P(" << k->prechargeTime << "/" << time - powerModel.lastCalculation << ")" << endl;

		//tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS * powerModel.tBurst / 2;

		float tRRDsch = (time - powerModel.lastCalculation) / perRankRASCount;

		PsysACT += ((float)powerModel.tRC / (float)tRRDsch) * factorA * powerModel.PdsACT;
		//powerOutStream << "Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(5) << powerModel.PdsACT * powerModel.tRC / (float)tRRDsch * factorA * 100 << "mW " <<
		//	" A(" << totalRAS << ") tRRDsch(" << setprecision(5) << tRRDsch / ((float)systemConfig->Frequency() * 1.0E-9F) << "ns) lastCalc[" << powerModel.lastCalculation << "] time[" << 
		//	time << "]" << endl;

		k->prechargeTime = 1;
	}			

	float RDschPct = totalCAS * timingSpecification.tBurst() / (float)(time - powerModel.lastCalculation);
	float WRschPct = totalCASW * timingSpecification.tBurst() / (float)(time - powerModel.lastCalculation);

	//cerr << RDschPct * 100 << "%\t" << WRschPct * 100 << "%"<< endl;

	powerOutStream << "Psys(ACT) ch[" << channelID << "] " << setprecision(5) << 
		PsysACT << " mW" << endl;

	powerOutStream << "Psys(RD) ch[" << channelID << "] " << setprecision(5) << 
		factorA * factorB * (powerModel.IDD4R - powerModel.IDD3N) * RDschPct << " mW" << endl;

	powerOutStream << "Psys(WR) ch[" << channelID << "] " << setprecision(5) << 
		factorA * factorB * (powerModel.IDD4W - powerModel.IDD3N) * WRschPct << " mW" << endl;
	powerModel.lastCalculation = time;

	powerOutStream.flush();
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
	refreshCounter[earliestRank]->setEnqueueTime(earliestTransaction->getEnqueueTime() + timingSpecification.tREFI());
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
		if ((tempTrans) && (time - tempTrans->getEnqueueTime() < timingSpecification.tBufferDelay()))
		{
#ifdef M5DEBUG
			timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timingSpecification.tBufferDelay() << endl;			
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
			if (time - tempTrans->getEnqueueTime() < timingSpecification.tBufferDelay())
			{
#ifdef M5DEBUG
				timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timingSpecification.tBufferDelay() << endl;			
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
		if ((tempTrans) && (time - tempTrans->getEnqueueTime() < timingSpecification.tBufferDelay()))
		{
#ifdef M5DEBUG
			timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timingSpecification.tBufferDelay() << endl;			
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
			if (time - tempTrans->getEnqueueTime() < timingSpecification.tBufferDelay())
			{
#ifdef M5DEBUG
				timingOutStream << "resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timingSpecification.tBufferDelay() << endl;			
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
