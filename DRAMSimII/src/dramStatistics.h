#ifndef DRAMSTATISTICS_H
#define DRAMSTATISTICS_H
#pragma once

#include "globals.h"
#include <fstream>
#include <map>
#include "transaction.h"
#include "command.h"

namespace DRAMSimII
{
	class dramStatistics
	{

	private:
		unsigned valid_transaction_count;
		int start_no;
		int end_no;
		int bo8_count;
		int bo4_count;
		std::map<unsigned,unsigned> commandDelay;
		std::map<unsigned,unsigned> commandExceution;
		std::map<unsigned,unsigned> commandTurnaround;
		std::map<unsigned,unsigned> transactionDecodeDelay;
		std::map<unsigned,unsigned> transactionExecution;
		std::map<unsigned long long, tick_t> workingSet;

	public:

		// constructors
		dramStatistics();

		// functions
		void clear();
		void collectTransactionStats(const transaction *);	
		void collectCommandStats(const command *);
		inline void set_valid_trans_count(int vtc) {valid_transaction_count = vtc;}
		friend std::ostream &operator<<(std::ostream &, const dramStatistics &);
	};
}
#endif
