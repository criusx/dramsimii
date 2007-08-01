#ifndef BANK_C_H
#define BANK_C_H
#pragma once

#include "queue.h"
#include "globals.h"
#include "command.h"
#include "dramSettings.h"

namespace DRAMSimII
{
	class bank_c
	{
	public:	
		// members
		queue<command> per_bank_q;		// per bank queue
		tick_t last_ras_time;			// when did last RAS command start?
		tick_t last_cas_time;			// when did last CAS command start?
		tick_t last_casw_time;			// when did last CASW command start?
		tick_t last_prec_time;			// when did last Precharge command start?
		tick_t last_refresh_all_time;	// must respect t_rfc. concurrent refresh takes time
		unsigned last_cas_length;		// the length of the last CAS command issued
		unsigned last_casw_length;		// the length of the last CASW command issued
		unsigned row_id;				// if the bank is open, what is the row id?
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
