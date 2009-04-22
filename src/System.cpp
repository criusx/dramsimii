// Copyright (C) 2008 University of Maryland.
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <assert.h>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/functional.hpp>

#ifndef WIN32
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
using boost::iostreams::bzip2_compressor;
using boost::iostreams::gzip_compressor;
using boost::iostreams::gzip_params;
#endif

#include "System.h"

using std::for_each;
using std::max;
using std::bind2nd;
using std::mem_fun_ref;
using boost::iostreams::null_sink;
using boost::iostreams::file_sink;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::cout;
using std::vector;
using std::endl;
using std::ostream;
using std::ios;
using std::setw;
using std::string;
using std::cerr;
using std::ifstream;
using std::setfill;
using std::setprecision;
using std::min;
using namespace DRAMsimII;
namespace bf = boost::filesystem;

//////////////////////////////////////////////////////////////////////
/// @brief constructor for a dramSystem, based on dramSettings
/// @author Joe Gross
/// @param settings the settings that define what the system should look like
//////////////////////////////////////////////////////////////////////
System::System(const Settings &settings):
systemConfig(settings),
simParameters(settings),
statistics(settings),
channel(systemConfig.getChannelCount(), Channel(settings, systemConfig, statistics)),
inputStream(settings, systemConfig, channel),
time(0),
nextStats(settings.epoch)
{
	Address::initialize(settings);

	// initialize the output streams
	if (!timingOutStream.is_complete())
	{

		string suffix;
		switch (settings.outFileType)
		{	
		case BZ:
#ifndef WIN32
			timingOutStream.push(bzip2_compressor());
			powerOutStream.push(bzip2_compressor());
			statsOutStream.push(bzip2_compressor());
			suffix = ".bz2";
			break;
#endif
		case GZ:
#ifndef WIN32
			timingOutStream.push(gzip_compressor(gzip_params(9)));
			powerOutStream.push(gzip_compressor(gzip_params(9)));
			statsOutStream.push(gzip_compressor(gzip_params(9)));
			suffix = ".gz";
			break;
#endif
		case UNCOMPRESSED:
			break;
		case COUT:
			timingOutStream.push(cout);
			powerOutStream.push(cout);
			statsOutStream.push(cout);
			break;
		case NONE:
			timingOutStream.push(null_sink());
			powerOutStream.push(null_sink());
			statsOutStream.push(null_sink());
			break;
		}

		bf::path outDir(settings.outFileDir.c_str());

		if (!bf::exists(outDir))
		{
			if (!bf::create_directory(outDir))
			{
				cerr << "Could not create dir " << outDir.leaf() << " and dir does not exist." << endl;
				exit(-1);
			}
		}
		else
		{
			if (!bf::is_directory(outDir))
			{
				cerr << "Directory " << outDir.leaf() << " exists, but is not a directory." << endl;
				exit(-1);
			}
		}
		if (settings.outFileType == GZ || settings.outFileType == BZ || settings.outFileType == UNCOMPRESSED)
		{
			string baseFilename = settings.outFile;

			// strip off the file suffix
			if (baseFilename.find("gz") > 0)
				baseFilename = baseFilename.substr(0,baseFilename.find(".gz"));
			if (baseFilename.find("bz2") > 0)
				baseFilename = baseFilename.substr(0,baseFilename.find(".bz2"));

			int counter = 0;		

			stringstream timingFilename;
			stringstream powerFilename;
			stringstream statsFilename;		
			stringstream settingsFilename;

			do
			{
				timingFilename.str("");
				powerFilename.str("");
				statsFilename.str("");
				settingsFilename.str("");
				timingFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(3) << counter << "-timing" << suffix;
				powerFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(3) << counter << "-power" << suffix;
				statsFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(3) << counter << "-stats" << suffix;
				settingsFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(3) << counter << "-settings.xml";	
				counter++;

			} while (fileExists(timingFilename) || fileExists(powerFilename) || fileExists(statsFilename) || fileExists(settingsFilename));

			timingOutStream.push(file_sink(timingFilename.str().c_str()));
			powerOutStream.push(file_sink(powerFilename.str().c_str()));
			statsOutStream.push(file_sink(statsFilename.str().c_str()));
			ofstream settingsOutStream(settingsFilename.str().c_str());

			if (!timingOutStream.good())
			{
				cerr << "Error opening file \"" << timingFilename << "\" for writing" << endl;
				exit(-12);
			}
			else if (!powerOutStream.good())
			{
				cerr << "Error opening file \"" << powerFilename << "\" for writing" << endl;
				exit(-12);
			}
			else if (!statsOutStream.good())
			{
				cerr << "Error opening file \"" << statsFilename << "\" for writing" << endl;
				exit(-12);
			}
			else if (!settingsOutStream.good())
			{
				cerr << "Error writing settings file" << settingsFilename << endl;
				exit(-12);
			}
			else
			{
				settingsOutStream.write(settings.settingsOutputFile.c_str(),settings.settingsOutputFile.length());
				settingsOutStream.close();
			}
		}
	}
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
	// else printing to these streams goes nowhere
	statsOutStream << "----Command Line: " << commandLine << " ch[" << settings.channelCount <<
		"] rk[" << settings.rankCount << "] bk[" << settings.bankCount << "] row[" << settings.rowCount <<
		"] col[" << settings.columnCount << "] [x" << settings.DQperDRAM << "] t_{RAS}[" << settings.tRAS <<
		"] t_{CAS}[" << settings.tCAS << "] t_{RCD}[" << settings.tRCD << "] t_{RC}[" << settings.tRC <<
		"] AMP[" << settings.addressMappingScheme << "] COA[" << settings.commandOrderingAlgorithm <<
		"] RBMP[" << settings.rowBufferManagementPolicy << "] DR[" << settings.dataRate / 1E6 <<
		"M] PBQ[" << settings.perBankQueueDepth << "] t_{FAW}[" << settings.tFAW << "]" << endl;

	powerOutStream << "----Command Line: " << commandLine << " ch[" << settings.channelCount <<
		"] rk[" << settings.rankCount << "] bk[" << settings.bankCount << "] row[" << settings.rowCount <<
		"] col[" << settings.columnCount << "] [x" << settings.DQperDRAM << "] t_{RAS}[" << settings.tRAS <<
		"] t_{CAS}[" << settings.tCAS << "] t_{RCD}[" << settings.tRCD << "] t_{RC}[" << settings.tRC <<
		"] AMP[" << settings.addressMappingScheme << "] COA[" << settings.commandOrderingAlgorithm <<
		"] RBMP[" << settings.rowBufferManagementPolicy << "] DR[" << settings.dataRate / 1E6 <<
		"M] PBQ[" << settings.perBankQueueDepth << "] t_{FAW}[" << settings.tFAW << "]" << endl;

	statsOutStream << "----Epoch " << setprecision(5) << (float)settings.epoch / (float)settings.dataRate << endl;

	powerOutStream << "----Epoch " << setprecision(5) << (float)settings.epoch / (float)settings.dataRate << endl;

	statsOutStream << "----Datarate " << setprecision(5) << (float)settings.dataRate << endl;

	powerOutStream << "----Datarate " << setprecision(5) << (float)settings.dataRate << endl;

	powerOutStream << "-+++ch[" << channel.size() << "]rk[" << systemConfig.getRankCount() << "]+++-" << endl;	

	statsOutStream << "-+++ch[" << channel.size() << "]rk[" << systemConfig.getRankCount() << "]+++-" << endl;


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

//////////////////////////////////////////////////////////////////////////
/// @brief simple destructor
//////////////////////////////////////////////////////////////////////////
System::~System()
{}

//////////////////////////////////////////////////////////////////////////
/// @brief helper function to determine if a file already exists
/// @return true if the file already exists, false otherwise
//////////////////////////////////////////////////////////////////////////
bool System::fileExists(stringstream& fileName) const
{
	bf::path newPath(fileName.str().c_str());
	return bf::exists(newPath);
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
	for (vector<Channel>::const_iterator currentChan = channel.begin(); currentChan != channel.end(); currentChan++)
	{
		tick channelNextWake = currentChan->nextTick();
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
		nextStats = time + systemConfig.getEpoch();
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

	for (;currentChan != channel.end(); currentChan++)
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

	DEBUG_TRANSACTION_LOG((result ? "" : "!") << "+T ch[" << currentTransaction->getAddress().getChannel() << "](" << channel[currentTransaction->getAddress().getChannel()].getTransactionQueueCount() << "/" << channel[currentTransaction->getAddress().getChannel()].getTransactionQueueDepth() << ") " << *currentTransaction)

		return result;
}

//////////////////////////////////////////////////////////////////////////
/// @brief resets various counters to account for the fact that time starts now
/// @param time the time to reset things to begin at
//////////////////////////////////////////////////////////////////////////
void System::resetToTime(const tick time)
{
	nextStats = time + systemConfig.getEpoch();

	for (vector<Channel>::iterator i = channel.begin(); i != channel.end(); i++)
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
void System::moveToTime(const tick endTime)
{
	//M5_TIMING_LOG("ch to [" << std::dec << endTime << "]");

	//unsigned finishedTransaction = UINT_MAX;
	//#pragma omp parallel for private(i)
	std::for_each(channel.begin(), channel.end(), boost::bind2nd(boost::mem_fun_ref(&Channel::moveToTime),endTime));
	// 	for (i = channel.size() - 1; i >= 0; i--)
	// 	{
	// 		channel[i].moveToTime(endTime);
	// 	}

	updateSystemTime();
	checkStats();
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
	for (; currentChan != channel.end(); currentChan++)
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
	statsOutStream << statistics;
	statistics.clear();
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

	for_each(channel.begin(),channel.end(),bind2nd(mem_fun_ref(&Channel::doPowerCalculation),time));
}

//////////////////////////////////////////////////////////////////////////
/// @brief the number of finished transactions queued up due to moving the channel to a time
/// @returns the number of transactions waiting in the queue
//////////////////////////////////////////////////////////////////////////
unsigned System::pendingTransactionCount() const
{
	unsigned count = 0;
	for (vector<Channel>::const_iterator i = channel.begin(); i != channel.end(); i++)
		count += i->pendingTransactionCount();
	return count;
}

//////////////////////////////////////////////////////////////////////////
/// @brief move all the pending, finished transactions into a new queue 
//////////////////////////////////////////////////////////////////////////
void System::getPendingTransactions(std::queue<std::pair<unsigned,tick> > &outputQueue)
{
	for (vector<Channel>::iterator i = channel.begin(); i != channel.end(); i++)
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
	Transaction *inputTransaction = NULL;

	inputTransaction = inputStream.getNextIncomingTransaction();

	if (inputTransaction)
	{
		tick newTime = inputTransaction->getArrivalTime();// = (rand() % 65536) + 65536;

		resetToTime(newTime);

		for (tick i = requestCount > 0 ? requestCount : simParameters.getRequestCount(); i > 0;)
		{				
			moveToTime(max(min(nextTick(), inputTransaction->getArrivalTime()),time + 1));

			if (this->pendingTransactionCount() > 0)
			{
				cerr << "not right, no host transactions here" << endl;
			}

			// attempt to enqueue external transactions
			// as internal transactions (REFRESH) are enqueued automatically
			if (time >= inputTransaction->getArrivalTime())
			{
				if (enqueue(inputTransaction))
				{
					inputTransaction = inputStream.getNextIncomingTransaction();

					if (!inputTransaction)
						break;

					i--;
				}
			}
		}

		if (inputTransaction)
			delete inputTransaction;

	}
//	moveToTime(channel[0].getTime() + 64000000);
}


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
	case WANG_RANK_HOP:
		os << "WANG] ";
		break;
	case FIRST_AVAILABLE:
		os << "GRDY] ";
		break;
	default:
		os << "UNKN] ";
		break;
	}
	//os << "BQD[" << this_a.systemConfig.getPerBankQueueDepth() << "] ";
	os << "BLR[" << setprecision(0) << floor(100*(thisSystem.systemConfig.getShortBurstRatio() + 0.0001) + .5) << "] ";
	os << "RP[" << (int)(100*thisSystem.systemConfig.getReadPercentage()) << "] ";
#if 0
	os << thisSystem.statistics;

	os << "----Command Line: " << settings.commandLine << " ch[" << thisSystem.systemConfig.getChannelCount() <<
		"] rk[" << settings.rankCount << "] bk[" << settings.bankCount << "] row[" << settings.rowCount <<
		"] col[" << settings.columnCount << "] [x" << settings.DQperDRAM << "] t_{RAS}[" << settings.tRAS <<
		"] t_{CAS}[" << settings.tCAS << "] t_{RCD}[" << settings.tRCD << "] t_{RC}[" << settings.tRC <<
		"] AMP[" << settings.addressMappingScheme << "] COA[" << settings.commandOrderingAlgorithm <<
		"] RBMP[" << settings.rowBufferManagementPolicy << "] DR[" << settings.dataRate / 1E6 << "M]" << endl;
#endif
	os << thisSystem.systemConfig;

	return os;
}
