#ifndef COMMAND_H
#define COMMAND_H
#pragma once

#include "addresses.h"
#include "enumTypes.h"
#include "globals.h"
#include "transaction.h"

class command
{
public:
	command_type_t this_command;	// which command is this?
	tick_t start_time;				// when did the command start?
	tick_t enqueue_time;			// when did it make it into the per bank command queue?
	tick_t completion_time;
	addresses	addr;
	transaction *host_t;			// backward pointer to the original transaction

	tick_t  link_comm_tran_comp_time;
	tick_t  amb_proc_comp_time;
	tick_t  dimm_comm_tran_comp_time;
	tick_t  dram_proc_comp_time;
	tick_t  dimm_data_tran_comp_time;
	tick_t  amb_down_proc_comp_time;
	tick_t  link_data_tran_comp_time;

	// Variables added for the FB-DIMM
	int bundle_id;			// Bundle into which command is being sent - Do we need this ??
	unsigned tran_id;		// The transaction id number
	int data_word;			// Which portion of data is returned i.e. entire cacheline or fragment thereof which portions are being sent
	int data_word_position;	// Which part of the data transmission are we doing : postions include FIRST , MIDDLE, LAST
	int refresh;			// This is used to determine if the ras/prec are part of refresh
	bool posted_cas;		// This is used to determine if the ras + cas were in the same bundle
	int length;

	command();
	explicit command(const command &);
};

#endif