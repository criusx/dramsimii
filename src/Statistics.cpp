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
#include <iomanip>
#include <iostream>

using std::ostream;
using std::endl;
using std::map;
using namespace DRAMSimII;

Statistics::Statistics(const Settings& settings):
validTransactionCount(0),
startNumber(0),
endNumber(0),
burstOf8Count(0),
burstOf4Count(0),
columnDepth(log2(settings.columnSize)),
readCount(0),
writeCount(0),
commandDelay(),
commandExecution()
{}

// no arg constructor for deserialization
Statistics::Statistics():
validTransactionCount(UINT_MAX),
startNumber(UINT_MAX),
endNumber(UINT_MAX),
burstOf8Count(UINT_MAX),
burstOf4Count(UINT_MAX),
columnDepth(UINT_MAX),
readCount(0),
writeCount(0)
{}


void Statistics::collectTransactionStats(const Transaction *currentTransaction)
{
	if (currentTransaction->getType() != AUTO_REFRESH_TRANSACTION)
	{
		if (currentTransaction->getLength() == 8)
		{
			++burstOf8Count;
		}
		else
		{
			++burstOf4Count;
		}
		transactionExecution[currentTransaction->getCompletionTime() - currentTransaction->getEnqueueTime()]++;
		transactionDecodeDelay[currentTransaction->getDecodeTime() - currentTransaction->getEnqueueTime()]++;

		// gather working set information for this epoch, exclude the entries which alias to the same column		
		workingSet[currentTransaction->getAddresses().getPhysicalAddress() >> columnDepth]++;
		if (currentTransaction->getType() == READ_TRANSACTION)
			readCount++;
		else
			writeCount++;

		// collect the number of bytes moved to and from the drams
		bytesTransferred += currentTransaction->getLength();
	}
}

void Statistics::collectCommandStats(const Command *currentCommand)
{
	if (currentCommand->getCommandType() != REFRESH_ALL)
	{
		commandDelay[currentCommand->getStartTime() - currentCommand->getEnqueueTime()]++;
		commandExecution[currentCommand->getCompletionTime() - currentCommand->getStartTime()]++;
		commandTurnaround[currentCommand->getCompletionTime() - currentCommand->getEnqueueTime()]++;
	}
}

ostream &DRAMSimII::operator<<(ostream &os, const Statistics &statsLog)
{
	//os << "RR[" << setw(6) << setprecision(6) << (double)statsLog.end_time/max(1,statsLog.bo4_count + statsLog.bo8_count) << "] ";
	//os << "BWE[" << setw(6) << setprecision(6) << ((double)statsLog.bo8_count * 8.0 + statsLog.bo4_count * 4.0) * 100.0 / max(statsLog.end_time,(tick)1) << "]" << endl;

	os << "----R W Total----" << endl;
	os << statsLog.readCount << " " << statsLog.writeCount << " " << statsLog.readCount + statsLog.writeCount << endl;

	os << "----Transaction Delay----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = statsLog.transactionDecodeDelay.begin(); currentValue != statsLog.transactionDecodeDelay.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Command Turnaround----" << endl;
	for (map<unsigned,unsigned>::const_iterator currentValue = statsLog.commandTurnaround.begin(); currentValue != statsLog.commandTurnaround.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Command Delay----" << endl;
	for (map<unsigned,unsigned>::const_iterator currentValue = statsLog.commandDelay.begin(); currentValue != statsLog.commandDelay.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----CMD Execution Time----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = statsLog.commandExecution.begin(); currentValue != statsLog.commandExecution.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Transaction Latency----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = statsLog.transactionExecution.begin(); currentValue != statsLog.transactionExecution.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Working Set----" << endl;
	os << statsLog.workingSet.size() << endl;

	os << "----Bandwidth----" << endl;
	os << statsLog.bytesTransferred << endl;

	return os;
}

void Statistics::clear()
{
	commandTurnaround.clear();
	commandDelay.clear();	
	commandExecution.clear();
	transactionExecution.clear();
	transactionDecodeDelay.clear();
	workingSet.clear();
	bytesTransferred = readCount = writeCount = 0;
}

bool Statistics::operator==(const Statistics& right) const
{
	return (validTransactionCount == right.validTransactionCount && startNumber == right.startNumber && endNumber == right.endNumber &&
		burstOf8Count == right.burstOf8Count && burstOf4Count == right.burstOf4Count && columnDepth == right.columnDepth && 
		commandDelay == right.commandDelay && commandExecution == right.commandExecution && commandTurnaround == right.commandTurnaround &&
		transactionDecodeDelay == right.transactionDecodeDelay && transactionExecution == right.transactionExecution && 
		workingSet == right.workingSet && readCount == right.readCount && writeCount == right.writeCount && bytesTransferred == right.bytesTransferred);
}
