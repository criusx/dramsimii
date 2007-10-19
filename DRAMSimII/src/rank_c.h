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
		tick_t lastRefreshTime;
		tick_t lastPrechargeTime;
		tick_t lastCASTime;
		tick_t lastCASWTime;
		unsigned lastCASLength;
		unsigned lastCASWLength;
		unsigned rankID;		// the ordinal number of this rank
		unsigned lastBankID; // id of the last accessed bank of this rank
		queue<tick_t> lastRASTimes; // ras time queue. useful to determine if t_faw is met
		std::vector<bank_c> bank;
		

		// constructors
		rank_c(const rank_c &);
		explicit rank_c(const dramSettings *settings);

		// accessors
		unsigned getRankID() const { return rankID; }
		// mutators
		void setRankID(const unsigned value) { rankID = value; }
	};
}
#endif
