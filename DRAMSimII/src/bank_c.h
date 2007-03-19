#ifndef BANK_C_H
#define BANK_C_H
#pragma once

#include "queue.h"
#include "globals.h"
#include "command.h"

class bank_c
{

public:	
	queue<command> per_bank_q;		// per bank queue
	tick_t last_ras_time;			// when did last ras start?
	tick_t last_cas_time;			// when did last cas start?
	tick_t last_casw_time;			// when did last cas write start?
	tick_t last_prec_time;			// when did last precharge start?
	tick_t last_refresh_all_time;	// must respect t_rfc. concurrent refresh takes time
	int last_cas_length;
	int last_casw_length;
	int row_id;						// if the bank is open, what is the row id?
	
	// stats
	int ras_count;
	int cas_count;
	int casw_count;

	// constructors
	explicit bank_c(const dramSettings *settings);
	explicit bank_c(unsigned);
	bank_c(const bank_c &);	
};

#endif
