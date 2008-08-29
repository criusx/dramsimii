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
using namespace DRAMSimII;

//////////////////////////////////////////////////////////////////////////
/// @brief constructs the dramChannel using this settings reference, also makes a reference to the dramSystemConfiguration object
/// @param settings the settings file that defines the number of ranks, refresh policy, etc.
/// @param sysConfig a const reference is made to this for some functions to grab parameters from
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
		// stagger the times that each rank will be refreshed so they don't all arrive in a burst
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
Channel::Channel(const Channel& rhs, const SystemConfiguration& systemConfig, Statistics& stats):
time(rhs.time),
lastRefreshTime(rhs.lastRefreshTime),
lastCommandIssueTime(rhs.lastCommandIssueTime),
lastRankID(rhs.lastRankID),
timingSpecification(rhs.timingSpecification),
refreshCounter(rhs.refreshCounter),
transactionQueue(rhs.transactionQueue),
historyQueue(rhs.historyQueue),
completionQueue(rhs.completionQueue),
systemConfig(systemConfig),
statistics(stats),
powerModel(rhs.powerModel),
algorithm(rhs.algorithm),
channelID(rhs.channelID),
// to initialize the references
rank((unsigned)systemConfig.getRankCount(), Rank(rhs.rank[0],timingSpecification, systemConfig))
{
	// to fill in the values
	rank = rhs.rank;
}

/// deserialization constructor
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
channelID(-1),
rank(newRank)
{}

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
/// @brief copy constructor, reassigns the ordinal to each rank as they are duplicated
/// @param rhs the dramChannel object to be copied
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
rank((unsigned)rhs.rank.size(), Rank(rhs.rank[0],timingSpecification, systemConfig))
{
	// TODO: copy over values in ranks now that reference members are init
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

//////////////////////////////////////////////////////////////////////////
/// @brief Moves the specified channel to at least the time given
/// @param endTime move the channel until it is at this time
/// @param transFinishTime the time that this transaction finished
//////////////////////////////////////////////////////////////////////////
const void *Channel::moveChannelToTime(const tick endTime, tick& transFinishTime)
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
			Command *executingCommand = getNextCommand();

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

					return NULL;
				}
				else // return what was pointed to
				{
					const void *origTrans = completedTransaction->getOriginalTransaction();

					M5_DEBUG(assert(origTrans));

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

	return NULL;
}


//////////////////////////////////////////////////////////////////////////
/// @brief determines when the next transaction is decoded, command ready to be executed or next refresh command arrives
/// @return the time when the next event happens
/// @sa readTransactionSimple() readNextCommand() earliestExecuteTime() checkForAvailableCommandSlots()
//////////////////////////////////////////////////////////////////////////
tick Channel::nextTick() const
{
	return max(min(nextTransactionDecodeTime(),nextCommandExecuteTime()),time + 1);
}

