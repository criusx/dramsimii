// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
// 
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DRAMsimII is distributed incomingTransaction the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>
#include <vector>
#include <iostream>

#include "Channel.h"
#include "reporting/soapDRAMsimWSSoapHttpProxy.h"
#include "reporting/DRAMsimWSSoapHttp.nsmap"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

using std::endl;
using std::setw;
using std::cerr;
using std::hex;
using std::dec;
using std::min;
using std::max;
using namespace DRAMsimII;

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// @brief constructs the dramChannel using this settings reference, also makes a reference to the dramSystemConfiguration object
/// @param settings the settings file that defines the number of ranks, refresh policy, etc.
/// @param sysConfig a const reference is made to this for some functions to grab parameters from
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel::Channel(const Settings& settings, const SystemConfiguration& sysConfig, Statistics& stats):
time(0ll),
lastRefreshTime(-100ll),
lastCommandIssueTime(-100ll),
lastRankID(0),
timingSpecification(settings),
transactionQueue(settings.transactionQueueDepth),
refreshCounter(settings.rankCount),
historyQueue(settings.historyQueueDepth),
completionQueue(settings.completionQueueDepth),
systemConfig(sysConfig),
statistics(stats),
powerModel(settings),
algorithm(settings),
dbReporting(settings.dbReporting),
rank(sysConfig.getRankCount(), Rank(settings, timingSpecification, sysConfig))
{
	// assign an id to each channel (normally done with commands)
	for (unsigned i = 0; i < settings.rankCount; i++)
	{
		rank[i].setRankID(i);
	}

	// initialize the refresh counters per rank
	if (settings.refreshPolicy != NO_REFRESH)
	{
		// stagger the times that each rank will be refreshed so they don't all arrive incomingTransaction a burst
		unsigned step = settings.tREFI / settings.rankCount;

		//Address addr;

		for (unsigned j = 0; j < refreshCounter.size(); ++j)
		{
			//addr.setRank(j);
			//addr.setBank(0);
			//newTrans->setEnqueueTime(j * (step +1));
			//refreshCounter[j] = new Transaction(AUTO_REFRESH_TRANSACTION,j * (step + 1), 8, addr, NULL);
			refreshCounter[j] = j * (step + 1);
			//refreshCounter[j]->setEnqueueTime(refreshCounter[j]->getArrivalTime());
		}
	}
}

/// normal copy constructor
/// @brief the copy constructor for building unitialized copies of a channel
/// @author Joe Gross
Channel::Channel(const Channel& rhs, const SystemConfiguration& systemConfig, Statistics& stats):
time(rhs.time),
lastRefreshTime(rhs.lastRefreshTime),
lastCommandIssueTime(rhs.lastCommandIssueTime),
lastRankID(rhs.lastRankID),
timingSpecification(rhs.timingSpecification),
transactionQueue(rhs.transactionQueue),
refreshCounter(rhs.refreshCounter),
historyQueue(rhs.historyQueue),
completionQueue(rhs.completionQueue),
systemConfig(systemConfig),
statistics(stats),
powerModel(rhs.powerModel),
algorithm(rhs.algorithm),
channelID(rhs.channelID),
dbReporting(rhs.dbReporting),
// to initialize the references
rank((unsigned)systemConfig.getRankCount(), Rank(rhs.rank[0],timingSpecification, systemConfig))
{
	// to fill incomingTransaction the values
	rank = rhs.rank;
}

/// deserialization constructor
/// @brief the constructor to build copies of a channel once it's been deserialized, needs further initialization before it's ready
/// @author Joe Gross
Channel::Channel(const Settings settings, const SystemConfiguration& sysConf, Statistics & stats, const PowerConfig &power, const std::vector<Rank> &newRank, const TimingSpecification &timing):
time(0),
lastRefreshTime(0),
lastCommandIssueTime(0),
lastRankID(0),
timingSpecification(timing),
transactionQueue(0),
refreshCounter(0),
historyQueue(0),
completionQueue(0),
systemConfig(sysConf),
statistics(stats),
powerModel(power),
algorithm(settings),
channelID(UINT_MAX),
dbReporting(settings.dbReporting),
rank(newRank)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor, reassigns the ordinal to each rank as they are duplicated
/// @param rhs the dramChannel object to be copied
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel::Channel(const Channel &rhs):
time(rhs.time),
lastRefreshTime(rhs.lastRefreshTime),
lastCommandIssueTime(rhs.lastCommandIssueTime),
lastRankID(rhs.lastRankID),
timingSpecification(rhs.timingSpecification),
transactionQueue(rhs.transactionQueue),
refreshCounter(rhs.refreshCounter),
historyQueue(rhs.historyQueue),
completionQueue(rhs.completionQueue),
systemConfig(rhs.systemConfig),
statistics(rhs.statistics),
powerModel(rhs.powerModel),
algorithm(rhs.algorithm),
channelID(rhs.channelID),
dbReporting(rhs.dbReporting),
rank((unsigned)rhs.rank.size(), Rank(rhs.rank[0],timingSpecification, systemConfig))
{
	// TODO: copy over values incomingTransaction ranks now that reference members are init
	// assign an id to each channel (normally done with commands)
	rank = rhs.rank;

	// initialize the refresh counters per rank
	// because the vector copy constructor doesn't make deep copies
	// 	if (rhs.systemConfig.getRefreshPolicy() != NO_REFRESH)
	// 	{
	// 		for (unsigned j = 0; j < rhs.refreshCounter.size(); ++j)
	// 		{
	// 			if (rhs.refreshCounter[j])
	// 				refreshCounter[j] = new Transaction(*rhs.refreshCounter[j]);
	// 		}
	// 	}

#if 0
	for (unsigned i = 0; i < rank.size(); i++)
	{
		rank[i].setRankID(i);
	}	
#endif
}

