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

#include "Statistics.h"
#include "globals.h"
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

using namespace DRAMsimII;

//////////////////////////////////////////////////////////////////////////
/// @brief constructor based on a given set of Settings
//////////////////////////////////////////////////////////////////////////
Statistics::Statistics(const Settings& settings):
channels(settings.channelCount),
ranks(settings.rankCount),
banks(settings.bankCount),
validTransactionCount(0),
startNumber(0),
endNumber(0),
burstOf8Count(0),
burstOf4Count(0),
columnDepth(log2(settings.columnSize)),
readCount(0),
writeCount(0),
readBytesTransferred(0),
writeBytesTransferred(0),
timePerEpoch((float)settings.epoch / settings.dataRate),
rowHits(0),
rowMisses(0),
issuedAtTFAW(0),
commandDelay(),
commandExecution(),
commandTurnaround(),
transactionDecodeDelay(),
transactionExecution(),
pcOccurrence(),
workingSet(),
aggregateBankUtilization(settings.channelCount * settings.rankCount * settings.bankCount),
bankLatencyUtilization(settings.channelCount * settings.rankCount * settings.bankCount)
{
	bankLatencyUtilization.reserve(settings.channelCount * settings.rankCount * settings.bankCount);
	aggregateBankUtilization.reserve(settings.channelCount * settings.rankCount * settings.bankCount);
}

