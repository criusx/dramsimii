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
#include "base/stats/statdb.hh"
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
commandDelay(),
commandExecution(),
commandTurnaround(),
transactionDecodeDelay(),
transactionExecution(),
pcOccurrence(),
workingSet(),
//channelUtilization(settings.channelCount),
//rankUtilization(settings.rankCount),
//bankUtilization(settings.bankCount),
aggregateBankUtilization(settings.channelCount * settings.rankCount * settings.bankCount)
{}

// no arg constructor for deserialization
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
commandDelay(),
commandExecution(),
commandTurnaround(),
transactionDecodeDelay(),
transactionExecution(),
pcOccurrence(),
workingSet(),
//channelUtilization(0),
//rankUtilization(0),
//bankUtilization(0),
aggregateBankUtilization(0)
{}


void Statistics::collectTransactionStats(const Transaction *currentTransaction)
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
			transactionExecution[currentTransaction->getLatency()]++;
			assert(currentTransaction->getLatency() > 4);
		}
		transactionDecodeDelay[currentTransaction->getDecodeDelay()]++;

		// gather working set information for this epoch, exclude the entries which alias to the same column		
		workingSet[currentTransaction->getAddress().getPhysicalAddress() >> columnDepth]++;

		if (currentTransaction->isRead())
		{
			readCount++;
			readBytesTransferred += currentTransaction->getLength();
		}
		else
		{
			// 64bit bus for most DDRx architectures
			writeBytesTransferred += currentTransaction->getLength();
			writeCount++;
		}	

		// ignore write / tlb transactions that don't have a specific PC
		if (currentTransaction->getProgramCounter() > 0x00)
		{
			pcOccurrence[currentTransaction->getProgramCounter()].countUp();
			pcOccurrence[currentTransaction->getProgramCounter()].delay(currentTransaction->getLatency());
		}
	}
}

void Statistics::collectCommandStats(const Command *currentCommand)
{
	if (!currentCommand->isRefresh())
	{
		commandDelay[currentCommand->getDelayTime()]++;
		commandExecution[currentCommand->getExecuteTime()]++;
		commandTurnaround[currentCommand->getLatency()]++;
	}
	//channelUtilization[currentCommand->getAddress().getChannel()]++;
	//rankUtilization[currentCommand->getAddress().getRank()]++;
	//bankUtilization[currentCommand->getAddress().getBank()]++;
	aggregateBankUtilization[currentCommand->getAddress().getChannel() * (ranks * banks) + 
		currentCommand->getAddress().getRank() * banks +
		currentCommand->getAddress().getBank()]++;
}

