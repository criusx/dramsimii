#ifndef BANK_C_H
#define BANK_C_H
#pragma once

#include "queue.h"
#include "globals.h"
#include "command.h"
#include "dramSettings.h"
#include "dramTimingSpecification.h"

namespace DRAMSimII
{
	/// this class logically represents a bank
	class bank_c
	{
	private:
		const dramTimingSpecification& timing;
	protected:	
		// members
		queue<command> perBankQueue;		// per bank queue
		tick_t lastRASTime;			// when did last RAS command start?
		tick_t lastCASTime;			// when did last CAS command start?
		tick_t lastCASWTime;			// when did last CASW command start?
		tick_t lastPrechargeTime;			// when did last Precharge command start?
		tick_t lastRefreshAllTime;	// must respect t_rfc. concurrent refresh takes time
		unsigned lastCASLength;		// the length of the last CAS command issued
		unsigned lastCASWLength;		// the length of the last CASW command issued
		unsigned openRowID;				// if the bank is open, what is the row id?
		bool activated;				// if the bank is activated, else precharged

		// stats
		unsigned RASCount;
		unsigned totalRASCount;		// the number of RAS commands since the last power calculation
		unsigned CASCount;
		unsigned totalCASCount;		// the number of CAS commands since the last power calculation
		unsigned CASWCount;
		unsigned totalCASWCount;		// the number of CASW commands since the last power calculation

	public:
		// functions
		void issueRAS(const tick_t currentTime, const command *currentCommand);
		void issuePRE(const tick_t currentTime, const command *currentCommand);
		void issueCAS(const tick_t currentTime, const command *currentCommand);
		void issueCASW(const tick_t currentTime, const command *currentCommand);
		void issueREF(const tick_t currentTime, const command *currentCommand);
		void accumulateAndResetCounts() { totalRASCount += RASCount; totalCASCount += CASCount; totalCASWCount += CASWCount; RASCount = CASWCount = CASCount = 0; }

		// accessors
		tick_t getLastRASTime() const { return lastRASTime; }
		tick_t getLastCASTime() const { return lastCASTime; }
		tick_t getLastCASWTime() const {return lastCASWTime; }
		tick_t getLastPrechargeTime() const { return lastPrechargeTime; }
		tick_t getLastRefreshAllTime() const { return lastRefreshAllTime; }
		unsigned getLastCASLength() const { return lastCASLength; }
		unsigned getLastCASWLength() const { return lastCASWLength; }
		unsigned getOpenRowID() const { return openRowID; }
		bool isActivated() const { return activated; }
		unsigned getRASCount() const { return RASCount; }
		unsigned getCASCount() const { return CASCount; }
		unsigned getCASWCount() const { return CASWCount; }
		queue<command> &getPerBankQueue() { return perBankQueue; }
		const queue<command> &getPerBankQueue() const { return perBankQueue; }

		// mutators
		void setLastRASTime(const tick_t value) { lastRASTime = value; }
		void setLastCASTime(const tick_t value) { lastCASTime = value; }
		void setLastCASWTime(const tick_t value) { lastCASWTime = value; }
		void setLastPrechargeTime(const tick_t value) { lastPrechargeTime = value; }
		void setLastRefreshAllTime(const tick_t value) { lastRefreshAllTime = value; }
		void setLastCASLength(const unsigned value) { lastCASLength = value; }
		void setLastCASWLength(const unsigned value) { lastCASWLength = value; }
		void setActivated(const bool value) { activated = value; }
		void incRASCount() { RASCount++; }
		void clrRASCount() { RASCount = 0; }
		void incCASCount() { CASCount++; }
		void clrCASCount() { CASCount = 0; }
		void incCASWCount() { CASWCount++; }
		void clrCASWCount() { CASWCount = 0; }

		// constructors
		explicit bank_c(const dramSettings& settings, const dramTimingSpecification &timingVal);
		bank_c(const bank_c &, const dramTimingSpecification &timingVal);	
	};
}
#endif
