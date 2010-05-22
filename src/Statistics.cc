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

#include "Statistics.hh"
#include "Channel.hh"
#include "globals.hh"
#ifdef M5
#include "base/statistics.hh"
#include "base/stats/types.hh"
#include "sim/async.hh"
#include "base/stats/text.hh"
#endif

#include <iomanip>
#include <iostream>
#include <vector>

using std::ostream;
using std::endl;
using std::map;
using std::ios;
using std::tr1::unordered_map;
using std::setprecision;
using std::setiosflags;
using std::string;
using std::pair;
using std::vector;

using namespace DRAMsimII;

//////////////////////////////////////////////////////////////////////////
/// @brief constructor based on a given set of Settings
//////////////////////////////////////////////////////////////////////////
Statistics::Statistics(const Settings& settings, const vector<Channel> &_cache):
channel(_cache),
channels(settings.channelCount),
ranks(settings.rankCount * settings.dimmCount),
banks(settings.bankCount),
cacheHitLatency(settings.hitLatency + settings.tCMD + settings.tBurst),
usingDimmCache(settings.usingCache),
validTransactionCount(0),
startNumber(0),
endNumber(0),
burstOf8Count(0),
burstOf4Count(0),
columnDepth(log2(settings.columnSize)),
readCount(0),
writeCount(0),
dimmCacheBandwidthData(settings.channelCount),
bandwidthData(settings.channelCount),
timePerEpoch((float)settings.epoch / settings.dataRate),
rowBufferAccesses(settings.channelCount, vector<pair<unsigned,unsigned> >(settings.rankCount * settings.dimmCount)),
rasReduction(settings.channelCount, vector<unsigned>(settings.rankCount * settings.dimmCount)),
issuedAtTFAW(0),
commandDelay(),
commandExecution(),
commandTurnaround(),
transactionDecodeDelay(),
transactionExecution(),
//adjustedTransactionExecution(),
//cumulativeTransactionExecution(),
//cumulativeAdjustedTransactionExecution(),
cacheLatency(0),
pcOccurrence(),
workingSet(),
aggregateBankUtilization(settings.channelCount * settings.rankCount * settings.dimmCount * settings.bankCount),
bankLatencyUtilization(settings.channelCount * settings.rankCount * settings.dimmCount * settings.bankCount)
{
	bankLatencyUtilization.reserve(settings.channelCount * settings.rankCount * settings.dimmCount * settings.bankCount);
	aggregateBankUtilization.reserve(settings.channelCount * settings.rankCount * settings.dimmCount * settings.bankCount);
}

//////////////////////////////////////////////////////////////////////////
/// @brief no arg constructor for deserialization, should not be called otherwise
//////////////////////////////////////////////////////////////////////////
Statistics::Statistics(const std::vector<Channel> &channel):
channel(channel),
channels(0),
ranks(0),
banks(0),
cacheHitLatency(0),
usingDimmCache(false),
validTransactionCount(UINT_MAX),
startNumber(UINT_MAX),
endNumber(UINT_MAX),
burstOf8Count(UINT_MAX),
burstOf4Count(UINT_MAX),
columnDepth(UINT_MAX),
readCount(0),
writeCount(0),
dimmCacheBandwidthData(0),
bandwidthData(0),
timePerEpoch(0),
rowBufferAccesses(0, vector<pair<unsigned,unsigned> >(0)),
rasReduction(0, vector<unsigned>(0)),
issuedAtTFAW(0),
commandDelay(),
commandExecution(),
commandTurnaround(),
transactionDecodeDelay(),
transactionExecution(),
//adjustedTransactionExecution(),
//cumulativeTransactionExecution(),
//cumulativeAdjustedTransactionExecution(),
cacheLatency(0),
pcOccurrence(),
workingSet(),
aggregateBankUtilization(0),
bankLatencyUtilization(0)
{}

