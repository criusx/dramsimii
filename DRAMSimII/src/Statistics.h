#ifndef DRAMSTATISTICS_H
#define DRAMSTATISTICS_H
#pragma once

#include "globals.h"
#include <fstream>
#include <map>
#include "transaction.h"
#include "command.h"
#include "dramSettings.h"

namespace DRAMSimII
{
	/// @brief stores statistics about this memory system, primarily relating to counts of transactions/commands 
	class Statistics
	{

	protected:
		unsigned validTransactionCount;
		unsigned startNumber;
		unsigned endNumber;
		unsigned burstOf8Count;
		unsigned burstOf4Count;
		unsigned columnDepth;
		std::map<unsigned,unsigned> commandDelay;			///< stores the start time - enqueue time stats for commands
		std::map<unsigned,unsigned> commandExceution;		///< stores the finish time - start time stats for commands
		std::map<unsigned,unsigned> commandTurnaround;		///< stores the finish time - enqueue time stats for commands
		std::map<unsigned,unsigned> transactionDecodeDelay;	///< stores the decode time - enqueue time stats for transactions
		std::map<unsigned,unsigned> transactionExecution;	///< stores the finish time - start time stats for transactions
		std::map<unsigned long long, tick> workingSet;	///< stores all the addresses seen in an epoch to calculate the working set

	public:

		// constructors
		explicit Statistics(const Settings& settings);

		// functions
		void clear();
		void collectTransactionStats(const Transaction*);	
		void collectCommandStats(const Command*);
		inline void setValidTransactionCount(int vtc) {validTransactionCount = vtc;}
		friend std::ostream &operator<<(std::ostream &, const Statistics &);
	};
}
#endif
