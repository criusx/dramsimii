#ifndef COMMAND_H
#define COMMAND_H
#pragma once

#include "globals.h"
#include "addresses.h"
#include "enumTypes.h"
#include "queue.h"

namespace DRAMSimII
{
	/// @brief represents a DRAM command from the memory controller to the DRAMs
	class command
	{
	private:
		static queue<command> freeCommandPool; ///< command objects are stored here to avoid allocating memory after initialization

		command_type_t commandType;		///< what type of command this is
		tick_t startTime;				///< the time at which the command started
		tick_t enqueueTime;				///< the time when this command was placed into the per bank queues
		tick_t completionTime;			///< the time when this command completed
		addresses addr;					///< the addresses where this command is targeted
		transaction *hostTransaction;	///< backward pointer to the original transaction

		// Variables added for the FB-DIMM
		//tick_t  link_comm_tran_comp_time;
		//tick_t  amb_proc_comp_time;
		//tick_t  dimm_comm_tran_comp_time;
		//tick_t  dram_proc_comp_time;
		//tick_t  dimm_data_tran_comp_time;
		//tick_t  amb_down_proc_comp_time;
		//tick_t  link_data_tran_comp_time;
		
		//int bundle_id;				// Bundle into which command is being sent - Do we need this ??
		//unsigned tran_id;				// The transaction id number
		//int data_word;				// Which portion of data is returned i.e. entire cacheline or fragment thereof which portions are being sent
		//int data_word_position;		// Which part of the data transmission are we doing : positions include FIRST , MIDDLE, LAST
		//bool refresh;					// This is used to determine if the ras/prec are part of refresh
		bool postedCAS;					///< This is used to determine if the ras + cas were in the same bundle
		unsigned length;				///< the burst length

	public:
		explicit command();
		explicit command(const command &);
		explicit command(const addresses address, const command_type_t commandType, const tick_t enqueueTime, transaction *hostTransaction, const bool postedCAS);
		explicit command(const addresses address, const command_type_t commandType, const tick_t enqueueTime, transaction *hostTransaction, const bool postedCAS, const int length);
		void *operator new(size_t size);
		void operator delete(void *);

		// accessors
		command_type_t getCommandType() const { return commandType; }
		addresses &getAddress() { return addr; }
		const addresses &getAddress() const { return addr; }
		tick_t getStartTime() const { return startTime; }
		tick_t getEnqueueTime() const { return enqueueTime; }
		tick_t getCompletionTime() const { return completionTime; }
		transaction *getHost() const { return hostTransaction; }
		unsigned getLength() const { return length; }
		bool isPostedCAS() const { return postedCAS; }

		// mutators
		void setStartTime(const tick_t st) { startTime = st; }
		void setCompletionTime(const tick_t ct) { completionTime = ct; }
		void setCommandType(const command_type_t ct) { commandType = ct; }

		// friends
		friend std::ostream &DRAMSimII::operator<<(std::ostream &, const DRAMSimII::command &);	
	};	
}
#endif
