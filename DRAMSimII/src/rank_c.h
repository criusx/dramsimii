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
		tick_t lastRefreshTime;		// the time of the last refresh
		tick_t lastPrechargeTime;	// the time of the last precharge
		tick_t lastCASTime;			// the time of the last CAS
		tick_t lastCASWTime;		// the time of the last CASW
		tick_t prechargeTime;		// total time that all banks in this rank are precharged
		unsigned lastCASLength;		// the length of the last CAS
		unsigned lastCASWLength;	// the length of the last CASW
		unsigned rankID;			// the ordinal number of this rank
		unsigned lastBankID;		// id of the last accessed bank of this rank
		unsigned banksPrecharged;	// the number of banks in the precharge state
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
