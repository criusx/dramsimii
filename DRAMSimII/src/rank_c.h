#ifndef RANK_C_H
#define RANK_C_H
#pragma once

#include <vector>
#include "bank_c.h"
#include "globals.h"
#include "queue.h"
#include "dramSettings.h"

namespace DRAMSimII
{
	class rank_c
	{
	public:
		unsigned bankCount;
		unsigned last_bank_id; // id of the last accessed bank of this rank
		tick_t last_refresh_time;
		tick_t last_cas_time;
		tick_t last_casw_time;
		unsigned last_cas_length;
		unsigned last_casw_length;
		queue<tick_t> last_ras_times; // ras time queue. useful to determine if t_faw is met
		std::vector<bank_c> bank;

		// constructors
		explicit rank_c();
		rank_c(const rank_c &);
		explicit rank_c(const unsigned,const unsigned);	
		explicit rank_c(const dramSettings *settings);
	};
}
#endif
