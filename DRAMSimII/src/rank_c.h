#ifndef RANK_C_H
#define RANK_C_H
#pragma once

#include <vector>
#include "bank_c.h"
#include "globals.h"
#include "queue.h"
#include "dramSettings.h"
#include "dramTimingSpecification.h"

namespace DRAMSimII
{
	/// @brief represents a logical rank and associated statistics
	class rank_c
	{
	protected:

		const dramTimingSpecification& timing;	///< reference to the timing information, used in calculations	
		tick_t lastRefreshTime;		///< the time of the last refresh
		tick_t lastPrechargeTime;	///< the time of the last precharge
		tick_t lastCASTime;			///< the time of the last CAS
		tick_t lastCASWTime;		///< the time of the last CASW
		tick_t prechargeTime;		///< total time that all banks in this rank are precharged in this epoch
		tick_t totalPrechargeTime;	///< total time that all banks are precharged, all time
		unsigned lastCASLength;		///< the length of the last CAS
		unsigned lastCASWLength;	///< the length of the last CASW
		unsigned rankID;			///< the ordinal number of this rank
		unsigned lastBankID;		///< id of the last accessed bank of this rank
		unsigned banksPrecharged;	///< the number of banks in the precharge state

	public:

		queue<tick_t> lastRASTimes; ///< ras time queue. useful to determine if t_faw is met
		std::vector<bank_c> bank;	///< the banks within this rank

		// functions
		void issueRAS(const tick_t currentTime, const command *currentCommand);
		void issuePRE(const tick_t currentTime, const command *currentCommand);
		void issueCAS(const tick_t currentTime, const command *currentCommand);
		void issueCASW(const tick_t currentTime, const command *currentCommand);
		void issueREF(const tick_t currentTime, const command *currentCommand);
		
		// constructors
		rank_c(const rank_c &, const dramTimingSpecification &timingVal);
		explicit rank_c(const dramSettings& settings, const dramTimingSpecification &timingVal);

		// accessors
		unsigned getRankID() const { return rankID; }		
		tick_t getTotalPrechargeTime() const { return totalPrechargeTime; }
		tick_t getPrechargeTime() const { return prechargeTime; }
		tick_t getLastRefreshTime() const { return lastRefreshTime; }
		tick_t getLastCASTime() const { return lastCASTime; }
		tick_t getLastCASWTime() const { return lastCASWTime; }
		tick_t getLastPrechargeTime() const { return lastPrechargeTime; }
		unsigned getLastBankID() const { return lastBankID; }
		unsigned getLastCASLength() const { return lastCASLength; }
		unsigned getLastCASWLength() const { return lastCASWLength; }
		

		// mutators
		void setRankID(const unsigned value) { rankID = value; }
		void setLastBankID(const unsigned value) { lastBankID = value; }
		void setPrechargeTime(const tick_t value) { prechargeTime = value; }
	};
}
#endif