ostream &DRAMsimII::operator<<(ostream &os, const Statistics &statsLog)
{
	using std::vector;
	//os << "RR[" << setw(6) << setprecision(6) << (double)statsLog.end_time/max(1,statsLog.bo4_count + statsLog.bo8_count) << "] ";
	//os << "BWE[" << setw(6) << setprecision(6) << ((double)statsLog.bo8_count * 8.0 + statsLog.bo4_count * 4.0) * 100.0 / max(statsLog.end_time,(tick)1) << "]" << endl;

	//os << "----R W Total----" << endl;
	//os << statsLog.readCount << " " << statsLog.writeCount << " " << statsLog.readCount + statsLog.writeCount << endl;

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
	for (std::map<PHYSICAL_ADDRESS, Statistics::DelayCounter>::const_iterator currentValue = statsLog.pcOccurrence.begin(); currentValue != statsLog.pcOccurrence.end(); currentValue++)
	{
		os << std::hex << (*currentValue).first << " " << std::noshowpoint << (float)(*currentValue).second.getAccumulatedLatency() / (float)(*currentValue).second.getCount() << " " << std::dec << (*currentValue).second.getCount() << endl;
	}
	
	os << "----Row Hit/Miss Counts----" << endl << statsLog.getHitCount() << " " << statsLog.getMissCount() << endl;
#if 0
	os << "----Channel Utilization----" << endl;
	for (vector<unsigned>::size_type i = 0; i < statsLog.channelUtilization.size(); i++)
		os << i << " " << statsLog.channelUtilization[i] << std::endl;
	os << "----Rank Utilization----" << endl;
	for (vector<unsigned>::size_type i = 0; i < statsLog.rankUtilization.size(); i++)
		os << i << " " << statsLog.rankUtilization[i] << endl;
	os << "----Bank Utilization----" << endl;
	for (vector<unsigned>::size_type i = 0; i < statsLog.bankUtilization.size(); i++)
		os << i << " " << statsLog.bankUtilization[i] << endl;
#endif
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
	

#ifdef M5
	Stats::Database::stat_list_t::iterator i = Stats::Database::stats().begin();
	Stats::Database::stat_list_t::iterator end = Stats::Database::stats().end();
	
	for (;i != end;++i) 
	{
		Stats::StatData *data = *i;
		if (data->name.find("ipc_total") != string::npos)
		{
			//cerr << data->name << " " << endl;
			//if (typeid(*data) == typeid(Stats::Formula))
			//	cerr << "Formula" << endl;
			os << "----IPC----" << endl;
			std::vector<Stats::Result>::const_iterator start = ((Stats::FormulaStatData<Stats::FormulaBase> *)data)->result().begin();
			std::vector<Stats::Result>::const_iterator end = ((Stats::FormulaStatData<Stats::FormulaBase> *)data)->result().end();
			while (start != end)
			{
				os << *start << endl;
				start++;
				// only considering single-threaded for now
				//break;
			}
			//cerr << ((Stats::FormulaData *)data)->str();
		}
		if ((data->name.find("dcache.overall_hits") != string::npos) ||
			(data->name.find("dcache.overall_misses") != string::npos) ||
			(data->name.find("dcache.overall_miss_latency") != string::npos) ||
			(data->name.find("icache.overall_hits") != string::npos) ||
			(data->name.find("icache.overall_misses") != string::npos) ||
			(data->name.find("icache.overall_miss_latency") != string::npos) ||
			(data->name.find("l2.overall_hits") != string::npos) ||
			(data->name.find("l2.overall_misses") != string::npos) ||
			(data->name.find("l2.overall_mshr_hits") != string::npos) ||
			(data->name.find("l2.overall_mshr_misses") != string::npos) ||
			(data->name.find("l2.overall_mshr_miss_latency") != string::npos) ||
			(data->name.find("l2.overall_miss_latency") != string::npos) )
		{
			{
				os << "----M5 Stat: " << data->name << " ";

				std::vector<Stats::Result>::const_iterator start = ((Stats::FormulaStatData<Stats::FormulaBase> *)data)->result().begin();
				std::vector<Stats::Result>::const_iterator end = ((Stats::FormulaStatData<Stats::FormulaBase> *)data)->result().end();
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

void Statistics::clear()
{
	using std::vector;

	commandTurnaround.clear();
	commandDelay.clear();	
	commandExecution.clear();
	transactionExecution.clear();
	transactionDecodeDelay.clear();
	workingSet.clear();
	rowHits = rowMisses = readBytesTransferred = writeBytesTransferred = readCount = writeCount = 0;
#if 0
	for (vector<unsigned>::size_type i = 0; i < channelUtilization.size(); i++)
		channelUtilization[i] = 0;
	for (vector<unsigned>::size_type i = 0; i < rankUtilization.size(); i++)
		rankUtilization[i] = 0;
	for (vector<unsigned>::size_type i = 0; i < bankUtilization.size(); i++)
		bankUtilization[i] = 0;
#endif
	for (vector<unsigned>::size_type i = 0; i < aggregateBankUtilization.size(); i++)
		aggregateBankUtilization[i] = 0;
#ifdef M5
	//async_statdump = 
	async_event = async_statreset = true;	
#endif // M5DEBUG
}

bool Statistics::operator==(const Statistics& right) const
{
	return (validTransactionCount == right.validTransactionCount && startNumber == right.startNumber && endNumber == right.endNumber &&
		burstOf8Count == right.burstOf8Count && burstOf4Count == right.burstOf4Count && columnDepth == right.columnDepth && 
		/// @todo restore comparisons once tr1 implementations support this
		//commandDelay == right.commandDelay && commandExecution == right.commandExecution && commandTurnaround == right.commandTurnaround &&
		//transactionDecodeDelay == right.transactionDecodeDelay && transactionExecution == right.transactionExecution && 
		//channelUtilization == right.channelUtilization && rankUtilization == right.rankUtilization && bankUtilization == right.bankUtilization &&
		channels == right.channels && ranks == right.ranks && banks == right.banks &&
		aggregateBankUtilization == right.aggregateBankUtilization &&
		pcOccurrence == right.pcOccurrence && workingSet == right.workingSet && readCount == right.readCount && writeCount == right.writeCount && 
		readBytesTransferred == right.readBytesTransferred && writeBytesTransferred == right.writeBytesTransferred);
}
