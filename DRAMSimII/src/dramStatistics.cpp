#include "dramStatistics.h"
#include "globals.h"
#include <iomanip>
#include <iostream>

using namespace DRAMSimII;
using namespace std;

dramStatistics::dramStatistics():
end_time(0),
valid_transaction_count(0),
start_no(0),
end_no(0),
start_time(0),
bo8_count(0),
bo4_count(0),
system_config_type(0)
{}

void dramStatistics::collectTransactionStats(const transaction *currentTransaction)
{
	if (currentTransaction->getType() != AUTO_REFRESH_TRANSACTION)
	{
		if (currentTransaction->getLength() == 8)
		{
			++bo8_count;
		}
		else
		{
			++bo4_count;
		}
		transactionExecution[currentTransaction->getCompletionTime() - currentTransaction->getEnqueueTime()]++;
		transactionDecodeDelay[currentTransaction->getDecodeTime() - currentTransaction->getEnqueueTime()]++;
		// gather working set information for this epoch
		workingSet[currentTransaction->getAddresses().phys_addr]++;
	}
}

void dramStatistics::collectCommandStats(const command *currentCommand)
{
	if (currentCommand->getCommandType() != AUTO_REFRESH_TRANSACTION)
	{
		commandDelay[currentCommand->getStartTime() - currentCommand->getEnqueueTime()]++;
		commandExceution[currentCommand->getCompletionTime() - currentCommand->getStartTime()]++;
		commandTurnaround[currentCommand->getCompletionTime() - currentCommand->getEnqueueTime()]++;
	}
}

ostream &DRAMSimII::operator<<(ostream &os, const dramStatistics &this_a)
{
	os << "RR[" << setw(6) << setprecision(6) << (double)this_a.end_time/max(1,this_a.bo4_count + this_a.bo8_count);
	os << "] BWE[" << setw(6) << setprecision(6) << ((double)this_a.bo8_count * 8.0 + this_a.bo4_count * 4.0) * 100.0 / max(this_a.end_time,(tick_t)1);
	os << "]" << endl;

	os << "----Delay----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = this_a.transactionDecodeDelay.begin(); currentValue != this_a.transactionDecodeDelay.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----CMD Execution Time----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = this_a.commandExceution.begin(); currentValue != this_a.commandExceution.end(); currentValue++)
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

void dramStatistics::clear()
{
	commandTurnaround.clear();
	commandDelay.clear();	
	commandExceution.clear();
	transactionExecution.clear();
	transactionDecodeDelay.clear();
	workingSet.clear();
}
