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

//////////////////////////////////////////////////////////////////////////
/// @brief constructs the dramChannel using this settings reference, also makes a reference to the dramSystemConfiguration object
/// @param settings the settings file that defines the number of ranks, refresh policy, etc.
/// @param sysConfig a const reference is made to this for some functions to grab parameters from
//////////////////////////////////////////////////////////////////////////
dramChannel::dramChannel(const dramSettings& settings, const dramSystemConfiguration &sysConfig):
time(0),
lastRefreshTime(0),
lastRankID(0),
timingSpecification(settings),
transactionQueue(settings.transactionQueueDepth),
refreshCounter(NULL),
historyQueue(settings.historyQueueDepth),
completionQueue(settings.completionQueueDepth),
systemConfig(sysConfig),
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
			newTrans->getAddresses().bank = 0;
			newTrans->setEnqueueTime(j * (step +1));
			refreshCounter[j] = newTrans;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor, reassigns the ordinal to each rank as they are duplicated
/// @param dc the dramChannel object to be copied
//////////////////////////////////////////////////////////////////////////
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

	// initialize the refresh counters per rank
	if (dc.systemConfig.getRefreshPolicy() != NO_REFRESH)
	{
		refreshCounter = dc.refreshCounter;

		for (unsigned j = 0; j < rank.size(); ++j)
		{
			refreshCounter[j] = dc.refreshCounter[j];
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief Moves the specified channel to at least the time given
/// @param endTime move the channel until it is at this time
/// @param transFinishTime the time that this transaction finished
//////////////////////////////////////////////////////////////////////////
const void *dramChannel::moveChannelToTime(const tick_t endTime, tick_t *transFinishTime)
{
	while (time < endTime)
	{	
		// has room to decode an available transaction
		if (checkForAvailableCommandSlots(readTransaction(true)))
		{
			// actually remove it from the queue now
			transaction *decodedTransaction = getTransaction();

			// then break into commands and insert into per bank command queues			
			bool t2cResult = transaction2commands(decodedTransaction);
			// checkForAvailablecommandSlots() should not have returned true if there was not enough space
			assert(t2cResult == true);

			DEBUG_TRANSACTION_LOG("T->C [" << time << "] Q[" << getTransactionQueueCount() << "]" << decodedTransaction)
		}
		else
		{
			M5_TIMING_LOG("!T2C " << decodedTransaction);

			// move time up by executing commands
			if (const command *temp_c = readNextCommand())
			{
				tick_t nextExecuteTime = earliestExecuteTime(temp_c);
				assert(nextExecuteTime == time + minProtocolGap(temp_c));

				//M5_TIMING_LOG("mg: " << min_gap);

				// move time to either when the next command executes or the next transaction decodes, whichever is earlier
				tick_t nextDecodeTime = nextTransactionDecodeTime();

				if (nextDecodeTime < min(nextExecuteTime, endTime))
				{
					time = nextDecodeTime;
				}
				// allow system to overrun so that it may send a command				
				else if (nextExecuteTime <= endTime + timingSpecification.tCMD())
				{
					command *nextCommand = getNextCommand();

					executeCommand(nextCommand);

					statistics->collectCommandStats(nextCommand);

					DEBUG_COMMAND_LOG("C F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << nextExecuteTime - time << "] " << *nextCommand);

					// only get completed commands if they have finished
					transaction *completed_t = completionQueue.pop();

					if (completed_t)
					{
						statistics->collectTransactionStats(completed_t);

						DEBUG_TRANSACTION_LOG("T CH[" << setw(2) << channelID << "] " << completed_t)

							// reuse the refresh transactions
							if (completed_t->getType() == AUTO_REFRESH_TRANSACTION)
							{
								//completed_t->setEnqueueTime(completed_t->getEnqueueTime() + systemConfig.getRefreshTime());

								assert(systemConfig.getRefreshPolicy() != NO_REFRESH);

								delete completed_t;

								return NULL;
							}
							else // return what was pointed to
							{
								const void *origTrans = completed_t->getOriginalTransaction();

								M5_DEBUG(assert(completed_t->getOriginalTransaction()));

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
			else
			{
				// the transaction queue and all the per bank queues are empty,
				// so just move time forward to the point where the transaction starts
				// or move time forward until the transaction is ready to be decoded
				const transaction *nextTrans = readTransaction(false);
				if ((nextTrans) && (nextTrans->getEnqueueTime() + timingSpecification.tBufferDelay() + 1 <= endTime) && checkForAvailableCommandSlots(nextTrans))
				{
					tick_t oldTime = time;
					assert(nextTrans->getEnqueueTime() + timingSpecification.tBufferDelay() + 1 <= endTime);

					time = timingSpecification.tBufferDelay() + nextTrans->getEnqueueTime() + 1;
					if (oldTime >= time)
						assert(oldTime < time);
				}
				// no transactions to convert, no commands to issue, just go forward
				else
				{
					time = endTime;
				}
			}
		}
	}

	assert(time <= endTime + timingSpecification.tCMD());
	//assert(time <= endTime);

	*transFinishTime = endTime;
	M5_TIMING_LOG("ch[" << channelID << "] @ " << std::dec << time);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// @brief determines when the next transaction is decoded, command ready to be executed or next refresh command arrives
/// @return the time when the next event happens
/// @sa readTransactionSimple() readNextCommand() earliestExecuteTime() checkForAvailableCommandSlots()
//////////////////////////////////////////////////////////////////////////
tick_t dramChannel::nextTick() const
{
	tick_t nextWake = TICK_T_MAX;

	// first look for transactions
	if (const transaction *nextTrans = readTransactionSimple())
	{
		// make sure it can finish
		tick_t tempWake = nextTrans->getEnqueueTime() + getTimingSpecification().tBufferDelay() + 1; 

		assert(nextTrans->getEnqueueTime() <= time);
		assert(tempWake <= time + timingSpecification.tBufferDelay() + 1);

		// whenever the next transaction is ready and there are available slots for the R/C/P commands
		if ((tempWake < nextWake) && (checkForAvailableCommandSlots(nextTrans)))
		{
			nextWake = tempWake;
		}
	}

	// then check to see when the next command occurs
	if (const command *tempCommand = readNextCommand())
	{
		int tempGap = minProtocolGap(tempCommand);
		tick_t tempCommandExecuteTime = earliestExecuteTime(tempCommand);
		assert(time + tempGap == tempCommandExecuteTime);

		if (tempCommandExecuteTime < nextWake)
		{
			nextWake = time + tempGap;
		}
	}

	// check the refresh queue
	if (systemConfig.getRefreshPolicy() != NO_REFRESH)
	{
		if (const transaction *refresh_t = readRefresh())
		{
			// add one because the transaction actually finishes halfway through the tick
			tick_t tempWake = refresh_t->getEnqueueTime() + timingSpecification.tBufferDelay() + 1;

			//assert(refresh_t->getEnqueueTime() <= currentChan->getTime());
			//assert(tempWake <= currentChan->getTime() + currentChan->getTimingSpecification().tBufferDelay());

			if ((refresh_t->getEnqueueTime() < nextWake) && (checkForAvailableCommandSlots(refresh_t)))
			{
				// a refresh transaction may have been missed, so ensure that the correct time is chosen in the future
				nextWake = tempWake;
			}
		}

		//const transaction *refresh_t = (*currentChan).readRefresh();

		//if (refresh_t->getEnqueueTime() < nextWake)
		//{
		//	// a refresh transaction may have been missed, so ensure that the correct time is chosen in the future
		//	nextWake = max(currentChan->getTime() + 1,refresh_t->getEnqueueTime());
		//}
	}

	return nextWake;
}

void dramChannel::recordCommand(command *latest_command)
{
	while (!historyQueue.push(latest_command))
	{
		delete historyQueue.pop();
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief enqueue the transaction into the transactionQueue
/// @param in the transaction to be put into the queue
/// @return true if there was room in the queue for this command and the algorithm allowed it, false otherwise
//////////////////////////////////////////////////////////////////////////
bool dramChannel::enqueue(transaction *in)
{
	/// @todo probably should set the enqueue time = time here
	if (systemConfig.getTransactionOrderingAlgorithm() == STRICT)
		return transactionQueue.push(in);
	// try to insert reads and fetches before writes
	/// @todo add support for additional transaction ordering algorithms, such as read, instruction, fetch first
	else
	{
		exit(-1);
		assert(systemConfig.getTransactionOrderingAlgorithm() == RIFF);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief determine when the next transaction in the queue will be decoded
/// @return the time when the decoding will be complete
//////////////////////////////////////////////////////////////////////////
tick_t dramChannel::nextTransactionDecodeTime() const
{
	tick_t nextTime = TICK_T_MAX;

	if (const transaction *nextTrans = readTransaction(false))
	{
		if (checkForAvailableCommandSlots(nextTrans))
		{
			nextTime = nextTrans->getEnqueueTime() + timingSpecification.tBufferDelay() + 1;
		}
	}
	return nextTime;
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

	DEBUG_LOG("Rank[" << rank_id << "] Read[" << read_count << "] Write[" << write_count << "] Empty[" << empty_count << "]")

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

		float tRRDsch = ((float)time - powerModel.lastCalculation) / perRankRASCount;

		PsysACT += ((float)powerModel.tRC / (float)tRRDsch) * factorA * powerModel.PdsACT;
		//powerOutStream << "Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(5) << powerModel.PdsACT * powerModel.tRC / (float)tRRDsch * factorA * 100 << "mW " <<
		//	" A(" << totalRAS << ") tRRDsch(" << setprecision(5) << tRRDsch / ((float)systemConfig.Frequency() * 1.0E-9F) << "ns) lastCalc[" << powerModel.lastCalculation << "] time[" << 
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

//////////////////////////////////////////////////////////////////////////
// get the next refresh command and remove it from the queue
//////////////////////////////////////////////////////////////////////////
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
	refreshCounter[earliestRank]->getAddresses().bank = 0;

	return earliestTransaction;
}

//////////////////////////////////////////////////////////////////////////
// read the next occurring refresh transaction without removing it
//////////////////////////////////////////////////////////////////////////
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


/// read the next available transaction for this channel without actually removing it from the queue
/// if bufferDelay is set, then only transactions that have been in the queue long enough to decode are considered
const transaction *dramChannel::readTransaction(const bool bufferDelay) const
{
	const unsigned delay = bufferDelay ? timingSpecification.tBufferDelay() : 0;

	const transaction *tempTrans = transactionQueue.front(); 

	if (systemConfig.getRefreshPolicy() == NO_REFRESH)
	{
		if ((tempTrans) && (time - tempTrans->getEnqueueTime() < delay))
		{
			M5_TIMING_LOG("resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << delay);
			return NULL; // not enough time has passed
		}
		else
		{
			return tempTrans;
		}
	}
	else
	{
		const transaction *refreshTrans = readRefresh();
		assert(refreshTrans != NULL);

		// give an advantage to normal transactions, but prevent starvation for refresh operations
		if (tempTrans && (tempTrans->getEnqueueTime() < refreshTrans->getEnqueueTime() + systemConfig.getSeniorityAgeLimit()))
		{
			// if this transaction has not yet been decoded, then look to see if the refresh trans has arrived
			if (time <= tempTrans->getEnqueueTime() + delay)
			{
				M5_TIMING_LOG("resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << delay);

				// if this refresh command has arrived
				if (refreshTrans->getEnqueueTime() + delay <= time) 
					return refreshTrans;
				else
					return NULL; // not enough time has passed and the newest refresh transaction hasn't arrived yet
			}
			else
			{
				return tempTrans;
			}
		}
		else if (time >= refreshTrans->getEnqueueTime() + delay)
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
	const transaction *tempTrans = transactionQueue.front(); 

	if (systemConfig.getRefreshPolicy() == NO_REFRESH)
	{
		if ((tempTrans) && (time - tempTrans->getEnqueueTime() < timingSpecification.tBufferDelay()))
		{
			M5_TIMING_LOG("resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timingSpecification.tBufferDelay());			
			return NULL; // not enough time has passed
		}
		return transactionQueue.pop();
	}
	else
	{
		const transaction *refreshTrans = readRefresh();
		assert(refreshTrans != NULL); // should always be refresh transactions coming

		if (tempTrans && (tempTrans->getEnqueueTime() < refreshTrans->getEnqueueTime() + systemConfig.getSeniorityAgeLimit()))
		{
			if (time <= tempTrans->getEnqueueTime() + timingSpecification.tBufferDelay())
			{
				M5_TIMING_LOG("resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timingSpecification.tBufferDelay());

				// if this refresh command has arrived
				if (refreshTrans->getEnqueueTime() + timingSpecification.tBufferDelay() <= time) 
					return getRefresh();
				else
					return NULL; // not enough time has passed and the newest refresh transaction hasn't arrived yet
			}
			return transactionQueue.pop();
		}
		else if (time >= refreshTrans->getEnqueueTime() + timingSpecification.tBufferDelay())
		{
			return getRefresh();
		}
		else
		{
			return NULL;
		}
	}
}
