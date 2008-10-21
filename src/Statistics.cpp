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
columnDepth(UINT_MAX)
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

ostream &DRAMSimII::operator<<(ostream &os, const Statistics &this_a)
{
	//os << "RR[" << setw(6) << setprecision(6) << (double)this_a.end_time/max(1,this_a.bo4_count + this_a.bo8_count) << "] ";
	//os << "BWE[" << setw(6) << setprecision(6) << ((double)this_a.bo8_count * 8.0 + this_a.bo4_count * 4.0) * 100.0 / max(this_a.end_time,(tick)1) << "]" << endl;

	os << "----Delay----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = this_a.transactionDecodeDelay.begin(); currentValue != this_a.transactionDecodeDelay.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----CMD Execution Time----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = this_a.commandExecution.begin(); currentValue != this_a.commandExecution.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Transaction Execution Time----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = this_a.transactionExecution.begin(); currentValue != this_a.transactionExecution.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----Working Set----" << endl;
	os << this_a.workingSet.size() << endl;

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
}

bool Statistics::operator==(const Statistics& right) const
{
	return (validTransactionCount == right.validTransactionCount && startNumber == right.startNumber && endNumber == right.endNumber &&
		burstOf8Count == right.burstOf8Count && burstOf4Count == right.burstOf4Count && columnDepth == right.columnDepth && 
		commandDelay == right.commandDelay && commandExecution == right.commandExecution && commandTurnaround == right.commandTurnaround &&
		transactionDecodeDelay == right.transactionDecodeDelay && transactionExecution == right.transactionExecution && workingSet == right.workingSet);
}
