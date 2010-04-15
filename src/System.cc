// Copyright (C) 2010 University of Maryland.
// This file is part of DRAMsimII.
//
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <assert.h>
#include <boost/functional.hpp>
#include <boost/lexical_cast.hpp>

#include "System.hh"

using std::setprecision;
using std::string;
using std::cerr;
using std::for_each;
using std::max;
using std::bind2nd;
using std::mem_fun_ref;
using std::vector;
using std::endl;
using std::ostream;
using std::min;
using std::stringstream;
using boost::lexical_cast;
using namespace DRAMsimII;

//////////////////////////////////////////////////////////////////////
/// @brief constructor for a dramSystem, based on dramSettings
/// @author Joe Gross
/// @param settings the settings that define what the system should look like
//////////////////////////////////////////////////////////////////////
System::System(const Settings &settings):
systemConfig(settings),
simParameters(settings),
statistics(settings, channel),
channel(systemConfig.getChannelCount(), Channel(settings, systemConfig, statistics)),
inputStream(settings, systemConfig, channel),
time(0),
nextStats(settings.epoch)
{
	//Address::initialize(settings);

	string commandLine(settings.commandLine);

	if (commandLine.length() < 1)
	{
		if (settings.inFileType == InputStream::RANDOM)
			commandLine = "Random";
		else if (settings.inFileType == InputStream::MASE_TRACE || settings.inFileType == InputStream::DRAMSIM)
		{			
			size_t begin = settings.inFile.find_last_of('/');
			size_t end = settings.inFile.find_last_of('.');
			if (begin != string::npos && end != string::npos)
				commandLine = settings.inFile.substr(begin + 1,end - begin - 1);
			else
				commandLine = settings.inFile;
		}
	}

	assert(systemConfig.statsOutStream.is_complete());

	// else printing to these streams goes nowhere
	string cacheSize = (settings.cacheSize >= 1024) ? lexical_cast<string>(settings.cacheSize / 1024) + "MB" : lexical_cast<string>(settings.cacheSize) + "kB";

	stringstream printCommandLine;

	printCommandLine << "----Command Line: " << commandLine << " ch[" << settings.channelCount << 
		"] dimm[" << settings.dimmCount <<
		"] rk[" << settings.rankCount * settings.dimmCount << "] bk[" << settings.bankCount << "] row[" << settings.rowCount <<
		"] col[" << settings.columnCount << "] [x" << settings.DQperDRAM << "] t_{RAS}[" << settings.tRAS <<
		"] t_{CAS}[" << settings.tCAS << "] t_{RCD}[" << settings.tRCD << "] t_{RC}[" << settings.tRC <<
		"] PC[" << (settings.postedCAS ? "T" : "F") << "] AMP[" << settings.addressMappingScheme << "] COA[" << settings.commandOrderingAlgorithm <<
		"] RBMP[" << settings.rowBufferManagementPolicy << "] DR[" << settings.dataRate / 1E6 <<
		"M] PBQ[" << settings.perBankQueueDepth << "] t_{FAW}[" << settings.tFAW << "] " <<
		"cache[" << cacheSize << "] " <<
		"blkSz[" << settings.blockSize << "] assoc[" << settings.associativity << "] sets[" <<
		settings.cacheSize* 1024  / settings.blockSize / settings.associativity << "]" << " policy[" <<
		settings.replacementPolicy << ((settings.replacementPolicy == Cache::NMRU) ? lexical_cast<string>(settings.nmruTrackingCount) : string("")) << "]";

	systemConfig.statsOutStream << printCommandLine.str() << endl;

	systemConfig.powerOutStream << printCommandLine.str() <<
		"IDD0[" << settings.IDD0 << "] IDD1[" << settings.IDD1 << "] IDD2N[" << settings.IDD2N << "] IDD2P[" << settings.IDD2P << "] IDD3N[" << settings.IDD3N <<
		"] IDD3P[" << settings.IDD3P << "] IDD4R[" << settings.IDD4R << "] IDD4W[" << settings.IDD4W << "] VDD[" << settings.VDD << "] VDDmax[" << settings.maxVCC <<
		"] spedFreq[" << settings.frequencySpec << "] ChannelWidth[" << settings.channelWidth * 8 << "] DQPerDRAM[" << settings.DQperDRAM << "] tBurst[" << settings.tBurst << "]" << endl;

#ifndef NDEBUG 
	systemConfig.timingOutStream << printCommandLine.str() << endl;
#endif


	systemConfig.statsOutStream << "----Epoch " << setprecision(5) << (float)settings.epoch / (float)settings.dataRate << endl;

	systemConfig.powerOutStream << "----Epoch " << setprecision(5) << (float)settings.epoch / (float)settings.dataRate << endl;
	
	systemConfig.powerOutStream << "-+++ch[" << channel.size() << "]rk[" << systemConfig.getRankCount() * systemConfig.getDimmCount() << "]+++-" << endl;	

	systemConfig.statsOutStream << "-+++ch[" << channel.size() << "]rk[" << systemConfig.getRankCount() * systemConfig.getDimmCount() << "]+++-" << endl;

	// set the channelID so that each channel may know its ordinal value
	for (unsigned i = 0; i < settings.channelCount; i++)
	{
		channel[i].setChannelID(i);		
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor
/// @details will copy channel 0 into all the other channels via copy constructor
/// and then use the assignment operator to copy the contents over
//////////////////////////////////////////////////////////////////////////
System::System(const System &rhs):
systemConfig(rhs.systemConfig),
simParameters(rhs.simParameters),
statistics(rhs.statistics),
channel(systemConfig.getChannelCount(), Channel(rhs.channel[0],systemConfig, statistics)),
inputStream(rhs.inputStream,systemConfig,channel),
time(0),
nextStats(rhs.nextStats)
{
	Address::initialize(systemConfig);
	channel = rhs.channel;
}

//////////////////////////////////////////////////////////////////////////
/// @brief deserialization constructor
//////////////////////////////////////////////////////////////////////////
System::System(const SystemConfiguration &sysConfig, const std::vector<Channel> &rhsChan, const SimulationParameters &simParams,
			   const Statistics &stats, const InputStream &inputStr):
systemConfig(sysConfig),
simParameters(simParams),
statistics(stats),
channel((unsigned)rhsChan.size(),Channel(rhsChan[0],sysConfig,statistics)),
inputStream(inputStr),
time(0),
nextStats(0)
{
	Address::initialize(systemConfig);
	channel = rhsChan;
}


//////////////////////////////////////////////////////////////////////
/// @brief returns the time at which the next event happens
/// @details returns the time when the memory system next has an event\n
/// the event may either be a conversion of a transaction into commands or it may be the the
/// next time a command may be issued
/// @author Joe Gross
/// @return the time of the next event, or TICK_MAX if there was no next event found
/// @todo should always be a next event due to perpetual stats collection, could throw an exception when there are no stats being collected
//////////////////////////////////////////////////////////////////////
tick System::nextTick() const
{
	tick nextEvent = nextStats;

	// find the next time to wake from among all the channels
	for (vector<Channel>::const_iterator currentChan = channel.begin(), end = channel.end(); currentChan != end; ++currentChan)
	{
		tick channelNextWake = currentChan->nextTick();
		assert(channelNextWake > currentChan->getTime());

		if (channelNextWake < nextEvent)
		{
			nextEvent = channelNextWake;
		}
	}
	assert(nextEvent < TICK_MAX);
	assert(nextEvent > time);
	return nextEvent;
}

//////////////////////////////////////////////////////////////////////////
/// @brief decides if enough time has passed to do a new power calculation
/// @brief or stats calculation, if so, aggregate and report results
//////////////////////////////////////////////////////////////////////////
void System::checkStats()
{
	if (time >= nextStats)
	{		
		DEBUG_TIMING_LOG("aggregate stats");
		doPowerCalculation();

		printStatistics();
	}

	while (time >= nextStats)
		nextStats += systemConfig.getEpoch();
}


//////////////////////////////////////////////////////////////////////
/// @brief updates the system time to be the same as that of the oldest channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void System::updateSystemTime()
{
	vector<Channel>::const_iterator currentChan = channel.begin();
	time = currentChan->getTime();
	currentChan++;

	for (;currentChan != channel.end(); ++currentChan)
	{
		if (currentChan->getTime() < time)
			time = currentChan->getTime();
	}
}

//////////////////////////////////////////////////////////////////////
/// @brief enqueue the transaction
/// @details attempts to enqueue the transaction in the correct channel
/// also sets the enqueue time to the current time
/// @author Joe Gross
/// @param currentTransaction the transaction to be added to the per-channel queues
/// @return true if the transaction was able to be enqueued
//////////////////////////////////////////////////////////////////////
bool System::enqueue(Transaction *currentTransaction)
{
	assert(!currentTransaction->isRefresh());

	// attempt to insert the transaction into the per-channel transaction queue
	bool result = channel[currentTransaction->getAddress().getChannel()].enqueue(currentTransaction);

	DEBUG_TRANSACTION_LOG((result ? "" : "!") << "+T ch[" << currentTransaction->getAddress().getChannel() << "](" << channel[currentTransaction->getAddress().getChannel()].getTransactionQueueCount() << "/" << channel[currentTransaction->getAddress().getChannel()].getTransactionQueueDepth() << ") " << *currentTransaction);

	return result;
}

//////////////////////////////////////////////////////////////////////////
/// @brief resets various counters to account for the fact that time starts now
/// @param time the time to reset things to begin at
//////////////////////////////////////////////////////////////////////////
void System::resetToTime(const tick time)
{
	nextStats = time + systemConfig.getEpoch();

	for (vector<Channel>::iterator i = channel.begin(); i != channel.end(); ++i)
	{
		i->resetToTime(time);
	}
}

//////////////////////////////////////////////////////////////////////
/// @brief moves all channels to the specified time
/// @details if a transaction completes before the end time is reached, it is returned and transFinishTime variable is set
/// @author Joe Gross
/// @param endTime the time which the channels should be moved to, finishing and queuing transactions as R/W finish
/// @return a transaction which finished somewhere before the end time
//////////////////////////////////////////////////////////////////////
bool System::moveToTime(const tick endTime)
{
	std::for_each(channel.begin(), channel.end(), boost::bind2nd(boost::mem_fun_ref(&Channel::moveToTime),endTime));
	//#pragma omp parallel for private(i)
	// 	for (i = channel.size() - 1; i >= 0; i--)
	// 	{
	// 		channel[i].moveToTime(endTime);
	// 	}

	updateSystemTime();

	bool result = true;

	if (time >= nextStats)
	{
		std::pair<unsigned,unsigned> rwBytes = statistics.getReadWriteBytes();
		result =  rwBytes.first + rwBytes.second > 0;
	}

	checkStats();

	return result;
}


//////////////////////////////////////////////////////////////////////
/// @brief goes through each channel to find the channel whose time is the least
/// @author Joe Gross
/// @return the ordinal of the oldest channel
//////////////////////////////////////////////////////////////////////
unsigned System::findOldestChannel() const
{
	vector<Channel>::const_iterator currentChan = channel.begin();
	tick oldestTime = currentChan->getTime();
	unsigned oldestChanID = currentChan->getChannelID();
	for (; currentChan != channel.end(); ++currentChan)
	{
		if (currentChan->getTime() < oldestTime)
		{
			oldestChanID = currentChan->getChannelID();
			oldestTime = currentChan->getTime();
		}
	}

	return oldestChanID;
}

//////////////////////////////////////////////////////////////////////
/// @brief prints out the statistics accumulated so far and about the current epoch
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void System::printStatistics()
{
	systemConfig.statsOutStream << statistics;
	statistics.clear();

	for (vector<Channel>::iterator h = channel.begin(), hEnd = channel.end();
		h != hEnd; ++h)
	{
		h->resetStats();		
	}
}

//////////////////////////////////////////////////////////////////////
/// @brief calculate and print the power consumption for each channel
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void System::doPowerCalculation()
{
	// waiting for OpenMP 3.0
	//#pragma omp parallel

	//#pragma omp for

	//for_each(channel.begin(),channel.end(),bind2nd(mem_fun_ref(&Channel::doPowerCalculation),time, systemConfig.powerOutStream));

	for (vector<Channel>::iterator i = channel.begin(), end = channel.end(); i != end; ++i)
	{
		i->doPowerCalculation(systemConfig.powerOutStream);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief the number of finished transactions queued up due to moving the channel to a time
/// @returns the number of transactions waiting in the queue
//////////////////////////////////////////////////////////////////////////
unsigned System::pendingTransactionCount() const
{
	unsigned count = 0;
	for (vector<Channel>::const_iterator i = channel.begin(), end = channel.end(); i != end; ++i)
		count += i->pendingTransactionCount();
	return count;
}

//////////////////////////////////////////////////////////////////////////
/// @brief move all the pending, finished transactions into a new queue 
//////////////////////////////////////////////////////////////////////////
void System::getPendingTransactions(std::queue<std::pair<unsigned,tick> > &outputQueue)
{
	for (vector<Channel>::iterator i = channel.begin(), end = channel.end(); i != end; ++i)
		i->getPendingTransactions(outputQueue);
}

//////////////////////////////////////////////////////////////////////
/// @brief automatically runs the simulations according to the set parameters
/// @details runs either until the trace file runs out or the request count reaches zero\n
/// will print power and general stats at regular intervals\n
/// pulls data from either a trace file or generates random requests according to parameters
/// @author Joe Gross
//////////////////////////////////////////////////////////////////////
void System::runSimulations(const unsigned requestCount)
{
	unsigned outstandingTransacitonCounter = 0;

	Transaction *inputTransaction = inputStream.getNextIncomingTransaction(outstandingTransacitonCounter++);

	resetToTime(inputTransaction->getArrivalTime());

	std::tr1::unordered_map<unsigned,std::pair<Transaction *, Transaction *> > outstandingTransactions;

	std::queue<std::pair<unsigned, tick> > finishedTransactions;

	bool running = true;

	if (inputTransaction)
	{		
		tick newTime = inputTransaction->getArrivalTime();// = (rand() % 65536) + 65536;

		resetToTime(newTime);

		for (tick i = (requestCount > 0) ? requestCount : simParameters.getRequestCount(); (i > 0) && (inputTransaction != NULL);)
		{				
			moveToTime(max(min(nextTick(), inputTransaction->getArrivalTime()),time + 1));

			if (this->pendingTransactionCount() > 0)
			{
				getPendingTransactions(finishedTransactions);

				while (!finishedTransactions.empty())
				{
					if (outstandingTransactions.find(finishedTransactions.front().first) == outstandingTransactions.end())
						assert(outstandingTransactions.find(finishedTransactions.front().first) != outstandingTransactions.end());

					delete outstandingTransactions[finishedTransactions.front().first].first;

					outstandingTransactions.erase(finishedTransactions.front().first);

					finishedTransactions.pop();
				}

			}

			// attempt to enqueue external transactions
			// as internal transactions (REFRESH) are enqueued automatically
			if (time >= inputTransaction->getArrivalTime())
			{
				if (!isFull(inputTransaction->getAddress().getChannel()))
				{
#ifndef NDEBUG
					bool result =
#endif
						enqueue(inputTransaction);

					assert(result);

					Transaction *duplicateTrans = new Transaction(*inputTransaction);

					outstandingTransactions[inputTransaction->getOriginalTransaction()] = std::pair<Transaction *, Transaction*>(duplicateTrans, inputTransaction);

					inputTransaction = inputStream.getNextIncomingTransaction(outstandingTransacitonCounter++);

					bool foundOne = false;

					if (outstandingTransacitonCounter % 5000 == 0)
					{
						for (std::tr1::unordered_map<unsigned, std::pair<Transaction *, Transaction *> >::const_iterator j = outstandingTransactions.begin(), end = outstandingTransactions.end(); j != end; ++j)
						{
							unsigned diff = time - j->second.first->getEnqueueTime();
							if (diff > 20000)
							{
								foundOne = true;
								//assert(time - i->second.first->getEnqueueTime() < 10000);
								std::cerr << j->first << " @ " << diff << std::endl;
							}
						}
					}

					if (foundOne)
						cerr << "-----------------------------" << endl;

					if (!inputTransaction)
						running = false;

					i--;
				}
			}
		}

		delete inputTransaction;

	}
	for (std::tr1::unordered_map<unsigned,std::pair<Transaction *, Transaction *> >::iterator i = outstandingTransactions.begin(),
		end = outstandingTransactions.end();
		i != end; ++i)
	{
		delete i->second.first;
		delete i->second.second;
	}
	//	moveToTime(channel[0].getTime() + 64000000);
}


//////////////////////////////////////////////////////////////////////////
/// @brief function to determine if there are any commands or transactions pending in this system
/// @return true if there are any transactions or commands in the queues, false otherwise
//////////////////////////////////////////////////////////////////////////
bool System::isEmpty() const
{
	for (vector<Channel>::const_iterator i = channel.begin(), end = channel.end(); i != end; ++i)
	{
		if (!i->isEmpty())
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
/// @brief equality operator for the System object
/// @return true if the systems are equivalent
//////////////////////////////////////////////////////////////////////////
bool System::operator==(const System &rhs) const
{
	return systemConfig == rhs.systemConfig && channel == rhs.channel && simParameters == rhs.simParameters &&
		statistics == rhs.statistics && inputStream == rhs.inputStream && time == rhs.time &&
		nextStats == rhs.nextStats;
}

//////////////////////////////////////////////////////////////////////
/// @brief serializes the dramSystem and prints it to the given ostream
/// @author Joe Gross
/// @param os the ostream to print the dramSystem information to
/// @param thisSystem the reference to the dramSystem to be printed
/// @return reference to the ostream passed in with information appended
//////////////////////////////////////////////////////////////////////
ostream &DRAMsimII::operator<<(ostream &os, const System &thisSystem)
{

	os << "SYS[";
	switch(thisSystem.systemConfig.getConfigType())
	{
	case BASELINE_CONFIG:
		os << "BASE] ";
		break;
	default:
		os << "UNKN] ";
		break;
	}
	os << "RC[" << thisSystem.systemConfig.getRankCount() << "] ";
	os << "BC[" << thisSystem.systemConfig.getBankCount() << "] ";
	os << "ALG[";
	switch(thisSystem.systemConfig.getCommandOrderingAlgorithm())
	{
	case STRICT_ORDER:
		os << "STR ] ";
		break;
	case RANK_ROUND_ROBIN:
		os << "RRR ] ";
		break;
	case BANK_ROUND_ROBIN:
		os << "BRR ] ";
		break;
	case COMMAND_PAIR_RANK_HOPPING:
		os << "CPRH] ";
		break;
	case FIRST_AVAILABLE_AGE:
		os << "GRDY] ";
		break;
	default:
		os << "UNKN] ";
		break;
	}
	
	os << "BLR[" << setprecision(0) << floor(100*(thisSystem.systemConfig.getShortBurstRatio() + 0.0001) + .5) << "] "
		<< "RP[" << (int)(100*thisSystem.systemConfig.getReadPercentage()) << "] "<< thisSystem.systemConfig;

	return os;
}

ostream& DRAMsimII::operator<<(ostream& os, const DRAMsimII::SystemConfigurationType ct)
{
	switch (ct)
	{
	case BASELINE_CONFIG:
		os << "baseline";
		break;
	case FBD_CONFIG:
		os << "fbd";
		break;
	default:
		os << "unknown";
		break;
	}

	return os;
}
ostream& DRAMsimII::operator<<(ostream& os, const DRAMsimII::RefreshPolicy rp)
{
	switch (rp)
	{
	case NO_REFRESH:
		os << "none";
		break;
	case BANK_CONCURRENT:
		os << "bankConcurrent";
		break;
	case BANK_STAGGERED_HIDDEN:
		os << "bankStaggeredHidden";
		break;
	case ONE_CHANNEL_ALL_RANK_ALL_BANK:
		os << "refreshOneChanAllRankAllBank";
		break;
	default:
		os << "unknown";
		break;
	}

	return os;
}

ostream& DRAMsimII::operator<<(ostream& os, const DRAMsimII::TransactionOrderingAlgorithm toa)
{
	switch (toa)
	{
	case RIFF:
		os << "RIFF";
		break;
	case STRICT:
		os << "strict";
		break;
	default:
		os << "unknown";
		break;
	}
	return os;
}

ostream& DRAMsimII::operator<<(ostream& os, const DRAMsimII::OutputFileType ot)
{
	switch (ot)
	{
	case COUT:
		os << "cout";
		break;
	case GZ:
		os << "gz";
		break;
	case BZ:
		os << "bz2";
		break;
	case UNCOMPRESSED:
		os << "uncompressed";
		break;
	case NONE:
		os << "none";
		break;
	default:
		os << "unknown";
		break;
	}

	return os;
}