//////////////////////////////////////////////////////////////////////////
/// @brief collects transaction statistics once a transaction has finished
//////////////////////////////////////////////////////////////////////////
void Statistics::collectTransactionStats(const Transaction *currentTransaction)
{
	//#pragma omp critical
	{
		if (currentTransaction->isRead() || currentTransaction->isWrite())
		{
			if (currentTransaction->getLength() == 8)
			{
				++burstOf8Count;
			}
			else
			{
				++burstOf4Count;
			}

			transactionExecution[currentTransaction->getLatency()]++;
			//cumulativeTransactionExecution[currentTransaction->getLatency()]++;

			if (currentTransaction->isRead())
			{
				if (usingDimmCache)
				{
					// read hit
					if (currentTransaction->isHit())
					{
						//cumulativeAdjustedTransactionExecution[cacheHitLatency]++;
						//adjustedTransactionExecution[cacheHitLatency]++;
						cacheLatency += cacheHitLatency;

						dimmCacheBandwidthData[currentTransaction->getAddress().getChannel()].first += currentTransaction->getLength() * 8;
					}
					// read miss
					else
					{
						//cumulativeAdjustedTransactionExecution[currentTransaction->getLatency()]++;
						//adjustedTransactionExecution[currentTransaction->getLatency()]++;	

						bandwidthData[currentTransaction->getAddress().getChannel()].first += currentTransaction->getLength() * 8;
					}
				}				

				assert(currentTransaction->getLatency() > 4);
				unsigned index = currentTransaction->getAddress().getChannel() * (ranks * banks) +
					currentTransaction->getAddress().getRank() * banks +
					currentTransaction->getAddress().getBank();
				bankLatencyUtilization[index] += currentTransaction->getLatency();
				aggregateBankUtilization[index]++;
				readCount++;
				//readBytesTransferred += currentTransaction->getLength() * 8;
				//bandwidthData[currentTransaction->getAddress().getChannel()].first += currentTransaction->getLength() * 8;
			}			
			else if (currentTransaction->isWrite())
			{
				if (usingDimmCache)
				{
					// write hit
					if (currentTransaction->isHit())
					{
						//dimmCacheBandwidthData[currentCommand->getAddress().getChannel()].second += currentCommand->getLength() * 8;
						bandwidthData[currentTransaction->getAddress().getChannel()].second += currentTransaction->getLength() * 8;
					}
					// write miss
					else
					{
						bandwidthData[currentTransaction->getAddress().getChannel()].second += currentTransaction->getLength() * 8;
					}
				}
				
				//cumulativeAdjustedTransactionExecution[currentTransaction->getLatency()]++;
				//adjustedTransactionExecution[currentTransaction->getLatency()]++;

				// 64bit bus for most DDRx architectures
				/// @todo use #DQ * length to calculate bytes Tx, Rx
				//writeBytesTransferred += currentTransaction->getLength() * 8;
				//bandwidthData[currentTransaction->getAddress().getChannel()].second += currentTransaction->getLength() * 8;
				writeCount++;
			}

			transactionDecodeDelay[currentTransaction->getDecodeDelay()]++;

			// gather working set information for this epoch, exclude the entries which alias to the same column		
			workingSet[currentTransaction->getAddress().getPhysicalAddress() >> columnDepth]++;

			// ignore write / tlb transactions that don't have a specific PC
			if (currentTransaction->getProgramCounter() > 0x00)
			{
				pcOccurrence[currentTransaction->getProgramCounter()].countUp();
				pcOccurrence[currentTransaction->getProgramCounter()].delay(currentTransaction->getLatency());
			}
		}
	}
}

void Statistics::reportRasReduction(const Command *currentCommand)
{
	rasReduction[currentCommand->getAddress().getChannel()][currentCommand->getAddress().getRank()]++;
}