Channel::~Channel()
{
	// 	if (systemConfig.getRefreshPolicy() != NO_REFRESH)
	// 	{
	// 		for (std::vector<Transaction *>::iterator i = refreshCounter.begin(); i != refreshCounter.end(); i++)
	// 		{
	// 			delete *i;
	// 		}
	// 	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief Moves the specified channel to at least the time given
/// @param endTime move the channel until it is at this time
/// @param transFinishTime the time that this transaction finished
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
unsigned Channel::moveChannelToTime(const tick endTime, tick& transFinishTime)
{
	// if there is an operation that takes place at time == endTime, this will allow it
	//assert(endTime >= time);

	while (time < endTime)
	{	
		// move time to either when the next command executes or the next transaction decodes, whichever is earlier
		// otherwise just go to the end
		tick oldTime = time;
		time = max(min(endTime,min(nextTransactionDecodeTime(),nextCommandExecuteTime())),time);
		assert(time <= endTime);
		assert(time >= oldTime);

		// has room to decode an available transaction
		if (checkForAvailableCommandSlots(readTransaction(true)))
		{
			// actually remove it from the queue now
			Transaction *decodedTransaction = getTransaction();

			// then break into commands and insert into per bank command queues			
			bool t2cResult = transaction2commands(decodedTransaction);
			// checkForAvailablecommandSlots() should not have returned true if there was not enough space
			assert(t2cResult == true);

			DEBUG_TRANSACTION_LOG("T->C [" << time << "] Q[" << getTransactionQueueCount() << "]" << decodedTransaction);
		}		

		// execute commands for this time, reevaluate what the next command is since this may have changed after decoding the transaction
		const Command *nextCommand = readNextCommand();

		if (nextCommand && (earliestExecuteTime(nextCommand) <= time))
		{
			Command *executingCommand = getNextCommand(nextCommand);

			assert(executingCommand == nextCommand);

			executeCommand(executingCommand);					

			DEBUG_COMMAND_LOG("C F[" << hex << setw(8) << time << "] MG[" << setw(2) << executingCommand->getStartTime() - time << "] " << *executingCommand);

			// only get completed commands if they have finished
			if (Transaction *completedTransaction = completionQueue.pop())
			{
				statistics.collectTransactionStats(completedTransaction);

				DEBUG_TRANSACTION_LOG("T CH[" << setw(2) << channelID << "] " << completedTransaction);

				// reuse the refresh transactions
				if (completedTransaction->getType() == AUTO_REFRESH_TRANSACTION)
				{
					//completedTransaction->setEnqueueTime(completedTransaction->getEnqueueTime() + systemConfig.getRefreshTime());

					assert(systemConfig.getRefreshPolicy() != NO_REFRESH);

					delete completedTransaction;

					return UINT_MAX;
				}
				else // return what was pointed to
				{
					const unsigned origTrans = completedTransaction->getOriginalTransaction();

					M5_DEBUG(assert(origTrans < UINT_MAX));

					transFinishTime = completedTransaction->getCompletionTime();

					delete completedTransaction;

					return origTrans;
				}
			}
		}
	}
	//assert(time <= endTime + timingSpecification.tCMD());
	//assert(time <= endTime);

	transFinishTime = endTime;
	M5_TIMING_LOG("ch[" << channelID << "] @ " << dec << time);

	return UINT_MAX;
}


//////////////////////////////////////////////////////////////////////////
/// @brief determines when the next transaction is decoded, command ready to be executed or next refresh command arrives
/// @return the time when the next event happens
/// @sa readTransactionSimple() readNextCommand() earliestExecuteTime() checkForAvailableCommandSlots()
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
tick Channel::nextTick() const
{
	//tick value = max(min(nextTransactionDecodeTime(),nextCommandExecuteTime()),time + 1);
	return max(min(nextTransactionDecodeTime(),nextCommandExecuteTime()),time + 1);
}

//////////////////////////////////////////////////////////////////////////
/// @brief adds this command to the history queue
/// @details this allows other groups to view a recent history of commands that were issued to decide what to execute next
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
void Channel::retireCommand(Command *newestCommand)
{
	statistics.collectCommandStats(newestCommand);
	
	// transaction complete? if so, put incomingTransaction completion queue
	// note that the host transaction should only be pointed to by a CAS command
	// since this is when a transaction is done from the standpoint of the requester
	if (newestCommand->getHost()) 
	{
		if (newestCommand->getCommandType() != REFRESH_ALL)
			assert(newestCommand->getEnqueueTime() >= newestCommand->getHost()->getEnqueueTime() && newestCommand->getCompletionTime() <= newestCommand->getHost()->getCompletionTime());

		// remove the host pointer so that when this is serialized there aren't multiple copies made
		if (!completionQueue.push(newestCommand->removeHost()))
		{
			cerr << "Fatal error, cannot insert transaction into completion queue." << endl;
			cerr << "Increase execution q depth and resume. Should not occur. Check logic." << endl;
			exit(2);
		}
	}

	assert(!newestCommand->getHost());

	while (!historyQueue.push(newestCommand))
	{
		delete historyQueue.pop();
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief enqueue the transaction into the transactionQueue
/// @param incomingTransaction the transaction to be put into the queue
/// @return true if there was room incomingTransaction the queue for this command and the algorithm allowed it, false otherwise
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Channel::enqueue(Transaction *incomingTransaction)
{
	//static unsigned success = 1;

	if (transactionQueue.isFull())
		return false;

	incomingTransaction->setEnqueueTime(time);

	assert(incomingTransaction->getAddresses().getChannel() == channelID);

	bool result;

	/// @todo probably should set the enqueue time = time here
	switch (systemConfig.getTransactionOrderingAlgorithm())
	{
	case STRICT:
		result = transactionQueue.push(incomingTransaction);
		break;
	case RIFF:
		/// @todo add priority insertion here
		// search from back to front
		// insert right after a conflict, any transaction over the seniority limit or any read
		if (incomingTransaction->isRead())
		{
			for (int currentIndex = transactionQueue.size() - 1; currentIndex >= 0; --currentIndex)
			{
				// prevent against starvation and RAW errors, reads do not switch their ordering
				if (transactionQueue[currentIndex]->isRead() ||
					(time - transactionQueue[currentIndex]->getEnqueueTime() > systemConfig.getSeniorityAgeLimit()) ||
					transactionQueue[currentIndex]->getAddresses() == incomingTransaction->getAddresses())
				{
					result = transactionQueue.insert(incomingTransaction, currentIndex + 1);					
					assert(result);
					//if (currentIndex < transactionQueue.size() - 1)
					//	success++;
					return result;
				}				
			}
			// if there were no conditions to force it to choose another location, insert at the head
			result = transactionQueue.push_front(incomingTransaction);
		}
		else
		{
			result = transactionQueue.push(incomingTransaction);
		}
		break;
	default:
		cerr << "Unknown transaction ordering algorithm." << endl;
		exit(-1);		
		break;
	}

	//if (success % 10000 == 0)
	//	cerr << success << endl;

	return result;
}

//////////////////////////////////////////////////////////////////////////
/// @brief determine when the next transaction incomingTransaction the queue will be decoded
/// @return the time when the decoding will be complete
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
tick Channel::nextTransactionDecodeTime() const
{
	tick nextTime = TICK_MAX;

	if (const Transaction *nextTrans = readTransaction(false))
	{
		if (checkForAvailableCommandSlots(nextTrans))
		{
			if (nextTrans->getType() == AUTO_REFRESH_TRANSACTION)
				nextTime = nextTrans->getArrivalTime();
			else
				nextTime = nextTrans->getEnqueueTime() + timingSpecification.tBufferDelay();
		}
	}

	return nextTime;
}

//////////////////////////////////////////////////////////////////////////
/// @brief determines the next time available for a command to issue
/// @return the next time an event occurs on this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
tick Channel::nextCommandExecuteTime() const
{
	tick nextTime = TICK_MAX;
	// then check to see when the next command occurs
	if (const Command *tempCommand = readNextCommand())
	{
		int tempGap = minProtocolGap(tempCommand);
		tick tempCommandExecuteTime = earliestExecuteTime(tempCommand);
		assert(time + tempGap == tempCommandExecuteTime);

		nextTime = tempCommandExecuteTime;
	}
	return nextTime;
}

//////////////////////////////////////////////////////////////////////////
/// @brief counts the number of reads and writes so far and returns whichever had more
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
TransactionType Channel::setReadWriteType(const int rank_id,const int bank_count) const
{
	int read_count = 0;
	int write_count = 0;
	int empty_count = 0;

	for(int i = 0; i < bank_count; ++i)
	{
		const Command *temp_c = rank[rank_id].bank[i].read(1);

		if(temp_c != NULL)
		{
			if (temp_c->getCommandType() == READ_AND_PRECHARGE)
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

	DEBUG_LOG("Rank[" << rank_id << "] Read[" << read_count << "] Write[" << write_count << "] Empty[" << empty_count << "]");

	if (read_count >= write_count)
		return READ_TRANSACTION;
	else
		return WRITE_TRANSACTION;
}

//////////////////////////////////////////////////////////////////////
/// @brief performs power calculations for this epoch and cumulative
/// @details calculates the power according to Micron technical note TN-47-04\n
/// also does breakdowns of power consumed per channel and per epoch as well as averaged over time
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void Channel::doPowerCalculation(const tick systemTime)
{	
	// the counts for the total number of operations
	//unsigned entireRAS = 1;
	//unsigned entireCAS = 1;
	//unsigned entireCASW = 1;

	// the counts for the operations this epoch
	unsigned totalRAS = 1;
	
	float PsysACTTotal = 0.0F;
	float PsysRD = 0.0F;
	float PsysWR = 0.0F;
	float PsysACT_STBY = 0.0F;
	float PsysPRE_STBY = 0.0F;
	float PsysPRE_PDN = 0.0F;
	float PsysACT_PDN = 0.0F;
	float PsysACT = 0.0F;

	float tRRDsch = 0.0F;

	vector<int> rankArray;
	vector<float> PsysACTSTBYArray, PsysACTArray;

	for (vector<Rank>::iterator k = rank.begin(); k != rank.end(); k++)
	{
		unsigned perRankRASCount = 1;
		//unsigned totalCAS = 1;
		//unsigned totalCASW = 1; // ensure no div/0

		rankArray.push_back(k->getRankID());

		for (vector<Bank>::iterator l = k->bank.begin(); l != k->bank.end(); l++)
		{
			totalRAS += l->getRASCount();			
			perRankRASCount += l->getRASCount();
			//totalCAS += l->getCASCount();
			//totalCASW += l->getCASWCount();
			l->accumulateAndResetCounts();
		}
	
		// FIXME: assumes CKE is always high, so (1 - CKE_LOW_PRE%) = 1
		float percentActive = 1.0F - (k->getPrechargeTime() / (float)(time - powerModel.getLastCalculation()));
		assert(percentActive >= 0.0F && percentActive <= 1.0F);
		assert(k->getPrechargeTime() <= time - powerModel.getLastCalculation());

		/// @todo actually simulate CKE, per rank
		float CKE_LO_PRE = 0.95F;
		float CKE_LO_ACT = 0.01F;

		// calculate PsysACT-STBY
		PsysACT_STBY += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * 
			powerModel.getIDD3N() * powerModel.getVDDmax() * percentActive * (1.0F - CKE_LO_ACT);

		PsysACTSTBYArray.push_back(PsysACT_STBY);

		// calculate PsysPRE-STBY
		PsysPRE_STBY += powerModel.getDevicesPerRank() * powerModel.getFrequencyScaleFactor() * powerModel.getVoltageScaleFactor() *
			powerModel.getIDD2N() * powerModel.getVDDmax() * (1 - percentActive) * (1 - CKE_LO_PRE);
		
		// calculate PsysPRE-PDN
		PsysPRE_PDN += powerModel.getVoltageScaleFactor() * powerModel.getIDD2P() * powerModel.getVDDmax() * (1 - percentActive) * CKE_LO_PRE;

		// calculate PsysACT-PDN
		/// @todo: account for CKE
		PsysACT_PDN += powerModel.getVoltageScaleFactor() * powerModel.getIDD3P() * powerModel.getVDDmax() * percentActive * (1 - CKE_LO_ACT);

		// calculate PsysACT
		tRRDsch = ((float)time - powerModel.getLastCalculation()) / perRankRASCount;

		PsysACT += powerModel.getDevicesPerRank() * ((float)powerModel.gettRC() / (float)tRRDsch) * powerModel.getVoltageScaleFactor() * powerModel.getPdsACT();

		PsysACTArray.push_back(PsysACT);
		
		PsysACTTotal += ((float)powerModel.gettRC() / (float)tRRDsch) * powerModel.getVoltageScaleFactor() * powerModel.getPdsACT();

		// calculate PdsRD
		float RDschPct = k->getReadCycles() / (float)(time - powerModel.getLastCalculation());

		PsysRD += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsRD() * RDschPct;

		// calculate PdsWR
		float WRschPct = k->getWriteCycles() / (float)(time - powerModel.getLastCalculation());

		PsysWR += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsWR() * WRschPct; 

		k->resetPrechargeTime(time);
		k->resetCycleCounts();
	}

	powerOutStream << "-Psys(ACT_STBY) ch[" << channelID << "] {" << setprecision(5) << 
		PsysACT_STBY << "} mW" << endl;
		//Pre(" << k->getPrechargeTime() << "/" << time - powerModel.getLastCalculation() << ")" << endl;

	powerOutStream << "-Psys(ACT) ch[" << channelID << "] {"<< setprecision(5) << 
		PsysACT << "} mW tRRD[" << tRRDsch << "]" << endl;

	powerOutStream << "-Psys(PRE_STBY) ch[" << channelID << "] {" << setprecision(5) << 
		PsysPRE_STBY << "} mW" << endl;
		//Pre(" << k->getPrechargeTime() << "/" << time - powerModel.getLastCalculation() << ")" << endl;
	
	powerOutStream << "-Psys(RD) ch[" << channelID << "] {" << setprecision(5) << PsysRD << "} mW" << endl;
	
	powerOutStream << "-Psys(WR) ch[" << channelID << "] {" << setprecision(5) << PsysWR << "} mW" << endl;

	// report these results
	if (dbReporting)
	{
		boost::thread(boost::bind(&DRAMsimII::Channel::sendPower,this,PsysRD, PsysWR, rankArray, PsysACTSTBYArray, PsysACTArray, systemTime));
	}

	// no total power calcs for now
#if 0
	powerOutStream << "++++++++++++++++++++++ total ++++++++++++++++++++++" << endl;

	PsysACTTotal = 0;

	for (vector<Rank>::const_iterator k = rank.begin(); k != rank.end(); k++)
	{
		unsigned perRankRASCount = 1;

		for (vector<Bank>::const_iterator l = k->bank.begin(); l != k->bank.end(); l++)
		{
			entireRAS += l->getTotalRASCount();
			entireCAS += l->getTotalCASCount();
			entireCASW += l->getTotalCASWCount();
			perRankRASCount += l->getTotalRASCount();
		}

		float percentActive = 1.0F - (float)(k->getTotalPrechargeTime())/(float)time;

		powerOutStream << "+Psys(ACT_STBY) ch[" << channelID << "] r[" << k->getRankID() << "] {" << setprecision(5) << 
			powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getIDD3N() * powerModel.getVDDmax() * percentActive << "} mW P(" << k->getTotalPrechargeTime() << "/" << time  << ")" << endl;

		float tRRDsch = ((float)time) / perRankRASCount;

		powerOutStream << "+Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] {"<< setprecision(5) << 
			powerModel.getDevicesPerRank() * ((float)powerModel.gettRC() / (float)tRRDsch) * powerModel.getVoltageScaleFactor() * powerModel.getPdsACT() << "} mW" << endl;

		PsysACTTotal += powerModel.getDevicesPerRank() * ((float)powerModel.gettRC() / (float)tRRDsch) * powerModel.getVoltageScaleFactor() * powerModel.getPdsACT();
	}

	float RDschPct = entireCAS * timingSpecification.tBurst() / (float)(time);
	float WRschPct = entireCAS * timingSpecification.tBurst() / (float)(time);

	//cerr << RDschPct * 100 << "%\t" << WRschPct * 100 << "%"<< endl;

	powerOutStream << "+Psys(ACT) ch[" << channelID << "] " << setprecision(5) << 
		PsysACTTotal << " mW" << endl;

	powerOutStream << "+Psys(RD) ch[" << channelID << "] {" << setprecision(5) << 
		powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * (powerModel.getIDD4R() - powerModel.getIDD3N()) * RDschPct << "} mW" << endl;

	powerOutStream << "+Psys(WR) ch[" << channelID << "] {" << setprecision(5) << 
		powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * (powerModel.getIDD4W() - powerModel.getIDD3N()) * WRschPct << "} mW" << endl;
#endif
	powerModel.setLastCalculation(time);
}

//////////////////////////////////////////////////////////////////////////
/// @brief sends power calculations off to a remote server, should be run incomingTransaction a separate thread
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Channel::sendPower(float PsysRD, float PsysWR, vector<int> rankArray, vector<float> PsysACTSTBYArray, vector<float> PsysACTArray, const tick currentTime) const 
{
	DRAMsimWSSoapHttp service;
	_ns2__submitEpochResultElement submit;

	string session(systemConfig.getSessionID());
	submit.sessionID = &session;

	submit.epoch = currentTime;

	vector<int> channelArray(rank.size(),channelID);	
	submit.channel = channelArray;

	submit.rank = rankArray;

	submit.PsysACTSTBY = PsysACTSTBYArray;
	submit.PsysACT = PsysACTArray;
	submit.PsysRD = PsysRD;
	submit.PsysWR = PsysWR;
	_ns2__submitEpochResultResponseElement response;
	int retVal = service.__ns1__submitEpochResult(&submit,&response);
	return (retVal == 0);
}


//////////////////////////////////////////////////////////////////////////
/// @brief get the next refresh time
/// @return the time at which the next refresh should be issued for this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
const tick Channel::nextRefresh() const
{
	assert(rank.size() >= 1);

	return *(std::min_element(refreshCounter.begin(), refreshCounter.end()));
}

//////////////////////////////////////////////////////////////////////////
/// @brief returns a pointer to the next transaction to issue to this channel without removing it
/// @detail read the next available transaction for this channel without actually removing it from the queue \n
/// if bufferDelay is set, then only transactions that have been incomingTransaction the queue long enough to decode are considered
/// @return the next transaction that should be issued to this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
const Transaction *Channel::readTransaction(const bool bufferDelay) const
{
	const unsigned delay = bufferDelay ? timingSpecification.tBufferDelay() : 0;

	const Transaction *nextTransaction = transactionQueue.front(); 

	if (systemConfig.getRefreshPolicy() == NO_REFRESH)
	{
		if ((nextTransaction) && (time - nextTransaction->getEnqueueTime() < delay))
		{
			M5_TIMING_LOG("resetting: " << time << " " << nextTransaction->getEnqueueTime() << " " << delay);
			return NULL; // not enough time has passed
		}
		else
		{
			return nextTransaction;
		}
	}
	else
	{
		const tick nextRefreshTransaction = nextRefresh();

		// give an advantage to normal transactions, but prevent starvation for refresh operations
		if (nextTransaction && (nextTransaction->getEnqueueTime() < nextRefreshTransaction + systemConfig.getSeniorityAgeLimit()))
		{
			// if this transaction has not yet been decoded, then look to see if the refresh trans has arrived
			if (time < nextTransaction->getEnqueueTime() + delay)
			{
				M5_TIMING_LOG("resetting: " << time << " " << nextTransaction->getEnqueueTime() << " " << delay);

				// if this refresh command has arrived
				if (nextRefreshTransaction <= time) 
					return readNextRefresh();
				else
					return NULL; // not enough time has passed and the newest refresh transaction hasn't arrived yet
			}
			else
			{
				return nextTransaction;
			}
		}
		else if (time >= nextRefreshTransaction)
		{
			return readNextRefresh();
		}
		else
		{
			return NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief get the next transaction, whether a refresh transaction or a normal R/W transaction
/// @detail gets the next transaction for this channel and removes it
/// @return the next transaction for this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Transaction *Channel::getTransaction()
{
	const Transaction *tempTrans = transactionQueue.front(); 

	// no refresh transactions, just see if normal transactions are decoded
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
		const tick refreshTrans = nextRefresh();

		// prioritize the normal transactions, but don't starve the refresh transactions
		if (tempTrans && (tempTrans->getEnqueueTime() < refreshTrans + systemConfig.getSeniorityAgeLimit()))
		{
			// if it's not decoded, go with the refresh transaction
			if (time < tempTrans->getEnqueueTime() + timingSpecification.tBufferDelay())
			{
				M5_TIMING_LOG("resetting: " << time << " " << tempTrans->getEnqueueTime() << " " << timingSpecification.tBufferDelay());

				// if this refresh command has arrived
				if (refreshTrans <= time) 
					return createNextRefresh();
				else
					return NULL; // not enough time has passed and the newest refresh transaction hasn't arrived yet
			}
			else
			{
				return transactionQueue.pop();
			}
		}
		else if (time >= refreshTrans)
		{
			return createNextRefresh();
		}
		else
		{
			return NULL;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
/// @brief get the next refresh command and remove it from the queue
/// @detail returns a pointer to a refresh transaction that represents what the next refresh \n
/// transaction would be. this should not be enqueued as it has not been removed yet
/// @return the next possible refresh transaction
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Transaction *Channel::createNextRefresh()
{
	//assert(rank.size() >= 1);
	unsigned currentRank = 0;
	unsigned earliestRank = 0;
	tick earliestTime = refreshCounter[0];
	for (vector<tick>::const_iterator i = refreshCounter.begin(); i != refreshCounter.end(); i++, currentRank++)
	{
		if (*i < earliestTime)
		{
			earliestRank = currentRank;
			earliestTime = *i;
		}
	}

	static Address address;

	address.setAddress(channelID,earliestRank,0,0,0);

	Transaction *newRefreshTransaction = new Transaction(AUTO_REFRESH_TRANSACTION, earliestTime, 8,address, 0, 0, UINT_MAX);

	refreshCounter[earliestRank] = refreshCounter[earliestRank] + timingSpecification.tREFI();

	return newRefreshTransaction;

#if 0
	Transaction *earliestTransaction = refreshCounter[0];

	unsigned earliestRank = 0;

	for (unsigned i = 1; i < rank.size(); ++i)
	{
		assert(refreshCounter[i]);
		if (refreshCounter[i]->getArrivalTime() < earliestTransaction->getArrivalTime())
		{
			earliestTransaction = refreshCounter[i];
			earliestRank = i;
		}
	}

	static Address address;

	address.setChannel(channelID);
	address.setRank(earliestRank);
	address.setBank(0);

	refreshCounter[earliestRank] = new Transaction(AUTO_REFRESH_TRANSACTION,earliestTransaction->getEnqueueTime() + timingSpecification.tREFI(),8,address,NULL);

	refreshCounter[earliestRank]->setEnqueueTime(refreshCounter[earliestRank]->getArrivalTime());

	return earliestTransaction;
#endif
}

//////////////////////////////////////////////////////////////////////////
/// @brief returns a pointer to the next refresh transaction that's going to be issued to this channel
/// @detail returns a pointer to a representative object for the next refresh that this channel will see
/// @return a pointer to a representative copy of the next refresh transaction
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
const Transaction *Channel::readNextRefresh() const
{
	assert(rank.size() >= 1);

	unsigned currentRank = 0;
	unsigned earliestRank = 0;
	tick earliestTime = refreshCounter[0];
	for (vector<tick>::const_iterator i = refreshCounter.begin(); i != refreshCounter.end(); i++, currentRank++)
	{
		if (*i < earliestTime)
		{
			earliestRank = currentRank;
			earliestTime = *i;
		}
	}

	static Address address;

	address.setAddress(channelID,earliestRank,0,0,0);

	static Transaction newRefreshTransaction;
	::new(&newRefreshTransaction)Transaction(AUTO_REFRESH_TRANSACTION, 0, 8, address, 0, 0, UINT_MAX);

	return &newRefreshTransaction;

}


//////////////////////////////////////////////////////////////////////
/// @brief determines if there are enough command slots for the transaction to be decoded
/// @details this will ensure that a given transaction can be broken into and inserted as commands in this channel
/// if there is not enough room according to a given algorithm, then it will indicate that this is not possible
/// @author Joe Gross
/// @param trans the transaction to be considered
/// @return true if there is enough room, false otherwise
//////////////////////////////////////////////////////////////////////
bool Channel::checkForAvailableCommandSlots(const Transaction *incomingTransaction) const
{
	if (incomingTransaction == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (incomingTransaction->getAddresses().getChannel() == channelID || incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION);

	const Bank &destinationBank = rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()];

	unsigned availableCommandSlots = (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION) ? 0 : rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()].freeCommandSlots();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	
		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			const Rank &destinationRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = destinationRank.bank.begin(); currentBank != destinationRank.bank.end(); currentBank++)
			{
				if (currentBank->isFull())
					return false;
			}
		}
		// every transaction translates into at least two commands
		else if (availableCommandSlots < 2)
		{
			return false;
		}
		// or three commands if the CAS+Precharge command is not available
		else if (!systemConfig.isAutoPrecharge() && (availableCommandSlots < 3))
		{
			return false;
		}
		break;
	case CLOSE_PAGE_OPTIMIZED:
		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			const Rank &destinationRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = destinationRank.bank.begin(); currentBank != destinationRank.bank.end(); currentBank++)
			{
				if (currentBank->isFull())
					return false;
			}
		}
		// need at least one free command slot
		else if (destinationBank.isFull())			
		{
			return false;
		}
		else if (destinationBank.back() 
			&& destinationBank.back()->getAddress().getRow() == incomingTransaction->getAddresses().getRow() // rows match
			&& (time - destinationBank.back()->getEnqueueTime() < systemConfig.getSeniorityAgeLimit()) // not starving the last command
			&& destinationBank.back()->getCommandType() != REFRESH_ALL) // ends with CAS+P or PRE
		{
			return true;
		}		
		else if ((!systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 3) ||
			(systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 2))
		{
			return false;
		}
		else
		{
			return true;
		}

		break;

		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be delayed

		// all break down into PRE,RAS,CAS
		// or CAS
	case OPEN_PAGE:
		{		
			// refresh transactions become only one command and are handled differently
			if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
			{
				const Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
				// make sure that there is room in all the queues for one command
				// refresh commands refresh a row, but kill everything currently in the sense amps
				// therefore, we need to make sure that the refresh commands happen when all banks
				// are available
				for (vector<Bank>::const_iterator currentBank = currentRank.bank.begin();
					currentBank != currentRank.bank.end();
					currentBank++)
				{					
					if (currentBank->isFull())
						return false;
				}
				return true;
			}
			// look in the bank_q and see if there's a precharge for this row
			bool bypass_allowed = true;

			// go from tail to head
			for (int tail_offset = rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()].size() - 1; (tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
			{	
				const Command *currentCommand = destinationBank.read(tail_offset);

				if (time - currentCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
				{
					break;
				}
				// goes right before the PRE command to ensure that the original order is preserved
				else if ((currentCommand->getCommandType() == PRECHARGE) &&
					(currentCommand->getAddress().getRow() == incomingTransaction->getAddresses().getRow()))
				{
					// can piggyback on other R-C-C...C-P sequences
					if (availableCommandSlots < 1)
						return false;

					return true;			
				}

				// even STRICT ORDER allows you to look at the tail of the queue
				// to see if that's the precharge command that you need. If so,
				// insert CAS COMMAND in front of PRECHARGE COMMAND

				else if (systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER) 
				{
					//bypass_allowed = false;
					break;
				}
			}
			if (availableCommandSlots < 3)
			{
				return false;
			}
		}
		break;

	default:

		cerr << "Unhandled row buffer management policy" << endl;
		return false;
		break;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
/// @brief converts a transaction into corresponding commands
/// @details takes a transaction and divides it into a number of commands, depending which row buffer management policy is chosen\n
/// currently will divide it into RAS, CAS, Pre or RAS, CAS+P and insert these commands into the per bank queues\n
/// the CAS command will contain a pointer to the host transaction to indicate that a read transaction has available data
/// @author Joe Gross
/// @param incomingTransaction the transaction which is divided up
/// @return true if the transaction was able to be divided up and put into per bank queues
//////////////////////////////////////////////////////////////////////
bool Channel::transaction2commands(Transaction *incomingTransaction) 
{
	if (incomingTransaction == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (incomingTransaction->getAddresses().getChannel() == channelID || incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION);

	Bank &destinationBank = rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()];

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = currentRank.bank.begin(); currentBank != currentRank.bank.end(); currentBank++)
			{
				if (currentBank->freeCommandSlots() < 1)
					return false;
			}
			// then add the command to all queues
			Command *refreshCommand = new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst());

			for (vector<Bank>::iterator currentBank = currentRank.bank.begin(); currentBank != currentRank.bank.end(); currentBank++)
			{
				bool result = currentBank->push(refreshCommand);
				assert (result);
			}
		}
		// every transaction translates into at least two commands
		else if (destinationBank.freeCommandSlots() < 2)
		{
			return false;
		}
		// or three commands if the CAS+Precharge command is not available
		else if (!systemConfig.isAutoPrecharge() && (destinationBank.freeCommandSlots() < 3))
		{
			return false;
		}
		// if there is enough space to insert the commands that this transaction becomes
		else
		{
			// close page always misses
			statistics.reportMiss();

			// command one, the RAS command to activate the row
			destinationBank.push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst(), ACTIVATE));

			// command two, CAS or CAS+Precharge
			destinationBank.push(new Command(*incomingTransaction, time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst()));

			// if CAS+Precharge is unavailable
			if (!systemConfig.isAutoPrecharge())
			{
				// command three, the Precharge command
				// only one of these commands has a pointer to the original transaction, thus NULL
				destinationBank.push(new Command(*incomingTransaction, time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst(), PRECHARGE));
			}
		}
		break;

		// will either convert a CAS+P into CAS, CAS+P by appending a new command or will add a CAS before a PRE (when autoprecharge is not available)
	case CLOSE_PAGE_OPTIMIZED:

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = currentRank.bank.begin(); currentBank != currentRank.bank.end(); currentBank++)
			{
				if (currentBank->isFull())
					return false;
			}
			// then add the command to all queues
			Command *refreshCommand = new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst());

			for (vector<Bank>::iterator currentBank = currentRank.bank.begin(); currentBank != currentRank.bank.end(); currentBank++)
			{
				bool result = currentBank->push(refreshCommand);
				assert (result);
			}
		}
		// need at least one free command slot
		// or three commands if the CAS+Precharge command is not available
		else if (destinationBank.isFull())			
		{
			return false;
		}
		// then it can be piggybacked on an existing R, C, P or R, C+P chain
		// also make sure not to starve		
		// R C1 P => R C1 C2 P
		// R C1+P => R C1 C2+P 
		// TODO: look for the last non-refresh command in the per-bank queue
		else
		{
			if (destinationBank.closePageOptimalInsert(incomingTransaction,time))
			{
				statistics.reportHit();
				return true;
			}
			/*if (destinationBank.back()
				&& destinationBank.back()->getAddress().getRow() == incomingTransaction->getAddresses().getRow() // rows match
				&& (time - destinationBank.back()->getEnqueueTime() < systemConfig.getSeniorityAgeLimit()) // not starving the last command
				&& destinationBank.back()->getCommandType() != REFRESH_ALL) // ends with CAS+P or PRE
			{
				// found existing command to piggyback on
				statistics.reportHit();

				// ignore other command types
				if (systemConfig.isAutoPrecharge())
				{
					// adjust existing commands
					destinationBank.back()->setAutoPrecharge(false);

					// insert new command
					destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst()));				
				}
				else // no CAS+P available
				{
					assert(destinationBank.back()->getCommandType() == PRECHARGE);

					// insert new command
					destinationBank.insert(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst()), destinationBank.size() - 1);

				}
			}*/

			// do normal insertion if optimal insertion doesn't work
			if ((!systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 3) ||
				(systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 2))
			{
				return false;
			}
			// if there is enough space to insert the commands that this transaction becomes
			else
			{
				// didn't find place to issue command
				statistics.reportMiss();

				// command one, the RAS command to activate the row
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst(), ACTIVATE));

				// command two, CAS or CAS+Precharge			
				destinationBank.push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst()));

				// possible command three, Precharge
				if (!systemConfig.isAutoPrecharge())
				{				
					destinationBank.push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), timingSpecification.tBurst(), PRECHARGE));
				}
			}		
		}
		break;

		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be pushed back one slot
	case OPEN_PAGE:

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				if (i->isFull())
					return false;
			}
			// then add the command to all queues
			Command *refreshCommand = new Command(*incomingTransaction, time, systemConfig.isPostedCAS(),systemConfig.isAutoPrecharge(), timingSpecification.tBurst());

			for (vector<Bank>::iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				bool result = i->push(refreshCommand);
				//bool result = i->push(new Command(incomingTransaction->getAddresses(), REFRESH_ALL, time, incomingTransaction, false));
				assert (result);
			}
		}
		else 
		{	
			// try to do a normal open page insert on this transaction
			if (destinationBank.openPageInsert(incomingTransaction, time))
			{
				// found place to insert, hit
				statistics.reportHit();

				incomingTransaction->setDecodeTime(time);
				return true;
			}
			else
			{
				// did not find place to insert
				statistics.reportMiss();

				// even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge
				// command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND
				// also prevent against starvation
#if 0
				if ((systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER) 
					|| ((time - temp_c->getEnqueueTime()) > (int)systemConfig.getSeniorityAgeLimit()))
				{
					break;
				}
#endif

				// if this command was not able to be combined with another, create a new series of commands
				if (destinationBank.freeCommandSlots() < 3)
				{
					return false;
				}

				// RAS command
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), false, timingSpecification.tBurst(), ACTIVATE));

				// CAS command
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(),false, timingSpecification.tBurst()));

				// last, the precharge command
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(),false, timingSpecification.tBurst(),PRECHARGE));
			}
		}
		break;


	default:

		cerr << "Unhandled row buffer management policy" << endl;
		return false;

	}

	incomingTransaction->setDecodeTime(time);

	return true;
}


