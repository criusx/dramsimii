#ifndef DRAMSTATISTICS_H
#define DRAMSTATISTICS_H
#pragma once

#include "globals.h"
#include <fstream>
#include "transaction.h"

class dramStatistics
{

private:
	tick_t end_time;
	int valid_transaction_count;
	std::fstream *fout;
	int start_no;
	int end_no;
	double start_time; 
	int bo8_count;
	int bo4_count;
	int system_config_type;

public:
	void collect_transaction_stats(transaction *);
	dramStatistics();
	inline void set_end_time(tick_t et) {end_time = et;}
	inline void set_valid_trans_count(int vtc) {valid_transaction_count = vtc;}
	friend std::ostream &operator<<(std::ostream &, const dramStatistics &);
};

#endif