//////////////////////////////////////////////////////////////////////////
/// @brief tells of a row buffer hit when setting up commands
//////////////////////////////////////////////////////////////////////////
void Statistics::reportRowBufferAccess(const Transaction *currentTransaction, bool isHit)
{
	if (isHit)
	{
		rowBufferAccesses[currentTransaction->getAddress().getChannel()][currentTransaction->getAddress().getRank()].first++;
	}
	else
	{
		rowBufferAccesses[currentTransaction->getAddress().getChannel()][currentTransaction->getAddress().getRank()].second++;
	}
	//#pragma omp atomic
	//rowHits++;
}

//////////////////////////////////////////////////////////////////////////
/// @brief collects commands stats once a command has executed
//////////////////////////////////////////////////////////////////////////
void Statistics::collectCommandStats(const Command *currentCommand)
{
	//#pragma omp critical
	{
		if (!currentCommand->isRefresh())
		{
			assert(currentCommand->getLatency() < 2147483648);
			commandDelay[currentCommand->getDelayTime()]++;
			commandExecution[currentCommand->getExecuteTime()]++;
			commandTurnaround[currentCommand->getLatency()]++;		
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief insertion operator overload to dump statistics
/// @details prints info about the statistics since the last reset
//////////////////////////////////////////////////////////////////////////
ostream &DRAMsimII::operator<<(ostream &os, const Statistics &statsLog)
{
	using std::vector;
#if 0
	os << "RR[" << setw(6) << setprecision(6) << (double)statsLog.end_time/max(1,statsLog.bo4_count + statsLog.bo8_count) << "] ";
	os << "BWE[" << setw(6) << setprecision(6) << ((double)statsLog.bo8_count * 8.0 + statsLog.bo4_count * 4.0) * 100.0 / max(statsLog.end_time,(tick)1) << "]" << endl;

	os << "----R W Total----" << endl;
	os << statsLog.readCount << " " << statsLog.writeCount << " " << statsLog.readCount + statsLog.writeCount << endl;
#endif

	Statistics::WeightedAverage<double> averageLatency;
	os << "----Transaction Latency";
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.transactionExecution.begin(); currentValue != statsLog.transactionExecution.end(); ++currentValue)
	{
		averageLatency.add(currentValue->first,currentValue->second);
		os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	os << endl;

	os << "----Average Transaction Latency {" << averageLatency.average() << "}" << endl;
#if 0
	averageLatency.clear();

	os << "----Adjusted Transaction Latency";
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.adjustedTransactionExecution.begin(); currentValue != statsLog.adjustedTransactionExecution.end(); ++currentValue)
	{
		averageLatency.add(currentValue->first,currentValue->second);
		os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	os << endl;

	os << "----Average Adjusted Transaction Latency {" << averageLatency.average() << "}" << endl;
#endif
#if 0
	averageLatency.clear();
	os << "----Cumulative Transaction Latency";
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.cumulativeTransactionExecution.begin(); currentValue != statsLog.cumulativeTransactionExecution.end(); ++currentValue)
	{
		averageLatency.add(currentValue->first,currentValue->second);
		os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	os << endl;
	
	os << "----Average Cumulative Transaction Latency {" << averageLatency.average() << "}" << endl;
#endif
	averageLatency.clear();
	//os << "----Cumulative Adjusted Transaction Latency";
	//for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.cumulativeAdjustedTransactionExecution.begin(); currentValue != statsLog.cumulativeAdjustedTransactionExecution.end(); ++currentValue)
	{
	//	averageLatency.add(currentValue->first,currentValue->second);
	//	os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	//os << endl;

	//os << "----Average Cumulative Adjusted Transaction Latency {" << averageLatency.average() << "}" << endl;

	// transaction delay
	os << "----Transaction Delay";
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.transactionDecodeDelay.begin(); currentValue != statsLog.transactionDecodeDelay.end(); ++currentValue)
	{
		os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	os << endl; 

#if 0
	// command time
	os << "----Command Turnaround";
	for (unordered_map<unsigned,unsigned>::const_iterator currentValue = statsLog.commandTurnaround.begin(); currentValue != statsLog.commandTurnaround.end(); ++currentValue)
	{
		os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	os << endl;

	os << "----Command Delay";
	for (unordered_map<unsigned,unsigned>::const_iterator currentValue = statsLog.commandDelay.begin(); currentValue != statsLog.commandDelay.end(); ++currentValue)
	{
		os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	os << endl;

	os << "----CMD Execution Time";
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.commandExecution.begin(); currentValue != statsLog.commandExecution.end(); ++currentValue)
	{
		os << " {" << currentValue->first << "," << currentValue->second << "}";
	}
	os << endl;
#endif
	
	os << "----DIMM Cache Latency {" << statsLog.cacheLatency << "}" << endl;

	os << "----Working Set {" << statsLog.workingSet.size() << "}" << endl;

	os << "----Bandwidth {" << setprecision(10) << (float)statsLog.getReadBytesTransferred() / statsLog.timePerEpoch << "} {" << (float)statsLog.getWriteBytesTransferred() / statsLog.timePerEpoch << "}" << endl;

	os << "----DIMM Cache Bandwidth {" << setprecision(10) << (float)statsLog.getDIMMReadBytesTransferred() / statsLog.timePerEpoch << "} {" << (float)statsLog.getDIMMWriteBytesTransferred() / statsLog.timePerEpoch << "}" << endl;

	os << "----Per Channel Bandwidth ";

	unsigned currentChannel = 0;
	for (vector<pair<unsigned,unsigned> >::const_iterator i = statsLog.getBandwidthData().begin(); i != statsLog.getBandwidthData().end(); ++i)
	{
		os << "ch[" << currentChannel++ << "] R{" << i->first << "} W{" << i->second << "} ";
	}
	os << endl;

	currentChannel = 0;
	os << "----Per Channel DIMM Cache Bandwidth ";
	for (vector<pair<unsigned,unsigned> >::const_iterator i = statsLog.getDimmCacheBandwidthData().begin(); i != statsLog.getDimmCacheBandwidthData().end(); ++i)
	{
		os << "ch[" << currentChannel++ << "] R{" << i->first << "} W{" << i->second << "} ";
	}
	os << endl;

	os << "----Average Transaction Latency Per PC Value";
	for (std::map<PhysicalAddress, Statistics::DelayCounter>::const_iterator currentValue = statsLog.pcOccurrence.begin(); currentValue != statsLog.pcOccurrence.end(); ++currentValue)
	{
		os << " {" << std::hex << currentValue->first << "," << std::noshowpoint << (float)(*currentValue).second.getAccumulatedLatency() / (float)(*currentValue).second.getCount() << "," << std::dec << (*currentValue).second.getCount() << "}";
	}
	os << endl;

	unsigned hitCount = 0, missCount = 0;
	for (vector<vector<pair<unsigned,unsigned> > >::const_iterator h = statsLog.getRowBufferAccesses().begin(); h != statsLog.rowBufferAccesses.end(); ++h)
	{
		for (vector<pair<unsigned,unsigned> >::const_iterator i = h->begin(); i != h->end(); ++i)
		{
			hitCount += i->first;
			missCount += i->second;
		}
	}
	os << "----Row Hit/Miss Counts {" << hitCount << "} {" << missCount << "}" << endl;

	os << "----DIMM Cache Per-DIMM Hit/Miss Counts ";
	currentChannel = 0;
	for (vector<Channel>::const_iterator h = statsLog.channel.begin(), hEnd = statsLog.channel.end();
		h != hEnd; ++h)
	{
		unsigned currentDimm = 0;
		for (vector<Cache>::const_iterator i = h->getDimmCache().begin(), iEnd = h->getDimmCache().end();
			i != iEnd; ++i)
		{
			os << "ch[" << currentChannel << "] dimm[" << currentDimm++ <<
				"] RHit{" << i->getReadHitsMisses().first << "} RMiss{" << i->getReadHitsMisses().second <<
				"} WHit{" << i->getHitsMisses().first - i->getReadHitsMisses().first << "} WMiss{" 
				<< i->getHitsMisses().second - i->getReadHitsMisses().second << "} ";
		}
		currentChannel++;
	}
	os << endl;


	uint64_t hits = 0, misses = 0;
	for (vector<Channel>::const_iterator h = statsLog.channel.begin(), hEnd = statsLog.channel.end();
		h != hEnd; ++h)
	{
		for (vector<Cache>::const_iterator i = h->getDimmCache().begin(), iEnd = h->getDimmCache().end();
			i != iEnd; ++i)
		{
			hits += i->getHitsMisses().first;
			misses += i->getHitsMisses().second;
		}
	}
	os << "----Cache Hit/Miss Counts {" << hits << "} {" << misses << "}" << endl;

	hits = 0, misses = 0;
	uint64_t readHits = 0, readMisses = 0;
	for (vector<Channel>::const_iterator h = statsLog.channel.begin(), hEnd = statsLog.channel.end();
		h != hEnd; ++h)
	{
		for (vector<Cache>::const_iterator i = h->getDimmCache().begin(), iEnd = h->getDimmCache().end();
			i != iEnd; ++i)
		{
			readHits += i->getCumulativeReadHitsMisses().first;
			readMisses += i->getCumulativeReadHitsMisses().second;
			hits += i->getCumulativeHitsMisses().first;
			misses += i->getCumulativeHitsMisses().second;
		}
	}

	os << "----Cumulative DIMM Cache Read Hits/Misses {" << readHits << "} {" << readMisses << "}" << endl;

	os << "----Cumulative DIMM Cache Hits/Misses {" << hits << "} {" << misses << "}" << endl;

	os << "----Utilization";
	for (unsigned i = 0; i < statsLog.channels; ++i)
	{
		for (unsigned j = 0; j < statsLog.ranks; j++)
		{
			for (unsigned k = 0; k < statsLog.banks; k++)
			{
				os << " (" << i << "," << j << "," << k << ") {" << statsLog.aggregateBankUtilization[i * statsLog.ranks * statsLog.banks + j * statsLog.banks + k] << "}";
			}
		}
	}
	os << endl;

	os << "----Latency Breakdown";
	for (unsigned i = 0; i < statsLog.channels; ++i)
	{
		for (unsigned j = 0; j < statsLog.ranks; j++)
		{
			for (unsigned k = 0; k < statsLog.banks; k++)
			{
				os << " (" << i << "," << j << "," << k << ") {" << statsLog.bankLatencyUtilization[i * statsLog.ranks * statsLog.banks + j * statsLog.banks + k] << "}";
			}
		}
	}
	os << endl;

	os << "----tFAW Limited Commands {" <<  statsLog.issuedAtTFAW << "}" << endl;
#ifdef M5

	using Stats::Info;
	std::list<Info *>::const_iterator i = Stats::statsList().begin();
	std::list<Info *>::const_iterator end = Stats::statsList().end();

	for (;i != end;++i)
	{
		Info *info = *i;
		if (info->name.find("ipc_total") != string::npos)
		{
			os << "----IPC";
			std::vector<Stats::Result>::const_iterator start = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().begin();
			std::vector<Stats::Result>::const_iterator end = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().end();
			while (start != end)
			{
				os << " {" << *start << "}";
				start++;
				// only considering single-threaded for now
				//break;
			}
			os << endl;
		}
		if ((info->name.find("dcache.overall_hits") != string::npos) ||
			(info->name.find("dcache.overall_misses") != string::npos) ||
			(info->name.find("dcache.overall_miss_latency") != string::npos) ||
			(info->name.find("icache.overall_hits") != string::npos) ||
			(info->name.find("icache.overall_misses") != string::npos) ||
			(info->name.find("icache.overall_miss_latency") != string::npos) ||
			(info->name.find("l2.overall_hits") != string::npos) ||
			(info->name.find("l2.overall_misses") != string::npos) ||
			(info->name.find("l2.overall_mshr_hits") != string::npos) ||
			(info->name.find("l2.overall_mshr_misses") != string::npos) ||
			(info->name.find("l2.overall_mshr_miss_latency") != string::npos) ||
			(info->name.find("l2.overall_miss_latency") != string::npos))
		{
			{
				os << "----M5 Stat: {" << info->name << "}";

				std::vector<Stats::Result>::const_iterator start = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().begin();
				std::vector<Stats::Result>::const_iterator end = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().end();
				while (start != end)
				{
					os << " {" << *start << "}";
					start++;
				}
				os << endl;
			}
		}		
	}		
#endif

	return os;
}

//////////////////////////////////////////////////////////////////////////
/// @brief reset collected statistics up to this point
//////////////////////////////////////////////////////////////////////////
void Statistics::clear()
{
	using std::vector;

	commandTurnaround.clear();
	commandDelay.clear();	
	commandExecution.clear();
	transactionExecution.clear();
	//adjustedTransactionExecution.clear();
	transactionDecodeDelay.clear();
	workingSet.clear();
	issuedAtTFAW = /*readBytesTransferred = writeBytesTransferred =*/ readCount = writeCount = 0;

	for (vector<pair<unsigned, unsigned> >::iterator i = bandwidthData.begin(); i != bandwidthData.end(); ++i)
	{
		i->first = i->second = 0;
	}

	for (vector<pair<unsigned, unsigned> >::iterator i = dimmCacheBandwidthData.begin(); i != dimmCacheBandwidthData.end(); ++i)
	{
		i->first = i->second = 0;
	}

	for (vector<vector<pair<unsigned,unsigned> > >::iterator h = rowBufferAccesses.begin(); h != rowBufferAccesses.end(); ++h)
	{
		for (vector<pair<unsigned,unsigned> >::iterator i = h->begin(); i != h->end(); ++i)
		{
			i->first = i->second = 0;
		}
	}

	for (vector<unsigned>::size_type i = 0; i < aggregateBankUtilization.size(); ++i)
		aggregateBankUtilization[i] = 0;
	for (vector<unsigned>::size_type i = 0; i < bankLatencyUtilization.size(); ++i)
		bankLatencyUtilization[i] = 0;
	pcOccurrence.clear();

	for (vector<vector<unsigned> >::iterator h = rasReduction.begin(); h != rasReduction.end(); ++h)
	{
		for (vector<unsigned>::iterator i = h->begin(); i != h->end(); ++i)
			*i = 0;
	}
#ifdef M5
	//async_statdump =
	async_event = async_statreset = true;	
#endif // M5DEBUG
}

//////////////////////////////////////////////////////////////////////////
/// @brief equality operator to test if two sets of statistics are equal
//////////////////////////////////////////////////////////////////////////
bool Statistics::operator==(const Statistics& rhs) const
{
	return (validTransactionCount == rhs.validTransactionCount && startNumber == rhs.startNumber && endNumber == rhs.endNumber &&
		burstOf8Count == rhs.burstOf8Count && burstOf4Count == rhs.burstOf4Count && columnDepth == rhs.columnDepth &&
		/// @todo restore comparisons once tr1 implementations support this
		commandDelay ==  rhs.commandDelay && commandExecution == rhs.commandExecution && commandTurnaround == rhs.commandTurnaround &&
		transactionDecodeDelay == rhs.transactionDecodeDelay && transactionExecution == rhs.transactionExecution &&
		channels == rhs.channels && ranks == rhs.ranks && banks == rhs.banks && aggregateBankUtilization == rhs.aggregateBankUtilization &&
		pcOccurrence == rhs.pcOccurrence && workingSet == rhs.workingSet && readCount == rhs.readCount && writeCount == rhs.writeCount &&
		bandwidthData == rhs.bandwidthData && dimmCacheBandwidthData == rhs.dimmCacheBandwidthData &&
		/*readBytesTransferred == rhs.readBytesTransferred && writeBytesTransferred == rhs.writeBytesTransferred &&*/ issuedAtTFAW == rhs.issuedAtTFAW);
}