/// @brief adds this command to the history queue
/// @details this allows other groups to view a recent history of commands that were issued to decide what to execute next
void Channel::retireCommand(Command *newestCommand)
{
	statistics.collectCommandStats(newestCommand);

	// transaction complete? if so, put in completion queue
	// note that the host transaction should only be pointed to by a CAS command
	// since this is when a transaction is done from the standpoint of the requester
	if (newestCommand->getHost()) 
	{
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
/// @param in the transaction to be put into the queue
/// @return true if there was room in the queue for this command and the algorithm allowed it, false otherwise
//////////////////////////////////////////////////////////////////////////
bool Channel::enqueue(Transaction *in)
{
	/// @todo probably should set the enqueue time = time here
	if (systemConfig.getTransactionOrderingAlgorithm() == STRICT)
		return transactionQueue.push(in);
	// try to insert reads and fetches before writes
	/// @todo add support for additional transaction ordering algorithms, such as read, instruction, fetch first
	else
	{
		assert(systemConfig.getTransactionOrderingAlgorithm() == RIFF);
		exit(-1);		
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief determine when the next transaction in the queue will be decoded
/// @return the time when the decoding will be complete
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

	DEBUG_LOG("Rank[" << rank_id << "] Read[" << read_count << "] Write[" << write_count << "] Empty[" << empty_count << "]");

	if (read_count >= write_count)
		return READ_TRANSACTION;
	else
		return WRITE_TRANSACTION;
}

using namespace std;

//////////////////////////////////////////////////////////////////////
/// @brief performs power calculations for this epoch and cumulative
/// @details calculates the power according to Micron technical note TN-47-04\n
/// also does breakdowns of power consumed per channel and per epoch as well as averaged over time
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void Channel::doPowerCalculation()
{
	float factorA = (powerModel.getVDD() / powerModel.getVDDmax()) * (powerModel.getVDD() / powerModel.getVDDmax());
	float factorB = (float)powerModel.getFrequency() / (float)powerModel.getSpecFrequency();

	// the counts for the total number of operations
	unsigned entireRAS = 1;
	unsigned entireCAS = 1;
	unsigned entireCASW = 1;

	// the counts for the operations this epoch
	unsigned totalRAS = 1;
	unsigned totalCAS = 1;
	unsigned totalCASW = 1; // ensure no div/0

	float PsysACTTotal = 0;

	powerOutStream << "---------------------- epoch ----------------------" << endl;

	vector<int> rankArray;
	vector<float> PsysACTSTBYArray, PsysACTArray;

	for (vector<Rank>::iterator k = rank.begin(); k != rank.end(); k++)
	{
		unsigned perRankRASCount = 1;

		rankArray.push_back(k->getRankID());

		for (vector<Bank>::iterator l = k->bank.begin(); l != k->bank.end(); l++)
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
		float percentActive = max(0.0F,1.0F - (k->getPrechargeTime() / (float)(time - powerModel.getLastCalculation())));


		//assert(RDschPct + WRschPct < 1.0F);

		float PsysACTSTBY = factorA * factorB * powerModel.getIDD3N() * powerModel.getVDDmax() * percentActive;

		PsysACTSTBYArray.push_back(PsysACTSTBY);

		powerOutStream << "-Psys(ACT_STBY) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(5) << 
			PsysACTSTBY << " mW P(" << k->getPrechargeTime() << "/" << time - powerModel.getLastCalculation() << ")" << endl;

		float tRRDsch = ((float)time - powerModel.getLastCalculation()) / perRankRASCount;

		float PsysACT = ((float)powerModel.gettRC() / (float)tRRDsch) * factorA * powerModel.getPdsACT();

		PsysACTArray.push_back(PsysACT);

		powerOutStream << "-Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] "<< setprecision(5) << 
			PsysACT << " mW" << endl;

		//tick tRRDsch = (time - powerModel.lastCalculation) / totalRAS * powerModel.tBurst / 2;


		PsysACTTotal += ((float)powerModel.gettRC() / (float)tRRDsch) * factorA * powerModel.getPdsACT();
		//powerOutStream << "Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(5) << powerModel.PdsACT * powerModel.tRC / (float)tRRDsch * factorA * 100 << "mW " <<
		//	" A(" << totalRAS << ") tRRDsch(" << setprecision(5) << tRRDsch / ((float)systemConfig.Frequency() * 1.0E-9F) << "ns) lastCalc[" << powerModel.lastCalculation << "] time[" << 
		//	time << "]" << endl;

		k->setPrechargeTime(1);
	}

	float RDschPct = totalCAS * powerModel.gettBurst() / (float)(time - powerModel.getLastCalculation());
	float WRschPct = totalCASW * powerModel.gettBurst() / (float)(time - powerModel.getLastCalculation());

	//cerr << RDschPct * 100 << "%\t" << WRschPct * 100 << "%"<< endl;

	powerOutStream << "-Psys(ACT) ch[" << channelID << "] " << setprecision(5) << 
		PsysACTTotal << " mW" << endl;

	float PsysRD = factorA * factorB * (powerModel.getIDD4R() - powerModel.getIDD3N()) * RDschPct;

	powerOutStream << "-Psys(RD) ch[" << channelID << "] " << setprecision(5) << 
		PsysRD << " mW" << endl;

	float PsysWR = factorA * factorB * (powerModel.getIDD4W() - powerModel.getIDD3N()) * WRschPct; 

	powerOutStream << "-Psys(WR) ch[" << channelID << "] " << setprecision(5) << 
		PsysWR << " mW" << endl;
	powerModel.setLastCalculation(time);

	// report these results
	//boost::thread(sendPower())
	boost::thread(boost::bind(&DRAMSimII::Channel::sendPower,this,PsysRD, PsysWR, rankArray, PsysACTSTBYArray, PsysACTArray));

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

		powerOutStream << "+Psys(ACT_STBY) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(5) << 
			factorA * factorB * powerModel.getIDD3N() * powerModel.getVDDmax() * percentActive << " mW P(" << k->getTotalPrechargeTime() << "/" << time  << ")" << endl;

		float tRRDsch = ((float)time) / perRankRASCount;

		powerOutStream << "+Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] "<< setprecision(5) << 
			((float)powerModel.gettRC() / (float)tRRDsch) * factorA * powerModel.getPdsACT() << " mW" << endl;

		PsysACTTotal += ((float)powerModel.gettRC() / (float)tRRDsch) * factorA * powerModel.getPdsACT();
	}

	RDschPct = entireCAS * timingSpecification.tBurst() / (float)(time);
	WRschPct = entireCAS * timingSpecification.tBurst() / (float)(time);

	//cerr << RDschPct * 100 << "%\t" << WRschPct * 100 << "%"<< endl;

	powerOutStream << "+Psys(ACT) ch[" << channelID << "] " << setprecision(5) << 
		PsysACTTotal << " mW" << endl;

	powerOutStream << "+Psys(RD) ch[" << channelID << "] " << setprecision(5) << 
		factorA * factorB * (powerModel.getIDD4R() - powerModel.getIDD3N()) * RDschPct << " mW" << endl;

	powerOutStream << "+Psys(WR) ch[" << channelID << "] " << setprecision(5) << 
		factorA * factorB * (powerModel.getIDD4W() - powerModel.getIDD3N()) * WRschPct << " mW" << endl;

	//powerOutStream.flush();
}

bool Channel::sendPower(float PsysRD, float PsysWR, vector<int> rankArray, vector<float> PsysACTSTBYArray, vector<float> PsysACTArray) const 
{
	DRAMsimWSSoapHttp service;
	_ns2__submitEpochResultElement submit;

	char newString[64];
	systemConfig.getSessionID().copy(newString,systemConfig.getSessionID().length());
	submit.sessionID = newString;

	submit.epoch = time;

	vector<int> channelArray(rank.size(),channelID);	
	submit.channel = &channelArray[0];
	submit.__sizechannel = channelArray.size();

	submit.rank = &rankArray[0];
	submit.__sizerank = rankArray.size();

	submit.PsysACTSTBY = &PsysACTSTBYArray[0];
	submit.__sizePsysACTSTBY = PsysACTSTBYArray.size();
	submit.PsysACT = &PsysACTArray[0];
	submit.__sizePsysACT = PsysACTArray.size();
	submit.PsysRD = PsysRD;
	submit.PsysWR = PsysWR;
	_ns2__submitEpochResultResponseElement response;
	int retVal = service.__ns1__submitEpochResult(&submit,&response);
	return true;
}


//////////////////////////////////////////////////////////////////////////
// @brief get the next refresh time
//////////////////////////////////////////////////////////////////////////
const tick Channel::nextRefresh() const
{
	assert(rank.size() >= 1);

	return *(std::min_element(refreshCounter.begin(), refreshCounter.end()));

// 	Transaction *earliestRefreshTransaction = refreshCounter[0];
// 
// 	for (unsigned i = 1; i < rank.size(); ++i)
// 	{
// 		assert(refreshCounter[i]);
// 		if (refreshCounter[i]->getEnqueueTime() < earliestRefreshTransaction->getEnqueueTime())
// 		{
// 			earliestRefreshTransaction = refreshCounter[i];
// 		}
// 	}
// 	return earliestRefreshTransaction;
}


/// read the next available transaction for this channel without actually removing it from the queue
/// if bufferDelay is set, then only transactions that have been in the queue long enough to decode are considered
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

// get the next transaction, whether a refresh transaction or a normal R/W transaction
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
// get the next refresh command and remove it from the queue
//////////////////////////////////////////////////////////////////////////
Transaction *Channel::createNextRefresh()
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

	address.setChannel(channelID);
	address.setRank(earliestRank);
	address.setBank(0);

	Transaction *newRefreshTransaction = new Transaction(AUTO_REFRESH_TRANSACTION, earliestTime, 8,address,NULL);
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

	address.setChannel(channelID);
	address.setRank(earliestRank);
	address.setBank(0);

	static Transaction newRefreshTransaction;
	::new(&newRefreshTransaction)Transaction(AUTO_REFRESH_TRANSACTION, 0, 8,address,NULL);
	
	return &newRefreshTransaction;

}

std::ostream& DRAMSimII::operator<<(std::ostream& os, const DRAMSimII::Channel& r)
{
	os << "T[" << r.time << "] ch[" << r.channelID << endl;
	os << r.timingSpecification << endl;
	os << r.powerModel << endl;
	return os;
}

Channel& Channel::operator =(const Channel &rhs)
{
	//Settings settings;
	//::new(this)DRAMSimII::Channel(settings,rhs.systemConfig,rhs.statistics, rhs.powerModel, rhs.rank, rhs.timingSpecification);
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

bool Channel::operator ==(const Channel& rhs) const
{
	return (time == rhs.time && lastRefreshTime == rhs.lastRefreshTime && lastCommandIssueTime == rhs.lastCommandIssueTime && lastRankID == rhs.lastRankID &&
		timingSpecification == rhs.timingSpecification && transactionQueue == rhs.transactionQueue && historyQueue == rhs.historyQueue &&
		completionQueue == rhs.completionQueue && systemConfig == rhs.systemConfig && statistics == rhs.statistics && powerModel == rhs.powerModel &&
		algorithm == rhs.algorithm && channelID == rhs.channelID && rank == rhs.rank && refreshCounter == rhs.refreshCounter);
}

