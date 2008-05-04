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

		const TimingSpecification& timing;	///< reference to the timing information, used in calculations	
		tick lastRefreshTime;		///< the time of the last refresh
		tick lastPrechargeTime;	///< the time of the last precharge
		tick lastCASTime;			///< the time of the last CAS
		tick lastCASWTime;		///< the time of the last CASW
		tick prechargeTime;		///< total time that all banks in this rank are precharged in this epoch
		tick totalPrechargeTime;	///< total time that all banks are precharged, all time
		unsigned lastCASLength;		///< the length of the last CAS
		unsigned lastCASWLength;	///< the length of the last CASW
		unsigned rankID;			///< the ordinal number of this rank
		unsigned lastBankID;		///< id of the last accessed bank of this rank
		unsigned banksPrecharged;	///< the number of banks in the precharge state

	public:

		Queue<tick> lastRASTimes; ///< ras time queue. useful to determine if t_faw is met
		std::vector<Bank> bank;	///< the banks within this rank

		// functions
		void issueRAS(const tick currentTime, const Command *currentCommand);
		void issuePRE(const tick currentTime, const Command *currentCommand);
		void issueCAS(const tick currentTime, const Command *currentCommand);
		void issueCASW(const tick currentTime, const Command *currentCommand);
		void issueREF(const tick currentTime, const Command *currentCommand);
		
		// constructors
		rank_c(const rank_c &, const TimingSpecification &timingVal);
		explicit rank_c(const Settings& settings, const TimingSpecification &timingVal);

		// accessors
		unsigned getRankID() const { return rankID; }		
		tick getTotalPrechargeTime() const { return totalPrechargeTime; }
		tick getPrechargeTime() const { return prechargeTime; }
		tick getLastRefreshTime() const { return lastRefreshTime; }
		tick getLastCASTime() const { return lastCASTime; }
		tick getLastCASWTime() const { return lastCASWTime; }
		tick getLastPrechargeTime() const { return lastPrechargeTime; }
		unsigned getLastBankID() const { return lastBankID; }
		unsigned getLastCASLength() const { return lastCASLength; }
		unsigned getLastCASWLength() const { return lastCASWLength; }
		

		// mutators
		void setRankID(const unsigned value) { rankID = value; }
		void setLastBankID(const unsigned value) { lastBankID = value; }
		void setPrechargeTime(const tick value) { prechargeTime = value; }

		rank_c& operator =(const rank_c &rs);
	};
}
#endif
