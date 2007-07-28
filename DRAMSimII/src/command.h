#ifndef COMMAND_H
#define COMMAND_H
#pragma once

class transaction;

#include "globals.h"
#include "addresses.h"
#include "enumTypes.h"
#include "transaction.h"
#include "queue.h"

class command
{
private:
	static queue<command> freeCommandPool; // command objects are stored here to avoid allocating memory after initialization

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
	int data_word_position;	// Which part of the data transmission are we doing : positions include FIRST , MIDDLE, LAST
	int refresh;			// This is used to determine if the ras/prec are part of refresh
	bool posted_cas;		// This is used to determine if the ras + cas were in the same bundle
	int length;

public:
	explicit command();
	explicit command(const command &);
	explicit command(const addresses address, const command_type_t commandType, const tick_t enqueueTime, transaction *hostTransaction, const bool postedCAS);
	explicit command(const addresses address, const command_type_t commandType, const tick_t enqueueTime, transaction *hostTransaction, const bool postedCAS, const int length);
	void *operator new(size_t size);
	void operator delete(void *);

	// accessors
	command_type_t getCommandType() const { return this_command; }
	addresses &getAddress() { return addr; }
	const addresses &getAddress() const { return addr; }
	tick_t getStartTime() const { return start_time; }
	tick_t getEnqueueTime() const { return enqueue_time; }
	tick_t getCompletionTime() const { return completion_time; }
	transaction *getHost() const { return host_t; }
	int getLength() const { return length; }
	bool isPostedCAS() const { return posted_cas; }

	// mutators
	void setStartTime(const tick_t st) { start_time = st; }
	void setCompletionTime(const tick_t ct) { completion_time = ct; }
	void setCommandType(const command_type_t ct) { this_command = ct; }

	// friends
	friend std::ostream &operator<<(std::ostream &, const command &);
	// FIXME: don't want this
	friend class dramAlgorithm;
};

#endif
