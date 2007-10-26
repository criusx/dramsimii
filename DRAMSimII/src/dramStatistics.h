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
	class dramStatistics
	{

	private:
		unsigned valid_transaction_count;
		unsigned start_no;
		unsigned end_no;
		unsigned bo8_count;
		unsigned bo4_count;
		unsigned columnDepth;
		std::map<unsigned,unsigned> commandDelay;
		std::map<unsigned,unsigned> commandExceution;
		std::map<unsigned,unsigned> commandTurnaround;
		std::map<unsigned,unsigned> transactionDecodeDelay;
		std::map<unsigned,unsigned> transactionExecution;
		std::map<unsigned long long, tick_t> workingSet;

	public:

		// constructors
		explicit dramStatistics(const dramSettings *settings);

		// functions
		void clear();
		void collectTransactionStats(const transaction *);	
		void collectCommandStats(const command *);
		inline void setValidTransactionCount(int vtc) {valid_transaction_count = vtc;}
		friend std::ostream &operator<<(std::ostream &, const dramStatistics &);
	};
}
#endif
