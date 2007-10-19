#ifndef BANK_C_H
#define BANK_C_H
#pragma once

#include "queue.h"
#include "globals.h"
#include "command.h"
#include "dramSettings.h"

namespace DRAMSimII
{
	/// this class logically represents a bank
	class bank_c
	{
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

		// constructors
		explicit bank_c(const dramSettings *settings);
		explicit bank_c(unsigned);
		bank_c(const bank_c &);	
	};
}
#endif
