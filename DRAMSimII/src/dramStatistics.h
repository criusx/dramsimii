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
	class dramStatistics
	{

	protected:
		unsigned valid_transaction_count;
		unsigned start_no;
		unsigned end_no;
		unsigned bo8_count;
		unsigned bo4_count;
		unsigned columnDepth;
		std::map<unsigned,unsigned> commandDelay;			///< stores the start time - enqueue time stats for commands
		std::map<unsigned,unsigned> commandExceution;		///< stores the finish time - start time stats for commands
		std::map<unsigned,unsigned> commandTurnaround;		///< stores the finish time - enqueue time stats for commands
		std::map<unsigned,unsigned> transactionDecodeDelay;	///< stores the decode time - enqueue time stats for transactions
		std::map<unsigned,unsigned> transactionExecution;	///< stores the finish time - start time stats for transactions
		std::map<unsigned long long, tick_t> workingSet;	///< stores all the addresses seen in an epoch to calculate the working set

	public:

		// constructors
		explicit dramStatistics(const dramSettings& settings);

		// functions
		void clear();
		void collectTransactionStats(const transaction *);	
		void collectCommandStats(const command *);
		inline void setValidTransactionCount(int vtc) {valid_transaction_count = vtc;}
		friend std::ostream &operator<<(std::ostream &, const dramStatistics &);
	};
}
#endif