//////////////////////////////////////////////////////////////////////
/// @brief chooses and dequeues the next command to execute
/// @details Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned has already been removed from the per_bank_command_queue
/// from which it was selected
/// @author Joe Gross
/// @return a pointer to the next command
//////////////////////////////////////////////////////////////////////
Command *Channel::getNextCommand(const Command *useThisCommand)
{
	// populate the cache if need be
	if (useThisCommand == NULL)
		useThisCommand = readNextCommand();

	if (useThisCommand)
	{
		Rank &currentRank = rank[useThisCommand->getAddress().getRank()];

		// if it was a refresh all command, then dequeue all n banks worth of commands
		if (useThisCommand->getCommandType() == REFRESH_ALL)
		{			
			for (vector<Bank>::iterator currentBank = currentRank.bank.begin();true;)
			{
				Command *tempCommand = currentBank->pop();

				assert(tempCommand->getCommandType() == REFRESH_ALL);
				assert(tempCommand != NULL);

				currentBank++;

				if (currentBank == currentRank.bank.end())
				{
					return tempCommand;
				}				
			}
		}
		else
		{			
			assert(currentRank.bank[useThisCommand->getAddress().getBank()].front() == useThisCommand);
			Command *tempCommand = currentRank.bank[useThisCommand->getAddress().getBank()].pop();
			// command is being removed, clear the cache
			assert(tempCommand && tempCommand == useThisCommand);
			return tempCommand;
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
/// the command returned may not be able to be executed yet, so it is important to check this
/// command is not actually removed from the per bank queues
/// @author Joe Gross
/// @return a const pointer to the next available command
//////////////////////////////////////////////////////////////////////
const Command *Channel::readNextCommand() const
{	
	switch (systemConfig.getCommandOrderingAlgorithm())
	{
	case GREEDY:
		{
			const Command *candidateCommand = NULL;

			tick candidateExecuteTime = TICK_MAX;

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				bool isRefreshCommand = true;

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
				{
					const Command *challengerCommand = currentBank->front();

					if (isRefreshCommand && challengerCommand && challengerCommand->getCommandType() == REFRESH_ALL && currentRank->refreshAllReady())
					{
						tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifdef DEBUG
						int minGap = minProtocolGap(challengerCommand);

						if (time + minGap != challengerExecuteTime)
							assert(max(time + minGap, (tick)0) == challengerExecuteTime);
#endif
						if (challengerExecuteTime < candidateExecuteTime || (candidateExecuteTime == challengerExecuteTime && challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))
						{						
							candidateCommand = challengerCommand;
							// stop searching since all the queues are proved to have refresh commands at the front
							break;
						}						
					}
					else
					{
						// can ignore refresh commands since it's known that not all the queues have a ref command at the front
						if (challengerCommand && challengerCommand->getCommandType() != REFRESH_ALL)
						{
							tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifdef DEBUG
							int minGap = minProtocolGap(challengerCommand);

							if (time + minGap != challengerExecuteTime)
								assert(max(time + minGap, (tick)0) == challengerExecuteTime);
#endif
							// set a new candidate if the challenger can be executed sooner or execution times are the same but the challenger is older
							if (challengerExecuteTime < candidateExecuteTime || (candidateExecuteTime == challengerExecuteTime && challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))
							{								
								candidateExecuteTime = challengerExecuteTime;
								candidateCommand = challengerCommand;															
							}
						}
					}

					// if it was a refresh command was chosen then it wouldn't make it this far, so it's not a refresh command
					// if a refresh command wasn't chosen then there one can't be found later
					isRefreshCommand = false;
				}				
			}

			if (candidateCommand)
			{
				assert(candidateCommand->getCommandType() == REFRESH_ALL || rank[candidateCommand->getAddress().getRank()].bank[candidateCommand->getAddress().getBank()].front() == candidateCommand);

#ifdef DEBUG_GREEDY
				timingOutStream << "R[" << candidateCommand->getAddress().rank << "] B[" << candidateCommand->getAddress().bank << "]\tWinner: " << *candidateCommand << "gap[" << candidateGap << "] now[" << time << "]" << endl;
#endif
			}

			return candidateCommand;
		}
		break;
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
						const tick executeTime = earliestExecuteTime(challengerCommand);
#ifdef DEBUG
						assert(executeTime == time + minProtocolGap(challengerCommand));
#endif
						// choose the oldest command that can be executed
						if ((challengerCommand->getEnqueueTime() < oldestExecutableCommandTime) && (executeTime <= time))
						{
							// if it's a refresh_all command and we haven't proved that all the queues aren't refresh_all commands, search
							if (challengerCommand->getCommandType() == REFRESH_ALL)
							{
								if (!notAllRefresh)
								{
									// try to show that at the head of each queue isn't a refresh command
									for (vector<Bank>::const_iterator thisBank = currentRank->bank.begin(); thisBank != currentRank->bank.end(); thisBank++)
									{
										// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
										if (thisBank->nextCommandType() != REFRESH_ALL)
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
								if (challengerCommand->getCommandType() == REFRESH_ALL)
								{
									if (!notAllRefresh)
									{
										// try to show that at the head of each queue isn't a refresh command
										for (vector<Bank>::const_iterator thisBank = currentRank->bank.begin(); thisBank != currentRank->bank.end(); thisBank++)
										{
											// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
											if (thisBank->nextCommandType() != REFRESH_ALL)
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
							// 							else
							// 							{
							// 								oldestExecutableCommandTime = challengerCommand->getEnqueueTime();
							// 								oldestExecutableBank = currentBank;
							// 							}
						}
					}
				}
			}

			// if any executable command was found, prioritize it over those which must wait
			if (oldestExecutableCommandTime < TICK_MAX)
			{
				assert(oldestExecutableBank->nextCommandType() == REFRESH_ALL || rank[oldestExecutableBank->front()->getAddress().getRank()].bank[oldestExecutableBank->front()->getAddress().getBank()].front() == oldestExecutableBank->front());

				return oldestExecutableBank->front();
			}
			// if there was a command found but is not yet ready to execute
			else if (oldestCommandTime < TICK_MAX)
			{
				assert(oldestBank->front()->getCommandType() == REFRESH_ALL || rank[oldestBank->front()->getAddress().getRank()].bank[oldestBank->front()->getAddress().getBank()].front() == oldestBank->front());

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
			// look at the most recently retired command in this channel's history
			const Command *lastCommand = historyQueue.back();

			const unsigned lastBankID = lastCommand ? lastCommand->getAddress().getBank() : systemConfig.getBankCount() - 1;
			const unsigned lastRankID = lastCommand ? lastCommand->getAddress().getRank() : systemConfig.getRankCount() - 1;
			const CommandType lastCommandType = lastCommand ? lastCommand->getCommandType() : WRITE_AND_PRECHARGE;

			// attempt to group RAS/CAS pairs together
			switch (lastCommandType)
			{
				// if it was RAS before and you want to finish doing the read/write
			case ACTIVATE:
				{
					// look at the command just after the RAS, it should be some sort of CAS
					const Command *nextCommand = rank[lastRankID].bank[lastBankID].front();

					if ((nextCommand) &&
						(nextCommand->isRead() || nextCommand->isWrite()))
					{
						return nextCommand;
					}
					else
					{
						cerr << "Serious problems. RAS not followed by CAS" << endl;
						exit(2);
					}
				}
				break;

			case READ_AND_PRECHARGE:
			case READ:
			case PRECHARGE:
				transactionType = READ_TRANSACTION;
				break;

			case REFRESH_ALL:
			case WRITE:
			case WRITE_AND_PRECHARGE:
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
							if (currentRank->refreshAllReady())
							{
								return currentRank->bank[currentBankID].front();
							}
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
								return  NULL;
							else
								allowNotReadyCommands = true;
						}
					}
				}

				const Command *potentialCommand = rank[currentRankID].bank[currentBankID].front();

				// refresh commands are considered elsewhere
				if (potentialCommand && potentialCommand->getCommandType() != REFRESH_ALL && (allowNotReadyCommands || (earliestExecuteTime(potentialCommand) <= lastCommandIssueTime + timingSpecification.tCMD() )))
				{
					if (!systemConfig.isReadWriteGrouping())
					{
						return potentialCommand;
					}
					else // have to follow read/write grouping considerations 
					{
						// look at the second command
						const Command *followingCommand = rank[currentRankID].bank[currentBankID].read(1);	

						if (followingCommand)
							if ((followingCommand->isRead() && (transactionType == READ_TRANSACTION)) ||
								(followingCommand->isWrite() && (transactionType == WRITE_TRANSACTION)))
							{
								assert(rank[currentRankID].bank[currentBankID].front()->getAddress().getBank() == currentBankID);
								assert(rank[currentRankID].bank[currentBankID].front()->getAddress().getRank() == currentRankID);
								return rank[currentRankID].bank[currentBankID].front();
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

			// look at the most recently retired command in this channel's history
			const Command *lastCommand = historyQueue.back();

			const unsigned lastBankID = lastCommand ? lastCommand->getAddress().getBank() : systemConfig.getBankCount() - 1;
			const unsigned lastRankID = lastCommand ? lastCommand->getAddress().getRank() : systemConfig.getRankCount() - 1;
			const CommandType lastCommandType = lastCommand ? lastCommand->getCommandType() : WRITE_AND_PRECHARGE;

			switch (lastCommandType)
			{
			case ACTIVATE:
				{
					const Command *nextCommand = rank[lastRankID].bank[lastBankID].front();

					if ((nextCommand) &&
						(nextCommand->isWrite() || nextCommand->isRead()))
					{
						return nextCommand;
					}
					else
					{
						cerr << "Serious problems. RAS not followed by CAS." << endl;
						exit(2);
					}
				}
				break;
			case READ_AND_PRECHARGE:
			case READ:
			case PRECHARGE:
				transactionType = READ_TRANSACTION;
				break;

			case REFRESH_ALL:
			case WRITE:
			case WRITE_AND_PRECHARGE:
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
			bool allowNotReadyCommands = false;

			while (true)
			{
				// select the next bank
				currentBankID = (currentBankID + 1) % systemConfig.getBankCount();

				// came back to the original bank ID, switch ranks
				if (lastBankID == currentBankID)
				{
					if (!noPendingRefreshes)
					{
						// before switching to the next bank, see if all the queues are refreshes in any rank
						for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
						{
							if (currentRank->refreshAllReady())
							{
								return currentRank->bank[currentBankID].front();
							}
						}
						noPendingRefreshes = true;
					}

					// then switch to the next rank
					currentRankID = (currentRankID + 1) % systemConfig.getRankCount();

					if (lastRankID == currentRankID)
					{
						transactionType = (transactionType == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
						// however, if this type has already been searched for, then there are no commands, so quit
						if (transactionType == originalTransactionType)
						{
							if (allowNotReadyCommands)
								return  NULL;
							else
								allowNotReadyCommands = true;
						}
					}
				}

				const Command *potentialCommand = rank[currentRankID].bank[currentBankID].front();

				if (potentialCommand && potentialCommand->getCommandType() != REFRESH_ALL && (allowNotReadyCommands || (earliestExecuteTime(potentialCommand) <= lastCommandIssueTime + timingSpecification.tCMD() )))
				{	
					if(!systemConfig.isReadWriteGrouping())
					{
						return potentialCommand;
					}
					else // have to follow read_write grouping considerations
					{
						// look at the second command
						const Command *nextCommand = rank[currentRankID].bank[currentBankID].read(1);

						if (nextCommand)
							if ((nextCommand->isRead() && (transactionType == READ_TRANSACTION)) ||
								(nextCommand->isWrite() && (transactionType == WRITE_TRANSACTION)))
							{
								assert(rank[currentRankID].bank[currentBankID].front()->getAddress().getBank() == currentBankID);
								assert(rank[currentRankID].bank[currentBankID].front()->getAddress().getRank() == currentRankID);
								return rank[currentRankID].bank[currentBankID].front();
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
/// Updates the channel time to what it would be had this command been executed
/// Updates the rank and bank records of the most recent RAS, CAS, etc. times
/// Enqueues RAS times to allow t_faw to be determined later
/// Updates rank and bank records of CAS, RAS lengths for later calculations in 
/// min_protocol_gap()
/// </summary>
/// <param name="this_command">The command to execute against the current state</param>
void Channel::executeCommand(Command *thisCommand)
{
	Rank &currentRank = rank[thisCommand->getAddress().getRank()];

	Bank &currentBank = currentRank.bank[thisCommand->getAddress().getBank()];

	currentRank.setLastBankID(thisCommand->getAddress().getBank());

	//int t_al = this_command->isPostedCAS() ? timingSpecification.tAL() : 0;

	// update the channel's idea of what time it is and set the start time for the command
	// ensure that the command is never started before it is enqueued
	// this implies that if there was an idle period for the memory system, commands
	// will not be seen as executing during this time	

	//thisCommand->setStartTime(max(earliestExecuteTime(thisCommand), thisCommand->getEnqueueTime()));

	thisCommand->setStartTime(time + timingSpecification.tCMD());

	assert(thisCommand->getStartTime() >= earliestExecuteTime(thisCommand));
	assert(thisCommand->getStartTime() >= thisCommand->getEnqueueTime() + timingSpecification.tCMD());

	// set this channel's time to the start time of this command
	//time = thisCommand->getStartTime();
	lastCommandIssueTime = time;

	switch(thisCommand->getCommandType())
	{
	case ACTIVATE:
		{
			currentRank.issueRAS(time, thisCommand);

			// specific for RAS command
			thisCommand->setCompletionTime(thisCommand->getStartTime() + timingSpecification.tCMD() + timingSpecification.tRAS());
			assert(!thisCommand->getHost());
		}
		break;

	case READ_AND_PRECHARGE:

		currentRank.issuePRE(time, thisCommand);
		// lack of break is intentional

	case READ:

		currentRank.issueCAS(time, thisCommand);

		// specific for CAS command
		// should account for tAL buffering the CAS command until the right moment
		//thisCommand->setCompletionTime(max(currentBank.getLastRASTime() + timingSpecification.tRCD() + timingSpecification.tCAS() + timingSpecification.tBurst(), time + timingSpecification.tCMD() + timingSpecification.tCAS() + timingSpecification.tBurst()));
		thisCommand->setCompletionTime(max(currentBank.getLastRASTime() + timingSpecification.tRCD() + timingSpecification.tCAS() + timingSpecification.tBurst(), time + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst()));
		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());
		break;

	case WRITE_AND_PRECHARGE:

		currentRank.issuePRE(time, thisCommand);		
		// missing break is intentional

	case WRITE:

		currentRank.issueCASW(time, thisCommand);

		// for the CAS write command
		//thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());
		thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tCWD() + timingSpecification.tBurst() );
		//thisCommand->getHost()->setCompletionTime(time);
		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());
		break;

	case PRECHARGE:

		currentRank.issuePRE(time, thisCommand);

		thisCommand->setCompletionTime(thisCommand->getStartTime() + timingSpecification.tCMD() + timingSpecification.tRP());

		break;

	case REFRESH_ALL:

		currentRank.issueREF(time, thisCommand);

		thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tRFC());
		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());
		break;

	case RETIRE_COMMAND:
		break;	

	case PRECHARGE_ALL:
		break;

	case ACTIVATE_ALL:
		break;

	case DRIVE_COMMAND:
		break;

	case DATA_COMMAND:
		break;

	case CAS_WITH_DRIVE_COMMAND:
		break;	

	case SELF_REFRESH:
		break;

	case DESELECT:
		break;

	case NOOP:
		break;

	case INVALID_COMMAND:
		break;
	}	

	// inserts into a queue which dequeues into the command pool
	retireCommand(thisCommand);
}


/// <summary>
/// find the protocol gap between a command and current system state
/// </summary>
tick Channel::minProtocolGap(const Command *currentCommand) const
{ 
	tick min_gap = 0;

	const unsigned currentRankID = currentCommand->getAddress().getRank();

	const Rank &currentRank = rank[currentRankID];

	const Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	switch(currentCommand->getCommandType())
	{
	case ACTIVATE:
		{
			// refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk

			// respect the row cycle time limitation
			int tRCGap = (int)(currentBank.getLastRASTime() - time) + timingSpecification.tRC();

			// respect tRRD and tRC of all other banks of same rank
			int tRRDGap;

			if (currentRank.lastActivateTimes.empty())
			{
				tRRDGap = 0;
			}
			else 
			{
				// read tail end of ras history
				const tick lastRASTime = currentRank.lastActivateTimes.front();
				// respect the row-to-row activation delay
				tRRDGap = (int)(lastRASTime - time) + timingSpecification.tRRD();				
			}

			// respect tRP of same bank
			int tRPGap = (int)(currentBank.getLastPrechargeTime() - time) + timingSpecification.tRP();

			// respect the t_faw value for DDR2 and beyond
			int tFAWGap;

			if (!currentRank.lastActivateTimes.full())
			{
				tFAWGap = 0;
			}
			else
			{
				// read head of ras history
				const tick fourthRASTime = currentRank.lastActivateTimes.back(); 
				tFAWGap = (fourthRASTime - time) + timingSpecification.tFAW();
			}

			// respect tRFC
			int tRFCGap = (currentRank.getLastRefreshTime() - time) + timingSpecification.tRFC();

			min_gap = max(max(max(tRFCGap,tRCGap) , tRPGap) , max(tRRDGap , tFAWGap));
		}
		break;

	case READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case READ:
		{
			//respect last ras of same rank
			int tRCDGap = ((currentBank.getLastRASTime() - time) + timingSpecification.tRCD() - timingSpecification.tAL());

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent cas(w) time and length
			for (vector<Rank>::const_iterator thisRank = rank.begin(); thisRank != rank.end(); thisRank++)
			{
				if (thisRank->getRankID() != currentRank.getRankID())
				{
					if (thisRank->getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = thisRank->getLastCASTime();
						otherRankLastCASLength = thisRank->getLastCASLength();
					}
					if (thisRank->getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = thisRank->getLastCASWTime();
						otherRankLastCASWLength = thisRank->getLastCASWLength();
					}
				}
			}			

			//respect last cas of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			int t_cas_gap = ((currentRank.getLastCASTime() - time) + timingSpecification.tBurst());

			//respect last cas write of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			t_cas_gap = max((tick)t_cas_gap,((currentRank.getLastCASWTime() - time) + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR()));

			if (rank.size() > 1) 
			{
				//respect most recent cas of different rank
				t_cas_gap = max((tick)t_cas_gap, (otherRankLastCASTime - time) + otherRankLastCASLength + timingSpecification.tRTRS());
				//respect timing of READ follow WRITE, different ranks
				t_cas_gap = max((tick)t_cas_gap, (otherRankLastCASWTime - time) + timingSpecification.tCWD() + otherRankLastCASWLength + timingSpecification.tRTRS() - timingSpecification.tCAS());
			}
			min_gap = max(tRCDGap,t_cas_gap);

			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will met tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple CAS COMMAND.

	case WRITE:
		{
			//respect last ras of same rank
			int t_ras_gap = ((currentBank.getLastRASTime() - time) + timingSpecification.tRCD() - timingSpecification.tAL());

			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent CAS/CASW time and length
			// FIXME: change to use iterators
			for (unsigned rank_id = 0; rank_id < rank.size() ; rank_id++)
			{
				if (rank_id != currentRankID)
				{
					if (rank[rank_id].getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = rank[rank_id].getLastCASTime();
						otherRankLastCASLength = rank[rank_id].getLastCASLength();
					}
					if (rank[rank_id].getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = rank[rank_id].getLastCASWTime();
						otherRankLastCASWLength = rank[rank_id].getLastCASWLength();
					}
				}
			}
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);

			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			int t_cas_gap = max(0,(int)(currentRank.getLastCASTime() - time) + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas to different ranks
			t_cas_gap = max((tick)t_cas_gap,(otherRankLastCASTime - time) + timingSpecification.tCAS() + otherRankLastCASLength + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			t_cas_gap = max((tick)t_cas_gap,(currentRank.getLastCASWTime() - time) + currentRank.getLastCASWLength());

			// respect last cas write to different ranks
			t_cas_gap = max((tick)t_cas_gap,(otherRankLastCASWTime - time) + otherRankLastCASWLength + timingSpecification.tOST());

			min_gap = max((tick)t_ras_gap,(tick)t_cas_gap);
		}
		break;

	case RETIRE_COMMAND:
		break;

	case PRECHARGE:
		{
			// respect t_ras of same bank
			int t_ras_gap = (currentBank.getLastRASTime() - time) + timingSpecification.tRAS();

			// respect t_cas of same bank
			int t_cas_gap = max(0,((int)(currentBank.getLastCASTime() - time) + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD())));

			// respect t_casw of same bank
			t_cas_gap = max((tick)t_cas_gap,((currentBank.getLastCASWTime() - time) + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR()));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case PRECHARGE_ALL:
		break;

	case ACTIVATE_ALL:
		break;

	case DRIVE_COMMAND:
		break;

	case DATA_COMMAND:
		break;

	case CAS_WITH_DRIVE_COMMAND:
		break;

	case REFRESH_ALL:
		// respect tRFC and tRP
		min_gap = max((currentRank.getLastRefreshTime() + timingSpecification.tRFC()),(currentRank.getLastPrechargeTime() + timingSpecification.tRP())) - time;
		break;

	case INVALID_COMMAND:
		break;

	default:
		break;
	}

	//return max(min_gap,timingSpecification.tCMD());
	return max(min_gap,max(lastCommandIssueTime - time + (tick)timingSpecification.tCMD(),(tick)0));
}

/// @brief Returns the soonest time that this command may execute
/// @details Looks at all of the timing parameters and decides when the this command may soonest execute
tick Channel::earliestExecuteTime(const Command *currentCommand) const
{ 
	tick nextTime;

	const unsigned rankID = currentCommand->getAddress().getRank();

	const Rank &currentRank = rank[rankID];

	const Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	switch(currentCommand->getCommandType())
	{
	case ACTIVATE:
		{
			// refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk

			// respect the row cycle time limitation
			tick tRCLimit = currentBank.getLastRASTime() + timingSpecification.tRC();

			const tick lastRASTime = currentRank.lastActivateTimes.front(); 

			// respect the row-to-row activation delay
			tick tRRDLimit = lastRASTime + timingSpecification.tRRD();				

			// respect tRP of same bank
			tick tRPLimit = currentBank.getLastPrechargeTime() + timingSpecification.tRP();

			// respect the t_faw value for DDR2 and beyond, look at the fourth activate ago
			tick tFAWLimit = currentRank.lastActivateTimes.back() + timingSpecification.tFAW();

			//tFAWLimit = time;

			// respect tRFC, refresh cycle time
			tick tRFCLimit = currentRank.getLastRefreshTime() + timingSpecification.tRFC();

			nextTime = max(max(max(tRFCLimit,tRCLimit) , tRPLimit) , max(tRRDLimit , tFAWLimit));

			assert(nextTime >= currentBank.getLastPrechargeTime() + timingSpecification.tRP());
			//DEBUG_TIMING_LOG(currentCommand->getCommandType() << " ras[" << setw(2) << t_ras_gap << "] rrd[" << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]");
		}
		break;

	case READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case READ:
		{
			//respect last RAS of same rank
			tick tRCDLimit = currentBank.getLastRASTime() + (timingSpecification.tRCD() - timingSpecification.tAL());

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent cas(w) time and length
			for (vector<Rank>::const_iterator thisRank = rank.begin(); thisRank != rank.end(); thisRank++)
			{
				if (thisRank->getRankID() != currentRank.getRankID())
				{
					if (thisRank->getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = thisRank->getLastCASTime();
						otherRankLastCASLength = thisRank->getLastCASLength();
					}
					if (thisRank->getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = thisRank->getLastCASWTime();
						otherRankLastCASWLength = thisRank->getLastCASWLength();
					}
				}
			}			

			// respect last CAS of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			tick tCASLimit = currentRank.getLastCASTime() + timingSpecification.tBurst();

			// respect last CASW of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			tCASLimit = max(tCASLimit,currentRank.getLastCASWTime() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR());

			if (rank.size() > 1) 
			{
				//respect most recent CAS of different rank
				tCASLimit = max(tCASLimit, otherRankLastCASTime + otherRankLastCASLength + timingSpecification.tRTRS());
				//respect timing of READ follow WRITE, different ranks
				tCASLimit = max(tCASLimit, otherRankLastCASWTime + timingSpecification.tCWD() + otherRankLastCASWLength + timingSpecification.tRTRS() - timingSpecification.tCAS());
			}

			nextTime = max(tRCDLimit,tCASLimit);

			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will meet tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple WRITE.

	case WRITE:
		{
			//respect last RAS of same rank
			tick tRASLimit = currentBank.getLastRASTime() + timingSpecification.tRCD() - timingSpecification.tAL();

			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent CAS/CASW time and length
			for (vector<Rank>::const_iterator rankIndex = rank.begin(); rankIndex != rank.end(); rankIndex++)
			{
				if (rankID != rankIndex->getRankID())
				{
					if (rankIndex->getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = rankIndex->getLastCASTime();
						otherRankLastCASLength = rankIndex->getLastCASLength();
					}
					if (rankIndex->getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = rankIndex->getLastCASWTime();
						otherRankLastCASWLength = rankIndex->getLastCASWLength();
					}
				}
			}
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);

			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			tick tCASLimit = max(time,currentRank.getLastCASTime() + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas to different ranks
			tCASLimit = max(tCASLimit,otherRankLastCASTime + timingSpecification.tCAS() + otherRankLastCASLength + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			tCASLimit = max(tCASLimit,currentRank.getLastCASWTime() + currentRank.getLastCASWLength());

			// respect last CASW to different ranks
			// TODO: should this not also be -tAL?
			tCASLimit = max(tCASLimit,otherRankLastCASWTime + otherRankLastCASWLength + timingSpecification.tOST());

			nextTime = max(tRASLimit,tCASLimit);
		}
		break;

	case PRECHARGE:
		{
			// respect t_ras of same bank
			tick tRASLimit = currentBank.getLastRASTime() + timingSpecification.tRAS();

			// respect t_cas of same bank
			// TODO: do not need tAL for these
			//tick tCASLimit = max(time,currentBank.getLastCASTime() + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tick tCASLimit = max(time,currentBank.getLastCASTime() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));

			// respect t_casw of same bank
			//tCASLimit = max(tCASLimit,currentBank.getLastCASWTime() + timingSpecification.tAL() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tCASLimit = max(tCASLimit,currentBank.getLastCASWTime() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());

			nextTime = max(tRASLimit,tCASLimit);
		}
		break;

	case REFRESH_ALL:
		// respect tRFC and tRP
		nextTime = max(currentRank.getLastRefreshTime() + timingSpecification.tRFC(), currentRank.getLastPrechargeTime() + timingSpecification.tRP());
		break;

	case RETIRE_COMMAND:
	case PRECHARGE_ALL:
	case ACTIVATE_ALL:
	case DRIVE_COMMAND:
	case DATA_COMMAND:
	case CAS_WITH_DRIVE_COMMAND:
	case SELF_REFRESH:
	case DESELECT:
	case NOOP:
	case INVALID_COMMAND:
	default:
		cerr << "Unsupported command encountered." << endl;
		nextTime = 0;
		break;
	}

	//return max(nextTime, time + timingSpecification.tCMD());
	//return max(nextTime, max(time, lastCommandIssueTime + timingSpecification.tCMD()));
	return max(nextTime, max(time , lastCommandIssueTime + timingSpecification.tCMD()));
}


//////////////////////////////////////////////////////////////////////////
/// @brief the assignment operator, will copy non-key items to this channel
/// @detail copies the non-reference items over, should be used for deserialization
/// @return a reference to this channel, for chaining
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel& Channel::operator =(const Channel &rhs)
{
	//Settings settings;
	//::new(this)DRAMsimII::Channel(settings,rhs.systemConfig,rhs.statistics, rhs.powerModel, rhs.rank, rhs.timingSpecification);
	time = rhs.time;
	lastRefreshTime = rhs.lastRefreshTime;
	lastCommandIssueTime = rhs.lastCommandIssueTime;
	lastRankID = rhs.lastRankID;
	//timingSpecification = rhs.timingSpecification;
	transactionQueue = rhs.transactionQueue;
	refreshCounter = rhs.refreshCounter;
	historyQueue = rhs.historyQueue;
	completionQueue = rhs.completionQueue;
	//systemConfig = rhs.systemConfig;
	//statistics = rhs.statistics;
	//powerModel = rhs.powerModel;
	//algorithm = rhs.algorithm;
	channelID = rhs.channelID;
	rank = rhs.rank;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
/// @brief the equality operator, to determine if the channels are equal incomingTransaction value
/// @return true if the channels are equal
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Channel::operator ==(const Channel& rhs) const
{
	return (time == rhs.time && lastRefreshTime == rhs.lastRefreshTime && lastCommandIssueTime == rhs.lastCommandIssueTime && lastRankID == rhs.lastRankID &&
		timingSpecification == rhs.timingSpecification && transactionQueue == rhs.transactionQueue && historyQueue == rhs.historyQueue &&
		completionQueue == rhs.completionQueue && systemConfig == rhs.systemConfig && statistics == rhs.statistics && powerModel == rhs.powerModel &&
		algorithm == rhs.algorithm && channelID == rhs.channelID && rank == rhs.rank && refreshCounter == rhs.refreshCounter);
}

//////////////////////////////////////////////////////////////////////////
/// @brief the insertion operator, adds a string to the given stream with stats about this channel
/// @return the input stream but with a string representing the channel state appended
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
std::ostream& DRAMsimII::operator<<(std::ostream& os, const DRAMsimII::Channel& r)
{
	os << "T[" << r.time << "] ch[" << r.channelID << endl;
	os << r.timingSpecification << endl;
	os << r.powerModel << endl;
	return os;
}

