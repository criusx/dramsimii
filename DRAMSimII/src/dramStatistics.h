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
		tick_t end_time;
		unsigned valid_transaction_count;
		int start_no;
		int end_no;
		double start_time; 
		int bo8_count;
		int bo4_count;
		int system_config_type;
		std::map<unsigned,unsigned> commandDelay;
		std::map<unsigned,unsigned> commandExceution;
		std::map<unsigned,unsigned> commandTurnaround;
		std::map<unsigned,unsigned> transactionDelay;
		std::map<unsigned,unsigned> transactionExecution;
		std::map<unsigned long long, tick_t> workingSet;

	public:

		// constructors
		dramStatistics();

		// functions
		void clear();
		void collectTransactionStats(const transaction *);	
		void collectCommandStats(const command *);
		inline void set_end_time(tick_t et) {end_time = et;}
		inline void set_valid_trans_count(int vtc) {valid_transaction_count = vtc;}
		friend std::ostream &operator<<(std::ostream &, const dramStatistics &);
	};
}
#endif
