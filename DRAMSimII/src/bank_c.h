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
	public:	
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
		bool isActivated;				// if the bank is activated, else precharged

		// stats
		unsigned RASCount;
		unsigned previousRASCount;		// the number of RAS commands since the last power calculation
		unsigned CASCount;
		unsigned previousCASCount;		// the number of CAS commands since the last power calculation
		unsigned CASWCount;
		unsigned previousCASWCount;		// the number of CASW commands since the last power calculation

		// functions
		void issueRAS(const tick_t currentTime, const command *currentCommand);
		void issuePRE(const tick_t currentTime, const command *currentCommand);
		void issueCAS(const tick_t currentTime, const command *currentCommand);
		void issueCASW(const tick_t currentTime, const command *currentCommand);
		void issueREF(const tick_t currentTime, const command *currentCommand);


		// constructors
		explicit bank_c(const dramSettings *settings, const dramTimingSpecification &timingVal);
		bank_c(const bank_c &);	
	};
}
#endif
