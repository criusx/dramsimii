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
	if (currentTransaction->getLength() == 8)
	{
		++bo8_count;
	}
	else
	{
		++bo4_count;
	}
	transactionDelay[currentTransaction->getEnqueueTime() - currentTransaction->getArrivalTime()]++;
	transactionExecution[currentTransaction->getCompletionTime() - currentTransaction->getEnqueueTime()]++;
	// gather working set information for this epoch
	workingSet[currentTransaction->getAddresses().phys_addr] = currentTransaction->getArrivalTime();
}

void dramStatistics::collectCommandStats(const command *currentCommand)
{
	commandDelay[currentCommand->getStartTime() - currentCommand->getEnqueueTime()]++;
	commandExceution[currentCommand->getCompletionTime() - currentCommand->getStartTime()]++;
	commandTurnaround[currentCommand->getCompletionTime() - currentCommand->getEnqueueTime()]++;
}

ostream &DRAMSimII::operator<<(ostream &os, const dramStatistics &this_a)
{
	os << "RR[" << setw(6) << setprecision(6) << (double)this_a.end_time/max(1,this_a.bo4_count + this_a.bo8_count);
	os << "] BWE[" << setw(6) << setprecision(6) << ((double)this_a.bo8_count * 8.0 + this_a.bo4_count * 4.0) * 100.0 / max(this_a.end_time,(tick_t)1);
	os << "]";

	os << "----Delay----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = this_a.transactionDelay.begin(); currentValue != this_a.transactionDelay.end(); currentValue++)
	{
		os << (*currentValue).first << " " << (*currentValue).second << endl;
	}
	os << "----CMD Execution Time----" << endl;
	for (map<unsigned, unsigned>::const_iterator currentValue = this_a.commandExceution.begin(); currentValue != this_a.transactionDelay.end(); currentValue++)
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
	transactionDelay.clear();
	workingSet.clear();
}
