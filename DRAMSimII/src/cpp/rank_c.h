#ifndef RANK_C_H
#define RANK_C_H
#pragma once

#include <vector>
#include "bank_c.h"
#include "globals.h"
#include "queue.h"

class rank_c
{


public:
	int bank_count;
	int last_bank_id; // id of the last accessed bank of this rank
	tick_t last_refresh_time;
	tick_t last_cas_time;
	tick_t last_casw_time;
	int last_cas_length;
	int last_casw_length;
	queue<tick_t> last_ras_times; // ras time queue. useful to determine if t_faw is met
	std::vector<bank_c> bank;

	
	explicit rank_c();
	rank_c(const rank_c &);
	explicit rank_c(const int,const int);
	void init_ranks(int,int);
};

#endif