//////////////////////////////////////////////////////////////////////////
/// @brief no arg constructor for deserialization, should not be called otherwise
//////////////////////////////////////////////////////////////////////////
Statistics::Statistics():
channels(0),
ranks(0),
banks(0),
validTransactionCount(UINT_MAX),
startNumber(UINT_MAX),
endNumber(UINT_MAX),
burstOf8Count(UINT_MAX),
burstOf4Count(UINT_MAX),
columnDepth(UINT_MAX),
readCount(0),
writeCount(0),
readBytesTransferred(0),
writeBytesTransferred(0),
timePerEpoch(0),
rowHits(0),
rowMisses(0),
issuedAtTFAW(0),
commandDelay(),
commandExecution(),
commandTurnaround(),
transactionDecodeDelay(),
transactionExecution(),
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
			if (currentTransaction->isRead())
			{
				//if (currentTransaction->getLatency() > 1024)
				//	std::cerr << currentTransaction->getLatency() << std::endl;
				transactionExecution[currentTransaction->getLatency()]++;
				assert(currentTransaction->getLatency() > 4);
				unsigned index = currentTransaction->getAddress().getChannel() * (ranks * banks) +
					currentTransaction->getAddress().getRank() * banks +
					currentTransaction->getAddress().getBank();
				bankLatencyUtilization[index] += currentTransaction->getLatency();
				aggregateBankUtilization[index]++;
				readCount++;
				readBytesTransferred += currentTransaction->getLength() * 8;
			}
			else
			{
				// 64bit bus for most DDRx architectures
				/// @todo use #DQ * length to calculate bytes Tx, Rx
				writeBytesTransferred += currentTransaction->getLength() * 8;
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

//////////////////////////////////////////////////////////////////////////
/// @brief collects commands stats once a command has executed
//////////////////////////////////////////////////////////////////////////
void Statistics::collectCommandStats(const Command *currentCommand)
{
	//#pragma omp critical
	{
		if (!currentCommand->isRefresh())
		{
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
	os << "----Transaction Delay " << statsLog.transactionDecodeDelay.size() << "----" << endl;
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.transactionDecodeDelay.begin(); currentValue != statsLog.transactionDecodeDelay.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Command Turnaround " << statsLog.commandTurnaround.size() << "----" << endl;
	for (unordered_map<unsigned,unsigned>::const_iterator currentValue = statsLog.commandTurnaround.begin(); currentValue != statsLog.commandTurnaround.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Command Delay " << statsLog.commandDelay.size() << "----" << endl;
	for (unordered_map<unsigned,unsigned>::const_iterator currentValue = statsLog.commandDelay.begin(); currentValue != statsLog.commandDelay.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----CMD Execution Time " << statsLog.commandExecution.size() << "----" << endl;
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.commandExecution.begin(); currentValue != statsLog.commandExecution.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Transaction Latency " << statsLog.transactionExecution.size() << "----" << endl;
	for (unordered_map<unsigned, unsigned>::const_iterator currentValue = statsLog.transactionExecution.begin(); currentValue != statsLog.transactionExecution.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}

	os << "----Working Set----" << endl << statsLog.workingSet.size() << endl;

	os << "----Bandwidth----" << endl << setprecision(10) << (float)statsLog.readBytesTransferred / statsLog.timePerEpoch << " " << (float)statsLog.writeBytesTransferred / statsLog.timePerEpoch << endl;

	os << "----Average Transaction Latency Per PC Value " << statsLog.pcOccurrence.size() << "----" << endl;
	for (std::map<PhysicalAddress, Statistics::DelayCounter>::const_iterator currentValue = statsLog.pcOccurrence.begin(); currentValue != statsLog.pcOccurrence.end(); currentValue++)
	{
		os << std::hex << (*currentValue).first << " " << std::noshowpoint << (float)(*currentValue).second.getAccumulatedLatency() / (float)(*currentValue).second.getCount() << " " << std::dec << (*currentValue).second.getCount() << endl;
	}

	os << "----Row Hit/Miss Counts----" << endl << statsLog.getHitCount() << " " << statsLog.getMissCount() << endl;

	os << "----Utilization----" << endl;
	for (unsigned i = 0; i < statsLog.channels; i++)
	{
		for (unsigned j = 0; j < statsLog.ranks; j++)
		{
			for (unsigned k = 0; k < statsLog.banks; k++)
			{
				os << "(" << i << "," << j << "," << k << ") " << statsLog.aggregateBankUtilization[i * statsLog.ranks * statsLog.banks + j * statsLog.banks + k] << endl;
			}
		}
	}

	os << "----Latency Breakdown----" << endl;
	for (unsigned i = 0; i < statsLog.channels; i++)
	{
		for (unsigned j = 0; j < statsLog.ranks; j++)
		{
			for (unsigned k = 0; k < statsLog.banks; k++)
			{
				os << "(" << i << "," << j << "," << k << ") " << statsLog.bankLatencyUtilization[i * statsLog.ranks * statsLog.banks + j * statsLog.banks + k] << endl;
			}
		}
	}

	os << "----tFAW Limited Commands----" << endl << statsLog.issuedAtTFAW << endl;
#ifdef M5

	using Stats::Info;
	std::list<Info *>::const_iterator i = Stats::statsList().begin();
	std::list<Info *>::const_iterator end = Stats::statsList().end();

	for (;i != end;++i)
	{
		Info *info = *i;
		if (info->name.find("ipc_total") != string::npos)
		{
			os << "----IPC----" << endl;
			std::vector<Stats::Result>::const_iterator start = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().begin();
			std::vector<Stats::Result>::const_iterator end = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().end();
			while (start != end)
			{
				os << *start << endl;
				start++;
				// only considering single-threaded for now
				//break;
			}
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
				os << "----M5 Stat: " << info->name << " ";

				std::vector<Stats::Result>::const_iterator start = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().begin();
				std::vector<Stats::Result>::const_iterator end = ((Stats::FormulaInfoProxy<Stats::Formula> *)info)->result().end();
				while (start != end)
				{
					os << *start << " ";
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
	transactionDecodeDelay.clear();
	workingSet.clear();
	issuedAtTFAW = rowHits = rowMisses = readBytesTransferred = writeBytesTransferred = readCount = writeCount = 0;
	for (vector<unsigned>::size_type i = 0; i < aggregateBankUtilization.size(); i++)
		aggregateBankUtilization[i] = 0;
	for (vector<unsigned>::size_type i = 0; i < bankLatencyUtilization.size(); i++)
		bankLatencyUtilization[i] = 0;
	pcOccurrence.clear();
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
		readBytesTransferred == rhs.readBytesTransferred && writeBytesTransferred == rhs.writeBytesTransferred && issuedAtTFAW == rhs.issuedAtTFAW);
}

