#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>
#include <vector>

#include "dramChannel.h"

using namespace std;
using namespace DRAMSimII;

dramChannel::dramChannel(const dramSettings *settings):
time(0),
rank(settings->rankCount, rank_c(settings)),
refreshRowIndex(0),
lastRefreshTime(0),
lastRankID(0),
timing_specification(settings),
transactionQueue(settings->transactionQueueDepth),
refreshQueue(settings->rowCount * settings->rankCount,true),
historyQueue(settings->historyQueueDepth),
completionQueue(settings->completionQueueDepth),
systemConfig(NULL),
powerModel(settings),
algorithm(settings)
{
	for (unsigned i = 0; i < settings->rankCount; i++)
	{
		rank[i].setRankID(i);
	}
}

dramChannel::dramChannel(const dramChannel &dc):
time(dc.time),
rank(dc.rank),
refreshRowIndex(dc.refreshRowIndex),
lastRefreshTime(dc.lastRefreshTime),
lastRankID(dc.lastRankID),
timing_specification(dc.timing_specification),
transactionQueue(dc.transactionQueue),
refreshQueue(dc.refreshQueue),
historyQueue(dc.historyQueue),
completionQueue(dc.completionQueue),
systemConfig(NULL),
powerModel(dc.powerModel),
algorithm(dc.algorithm)
{
}


void dramChannel::initRefreshQueue(const unsigned rowCount,
								   const unsigned refreshTime,
								   const unsigned channel)
{
	unsigned step = refreshTime;
	step /= rowCount;
	step /= rank.size();
	int count = 0;

	for (int i = rowCount - 1; i >= 0; i--)
		for (int j = rank.size() - 1; j >= 0; j--)
		{
			refreshQueue.read(count)->setArrivalTime(count * step);
			refreshQueue.read(count)->setType(AUTO_REFRESH_TRANSACTION);
			refreshQueue.read(count)->getAddresses().rank_id = j;
			refreshQueue.read(count)->getAddresses().row_id = i;
			refreshQueue.read(count)->getAddresses().chan_id = channel;
			count++;
		}

}

void dramChannel::record_command(command *latest_command)
{
	while (historyQueue.enqueue(latest_command) == FAILURE)
	{
		delete historyQueue.dequeue();
	}
}


enum transaction_type_t	dramChannel::set_read_write_type(const int rank_id,const int bank_count) const
{
	int read_count = 0;
	int write_count = 0;
	int empty_count = 0;

	for(int i = 0; i < bank_count; ++i)
	{
		command *temp_c = rank[rank_id].bank[i].per_bank_q.read(1);

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
#ifdef DEBUG_FLAG
	cerr << "Rank[" << rank_id << "] Read[" << read_count << "] Write[" << write_count << "] Empty[" << empty_count << "]" << endl;
#endif

	if (read_count >= write_count)
		return READ_TRANSACTION;
	else
		return WRITE_TRANSACTION;
}

// calculate the power consumed by all channels during the last epoch
void dramChannel::doPowerCalculation()
{	
	for (std::vector<rank_c>::iterator k = rank.begin(); k != rank.end(); k++)
	{
		tick_t totalRAS = 1;
		for (std::vector<bank_c>::iterator l = k->bank.begin(); l != k->bank.end(); l++)
		{
			// Psys(ACT)
			totalRAS += (l->RASCount - l->previousRASCount);
			l->previousRASCount = l->RASCount;
		}
		//tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS * powerModel.tBurst / 2;
		tick_t tRRDsch = (time - powerModel.lastCalculation) / totalRAS;
		cerr << "Psys(ACT) ch[" << channelID << "] r[" << k->getRankID() << "] " << setprecision(3) << powerModel.PdsACT * powerModel.tRC / tRRDsch * (powerModel.VDD / powerModel.VDDmax) * (powerModel.VDD / powerModel.VDDmax) <<
			"(" << totalRAS << ") tRRDsch(" << tRRDsch / systemConfig->Frequency() / 1.0E-9 << "ns) lastCalc[" << powerModel.lastCalculation << "] time[" << 
			time << "]" << endl;		
	}
	powerModel.lastCalculation = time;
}

transaction *dramChannel::read_transaction() const
{
	transaction *temp_t = transactionQueue.read_back(); 
	if ((temp_t) && (time - temp_t->getEnqueueTime() < timing_specification.t_buffer_delay))
	{
#ifdef M5DEBUG
		outStream << "resetting: ";
		outStream << time << " ";
		outStream << temp_t->getEnqueueTime() << " ";
		outStream << timing_specification.t_buffer_delay << endl;
#endif
		temp_t = NULL; // not enough time has passed		
	}
	return temp_t;
}

dramChannel& dramChannel::operator =(const DRAMSimII::dramChannel &rs)
{
	if (this == &rs)
	{
		return *this;
	}
	time = rs.time;
	rank = rs.rank;
	refreshRowIndex = rs.refreshRowIndex;
	lastRankID = rs.lastRankID;
	timing_specification = rs.timing_specification;
	transactionQueue = rs.transactionQueue;
	refreshQueue = rs.refreshQueue;
	historyQueue = rs.historyQueue;
	completionQueue = rs.completionQueue;
	systemConfig = new dramSystemConfiguration(rs.systemConfig);
	powerModel = rs.powerModel;
	algorithm = rs.algorithm;
	return *this;
}
