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

#include "Channel.hh"

using std::endl;
using std::setw;
using std::cerr;
using std::hex;
using std::dec;
using std::min;
using std::max;
using std::vector;

using namespace DRAMsimII;

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// @brief constructs the dramChannel using this _settings reference, also makes a reference to the dramSystemConfiguration object
/// @param _settings the _settings file that defines the number of ranks, refresh policy, etc.
/// @param _systemConfig a const reference is made to this for some functions to grab parameters from
/// @param _stats a reference to the _stats object that will be collecting data
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel::Channel(const Settings& _settings, const SystemConfiguration& _systemConfig, Statistics& _stats):
time(0ll),
lastCommandIssueTime(-100ll),
lastCommand(NULL),
timingSpecification(_settings),
transactionQueue(_settings.transactionQueueDepth),
refreshCounter(_settings.rankCount),
systemConfig(_systemConfig),
statistics(_stats),
powerModel(_settings),
channelID(UINT_MAX),
//dimm(_settings.dimmCount, DIMM(_settings,timingSpecification,_systemConfig, statistics)),
rank(_settings.rankCount * _settings.dimmCount, Rank(_settings, timingSpecification, _systemConfig, _stats)),
finishedTransactions(),
cache(_settings.dimmCount, Cache(_settings))
{
	// assign an id to each channel (normally done with commands)
// 	for (unsigned i = 0; i < _settings.rankCount; ++i)
// 	{
// 		rank[i].setRankID(i);
// 	}

	// initialize the refresh counters per rank
	
	if (_settings.refreshPolicy != NO_REFRESH)
	{
		// stagger the times that each rank will be refreshed so they don't all arrive incomingTransaction a burst
		unsigned step = _settings.tREFI / _settings.rankCount;
		Address addr(0,0,0,0,0);
		
		for (unsigned j = 0; j < refreshCounter.size(); ++j)
		{
			addr.setRank(j);

			refreshCounter[j] = new Transaction(Transaction::AUTO_REFRESH_TRANSACTION,
				j * step + 1,
				8,
				addr,
				0,
				0);

			assert(refreshCounter[j]->isRefresh());

			//refreshCounter[j] = j * (step + 1);

		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// normal copy constructor
/// @brief the copy constructor for building unitialized copies of a channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel::Channel(const Channel& rhs, const SystemConfiguration& systemConfig, Statistics& stats):
time(rhs.time),
lastCommandIssueTime(rhs.lastCommandIssueTime),
lastCommand(rhs.lastCommand ? new Command(*rhs.lastCommand) : NULL),
timingSpecification(rhs.timingSpecification),
transactionQueue(rhs.transactionQueue),
refreshCounter(rhs.refreshCounter.size()),
systemConfig(systemConfig),
statistics(stats),
powerModel(rhs.powerModel),
channelID(rhs.channelID),
// to initialize the references
//dimm((unsigned)systemConfig.getDimmCount(), DIMM(rhs.dimm[0], timingSpecification, systemConfig, stats)),
rank((unsigned)systemConfig.getRankCount() * systemConfig.getDimmCount(), Rank(rhs.rank[0], timingSpecification, systemConfig, stats)),
finishedTransactions(),
cache(rhs.cache.size(), Cache(rhs.cache[0]))
{
	// to fill incomingTransaction the values
	rank = rhs.rank;
	
	for (vector<Transaction *>::size_type i = 0; i < refreshCounter.size(); ++i)
	{
		refreshCounter[i] = new Transaction(*rhs.refreshCounter[i]);
	}	
}

//////////////////////////////////////////////////////////////////////////
/// deserialization constructor
/// @brief the constructor to build copies of a channel once it's been deserialized, needs further initialization before it's ready
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel::Channel(const Settings& settings, const SystemConfiguration& sysConf, Statistics &stats, const PowerConfig &power, const std::vector<Rank> &newRank, const TimingSpecification &timing):
time(0),
lastCommandIssueTime(0),
lastCommand(NULL),
timingSpecification(timing),
transactionQueue(0),
refreshCounter(0),
systemConfig(sysConf),
statistics(stats),
powerModel(power),
channelID(UINT_MAX),
rank(newRank),
finishedTransactions(),
cache()
{}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor, reassigns the ordinal to each rank as they are duplicated
/// @param rhs the dramChannel object to be copied
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel::Channel(const Channel &rhs):
time(rhs.time),
lastCommandIssueTime(rhs.lastCommandIssueTime),
lastCommand(rhs.lastCommand),
timingSpecification(rhs.timingSpecification),
transactionQueue(rhs.transactionQueue),
refreshCounter(rhs.refreshCounter.size()),
systemConfig(rhs.systemConfig),
statistics(rhs.statistics),
powerModel(rhs.powerModel),
channelID(rhs.channelID),
//dimm((unsigned)rhs.rank.size(), DIMM(rhs.dimm[0], timingSpecification, systemConfig, statistics)),
rank((unsigned)rhs.rank.size(), Rank(rhs.rank[0], timingSpecification, systemConfig, statistics)),
finishedTransactions(),
cache((unsigned)rhs.cache.size(), Cache(rhs.cache[0]))
{
	// TODO: copy over values incomingTransaction ranks now that reference members are init
	// assign an id to each channel (normally done with commands)
	//dimm = rhs.dimm;
	rank = rhs.rank;

	for (vector<Transaction *>::size_type i = 0; i < refreshCounter.size(); ++i)
	{
		refreshCounter[i] = new Transaction(*rhs.refreshCounter[i]);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief channel destructor
/// @details need to remove all the commands in the queues before destructing the channel
/// because some refresh commands are simultaneously in two or more queues and will be
/// duplicated in the command pool if they are not removed properly
//////////////////////////////////////////////////////////////////////////
Channel::~Channel()
{
#if 1
	// must remove commands this way to prevent queues from being automatically deleted, thus creating double frees on refresh commands
	while (Command *cmd = getNextCommand())
	{
		Transaction *hostTransaction = cmd->removeHost();
		if (hostTransaction != NULL)
			Transaction::release(hostTransaction);

		delete lastCommand;

		lastCommand = cmd;
	}
#endif
	if (lastCommand)
	{
		delete lastCommand;
		lastCommand = NULL;
	}
#if 1
	for (vector<Transaction *>::iterator i = refreshCounter.begin(), end = refreshCounter.end(); i != end; ++i)
	{
		Transaction::release(*i);
		*i = NULL;
	}

	while (Transaction *trans = transactionQueue.pop())
	{
		Transaction::release(trans);
	}
#endif
}

void Channel::setChannelID(const unsigned value)
{
	 channelID = value;
	 unsigned currentId = 0;
	 for (vector<Rank>::iterator i = rank.begin(); i != rank.end(); ++i)
	 {
		 i->setRankID(value, currentId++);
	 }
	 
	 currentId = 0;
// 	 for (vector<DIMM>::iterator i = dimm.begin(), end = dimm.end();
// 		 i != end; ++i)
// 	 {
// 		 i->setDimmId(value, currentId++);
// 	 }
}

//////////////////////////////////////////////////////////////////////////
/// @brief Moves the specified channel to at least the time given
/// @param currentTime issue all events up to and including this time
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
void Channel::moveToTime(const tick currentTime)
{	
	assert(finishedTransactions.empty());

	/// @todo continue until no events are processed, no commands issued, no transactions decoded
	while (time < currentTime)
	{	
		// move time to either when the next command executes or the next transaction decodes, whichever is earlier
		// otherwise just go to the end
#ifndef NDEBUG
		tick oldTime = time;
#endif
		/// @todo verify that this is right
		//time = max(min(currentTime,min(nextTransactionDecodeTime(),min(nextCommandExecuteTime(),nextRefreshTime()))),time);
		time = max(min(currentTime,min(nextTransactionDecodeTime(),nextCommandExecuteTime())),time);
		assert(time <= currentTime);
		assert(time >= oldTime);

		// has room to decode an available transaction, as many as are ready
		// 		unsigned decodedCount = 0;
		// 		unsigned decodedRefreshCount = 0;

		while (Transaction *nextTransaction = getTransaction())
		{
			// actually remove it from the queue now
			//Transaction *decodedTransaction = getTransaction();
			assert(nextTransaction);

			// 			if (nextTransaction->isRefresh())
			// 				decodedRefreshCount++;
			// 			else
			// 				decodedCount++;
			// then break into commands and insert into per bank command queues			
			assert(checkForAvailableCommandSlots(nextTransaction));

#ifndef NDEBUG
			bool t2cResult =
#endif
				transaction2commands(nextTransaction);


			assert(t2cResult);

			nextTransaction->setDecodeTime(time);
			// checkForAvailablecommandSlots() should not have returned true if there was not enough space

			DEBUG_TRANSACTION_LOG("T->C [" << std::dec << time << "] Q[" << std::dec << getTransactionQueueCount() << "/" << std::dec << transactionQueue.depth() << "]->[" << std::dec <<
				rank[nextTransaction->getAddress().getRank()].bank[nextTransaction->getAddress().getBank()].size() << "/" << std::dec <<
				rank[nextTransaction->getAddress().getRank()].bank[nextTransaction->getAddress().getBank()].depth() << "] " << *nextTransaction);

			//nextToDecode = readAvailableTransaction();
			//nextTransaction = readTransaction(true);
		}		
#ifndef NDEBUG
		//if (decodedCount > 3 || decodedRefreshCount > 1)
		//cerr << "decoded " << decodedCount << "/" << decodedRefreshCount << endl;
#endif

		// execute commands for this time, reevaluate what the next command is since this may have changed after decoding the transaction
		const Command *nextCommand = readNextCommand();

		while (nextCommand && (earliestExecuteTime(nextCommand) <= time))
		{
			Command *executingCommand = getNextCommand(nextCommand);

			assert(executingCommand == nextCommand);

			assert(earliestExecuteTimeLog(nextCommand) <= time);

			executeCommand(executingCommand);	

			DEBUG_COMMAND_LOG("C " << *executingCommand);

#ifndef NDEBUG
			printVerilogCommand(executingCommand);
#endif

			nextCommand = readNextCommand();
		}
	}

	//transFinishTime = currentTime;
	//M5_TIMING_LOG("ch[" << channelID << "] @ " << dec << time);
}


//////////////////////////////////////////////////////////////////////////
/// @brief determines when the next transaction is decoded, command ready to be executed or next refresh command arrives
/// @return the time when the next event happens
/// @sa readTransactionSimple() readNextCommand() earliestExecuteTime() checkForAvailableCommandSlots() nextRefreshTime()
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
tick Channel::nextTick() const
{
	return max(min(min(nextTransactionDecodeTime(),nextCommandExecuteTime()),nextRefreshTime()),time + 1);
}


//////////////////////////////////////////////////////////////////////////
/// @brief determine when the next transaction incomingTransaction the queue will be decoded
/// @return the time when the decoding will be complete
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
tick Channel::nextTransactionDecodeTime() const
{	
	if (const Transaction *nextTrans = readTransaction(false))
	{
		if (nextTrans)
		{
			if (nextTrans->isRefresh())
				return nextTrans->getArrivalTime();
			else
				return nextTrans->getEnqueueTime() + timingSpecification.tBufferDelay();
		}
	}

	return TICK_MAX;
}

//////////////////////////////////////////////////////////////////////////
/// @brief return any transactions that finished when moveToTime() was called
/// @details moves results from the internal queue to the given queue
/// @returns a queue that has the results in transactionID, finished time pairs
//////////////////////////////////////////////////////////////////////////
void Channel::getPendingTransactions(std::queue<std::pair<unsigned,tick> > &outputQueue)
{
	while (!finishedTransactions.empty())
	{
		outputQueue.push(finishedTransactions.front());
		finishedTransactions.pop();
	}
}


//////////////////////////////////////////////////////////////////////////
/// @brief determines the next time available for a command to issue
/// @return the next time an event occurs on this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
tick Channel::nextCommandExecuteTime() const
{
	// then check to see when the next command occurs
	if (const Command *tempCommand = readNextCommand())
	{
		tick tempCommandExecuteTime = earliestExecuteTime(tempCommand);
#ifndef NDEBUG
		int tempGap = minProtocolGap(tempCommand);

		if (time + tempGap != tempCommandExecuteTime)
			cerr << time << " " << tempGap << " " << tempCommandExecuteTime << " " << tempCommand->getCommandType() << " " << tempCommand->getAddress() << endl;
		;
		assert(time + tempGap == tempCommandExecuteTime);
#endif
		return tempCommandExecuteTime;
	}
	else
		return TICK_MAX;
}


//////////////////////////////////////////////////////////////////////////
/// @brief get the next refresh time
/// @return the time at which the next refresh should be issued for this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
tick Channel::nextRefreshTime() const
{
	assert(rank.size() >= 1);

	if (systemConfig.getRefreshPolicy() != NO_REFRESH)
	{		
		return readNextRefresh()->getArrivalTime();
		//return *(std::min_element(refreshCounter.begin(), refreshCounter.end()));
	}
	else
		return TICK_MAX;
}

//////////////////////////////////////////////////////////////////////////
/// @brief adds this command to the history queue
/// @details this allows other groups to view a recent history of commands that were issued to decide what to execute next
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
void Channel::retireCommand(Command *newestCommand)
{
	statistics.collectCommandStats(newestCommand);

	assert(((newestCommand->isReadOrWrite() || newestCommand->isRefresh()) && newestCommand->getHost()) ||
		(!(newestCommand->isReadOrWrite() || newestCommand->isRefresh()) && !newestCommand->getHost()));

	// transaction complete? if so, put incomingTransaction completion queue
	// note that the host transaction should only be pointed to by a CAS command
	// since this is when a transaction is done from the standpoint of the requester
	if (Transaction *completedTransaction = newestCommand->removeHost())
	{
		DEBUG_TRANSACTION_LOG("-T " << *completedTransaction);

		if (!newestCommand->isRefresh())
		{
			statistics.collectTransactionStats(completedTransaction);

			assert(newestCommand->getEnqueueTime() >= completedTransaction->getEnqueueTime() && 
				newestCommand->getCompletionTime() <= completedTransaction->getCompletionTime());

			const unsigned origTrans = completedTransaction->getOriginalTransaction();

			M5_DEBUG(assert(origTrans < UINT_MAX));

			if (origTrans < UINT_MAX)
				finishedTransactions.push(std::pair<unsigned,tick>(origTrans, completedTransaction->getCompletionTime()));
		}
		else
		{
			assert(systemConfig.getRefreshPolicy() != NO_REFRESH);
		}

		delete completedTransaction;

	}
	assert(!newestCommand->getHost());

	delete lastCommand;

	lastCommand = newestCommand;
}

//////////////////////////////////////////////////////////////////////////
/// @brief enqueue the transaction into the transactionQueue
/// @param incomingTransaction the transaction to be put into the queue
/// @return true if there was room incomingTransaction the queue for this command and the algorithm allowed it, false otherwise
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Channel::enqueue(Transaction *incomingTransaction)
{
	if (transactionQueue.isFull())
		return false;

	incomingTransaction->setEnqueueTime(time);

	assert(!incomingTransaction->isRefresh());

	assert(incomingTransaction->getAddress().getChannel() == channelID);

	bool result;

	switch (systemConfig.getTransactionOrderingAlgorithm())
	{
	case STRICT:
		result = transactionQueue.push(incomingTransaction);
		break;
	case RIFF:
		// search from back to front
		// insert right after a conflict, any transaction over the seniority limit or any read
		if (incomingTransaction->isRead())
		{
			for (int currentIndex = transactionQueue.size() - 1; currentIndex >= 0; --currentIndex)
			{
				// prevent against starvation and RAW errors, reads do not switch their ordering
				if (transactionQueue[currentIndex]->isRead() ||
					(time - transactionQueue[currentIndex]->getEnqueueTime() > systemConfig.getSeniorityAgeLimit()) ||
					transactionQueue[currentIndex]->getAddress() == incomingTransaction->getAddress())
				{
					result = transactionQueue.insert(incomingTransaction, currentIndex + 1);					
					assert(result);
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

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief counts the number of reads and writes so far and returns whichever had more
/// @param rankID which rank to look at when determining which transaction type to choose
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Transaction::TransactionType Channel::setReadWriteType(const int rankID) const
{
	unsigned readCount = 0;
	unsigned writeCount = 0;
	unsigned emptyCount = 0;

	vector<Bank>::const_iterator currentBank = rank[rankID].bank.begin();
	vector<Bank>::const_iterator bankEnd = rank[rankID].bank.end();

	for(; currentBank != bankEnd; ++currentBank)
	{
		if (const Command *currentCommand = currentBank->front())
		{
			if (currentCommand->isActivate())
			{
				currentCommand = currentBank->read(1);
				assert(currentCommand && currentCommand->isReadOrWrite());
			}

			if (currentCommand->isRead())
			{
				readCount++;
			}
			else
			{
				writeCount++;
			}
		}
		else
		{
			emptyCount++;
		}
	}

	DEBUG_LOG("Rank[" << rankID << "] Read[" << readCount << "] Write[" << writeCount << "] Empty[" << emptyCount << "]");

	if (readCount >= writeCount)
		return Transaction::READ_TRANSACTION;
	else
		return Transaction::WRITE_TRANSACTION;
}

//////////////////////////////////////////////////////////////////////
/// @brief performs power calculations for this epoch and cumulative
/// @details calculates the power according to Micron technical note TN-47-04\n
/// also does breakdowns of power consumed per channel and per epoch as well as averaged over time
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void Channel::doPowerCalculation(ostream& os)
{	
	double PsysRD = 0.0;
	double PsysRdAdjusted = 0.0;
	double PsysWR = 0.0;

	double PsysACT_STBY = 0.0;
	double PsysPRE_STBY = 0.0;
	double PsysPRE_PDN = 0.0;
	double PsysACT_PDN = 0.0;
	double PsysACT = 0.0;
	double PsysACTAdjusted = 0.0;

	float tRRDschAdjusted = 0.0F;

	uint64_t totalReadHits = 0;

	os << "+ch[" << channelID << "]";
	
	for (vector<Rank>::iterator k = rank.begin(); k != rank.end(); ++k)
	{
		unsigned thisRankRasCount = 1;
		
		//rankArray.push_back(k->getRankId());

		for (vector<Bank>::iterator l = k->bank.begin(); l != k->bank.end(); ++l)
		{
			thisRankRasCount += l->getRASCount();
			l->accumulateAndResetCounts();
		}

		// what if the RAS could be reduced by specific caching
		unsigned thisRankAdjustedRasCount = thisRankRasCount - statistics.getRowReduction()[getChannelID()][k->getRankId()]; 

		if (thisRankRasCount < statistics.getRowReduction()[getChannelID()][k->getRankId()])
			cerr << thisRankRasCount << " " << statistics.getRowReduction()[getChannelID()][k->getRankId()] << endl;
#if 0
		cerr << "!!! rasCount " << allBankRASCount << " reduxBy " << statistics.getRowReduction()[getChannelID()][k->getRankID()] << 
			" reducedTo " << allBankRASCount - statistics.getRowReduction()[getChannelID()][k->getRankID()] << " totalReadHits " <<
			statistics.getHitRate()[getChannelID()][k->getRankID()].first.first << endl;
#endif
		//totalReadHits += statistics.getHitRate()[getChannelID()][k->getRankId()].first.first;
		totalReadHits += cache[k->getRankId() / systemConfig.getRankCount()].getReadHitsMisses().first;
		BOOST_ASSERT(thisRankAdjustedRasCount >= 0);
		BOOST_ASSERT(thisRankAdjustedRasCount <= thisRankRasCount);

		// FIXME: assumes CKE is always high, so (1 - CKE_LOW_PRE%) = 1
		double percentActive = 1.0 - (k->getPrechargeTime(time) / max((double)(time - powerModel.getLastCalculation()), 0.00000001));

		assert(percentActive >= 0.0F && percentActive <= 1.0F);
		assert(k->getPrechargeTime(time) <= time - powerModel.getLastCalculation());

		/// @todo actually simulate CKE, per rank
		double CKE_LO_PRE = 0.95F;
		double CKE_LO_ACT = 0.01F;

		// calculate background power
		// calculate PsysACT-STBY	
		double PschACT_STBY = powerModel.getPdsACT_STBY() * percentActive * (1 - CKE_LO_ACT);
		PsysACT_STBY += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() *
			powerModel.getFrequencyScaleFactor() * PschACT_STBY;

		//PsysACTSTBYArray.push_back(PsysACT_STBY);

		// calculate PsysPRE-STBY
		double PschPRE_STBY = powerModel.getPdsPRE_STBY() * (1.0 - percentActive) * (1 - CKE_LO_PRE);
		PsysPRE_STBY += powerModel.getDevicesPerRank() * powerModel.getFrequencyScaleFactor() *
			powerModel.getVoltageScaleFactor() * PschPRE_STBY;

		// calculate PsysPRE-PDN
		double PschPRE_PDN = powerModel.getPdsPRE_PDN() * (1.0 - percentActive) * (CKE_LO_PRE);
		PsysPRE_PDN += powerModel.getDevicesPerRank() * powerModel.getFrequencyScaleFactor() *
			powerModel.getVoltageScaleFactor() * PschPRE_PDN;

		// calculate PsysACT-PDN
		double PschACT_PDN = powerModel.getPdsACT_PDN() * percentActive * CKE_LO_ACT;
		PsysACT_PDN += powerModel.getDevicesPerRank() * powerModel.getFrequencyScaleFactor() * 
			powerModel.getVoltageScaleFactor() * PschACT_PDN;

		// calculate PsysACT
		double tRRDsch = ((double)(time - powerModel.getLastCalculation())) / (thisRankRasCount > 0 ? thisRankRasCount : 0.00000001);
	
#ifndef NDEBUG
		cerr << "rrd " << tRRDsch << " " << powerModel.gettRC() << endl;
#endif
		double PschACT = powerModel.getPdsACT() * powerModel.gettRC() / tRRDsch;


		tRRDschAdjusted = ((float)(time - powerModel.getLastCalculation()) / thisRankAdjustedRasCount);
#if 0
		if (tRRDsch > 200.0F)
			cerr << "t=" << time << ", last t=" << powerModel.getLastCalculation() << ", #RAS=" << perRankRASCount << endl;
#endif
		PsysACT += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * PschACT;

		PsysACTAdjusted += powerModel.getDevicesPerRank() * ((double)powerModel.gettRC() / (double)tRRDschAdjusted) * powerModel.getVoltageScaleFactor() * powerModel.getPdsACT();

		//PsysACTArray.push_back(PsysACT);

		double RDschPct = k->getReadCycles() / (double)(time - powerModel.getLastCalculation());

		double RDschPctAdjusted = (k->getReadCycles() - timingSpecification.tBurst() * 
			cache[k->getRankId() / systemConfig.getRankCount()].getReadHitsMisses().first) /
			(double)(time - powerModel.getLastCalculation());

		BOOST_ASSERT(RDschPctAdjusted >= 0.0);

		PsysRD += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsRD() * RDschPct;

		PsysRdAdjusted += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() *
			powerModel.getPdsRD() * RDschPctAdjusted;

		double WRschPct = k->getWriteCycles() / (double)(time - powerModel.getLastCalculation());

		// using a write-through cache, no help for writes

		PsysWR += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsWR() * WRschPct;

		os << " rk[" << k->getRankId() << "] rasCount{" << thisRankRasCount << "} adjRasCount{" << thisRankAdjustedRasCount <<
			"} duration{" << time - powerModel.getLastCalculation() << "} read{" << k->getReadCycles() << "} readHits{" <<
			cache[k->getRankId() / systemConfig.getRankCount()].getReadHitsMisses().first << "} write{" << k->getWriteCycles() << "}";

		k->resetPrechargeTime(time);
		k->resetCycleCounts();
	}
	os << endl;

	os << "-Psys(ACT_STBY) ch[" << channelID << "] {" << setprecision(5) << PsysACT_STBY << "} mW EsysAdjusted {" << setprecision(5) <<
		(PsysRD + PsysWR + PsysACT + PsysACT_STBY + PsysACT_PDN + PsysPRE_STBY + PsysPRE_PDN) * systemConfig.getEpoch() / systemConfig.getDatarate() << "/" <<
		powerModel.getDevicesPerRank() * powerModel.getIDD1() * ((float)timingSpecification.tRC() / systemConfig.getDatarate()) * powerModel.getVDD() * totalReadHits << "} mJ" <<
		endl;
	//Pre(" << k->getPrechargeTime() << "/" << time - powerModel.getLastCalculation() << ")" << endl;

	os << "-Psys(ACT) ch[" << channelID << "] {"<< setprecision(5) << PsysACT << "} mW" << endl;
	//tRRD[" << tRRDsch << "]" <<
		//<<Psys(ACT)adjusted {" << setprecision(5) << PsysACTAdjusted << "} mW" << 
	//	endl;

	os << "-Psys(PRE_STBY) ch[" << channelID << "] {" << setprecision(5) << PsysPRE_STBY << "} mW" << endl;
	//Pre(" << k->getPrechargeTime() << "/" << time - powerModel.getLastCalculation() << ")" << endl;

	os << "-Psys(RD) ch[" << channelID << "] {" << setprecision(5) << PsysRD << "} mW Psys(RD)adjusted {" << 
		setprecision(5) << PsysRdAdjusted << "} mW " << endl;
// 	cerr << PsysRD * systemConfig.getEpoch() / systemConfig.getDatarate() << " " <<
// 		powerModel.getIDD1() << " " <<  ((float)timingSpecification.tRC() / systemConfig.getDatarate()) << " " << powerModel.getVDD() << " " << totalReadHits << endl;

	os << "-Psys(WR) ch[" << channelID << "] {" << setprecision(5) << PsysWR << "} mW" << endl;

	
	//////////////////////////////////////////////////////////////////////////
	// do speculative power calcs that leave out cache hits


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

		float percentActive = 1.0F - (double)(k->getTotalPrechargeTime())/(double)time;

		powerOutStream << "+Psys(ACT_STBY) ch[" << channelID << "] r[" << k->getRankID() << "] {" << setprecision(5) <<
			powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getIDD3N() * powerModel.getVDDmax() * percentActive << "} mW P(" << k->getTotalPrechargeTime() << "/" << time  << ")" << endl;

		float tRRDsch = ((double)time) / perRankRASCount;

		powerOutStream << "+Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] {"<< setprecision(5) <<
			powerModel.getDevicesPerRank() * ((double)powerModel.gettRC() / (double)tRRDsch) * powerModel.getVoltageScaleFactor() * powerModel.getPdsACT() << "} mW" << endl;

		PsysACTTotal += powerModel.getDevicesPerRank() * ((double)powerModel.gettRC() / (double)tRRDsch) * powerModel.getVoltageScaleFactor() * powerModel.getPdsACT();
	}

	double RDschPct = entireCAS * timingSpecification.tBurst() / (double)(time);
	double WRschPct = entireCAS * timingSpecification.tBurst() / (double)(time);

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
/// @brief returns a pointer to the next transaction to issue to this channel without removing it
/// @details read the next available transaction for this channel without actually removing it from the queue \n
/// if bufferDelay is set, then only transactions that have been incomingTransaction the queue long enough to decode are considered
/// @return the next transaction that should be issued to this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
const Transaction *Channel::readTransaction(const bool bufferDelay) const
{
	const unsigned delay = bufferDelay ? timingSpecification.tBufferDelay() : 0;

	unsigned val = readAvailableTransaction(bufferDelay);

	const Transaction *nextTransaction = (val < UINT_MAX) ? transactionQueue[val] : NULL;

	if (systemConfig.getRefreshPolicy() == NO_REFRESH)
	{
		if ((nextTransaction) && (time >= nextTransaction->getEnqueueTime() + delay))
		{
			return nextTransaction;
		}
	}
	else
	{
		const tick nextRefresh = nextRefreshTime();

		// give an advantage to normal transactions, but prevent starvation for refresh operations
		// if there is a normal transaction ready to go, it's ready to go and the refresh command isn't starving
		if (nextTransaction &&
			((time < nextRefresh + systemConfig.getSeniorityAgeLimit()) &&
			(time >= nextTransaction->getEnqueueTime() + delay)))
		{
			return nextTransaction;
		}
		// either there is no transaction or the refresh command will starve
		else if (((time >= nextRefresh) || !bufferDelay) && (checkForAvailableCommandSlots(readNextRefresh())))
		{
			return readNextRefresh();
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// @brief read a transaction that is ready to go, out of order possibly
/// @details searches from the head to the end or the decode window to find
/// a transaction that is able to be decoded at the current time
/// @returns a pointer to the next transaction that can currently be decoded
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
unsigned Channel::readAvailableTransaction(const bool bufferDelay) const
{
	const unsigned delay = bufferDelay ? timingSpecification.tBufferDelay() : 0;

	unsigned limit = min(systemConfig.getDecodeWindow(), transactionQueue.size());

	for (unsigned i = 0; i < limit; ++i)
	{
		const Transaction *currentTrans = transactionQueue[i];
		tick enqueueTime = currentTrans->getEnqueueTime();
		
		// if it's ready to decode
		if ((enqueueTime + delay <= time) &&
			checkForAvailableCommandSlots(currentTrans))
		{
			bool conflict = false;
			// make sure not to create a RAW, WAR, WAW problem
			for (unsigned j = 0; j < i; ++j)
			{
				if (currentTrans->getAddress() == transactionQueue[j]->getAddress())
				{
					conflict = true;
					break;
				}
			}
			if (!conflict)
				return i;
		}
		else if (time - enqueueTime > systemConfig.getSeniorityAgeLimit())
			break;
	}
	return UINT_MAX;
}

//////////////////////////////////////////////////////////////////////////
/// @brief removes and returns the next decodable transaction
/// @sa readAvailableTransaction()
/// @details returns a transaction that can be decoded right now in the same
/// way as readAvailableTransaction(), but actually removes it
//////////////////////////////////////////////////////////////////////////
Transaction *Channel::getAvailableTransaction(unsigned useThis)
{
	if (useThis < UINT_MAX)
		return transactionQueue.remove(useThis);
	else
	{
		unsigned val = readAvailableTransaction(true);
		if (val < UINT_MAX)
			return transactionQueue.remove(val);
		else
			return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief get the next transaction, whether a refresh transaction or a normal R/W transaction
/// @details gets the next transaction for this channel and removes it, always returns a transaction that is able to decode
/// @return the next transaction for this channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Transaction *Channel::getTransaction()
{
	unsigned val = readAvailableTransaction(true);

	const Transaction *nextTransaction = (val < UINT_MAX) ? transactionQueue[val] : NULL;

	// no refresh transactions, just see if normal transactions are decoded
	if (systemConfig.getRefreshPolicy() == NO_REFRESH)
	{
		if ((nextTransaction) && (time >= nextTransaction->getEnqueueTime() + timingSpecification.tBufferDelay()))
		{
			return getAvailableTransaction(val);
		}
	}
	else
	{
		const tick nextRefresh = nextRefreshTime();

		// give an advantage to normal transactions, but prevent starvation for refresh operations
		// if there is a normal transaction ready to go, it's ready to go and the refresh command isn't starving
		if (nextTransaction &&
			((time < nextRefresh + systemConfig.getSeniorityAgeLimit()) &&
			(time >= nextTransaction->getEnqueueTime() + timingSpecification.tBufferDelay())))
		{
			return getAvailableTransaction(val);
		}
		else if ((time >= nextRefresh) && (checkForAvailableCommandSlots(readNextRefresh())))
		{
			return createNextRefresh();
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// @brief get the next refresh command and remove it from the queue
/// @details returns a pointer to a refresh transaction that represents what the next refresh \n
/// transaction would be. this should not be enqueued as it has not been removed yet
/// @return the next possible refresh transaction
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Transaction *Channel::createNextRefresh()
{
	Transaction *newRefreshTransaction = refreshCounter[readNextRefresh()->getAddress().getRank()];
	assert(refreshCounter[newRefreshTransaction->getAddress().getRank()] == newRefreshTransaction);

	unsigned rank = newRefreshTransaction->getAddress().getRank();

	refreshCounter[rank] = new Transaction(Transaction::AUTO_REFRESH_TRANSACTION,
		newRefreshTransaction->getArrivalTime() + timingSpecification.tREFI(),
		8,
		newRefreshTransaction->getAddress(),
		0,
		0);

	//refreshCounter[rank] = refreshCounter[rank] + timingSpecification.tREFI();

	//static Address address;

	//address.setAddress(channelID,earliestRank,0,0,0);

	//::new(&newRefreshTransaction)Transaction(Transaction::AUTO_REFRESH_TRANSACTION, earliestTime, 8, address, 0, 0, UINT_MAX);

	assert(refreshCounter[rank]->isRefresh());
	assert(newRefreshTransaction->isRefresh());

	return newRefreshTransaction;
}

//////////////////////////////////////////////////////////////////////////
/// @brief returns a pointer to the next refresh transaction that's going to be issued to this channel
/// @details returns a pointer to a representative object for the next refresh that this channel will see
/// @return a pointer to a representative copy of the next refresh transaction
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
const Transaction *Channel::readNextRefresh() const
{
	assert(rank.size() >= 1);
	
	vector<Transaction *>::const_iterator i = refreshCounter.begin();
	Transaction *earliestRefresh = *i;
	++i;

	for (vector<Transaction *>::const_iterator end = refreshCounter.end(); i != end; ++i)
	{
		if ((*i)->getArrivalTime() < earliestRefresh->getArrivalTime())
		{
			earliestRefresh = *i;
		}
	}

	assert(earliestRefresh->isRefresh());

	return earliestRefresh;
}

//////////////////////////////////////////////////////////////////////////
/// @brief reset some _stats to account for the fact that fast-forwarding has moved time forward significantly
/// @param time the time at which the timing model begins
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
void Channel::resetToTime(const tick time)
{
	lastCommandIssueTime = time - timingSpecification.tCMD();
	this->time = time;
	powerModel.resetToTime(time);
	// adjust the start time of the refreshes to match the new time
	for (vector<Transaction *>::iterator i = refreshCounter.begin(); i != refreshCounter.end(); ++i)
	{
		(*i)->setArrivalTime((*i)->getArrivalTime() + time);
		//*i = *i + time;
	}

	for (vector<Rank>::iterator i = rank.begin(); i != rank.end(); ++i)
	{
		i->resetToTime(time);
	}

// 	for (vector<DIMM>::iterator i = dimm.begin(), end = dimm.end();
// 		i != end; ++i)
// 	{
// 		i->resetToTime(time);
// 	}
}

//////////////////////////////////////////////////////////////////////
/// @brief determines if there are enough command slots for the transaction to be decoded
/// @details this will ensure that a given transaction can be broken into and inserted as commands in this channel
/// if there is not enough room according to a given algorithm, then it will indicate that this is not possible
/// @author Joe Gross
/// @param incomingTransaction trans the transaction to be considered
/// @return true if there is enough room, false otherwise
//////////////////////////////////////////////////////////////////////
bool Channel::checkForAvailableCommandSlots(const Transaction *incomingTransaction) const
{
	if (incomingTransaction == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (incomingTransaction->getAddress().getChannel() == channelID || incomingTransaction->isRefresh());

	/// @todo switch to iterator arithmetic
	const Bank &destinationBank = rank[incomingTransaction->getAddress().getRank()].bank[incomingTransaction->getAddress().getBank()];

	unsigned availableCommandSlots = (incomingTransaction->isRefresh()) ? 0 : rank[incomingTransaction->getAddress().getRank()].bank[incomingTransaction->getAddress().getBank()].freeCommandSlots();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE_AGGRESSIVE:
	case CLOSE_PAGE:	
		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->isRefresh())
		{
			const Rank &destinationRank = rank[incomingTransaction->getAddress().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = destinationRank.bank.begin(); currentBank != destinationRank.bank.end(); ++currentBank)
			{
				if (currentBank->isFull())
					return false;
			}
		}
		// must know that there is >0 slots open or the result may not be accurate
		else if (systemConfig.getRowBufferManagementPolicy() == CLOSE_PAGE_AGGRESSIVE &&
			destinationBank.closePageAggressiveInsertCheck(incomingTransaction, time))
		{
			return true;
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

		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be delayed

		// all break down into PRE,RAS,CAS
		// or CAS
	case OPEN_PAGE:
	case OPEN_PAGE_AGGRESSIVE:

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->isRefresh())
		{
			const Rank &currentRank = rank[incomingTransaction->getAddress().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); ++i)
			{					
				if (i->freeCommandSlots() < 2)
					return false;
			}
			return true;
		}
		else if (!destinationBank.isFull())
		{	
			// try to do a normal open page insert on this transaction
			if ((systemConfig.getRowBufferManagementPolicy() == OPEN_PAGE_AGGRESSIVE) &&
				(destinationBank.openPageAggressiveInsertCheck(incomingTransaction, time)))
			{					
				return true;
			}
			else
			{
				// first, the precharge command, if necessary
				if (((destinationBank.isEmpty() && destinationBank.isActivated()) || (!destinationBank.isEmpty() && !destinationBank.back()->isRefresh())))
				{
					if (destinationBank.freeCommandSlots() < 3)
					{
						return false;
					}
					assert(!destinationBank.back() || destinationBank.back()->getCommandType() != Command::REFRESH_ALL);
					assert(!destinationBank.back() || destinationBank.back()->getCommandType() != Command::PRECHARGE);
				}
				else if (destinationBank.freeCommandSlots() < 2)
				{
					return false;
				}

				return true;	
			}
		}
		else
		{
			return false;
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
	assert(incomingTransaction->getAddress().getChannel() == channelID || incomingTransaction->isRefresh());
	assert(incomingTransaction->getAddress().getChannel() < systemConfig.getChannelCount());
	assert(incomingTransaction->getAddress().getRank() < (systemConfig.getRankCount() * systemConfig.getDimmCount()));

	vector<Rank>::iterator destinationRank = rank.begin() + incomingTransaction->getAddress().getRank();

	vector<Bank>::iterator destinationBank = destinationRank->bank.begin() + incomingTransaction->getAddress().getBank();

	assert(incomingTransaction->getAddress().getBank() < systemConfig.getBankCount());
	assert(incomingTransaction->getAddress().getRow() < systemConfig.getRowCount());
	assert(incomingTransaction->getAddress().getColumn() < systemConfig.getColumnCount());

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
		// will either convert a CAS+P into CAS, CAS+P by appending a new command or will add a CAS before a PRE (when autoprecharge is not available)
	case CLOSE_PAGE:
	case CLOSE_PAGE_AGGRESSIVE:

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->isRefresh())
		{
			// check to see if every per bank command queue has room for one command
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = destinationRank->bank.begin(); currentBank != destinationRank->bank.end(); ++currentBank)
			{
				if (currentBank->isFull())
					return false;
			}
			// then add the command to all queues
			Command *refreshCommand = new Command(incomingTransaction, time,  systemConfig.isAutoPrecharge(), timingSpecification.tBurst());

			for (vector<Bank>::iterator currentBank = destinationRank->bank.begin(); currentBank != destinationRank->bank.end(); ++currentBank)
			{
#ifndef NDEBUG
				bool result =
#endif // NDEBUG
					currentBank->push(refreshCommand);
				assert(result);
				assert(currentBank->back() == refreshCommand);
			}
		}
		// need at least one free command slot
		// or three commands if the CAS+Precharge command is not available
		else if (systemConfig.getRowBufferManagementPolicy() == CLOSE_PAGE_AGGRESSIVE &&
			destinationBank->aggressiveInsert(incomingTransaction,time))
		{
			statistics.reportRowBufferAccess(incomingTransaction,true);
			//statistics.reportHit();
		}
		// every transaction translates into at least two commands
		// or three commands if the CAS+Precharge command is not available
		else if ((destinationBank->freeCommandSlots() < 2) ||
			(!systemConfig.isAutoPrecharge() && (destinationBank->freeCommandSlots() < 3)))
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
			// didn't find place to issue command
			//statistics.reportMiss();
			statistics.reportRowBufferAccess(incomingTransaction,false);

			// command one, the RAS command to activate the row
#ifndef NDEBUG
			bool result =
#endif
				destinationBank->push(new Command(incomingTransaction, time, false, timingSpecification.tBurst(), Command::ACTIVATE));
			assert(result);
			assert(destinationBank->back()->getAddress() == incomingTransaction->getAddress());

			// command two, CAS or CAS+Precharge			
#ifndef NDEBUG
			result =
#endif
				destinationBank->push(new Command(incomingTransaction, time, systemConfig.isAutoPrecharge(), timingSpecification.tBurst()));
			assert(result);
			assert(destinationBank->back()->getAddress() == incomingTransaction->getAddress());

			// possible command three, Precharge
			if (!systemConfig.isAutoPrecharge())
			{				
#ifndef NDEBUG
				result =
#endif
					destinationBank->push(new Command(incomingTransaction, time, false, timingSpecification.tBurst(), Command::PRECHARGE));
				assert(result);
				assert(destinationBank->back()->getAddress() == incomingTransaction->getAddress());
			}
		}
		break;

		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be pushed back one slot
	case OPEN_PAGE_AGGRESSIVE:
		// look to see if this queue or queues are getting too full and collapse CAS(W), Pre into
		// CAS+P commands to relieve congestion
		if (incomingTransaction->isRefresh())
		{
			vector<Bank>::iterator bankEnd = destinationRank->bank.end();
			// evaluate every per bank queue in this rank and collapse
			for (vector<Bank>::iterator i = destinationRank->bank.begin(); i != bankEnd; ++i)
			{
				if (i->isHighUtilization())
				{
					i->collapse();
				}
			}
		}
		else
		{
			if (destinationBank->isHighUtilization())
			{
				destinationBank->collapse();
			}
		}

		// lack of break it intentional
	case OPEN_PAGE:

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->isRefresh())
		{
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			vector<Bank>::const_iterator bankEnd = destinationRank->bank.end();
			for (vector<Bank>::const_iterator i = destinationRank->bank.begin(); i != bankEnd; ++i)
			{
				if (i->back())
					assert(i->back()->isRefresh() || i->back()->isReadOrWrite());

				// either cannot fit a single REF command or cannot fit Pre + REF
				if (i->isFull() ||
					(i->freeCommandSlots() < 2 && !i->back()->isPrecharge()))
					return false;
			}
			// then add the command to all queues
			Command *refreshCommand = new Command(incomingTransaction, time, false, timingSpecification.tBurst());

			Address tempAddr(incomingTransaction->getAddress());
			unsigned bankNumber = 0;

			for (vector<Bank>::iterator i = destinationRank->bank.begin(); i != bankEnd; ++i)
			{
				// can only refresh banks that are in the precharged state
				if (i->back() && i->back()->isReadOrWrite())
				{
					i->back()->setAutoPrecharge(true);
				}
				// add a Pre command before the REF to flush written data back to the banks before executing a refresh
				else if (i->isEmpty() && i->isActivated())
				{
					// then need to precharge before
					tempAddr.setBank(bankNumber);

					assert(i->freeCommandSlots() >= 2);
#ifndef NDEBUG
					bool result =
#endif // NDEBUG
						i->push(new Command(incomingTransaction, tempAddr, time, false, timingSpecification.tBurst(), Command::PRECHARGE));
					assert(result);
					assert(&(rank[tempAddr.getRank()].bank[bankNumber]) == &*i);
				}
				else
				{
					if (i->isEmpty())
						assert(!i->isActivated());
					else 
						assert(i->back()->isRefresh());
				}
				
#ifndef NDEBUG
				bool result =
#endif // NDEBUG
					i->push(refreshCommand);
				assert (result);
				bankNumber++;
			}

			for (vector<Bank>::const_iterator i = rank[incomingTransaction->getAddress().getRank()].bank.begin(); 
				i != rank[incomingTransaction->getAddress().getRank()].bank.end(); ++i)
			{
				assert(i->back() == refreshCommand);
			}

			return true;
		}
		else if (!destinationBank->isFull())
		{	
			// try to do a normal open page insert on this transaction
			if (destinationBank->aggressiveInsert(incomingTransaction, time))
			{
				// found place to insert, hit
				//statistics.reportHit();
				statistics.reportRowBufferAccess(incomingTransaction,true);
			}
			else
			{
				// first, the precharge command, if necessary
				if (((destinationBank->isEmpty() && destinationBank->isActivated()) || 
					(!destinationBank->isEmpty() && !destinationBank->back()->isRefresh())))
				{
					assert(!destinationBank->back() || !destinationBank->back()->isPrecharge());

					if (destinationBank->freeCommandSlots() < 3) 
					{
						// for aggressive mode, set CAS(W) to CAS(W)+P when there are only two slots left
						if (systemConfig.getRowBufferManagementPolicy() == OPEN_PAGE_AGGRESSIVE &&
							destinationBank->freeCommandSlots() == 2 && 
							destinationBank->back()->isReadOrWrite())
						{
							assert(!destinationBank->back()->isPrecharge());
							assert(destinationBank->back()->isReadOrWrite());
							destinationBank->back()->setAutoPrecharge(true);
						}
						// not enough slots left
						else
						{
							return false;
						}
					}
					else
					{
						assert(!destinationBank->back() || !destinationBank->back()->isRefresh());
						assert(!destinationBank->isFull());
#ifndef NDEBUG
						bool result =
#endif // NDEBUG
							destinationBank->push(new Command(incomingTransaction, time, false, timingSpecification.tBurst(), Command::PRECHARGE));
						assert(result);
					}
				}
				else if (destinationBank->freeCommandSlots() < 2)
				{
					return false;
				}

				// did not find place to insert
				//statistics.reportMiss();
				statistics.reportRowBufferAccess(incomingTransaction,false);

				// RAS command
				assert(!destinationBank->isFull());
#ifndef NDEBUG
				bool result =
#endif // NDEBUG
					destinationBank->push(new Command(incomingTransaction, time, false, timingSpecification.tBurst(), Command::ACTIVATE));
				assert(result);

				// CAS/CASW command
				assert(!destinationBank->isFull());
#ifndef NDEBUG
				result =
#endif // NDEBUG
					destinationBank->push(new Command(incomingTransaction, time, false, timingSpecification.tBurst()));
				assert(result);
			}
		}
		else
		{
			return false;
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
		vector<Rank>::iterator currentRank = rank.begin() + useThisCommand->getAddress().getRank();

		if (rank[useThisCommand->getAddress().getRank()].bank[useThisCommand->getAddress().getBank()].front() != useThisCommand)
			assert(rank[useThisCommand->getAddress().getRank()].bank[useThisCommand->getAddress().getBank()].front() == useThisCommand);

		// if it was a refresh all command, then dequeue all n banks worth of commands
		if (useThisCommand->isRefresh())
		{		
			if (!currentRank->refreshAllReady())
				assert(currentRank->refreshAllReady());

			vector<Bank>::iterator bankEnd = currentRank->bank.end();

			vector<Bank>::iterator currentBank = currentRank->bank.begin();

			assert(currentBank->front() == useThisCommand);

			//Command *tempCommand = const_cast<Command*>(currentBank->front());

			for (;currentBank != bankEnd; ++currentBank)
			{				
				assert(currentBank->front() != NULL);
				if (!currentBank->front()->isRefresh())
					assert(currentBank->front()->isRefresh());

				currentBank->pop();
			}

			return const_cast<Command*>(useThisCommand);
		}
		else
		{			
			assert((useThisCommand->isPrecharge() && systemConfig.getRowBufferManagementPolicy() == OPEN_PAGE) ||
				(currentRank->bank[useThisCommand->getAddress().getBank()].front() == useThisCommand));

			vector<Bank>::iterator currentBank = currentRank->bank.begin() + useThisCommand->getAddress().getBank();

			assert(currentBank->front() == useThisCommand);

			Command *tempCommand = currentBank->pop();

			assert(tempCommand && tempCommand == useThisCommand);

			if ((systemConfig.getRowBufferManagementPolicy() == OPEN_PAGE_AGGRESSIVE) &&
				useThisCommand->isReadOrWrite() &&
				!currentBank->isEmpty() && 
				currentBank->isHighUtilization() &&
				currentBank->front()->isBasicPrecharge())
			{
				assert(!tempCommand->isPrecharge());
				tempCommand->setAutoPrecharge(true);
				assert(currentBank->front()->isBasicPrecharge());
				delete currentBank->pop();
			}

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
	case FIRST_AVAILABLE_AGE:
		{
			const Command *candidateCommand = NULL;

			tick candidateExecuteTime = TICK_MAX;

			vector<Rank>::const_iterator rankEnd = rank.end();

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rankEnd; ++currentRank)
			{
				bool isRefreshCommand = true;

				vector<Bank>::const_iterator bankEnd = currentRank->bank.end();

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != bankEnd; ++currentBank)
				{	
					if (!currentBank->isEmpty())
					{
						const Command *challengerCommand = currentBank->front();

						assert(!challengerCommand || challengerCommand->isRefresh() || rank[challengerCommand->getAddress().getRank()].bank[challengerCommand->getAddress().getBank()].front() == challengerCommand);

						// see if it is a refresh command
						if (isRefreshCommand && challengerCommand->isRefresh() && currentRank->refreshAllReady())
						{
							tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifndef NDEBUG
							int minGap = minProtocolGap(challengerCommand);

							if (time + minGap != challengerExecuteTime)
							{
								cerr << "cet " << challengerExecuteTime << ", mingap " << time+minGap << endl;
								cerr << challengerCommand << endl;
								assert(time + minGap == challengerExecuteTime);
							}
#endif
							if (challengerExecuteTime < candidateExecuteTime ||
								(candidateExecuteTime == challengerExecuteTime && 
								challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))
							{						
								candidateCommand = challengerCommand;
								// stop searching since all the queues are proved to have refresh commands at the front
								break;
							}						
						}
						// can ignore refresh commands since it's known that not all the queues have a ref command at the front
						else if (!challengerCommand->isRefresh())
						{
							tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifndef NDEBUG
							int minGap = minProtocolGap(challengerCommand);

							if (time + minGap != challengerExecuteTime)
							{
								cerr << time << " " << minGap << " " << challengerExecuteTime << " " << challengerCommand->getCommandType() << " " << challengerCommand->getAddress() << endl;

								if (challengerCommand && (time + minGap != challengerExecuteTime))
								{
									minGap = minProtocolGap(challengerCommand);
									assert(challengerCommand && (time + minGap == challengerExecuteTime));
								}
							}
#endif
							// set a new candidate if the challenger can be executed sooner or execution times are the same but the challenger is older
							if ((challengerExecuteTime < candidateExecuteTime) ||
								(candidateExecuteTime == challengerExecuteTime &&
								challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))
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
				assert(candidateCommand->isRefresh() || rank[candidateCommand->getAddress().getRank()].bank[candidateCommand->getAddress().getBank()].front() == candidateCommand);

#ifdef DEBUG_GREEDY
				timingOutStream << "rk[" << candidateCommand->getAddress().rank << "] rk[" << candidateCommand->getAddress().bank << "]\tWinner: " << *candidateCommand << "gap[" << candidateGap << "] now[" << time << "]" << endl;
#endif
			}

			return candidateCommand;
		}
		break;

	case FIRST_AVAILABLE_RIFF:
		{
			const Command *candidateCommand = NULL;

			tick candidateExecuteTime = TICK_MAX;

			vector<Rank>::const_iterator rankEnd = rank.end();

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rankEnd; ++currentRank)
			{
				bool isRefreshCommand = true;

				vector<Bank>::const_iterator bankEnd = currentRank->bank.end();

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != bankEnd; ++currentBank)
				{	
					if (!currentBank->isEmpty())
					{
						const Command *challengerCommand = currentBank->front();

						assert(challengerCommand == NULL || challengerCommand->isRefresh() || rank[challengerCommand->getAddress().getRank()].bank[challengerCommand->getAddress().getBank()].front() == challengerCommand);

						// see if it is a refresh command
						if (isRefreshCommand && challengerCommand->isRefresh() && currentRank->refreshAllReady())
						{
							tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifndef NDEBUG
							int minGap = minProtocolGap(challengerCommand);

							if (time + minGap != challengerExecuteTime)
								assert(time + minGap == challengerExecuteTime);
#endif
							if ((challengerExecuteTime < candidateExecuteTime) ||
								(candidateExecuteTime == challengerExecuteTime && ((challengerCommand->isRead() && candidateCommand->isWrite()) ||
								(challengerCommand->isRead() && candidateCommand->isRead() && challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))) ||
								(time - challengerCommand->getEnqueueTime() > 12 * systemConfig.getSeniorityAgeLimit()))
							{						
								candidateCommand = challengerCommand;
								// stop searching since all the queues are proved to have refresh commands at the front
								break;
							}						
						}
						// can ignore refresh commands since it's known that not all the queues have a ref command at the front
						else if (!challengerCommand->isRefresh())
						{
							tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifndef NDEBUG
							int minGap = minProtocolGap(challengerCommand);

							if (time + minGap != challengerExecuteTime)
							{
								cerr << time << " " << minGap << " " << challengerExecuteTime;

								assert(time + minGap == challengerExecuteTime);
							}
#endif
							// set a new candidate if the challenger can be executed sooner or execution times are the same but the challenger is older
							if ((challengerExecuteTime < candidateExecuteTime) ||
								(candidateExecuteTime == challengerExecuteTime && ((challengerCommand->isRead() && candidateCommand->isWrite()) ||
								(challengerCommand->isRead() && candidateCommand->isRead() && challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))) ||
								(time - challengerCommand->getEnqueueTime() > 12 *systemConfig.getSeniorityAgeLimit()))
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
				assert(candidateCommand->isRefresh() || rank[candidateCommand->getAddress().getRank()].bank[candidateCommand->getAddress().getBank()].front() == candidateCommand);

#ifdef DEBUG_GREEDY
				timingOutStream << "rk[" << candidateCommand->getAddress().rank << "] rk[" << candidateCommand->getAddress().bank << "]\tWinner: " << *candidateCommand << "gap[" << candidateGap << "] now[" << time << "]" << endl;
#endif
			}

			return candidateCommand;
		}
		break;

	case FIRST_AVAILABLE_QUEUE:
		{
			const Command *candidateCommand = NULL;

			tick candidateExecuteTime = TICK_MAX;

			vector<Rank>::const_iterator rankEnd = rank.end();

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rankEnd; ++currentRank)
			{
				bool isRefreshCommand = true;

				vector<Bank>::const_iterator bankEnd = currentRank->bank.end();

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != bankEnd; ++currentBank)
				{	
					if (!currentBank->isEmpty())
					{
						const Command *challengerCommand = currentBank->front();

						assert(challengerCommand == NULL || challengerCommand->isRefresh() || rank[challengerCommand->getAddress().getRank()].bank[challengerCommand->getAddress().getBank()].front() == challengerCommand);

						// see if it is a refresh command
						if (isRefreshCommand && challengerCommand->isRefresh() && currentRank->refreshAllReady())
						{
							tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifndef NDEBUG
							int minGap = minProtocolGap(challengerCommand);

							if (time + minGap!= max(challengerExecuteTime,time))
								assert(time + minGap == challengerExecuteTime);
#endif
							// if it can execute earlier, at the same time and has greater queue pressure or it is starving
							if ((challengerExecuteTime < candidateExecuteTime) ||
								(candidateExecuteTime == challengerExecuteTime && currentBank->size() > rank[candidateCommand->getAddress().getRank()].bank[candidateCommand->getAddress().getBank()].size()) ||
								(time - challengerCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit()))
							{						
								candidateCommand = challengerCommand;
								// stop searching since all the queues are proved to have refresh commands at the front
								break;
							}						
						}
						// can ignore refresh commands since it's known that not all the queues have a ref command at the front
						else if (!challengerCommand->isRefresh())
						{
							tick challengerExecuteTime = earliestExecuteTime(challengerCommand);
#ifndef NDEBUG
							int minGap = minProtocolGap(challengerCommand);

							if (time + minGap != challengerExecuteTime)
							{
								cerr << time << " " << minGap << " " << challengerExecuteTime;

								assert(time + minGap == challengerExecuteTime);
							}
#endif
							// if it can execute earlier, at the same time and has greater queue pressure or it is starving
							if ((challengerExecuteTime < candidateExecuteTime) ||
								(candidateExecuteTime == challengerExecuteTime && currentBank->size() > rank[candidateCommand->getAddress().getRank()].bank[candidateCommand->getAddress().getBank()].size()) ||
								(time - challengerCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit()))
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
				assert(candidateCommand->isRefresh() || rank[candidateCommand->getAddress().getRank()].bank[candidateCommand->getAddress().getBank()].front() == candidateCommand);

#ifdef DEBUG_GREEDY
				timingOutStream << "rk[" << candidateCommand->getAddress().rank << "] rk[" << candidateCommand->getAddress().bank << "]\tWinner: " << *candidateCommand << "gap[" << candidateGap << "] now[" << time << "]" << endl;
#endif
			}

			return candidateCommand;
		}
		break;

		// this strategy executes all commands in the order they were entered
	case STRICT_ORDER:
		{
			tick oldestCommandTime = TICK_MAX;
			const Command *oldestCommand = NULL;

			vector<Rank>::const_iterator rankEnd = rank.end();

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rankEnd; ++currentRank)
			{
				bool notAllRefresh = false;

				vector<Bank>::const_iterator bankEnd = currentRank->bank.end();

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != bankEnd; ++currentBank)
				{
					if (const Command *challengerCommand = currentBank->front())
					{
#ifndef NDEBUG
						{
							const tick executeTime = earliestExecuteTime(challengerCommand);
							if (executeTime != time + minProtocolGap(challengerCommand))
							{							
								assert(executeTime == time + minProtocolGap(challengerCommand));
							}
						}						
#endif
						// choose the oldest command that can be executed
						if (challengerCommand->getEnqueueTime() < oldestCommandTime)
						{
							// if it's a refresh command and
							// we haven't proved that all the queues aren't refresh_all commands, search
							if (challengerCommand->isRefresh())
							{
								if (!notAllRefresh)
								{
									// if all are known now to be refresh commands
									if (currentRank->refreshAllReady())
									{
										oldestCommandTime = challengerCommand->getEnqueueTime();
										oldestCommand = challengerCommand;
										// don't need to check other banks again
										break;
									}
								}
							}
							else
							{
								oldestCommandTime = challengerCommand->getEnqueueTime();
								oldestCommand = challengerCommand;
							}
						}
					}

					notAllRefresh = true;
				}
			}

			// if any executable command was found, prioritize it over those which must wait

			return oldestCommand;
		}
		break;

		// alternate ranks as we go down banks
	case RANK_ROUND_ROBIN:
		{
			// the command type to look for first
			bool readSweep = true;

			// look at the most recently retired command in this channel's history
			//const unsigned lastBankOffset = lastCommand ? lastCommand->getAddress().getBank() : 0;
			//const vector<Rank>::const_iterator lastRank = rank.begin() + (lastCommand ? lastCommand->getAddress().getRank() : 0);
			const Command::CommandType lastCommandType = lastCommand ? (lastCommand->getCommandType()) : Command::READ;

			// attempt to group RAS/CAS pairs together
			switch (lastCommandType)
			{
				// if it was RAS before and you want to finish doing the read/write
			case Command::ACTIVATE:
				{
					// look at the command just after the RAS, it should be some sort of CAS

					const Command *nextCommand = ((rank.begin() + lastCommand->getAddress().getRank())->bank.begin() + lastCommand->getAddress().getBank())->front();

					if (nextCommand && nextCommand->isReadOrWrite())
					{
						return nextCommand;
					}
					else
					{
						cerr << "error: Found a row activate not followed by a column command." << endl;
						exit(2);
					}
				}
				break;

			case Command::READ:
				// try to reuse open rows
				if (lastCommand)
				{
					const Command *nextCommand = ((rank.begin() + lastCommand->getAddress().getRank())->bank.begin() + lastCommand->getAddress().getBank())->front();

					if (nextCommand && (nextCommand->isReadOrWrite() || nextCommand->isBasicPrecharge()))
					{
						return nextCommand;
					}
					assert(!nextCommand || !nextCommand->isBasicPrecharge());
				}	
			case Command::READ_AND_PRECHARGE:
			case Command::PRECHARGE:
			case Command::REFRESH_ALL:
				readSweep = true;
				break;

			case Command::WRITE:
				// try to reuse open rows
				if (lastCommand)
				{
					const Command *nextCommand = ((rank.begin() + lastCommand->getAddress().getRank())->bank.begin() + lastCommand->getAddress().getBank())->front();

					if (nextCommand && (nextCommand->isReadOrWrite() || nextCommand->isBasicPrecharge()))
					{
						return nextCommand;
					}
					assert(!nextCommand || !nextCommand->isBasicPrecharge());
				}		
			case Command::WRITE_AND_PRECHARGE:

				readSweep = false;
				break;	

			default:
				readSweep = true;
				cerr << "warn: Unhandled command type." << endl;
				break;
			}

			unsigned currentBankOffset = lastCommand ? lastCommand->getAddress().getBank() : 0;

			vector<Rank>::const_iterator currentRank = rank.begin() + (lastCommand ? lastCommand->getAddress().getRank() : 0) + 1;
			if (currentRank == rank.end())
			{
				currentBankOffset = (currentBankOffset + 1) % systemConfig.getBankCount();
				currentRank = rank.begin();
			}

			// set original values to know when a full sweep is finished
			const vector<Rank>::const_iterator rankEnd = rank.end();
			const unsigned startingBankOffset = currentBankOffset;
			const vector<Rank>::const_iterator startingRank = currentRank;
			const bool originalReadSweep = readSweep;

			while (true)
			{		
				const Command *potentialCommand = (currentRank->bank.begin() + currentBankOffset)->front();

				// refresh commands are considered elsewhere
				if (potentialCommand && !potentialCommand->isRefresh() )
				{
					// group reads and writes with each other
					if (systemConfig.isReadWriteGrouping())
					{
						if (potentialCommand->isActivate())
							assert((currentRank->bank.begin() + currentBankOffset)->read(1));

						const Command *secondCommand = potentialCommand->isActivate() ? (currentRank->bank.begin() + currentBankOffset)->read(1) : potentialCommand;
						assert(secondCommand->isReadOrWrite() || secondCommand->isBasicPrecharge());

						if ((secondCommand->isRead() && readSweep) ||
							(secondCommand->isWrite() && !readSweep) ||
							secondCommand->isBasicPrecharge())
						{
							assert((currentRank->bank.begin() + currentBankOffset)->front() == potentialCommand);
							return potentialCommand;
						}
					}
					else // don't have to follow read_write grouping considerations
					{
						return potentialCommand;
					}
				}

				// before switching to the next rank, see if all the queues are refreshes in any rank
				if (currentRank->refreshAllReady())
				{
					return currentRank->bank.begin()->front();
				}

				// move on to the next rank
				currentRank++;

				if (currentRank == rank.end())
				{
					currentRank = rank.begin();

					// select the next bank
					currentBankOffset = (currentBankOffset + 1) % systemConfig.getBankCount();
				}

				// swap R/W for W/R when doing read/write grouping
				if (currentBankOffset == startingBankOffset && currentRank == startingRank)
				{				
					if (systemConfig.isReadWriteGrouping())
					{
						// try other types
						readSweep = !readSweep;

						if (readSweep == originalReadSweep)
						{
#ifndef NDEBUG
							for (vector<Rank>::const_iterator i = rank.begin(); i != rankEnd; ++i)
							{
								for (vector<Bank>::const_iterator j = i->bank.begin(); j != i->bank.end(); ++j)
								{
									assert(j->isEmpty());
								}
							}
#endif
							return NULL;
						}						
					}
					else
					{
						return NULL;
					}
				}
			}
		}
		break;

		// keep rank id as long as possible, go round robin down a given rank
	case BANK_ROUND_ROBIN:
		{			
			bool readSweep = true;

			// look at the most recently retired command in this channel's history
			const Command::CommandType lastCommandType = lastCommand ? (lastCommand->getCommandType()) : Command::READ;

			// attempt to issue RAS and CAS together
			switch (lastCommandType)
			{
			case Command::ACTIVATE:
				{
					// look at the command just after the RAS, it should be some sort of CAS

					const Command *nextCommand = ((rank.begin() + lastCommand->getAddress().getRank())->bank.begin() + lastCommand->getAddress().getBank())->front();

					if (nextCommand && nextCommand->isReadOrWrite())
					{
						return nextCommand;
					}
					else
					{
						cerr << "error: row activate command not followed by a column command." << endl;
						exit(2);
					}
				}
				break;
				// doing read sweeping
			case Command::READ:
				// try to reuse open rows
				if (lastCommand)
				{
					const Command *nextCommand = ((rank.begin() + lastCommand->getAddress().getRank())->bank.begin() + lastCommand->getAddress().getBank())->front();

					if (nextCommand && (nextCommand->isReadOrWrite() || nextCommand->isBasicPrecharge()))
					{
						return nextCommand;
					}
					assert(!nextCommand || !nextCommand->isBasicPrecharge());
				}			
			case Command::REFRESH_ALL:
			case Command::READ_AND_PRECHARGE:
			case Command::PRECHARGE:
				readSweep = true;
				break;

				// doing write sweeping
			case Command::WRITE:
				// try to reuse open rows
				if (lastCommand)
				{
					const Command *nextCommand = ((rank.begin() + lastCommand->getAddress().getRank())->bank.begin() + lastCommand->getAddress().getBank())->front();

					if (nextCommand && (nextCommand->isReadOrWrite() || nextCommand->isBasicPrecharge()))
					{
						return nextCommand;
					}
					assert(!nextCommand || !nextCommand->isBasicPrecharge());
				}			
			case Command::WRITE_AND_PRECHARGE:
				readSweep = false;
				break;

			default:
				readSweep = true; // FIXME: added this to ensure no uninit vars
				cerr << "warn: unhandled command type" << endl;
				break;
			}

			vector<Rank>::const_iterator currentRank = rank.begin() + (lastCommand ? lastCommand->getAddress().getRank() : 0);
			vector<Bank>::const_iterator currentBank = currentRank->bank.begin() + (lastCommand ? lastCommand->getAddress().getBank() : 0) + 1;
			if (currentBank == currentRank->bank.end())
			{
				currentRank++;
				if (currentRank == rank.end())
					currentRank = rank.begin();
				currentBank = currentRank->bank.begin();
			}

			// set original values to know when a full sweep is finished
			const vector<Rank>::const_iterator rankEnd = rank.end();
			const vector<Bank>::const_iterator startingBank = currentBank;
			const vector<Rank>::const_iterator startingRank = currentRank;
			const bool originalReadSweep = readSweep;

			while (true)
			{
				const Command *potentialCommand = currentBank->front();

				// see if this command could be used
				if (potentialCommand && !potentialCommand->isRefresh())
				{	
					if (systemConfig.isReadWriteGrouping())
					{
						if (potentialCommand->isActivate())
							assert(currentBank->read(1));

						const Command *secondCommand = (potentialCommand->isActivate() && currentBank->read(1)) ? currentBank->read(1) : potentialCommand;
						assert(secondCommand->isReadOrWrite() || secondCommand->isBasicPrecharge());

						if ((secondCommand->isRead() && readSweep) ||
							(secondCommand->isWrite() && !readSweep) || 
							secondCommand->isBasicPrecharge())
						{
							assert(currentBank->front()->getAddress().getRank() == currentRank->getRankId());
							assert(currentBank->front() == potentialCommand);
							return potentialCommand;
						}
					}
					else // don't have to follow read_write grouping considerations
					{
						return potentialCommand;
					}
				}

				// then switch to the next bank
				currentBank++;
				if (currentBank == currentRank->bank.end())
				{
					// before switching to the next rank, see if all the queues are refreshes in any rank
					if (currentRank->refreshAllReady())
					{
						return currentRank->bank.begin()->front();
					}

					currentRank++;
					if (currentRank == rankEnd)
						currentRank = rank.begin();
					currentBank = currentRank->bank.begin();		
				}

				// back to the beginning, either no result or look for the opposite type
				// n.b. must compare rank iterators first, otherwise comparing bank iterators based on unequal rank iterators is a runtime error
				if (currentRank == startingRank && currentBank == startingBank)
				{						
					if (systemConfig.isReadWriteGrouping())
					{
						// try other types
						readSweep = !readSweep;

						if (readSweep == originalReadSweep)
						{
#ifndef NDEBUG
							for (vector<Rank>::const_iterator i = rank.begin(); i != rankEnd; ++i)
							{
								for (vector<Bank>::const_iterator j = i->bank.begin(); j != i->bank.end(); ++j)
								{
									assert(j->isEmpty());
								}
							}
#endif
							return NULL;
						}	
					}					
					else
					{

						return NULL;
					}
				}			
			}
		}
		break;

	case COMMAND_PAIR_RANK_HOPPING:
		{	
			// determine
			bool isActivate;
			unsigned nextRank, nextBank;

			if (lastCommand)
			{
				isActivate = lastCommand->isActivate();
				nextRank = lastCommand->getAddress().getRank();
				nextBank = lastCommand->getAddress().getBank();
				getNextCPRHValues(nextRank, nextBank, isActivate);
				assert(nextRank < systemConfig.getRankCount());
				assert(nextBank < systemConfig.getBankCount());
				isActivate = !isActivate;
			}
			else // special case, must reset to the first value in the pattern
			{
				isActivate = true;
				nextRank = 0;
				nextBank = systemConfig.getBankCount() / 2;
			}

			const unsigned originalRank = nextRank;
			const unsigned originalBank = nextBank;
			const bool originalActivate = isActivate;

			while (true)
			{
				const Command *potentialCommand = 						
					((rank.begin() + nextRank)->bank.begin() + nextBank)->front();

				// see if this command could be used
				if (potentialCommand)
				{
					if (!potentialCommand->isRefresh())
					{
						if (potentialCommand->isActivate() == isActivate)
						{							
							return potentialCommand;
						}
					}
					else
					{
						// look for refreshes
						if ((rank.begin() + nextRank)->refreshAllReady())
						{
							return potentialCommand;
						}
					}
				}

				// then switch command
				getNextCPRHValues(nextRank, nextBank, isActivate);
				isActivate = !isActivate;	

				// quit if already checked every rank/bank combination
				if (nextRank == originalRank && nextBank == originalBank && originalActivate == isActivate)
				{
					return NULL;
				}							
			}
		}
		break;

	default:
		{
			cerr << "This configuration and algorithm combination is not supported" << endl;
			exit(-2);
		}
		break;
	}
	return NULL;
}

void Channel::getNextCPRHValues(unsigned &nextRank, unsigned &nextBank, const bool isActivate) const
{
	unsigned oldBank = nextBank;
	unsigned oldRank = nextRank;

	if (isActivate)
	{
		// set the rank of the next CAS
		nextBank = 2 * (oldBank % (systemConfig.getBankCount() / 2)) + !(oldBank / (systemConfig.getBankCount() / 2));
		nextRank = (systemConfig.getRankCount() - ((2 * (oldBank % (systemConfig.getBankCount() / 2)) + !(oldBank / (systemConfig.getBankCount() / 2))) % systemConfig.getRankCount()) + oldRank) % systemConfig.getRankCount();
	}
	else
	{
		if (oldBank == systemConfig.getBankCount() - 1)
		{
			nextRank = (oldRank + 1) % systemConfig.getRankCount();
			nextBank = systemConfig.getBankCount() / 2;
		}
		else
		{				
			nextBank = (oldBank % 2) * (systemConfig.getBankCount() / 2) + ((oldBank + 1) / 2);
			nextRank = (oldRank + oldBank + 1) % systemConfig.getRankCount();
		}
	}

}


//////////////////////////////////////////////////////////////////////////
/// @brief Updates the channel time to what it would be had this command been executed
/// @details Updates the rank and bank records of the most recent RAS, CAS, etc. times
/// Enqueues RAS times to allow t_faw to be determined later
/// Updates rank and bank records of CAS, RAS lengths for later calculations in
/// min_protocol_gap()
/// @param thisCommand the command to execute at this time
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
void Channel::executeCommand(Command *thisCommand)
{
	vector<Rank>::iterator currentRank = rank.begin() + thisCommand->getAddress().getRank();

	vector<Bank>::iterator currentBank = currentRank->bank.begin() + thisCommand->getAddress().getBank();
	
	currentRank->setLastBankID(thisCommand->getAddress().getBank());

	thisCommand->setStartTime(time);

	assert(canIssue(thisCommand));

	bool wasActivated;

	lastCommandIssueTime = time;

	switch(thisCommand->getCommandType())
	{
	case Command::ACTIVATE:
		{
			assert(!thisCommand->getHost());

			currentRank->issueRAS(time, thisCommand);	

			// specific for RAS command
			thisCommand->setCompletionTime(thisCommand->getStartTime() + timingSpecification.tCMD() + timingSpecification.tRAS());
		}
		break;

	case Command::READ_AND_PRECHARGE:

		wasActivated = currentBank->isActivated();
		// precharge may be issued first because timings are based on time, not the last time at which a read command was issued		
		currentRank->issuePRE(time, thisCommand);

		// lack of break is intentional
	case Command::READ:

		//////////////////////////////////////////////////////////////////////////
		{
			bool satisfied = cache[thisCommand->getAddress().getDimm()].timingAccess(thisCommand, thisCommand->getStartTime());
			thisCommand->getHost()->setHit(satisfied);
			if (!satisfied)
				currentRank->bank[thisCommand->getAddress().getBank()].setAllHits(false);
			// end of the line on reuse
			if (currentRank->bank[thisCommand->getAddress().getBank()].isAllHits() && thisCommand->isPrecharge())
			{
				statistics.reportRasReduction(thisCommand);
			}
			//std::cout << (satisfied ? "|" : ".");
		}
		//////////////////////////////////////////////////////////////////////////

		// specific for CAS command
		// should account for tAL buffering the CAS command until the right moment
		//thisCommand->setCompletionTime(max(currentBank.getLastRasTime() + timingSpecification.tRCD() + timingSpecification.tCAS() + timingSpecification.tBurst(), time + timingSpecification.tCMD() + timingSpecification.tCAS() + timingSpecification.tBurst()));
		assert(wasActivated);
	
		thisCommand->setCompletionTime(max(currentBank->getLastRasTime() + timingSpecification.tRCD() + timingSpecification.tCAS() + timingSpecification.tBurst(), time + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst()));
		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());

		for (vector<Rank>::iterator i = rank.begin(); i != rank.end(); ++i)
		{
			i->issueCAS(time, thisCommand);
		}
		break;

	case Command::WRITE_AND_PRECHARGE:

		wasActivated = currentBank->isActivated();
		// precharge may be issued first because timings are based on time, not the last time at which a read command was issued
		currentRank->issuePRE(time, thisCommand);	

		// missing break is intentional
	case Command::WRITE:

		//////////////////////////////////////////////////////////////////////////
		{
#ifndef NDEBUG
			bool satisfied = 
#endif
				cache[thisCommand->getAddress().getDimm()].timingAccess(thisCommand, thisCommand->getStartTime());
			currentRank->bank[thisCommand->getAddress().getBank()].setAllHits(false);
#ifndef NDEBUG
			std::cout << (satisfied ? "|" : ".");
#endif
		}
		//////////////////////////////////////////////////////////////////////////

		// for the CAS write command
		//thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());
		assert(wasActivated);
	
		thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tCWD() + timingSpecification.tBurst());

		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());

		for (vector<Rank>::iterator i = rank.begin(); i != rank.end(); ++i)
		{
			i->issueCASW(time,thisCommand);
		}
		break;

	case Command::PRECHARGE:

		// issued to the rank which issues to banks
		currentRank->issuePRE(time, thisCommand);

		thisCommand->setCompletionTime(thisCommand->getStartTime() + timingSpecification.tCMD() + timingSpecification.tRP());

		break;

	case Command::REFRESH_ALL:

		currentRank->issueREF(time);

		thisCommand->setCompletionTime(time + timingSpecification.tCMD() + timingSpecification.tRFC());
		thisCommand->getHost()->setCompletionTime(thisCommand->getCompletionTime());
		break;

	case Command::RETIRE_COMMAND:
		cerr << "Unimplemented command type" << endl;
		break;	

	case Command::PRECHARGE_ALL:
		cerr << "Unimplemented command type" << endl;
		break;

	case Command::ACTIVATE_ALL:
		cerr << "Unimplemented command type" << endl;
		break;

	case Command::DRIVE_COMMAND:
		cerr << "Unimplemented command type" << endl;
		break;

	case Command::DATA_COMMAND:
		cerr << "Unimplemented command type" << endl;
		break;

	case Command::CAS_WITH_DRIVE_COMMAND:
		cerr << "Unimplemented command type" << endl;
		break;	

	case Command::SELF_REFRESH:
		cerr << "Unimplemented command type" << endl;
		break;

	case Command::DESELECT:
		cerr << "Unimplemented command type" << endl;
		break;

	case Command::NOOP:
		cerr << "Unimplemented command type" << endl;
		break;

	case Command::INVALID_COMMAND:
		cerr << "Unimplemented command type" << endl;
		break;

	default:
		cerr << "Unknown command type" << endl;
		break;
	}	

	// inserts into a queue which dequeues into the command pool
	retireCommand(thisCommand);
}


//////////////////////////////////////////////////////////////////////////
/// @brief find the protocol gap between a command and current system state
/// @details old version of the function to check the next available command 
/// time, now used to check newer functions and ensure changes don't break
/// timing requirements
//////////////////////////////////////////////////////////////////////////
tick Channel::minProtocolGap(const Command *currentCommand) const
{
	tick min_gap = 0;

	const unsigned currentRankID = currentCommand->getAddress().getRank();

	const Rank &currentRank = rank[currentRankID];

	const Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	switch(currentCommand->getCommandType())
	{
	case Command::ACTIVATE:
		{
			// refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk

			// respect the row cycle time limitation
			tick tRCGap = (tick)(currentBank.getLastRasTime() - time) + timingSpecification.tRC();

			// respect tRRD and tRC of all other banks of same rank
			tick tRRDGap = (tick)(currentRank.lastActivateTimes.front() - time) + timingSpecification.tRRD();

			// respect tRP of same bank
			tick tRPGap = (tick)(currentBank.getLastPrechargeTime() - time) + timingSpecification.tRP();

			// respect the t_faw value for DDR2 and beyond
			tick tFAWGap = (tick)(currentRank.lastActivateTimes.back() - time) + timingSpecification.tFAW();

			// respect tRFC
			tick tRFCGap = (tick)(currentRank.getLastRefreshTime() - time) + timingSpecification.tRFC();

			min_gap = max(max(max(tRFCGap,tRCGap) , tRPGap) , max(tRRDGap , tFAWGap));
		}
		break;

	case Command::READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case Command::READ:
		{
			//respect last ras of same rank
			int tRCDGap = ((currentBank.getLastRasTime() - time) + timingSpecification.tRCD() - timingSpecification.tAL());

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			tick otherRankLastCASTime = time - 10000000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 10000000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent cas(w) time and length
			for (vector<Rank>::const_iterator thisRank = rank.begin(); thisRank != rank.end(); ++thisRank)
			{
				if (thisRank->getRankId() != currentRank.getRankId())
				{
					if (thisRank->getLastCasTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = thisRank->getLastCasTime();
						otherRankLastCASLength = thisRank->getLastCasLength();
					}
					if (thisRank->getLastCaswTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = thisRank->getLastCaswTime();
						otherRankLastCASWLength = thisRank->getLastCaswLength();
					}
				}
			}			

			//respect last cas of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			int t_cas_gap = ((currentRank.getLastCasTime() - time) + timingSpecification.tBurst());

			//respect last cas write of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			t_cas_gap = max((tick)t_cas_gap,((currentRank.getLastCaswTime() - time) + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR()));

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

	case Command::WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will met tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple CAS COMMAND.

	case Command::WRITE:
		{
			//respect last ras of same rank
			int t_ras_gap = ((currentBank.getLastRasTime() - time) + timingSpecification.tRCD() - timingSpecification.tAL());

			tick otherRankLastCASTime = time - 10000000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 10000000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent CAS/CASW time and length
			// FIXME: change to use iterators
			for (unsigned rank_id = 0; rank_id < rank.size() ; ++rank_id)
			{
				if (rank_id != currentRankID)
				{
					if (rank[rank_id].getLastCasTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = rank[rank_id].getLastCasTime();
						otherRankLastCASLength = rank[rank_id].getLastCasLength();
					}
					if (rank[rank_id].getLastCaswTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = rank[rank_id].getLastCaswTime();
						otherRankLastCASWLength = rank[rank_id].getLastCaswLength();
					}
				}
			}
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);

			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			tick t_cas_gap = (tick)(currentRank.getLastCasTime() - time) + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD();

			// respect last cas to different ranks
			t_cas_gap = max((tick)t_cas_gap,(otherRankLastCASTime - time) + timingSpecification.tCAS() + otherRankLastCASLength + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			t_cas_gap = max((tick)t_cas_gap,(currentRank.getLastCaswTime() - time) + currentRank.getLastCaswLength());

			// respect last cas write to different ranks
			t_cas_gap = max((tick)t_cas_gap,(otherRankLastCASWTime - time) + otherRankLastCASWLength + timingSpecification.tOST());

			min_gap = max((tick)t_ras_gap,(tick)t_cas_gap);
		}
		break;

	case Command::RETIRE_COMMAND:
		break;

	case Command::PRECHARGE:
		{
			// respect t_ras of same bank
			tick t_ras_gap = (currentBank.getLastRasTime() - time) + timingSpecification.tRAS();

			// respect t_cas of same bank
			tick t_cas_gap = ((tick)(currentBank.getLastCasTime() - time) + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));

			// respect t_casw of same bank
			t_cas_gap = max((tick)t_cas_gap,((currentBank.getLastCaswTime() - time) + timingSpecification.tAL() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR()));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case Command::PRECHARGE_ALL:
		break;

	case Command::ACTIVATE_ALL:
		break;

	case Command::DRIVE_COMMAND:
		break;

	case Command::DATA_COMMAND:
		break;

	case Command::CAS_WITH_DRIVE_COMMAND:
		break;

	case Command::REFRESH_ALL:
		// respect tRFC and tRP
		min_gap = max((currentRank.getLastRefreshTime() + timingSpecification.tRFC()),(currentRank.getLastPrechargeTime() + timingSpecification.tRP())) - time;
		break;

	case Command::INVALID_COMMAND:
		break;

	default:
		break;
	}

	//return max(min_gap,timingSpecification.tCMD());
	//return max(min_gap,max(lastCommandIssueTime - time + (tick)timingSpecification.tCMD(),(tick)0));
	return max(min_gap, lastCommandIssueTime + (tick)(timingSpecification.tCMD()) - time);
}

//////////////////////////////////////////////////////////////////////////
/// @brief Returns the soonest time that this command may execute
/// @details refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk by Jacob/Wang
/// @details Looks at all of the timing parameters and decides when the this command may soonest execute
//////////////////////////////////////////////////////////////////////////
tick Channel::earliestExecuteTime(const Command *currentCommand) const
{
	const vector<Rank>::const_iterator currentRank = rank.begin() + currentCommand->getAddress().getRank();

	const vector<Bank>::const_iterator currentBank = currentRank->bank.begin() + currentCommand->getAddress().getBank();

#ifndef NDEBUG
	tick nextTime = 0;

	switch(currentCommand->getCommandType())
	{
	case Command::ACTIVATE:
		{
			// refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk

			// respect the row cycle time limitation
			tick tRCLimit = currentBank->getLastRasTime() + timingSpecification.tRC();

			// respect the row-to-row activation delay for different banks within a rank
			tick tRRDLimit = currentRank->lastActivateTimes.front() + timingSpecification.tRRD();				

			// respect tRP of same bank
			tick tRPLimit = currentBank->getLastPrechargeTime() + timingSpecification.tRP();

			// respect the t_faw value for DDR2 and beyond, look at the fourth activate ago
			tick tFAWLimit = currentRank->lastActivateTimes.back() + timingSpecification.tFAW();

			// respect tRFC, refresh cycle time
			tick tRFCLimit = currentRank->getLastRefreshTime() + timingSpecification.tRFC();

			nextTime = max(max(max(tRFCLimit,tRCLimit) , tRPLimit) , max(tRRDLimit , tFAWLimit));

			assert(nextTime >= currentBank->getLastPrechargeTime() + timingSpecification.tRP());
			//DEBUG_TIMING_LOG(currentCommand->getCommandType() << " ras[" << setw(2) << t_ras_gap << "] rrd[" << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]");
		}
		break;

	case Command::READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case Command::READ:
		{
			//respect last RAS of same rank
			tick tRCDLimit = currentBank->getLastRasTime() + (timingSpecification.tRCD() - timingSpecification.tAL());

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago

			// respect last CAS of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			tick tCASLimit = currentRank->getLastCasTime() + timingSpecification.tBurst();

			// respect last CASW of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			tCASLimit = max(tCASLimit,currentRank->getLastCaswTime() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR());

			//if (rank->size() > 1)
			{
				//respect most recent CAS of different rank
				tCASLimit = max(tCASLimit, currentRank->getOtherLastCasTime() + currentRank->getOtherLastCasLength() + timingSpecification.tRTRS());
				//respect timing of READ follow WRITE, different ranks
				tCASLimit = max(tCASLimit, currentRank->getOtherLastCaswTime() + timingSpecification.tCWD() + currentRank->getOtherLastCaswLength() + timingSpecification.tRTRS() - timingSpecification.tCAS());
			}

			nextTime = max(tRCDLimit,tCASLimit);

			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case Command::WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will meet tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple WRITE.

	case Command::WRITE:
		{
			//respect last RAS of same rank
			tick tRASLimit = currentBank->getLastRasTime() + timingSpecification.tRCD() - timingSpecification.tAL();

			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);

			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			tick tCASLimit = currentRank->getLastCasTime() + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD();

			// respect last cas to different ranks
			tCASLimit = max(tCASLimit,currentRank->getOtherLastCasTime() + timingSpecification.tCAS() + currentRank->getOtherLastCasLength() + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			tCASLimit = max(tCASLimit,currentRank->getLastCaswTime() + currentRank->getLastCaswLength());

			// respect last CASW to different ranks
			// TODO: should this not also be -tAL?
			tCASLimit = max(tCASLimit,currentRank->getOtherLastCaswTime() + currentRank->getOtherLastCaswLength() + timingSpecification.tOST());

			nextTime = max(tRASLimit,tCASLimit);
		}
		break;

	case Command::PRECHARGE:
		{
			// respect t_ras of same bank
			tick tRASLimit = currentBank->getLastRasTime() + timingSpecification.tRAS();

			// respect t_cas of same bank
			//tick tCASLimit = max(time,currentBank->getLastCasTime() + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tick tCASLimit = currentBank->getLastCasTime() + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD());

			// respect t_casw of same bank
			//tCASLimit = max(tCASLimit,currentBank->getLastCaswTime() + timingSpecification.tAL() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tCASLimit = max(tCASLimit,currentBank->getLastCaswTime() + timingSpecification.tAL() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());

			nextTime = max(tRASLimit,tCASLimit);
		}
		break;

	case Command::REFRESH_ALL:
		// respect tRFC and tRP
		nextTime = max(currentRank->getLastRefreshTime() + timingSpecification.tRFC(), currentRank->getLastPrechargeTime() + timingSpecification.tRP());
		break;

	case Command::RETIRE_COMMAND:
	case Command::PRECHARGE_ALL:
	case Command::ACTIVATE_ALL:
	case Command::DRIVE_COMMAND:
	case Command::DATA_COMMAND:
	case Command::CAS_WITH_DRIVE_COMMAND:
	case Command::SELF_REFRESH:
	case Command::DESELECT:
	case Command::NOOP:
	case Command::INVALID_COMMAND:
	default:
		cerr << "Unsupported command encountered." << endl;
		nextTime = 0;
		break;
	}

	tick actualNext = max(nextTime, lastCommandIssueTime + timingSpecification.tCMD());
	tick predictedNext = max(currentRank->next(currentCommand->getCommandType()), 
		max(currentBank->next(currentCommand->getCommandType()), lastCommandIssueTime + timingSpecification.tCMD()));
	if (actualNext != predictedNext )
		assert(actualNext == predictedNext);
#endif

	return max(currentRank->next(currentCommand->getCommandType()), 
		max(currentBank->next(currentCommand->getCommandType()), lastCommandIssueTime + timingSpecification.tCMD()));
}

bool Channel::isEmpty() const
{
	for (vector<Rank>::const_iterator i = rank.begin(); i != rank.end(); ++i)
	{
		if (!i->isEmpty())
			return false;
	}
	return transactionQueue.isEmpty();
}


//////////////////////////////////////////////////////////////////////////
/// @brief Returns the soonest time that this command may execute, tracks the limiting factor
/// @details refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk by Jacob/Wang
/// @details Looks at all of the timing parameters and decides when the this command may execute soonest 
//////////////////////////////////////////////////////////////////////////
tick Channel::earliestExecuteTimeLog(const Command *currentCommand) const
{
	static unsigned tRCcount, tRRDcount, tRPcount, tFAWcount, tRFCcount;
	static unsigned tRCDcount, tCAScount, tCASWcount, tCASotherCount, tCASWotherCount;
	static unsigned tRAScount;
	tick nextTime;	

	const Rank &currentRank = rank[currentCommand->getAddress().getRank()];

	const Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	switch(currentCommand->getCommandType())
	{
	case Command::ACTIVATE:
		{
			// refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk

			// respect the row cycle time limitation
			tick tRCLimit = currentBank.getLastRasTime() + timingSpecification.tRC();

			const tick lastRASTime = currentRank.lastActivateTimes.front();

			// respect the row-to-row activation delay for different banks within a rank
			tick tRRDLimit = lastRASTime + timingSpecification.tRRD();				

			// respect tRP of same bank
			tick tRPLimit = currentBank.getLastPrechargeTime() + timingSpecification.tRP();

			// respect the t_faw value for DDR2 and beyond, look at the fourth activate ago
			tick tFAWLimit = currentRank.lastActivateTimes.back() + timingSpecification.tFAW();

			// respect tRFC, refresh cycle time
			tick tRFCLimit = currentRank.getLastRefreshTime() + timingSpecification.tRFC();

			nextTime = max(max(max(tRFCLimit,tRCLimit) , tRPLimit) , max(tRRDLimit , tFAWLimit));

			if (nextTime == tRCLimit)
				tRCcount++;				
			if (nextTime == tRRDLimit)
				tRRDcount++;
			if (nextTime == tRPLimit)
				tRPcount++;
			if (nextTime == tFAWLimit)
				tFAWcount++;
			if (nextTime == tRFCLimit)
				tRFCcount++;

			assert(nextTime >= currentBank.getLastPrechargeTime() + timingSpecification.tRP());
			//DEBUG_TIMING_LOG(currentCommand->getCommandType() << " ras[" << setw(2) << t_ras_gap << "] rrd[" << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]");
		}
		break;

	case Command::READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case Command::READ:
		{
			//respect last RAS of same rank
			tick tRCDLimit = currentBank.getLastRasTime() + (timingSpecification.tRCD() - timingSpecification.tAL());

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago

			// respect last CAS of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			tick tCASLimit = currentRank.getLastCasTime() + timingSpecification.tBurst();

			// respect last CASW of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			tick tCASWLimit = currentRank.getLastCaswTime() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR();

			//respect most recent CAS of different rank
			tick tCASOtherLimit = currentRank.getOtherLastCasTime() + currentRank.getOtherLastCasLength() + timingSpecification.tRTRS();
			//respect timing of READ follow WRITE, different ranks
			tick tCASWOtherLimit = currentRank.getOtherLastCaswTime() + timingSpecification.tCWD() + currentRank.getOtherLastCaswLength() + timingSpecification.tRTRS() - timingSpecification.tCAS();

			nextTime = max(max(tRCDLimit,tCASLimit),max(max(tCASWLimit,tCASOtherLimit),tCASWOtherLimit));

			if (nextTime == tRCDLimit)
				tRCDcount++;				
			if (nextTime == tCASLimit)
				tCAScount++;
			if (nextTime == tCASWLimit)
				tCASWcount++;
			if (nextTime == tCASOtherLimit)
				tCASotherCount++;
			if (nextTime == tCASWOtherLimit)
				tCASWotherCount++;

			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case Command::WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will meet tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple WRITE.

	case Command::WRITE:
		{
			//respect last RAS of same rank
			tick tRASLimit = currentBank.getLastRasTime() + timingSpecification.tRCD() - timingSpecification.tAL();

			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);

			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			tick tCASLimit = max(time,currentRank.getLastCasTime() + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas to different ranks
			tick tCASOtherLimit = currentRank.getOtherLastCasTime() + timingSpecification.tCAS() + currentRank.getOtherLastCasLength() + timingSpecification.tRTRS() - timingSpecification.tCWD();

			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			tick tCASWLimit = currentRank.getLastCaswTime() + currentRank.getLastCaswLength();

			// respect last CASW to different ranks
			// TODO: should this not also be -tAL?
			tick tCASWOtherLimit = currentRank.getOtherLastCaswTime() + currentRank.getOtherLastCaswLength() + timingSpecification.tOST();

			nextTime = max(max(tRASLimit,tCASLimit), max(max(tCASOtherLimit,tCASWOtherLimit),tCASWLimit));

			if (nextTime == tRASLimit)
				tRAScount++;				
			if (nextTime == tCASLimit)
				tCAScount++;
			if (nextTime == tCASWLimit)
				tCASWcount++;
			if (nextTime == tCASOtherLimit)
				tCASotherCount++;
			if (nextTime == tCASWOtherLimit)
				tCASWotherCount++;
		}
		break;

	case Command::PRECHARGE:
		{
			// respect t_ras of same bank
			tick tRASLimit = currentBank.getLastRasTime() + timingSpecification.tRAS();

			// respect t_cas of same bank
			// TODO: do not need tAL for these
			//tick tCASLimit = max(time,currentBank.getLastCasTime() + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tick tCASLimit = max(time,currentBank.getLastCasTime() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));

			// respect t_casw of same bank
			//tCASLimit = max(tCASLimit,currentBank.getLastCaswTime() + timingSpecification.tAL() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tCASLimit = max(tCASLimit,currentBank.getLastCaswTime() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());

			nextTime = max(tRASLimit,tCASLimit);
		}
		break;

	case Command::REFRESH_ALL:
		// respect tRFC and tRP
		nextTime = max(currentRank.getLastRefreshTime() + timingSpecification.tRFC(), currentRank.getLastPrechargeTime() + timingSpecification.tRP());
		break;

	case Command::RETIRE_COMMAND:
	case Command::PRECHARGE_ALL:
	case Command::ACTIVATE_ALL:
	case Command::DRIVE_COMMAND:
	case Command::DATA_COMMAND:
	case Command::CAS_WITH_DRIVE_COMMAND:
	case Command::SELF_REFRESH:
	case Command::DESELECT:
	case Command::NOOP:
	case Command::INVALID_COMMAND:
	default:
		cerr << "Unsupported command encountered." << endl;
		nextTime = 0;
		break;
	}

	//return max(nextTime, time + timingSpecification.tCMD());
	//return max(nextTime, max(time, lastCommandIssueTime + timingSpecification.tCMD()));
	return max(nextTime, max(time , lastCommandIssueTime + timingSpecification.tCMD()));
}

void Channel::resetStats()
{
	for (vector<Cache>::iterator i = cache.begin(), iEnd = cache.end();
		i != iEnd; ++i)
	{
		i->resetStats();
	}
}


void Channel::printVerilogCommand(const Command *thisCommand)
{
	static tick lastTime;

	systemConfig.verilogOutStream << "nop(" << (time - lastTime) / systemConfig.getDatarate() * 1.0E9 << "); //" << time - lastTime << endl;

	lastTime = time;

	if (thisCommand->isRead())
	{
		systemConfig.verilogOutStream << "read\t\t(" << thisCommand->getAddress().getRank() << ",\t"
			<< thisCommand->getAddress().getBank() << ",\t" << thisCommand->getAddress().getColumn() <<
			",\t" << (thisCommand->isPrecharge() ? "1" : "0") << ",\t" <<
			(thisCommand->getLength() < (timingSpecification.tBurst()) ? "1" : "0") << "); //" <<
			time << endl;
	}
	else if (thisCommand->isWrite())
	{
		systemConfig.verilogOutStream << "write\t\t(" << thisCommand->getAddress().getRank() << ",\t" << thisCommand->getAddress().getBank() <<
			",\t" << thisCommand->getAddress().getColumn() << ",\t" << (thisCommand->isPrecharge() ? "1" : "0") << ",\t" <<
			(thisCommand->getLength() < (timingSpecification.tBurst()) ? "1" : "0") << ",\t0,\t10); //" << time << endl;
	}
	else if (thisCommand->isActivate())
	{
		systemConfig.verilogOutStream << "activate\t(" << thisCommand->getAddress().getRank() << ",\t" << 
			thisCommand->getAddress().getBank() << ",\t" << thisCommand->getAddress().getRow() << "); //" <<
			time << endl;
	}
	else if (thisCommand->isRefresh())
	{
		systemConfig.verilogOutStream << "refresh(" << thisCommand->getAddress().getRank() << ");" << endl;
	}
	else if (thisCommand->isBasicPrecharge())
	{
		systemConfig.verilogOutStream << "precharge\t(" << thisCommand->getAddress().getRank() << ",\t" <<
			thisCommand->getAddress().getBank() << ",\t" << "0" << "); //" <<
			time << endl;
	}

}

//////////////////////////////////////////////////////////////////////////
/// @brief the assignment operator, will copy non-key items to this channel
/// @details copies the non-reference items over, should be used for deserialization
/// @return a reference to this channel, for chaining
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
Channel& Channel::operator =(const Channel &rhs)
{
	time = rhs.time;
	lastCommandIssueTime = rhs.lastCommandIssueTime;
	lastCommand = rhs.lastCommand ? new Command(*(rhs.lastCommand)) : NULL;
	transactionQueue = rhs.transactionQueue;
	refreshCounter = rhs.refreshCounter;
	channelID = rhs.channelID;
	rank = rhs.rank;
	//dimm = rhs.dimm;
	powerModel = rhs.powerModel;
	timingSpecification = rhs.timingSpecification;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
/// @brief the equality operator, to determine if the channels are equal incomingTransaction value
/// @return true if the channels are equal
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////////
bool Channel::operator ==(const Channel& rhs) const
{
	return (time == rhs.time && lastCommandIssueTime == rhs.lastCommandIssueTime && 
		timingSpecification == rhs.timingSpecification && transactionQueue == rhs.transactionQueue && 
		((lastCommand == NULL && rhs.lastCommand == NULL) || (*lastCommand == *(rhs.lastCommand)) )
		&& systemConfig == rhs.systemConfig && 
		statistics == rhs.statistics && powerModel == rhs.powerModel && channelID == rhs.channelID 
		&& rank == rhs.rank && refreshCounter == rhs.refreshCounter);
}

//////////////////////////////////////////////////////////////////////////
/// @brief the insertion operator, adds a string to the given stream with _stats about this channel
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

