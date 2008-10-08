#ifndef COMMAND_H
#define COMMAND_H
#pragma once

#include "globals.h"
#include "Address.h"
#include "queue.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/is_abstract.hpp>

namespace DRAMSimII
{
	/// @brief represents a DRAM command from the memory controller to the DRAMs
	class Command
	{
	protected:
		static Queue<Command> freeCommandPool; ///< command objects are stored here to avoid allocating memory after initialization

		mutable CommandType commandType;///< what type of command this is
		tick startTime;					///< the time at which the command started
		tick enqueueTime;				///< the time when this command was placed into the per bank queues
		tick completionTime;			///< the time when this command completed
		Address addr;					///< the addresses where this command is targeted
		Transaction *hostTransaction;	///< backward pointer to the original transaction

		// Variables added for the FB-DIMM
		//tick  link_comm_tran_comp_time;
		//tick  amb_proc_comp_time;
		//tick  dimm_comm_tran_comp_time;
		//tick  dram_proc_comp_time;
		//tick  dimm_data_tran_comp_time;
		//tick  amb_down_proc_comp_time;
		//tick  link_data_tran_comp_time;
		
		//int bundle_id;				// Bundle into which command is being sent - Do we need this ??
		//unsigned tran_id;				// The transaction id number
		//int data_word;				// Which portion of data is returned i.e. entire cacheline or fragment thereof which portions are being sent
		//int data_word_position;		// Which part of the data transmission are we doing : positions include FIRST , MIDDLE, LAST
		//bool refresh;					// This is used to determine if the ras/prec are part of refresh
		bool postedCAS;					///< This is used to determine if the ras + cas were in the same bundle
		unsigned length;				///< the burst length

	public:

		// constructors
		explicit Command(const Command&);
		explicit Command();		
		explicit Command(Transaction& hostTransaction, const tick enqueueTime, const bool postedCAS, const bool autoPrecharge, const unsigned commandLength, const CommandType commandType = READ);
		void *operator new(size_t size);
		void operator delete(void *);
		~Command();

		// accessors
		CommandType getCommandType() const { return commandType; }
		Address &getAddress() { return addr; }
		const Address &getAddress() const { return addr; }
		tick getStartTime() const { return startTime; }
		tick getEnqueueTime() const { return enqueueTime; }
		tick getCompletionTime() const { return completionTime; }
		Transaction *getHost() const { return hostTransaction; }
		unsigned getLength() const { return length; }
		bool isPostedCAS() const { return postedCAS; }

		// mutators
		Transaction *removeHost() { Transaction* host = hostTransaction; hostTransaction = NULL; return host; }
		void setStartTime(const tick st) { startTime = st; }
		void setCompletionTime(const tick ct) { completionTime = ct; }
		void setCommandType(const CommandType ct) { commandType = ct; }
		void setAutoPrecharge(const bool autoPrecharge) const;

		// friends
		friend std::ostream &DRAMSimII::operator<<(std::ostream &, const DRAMSimII::Command &);	

		// overloads
		bool operator==(const Command& right) const;

	private:
		Command& operator=(const Command& right);

		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned version)
		{
			//ar & freeCommandPool;
			ar & commandType & startTime & enqueueTime & completionTime & addr & hostTransaction & postedCAS & length;
		}
	};	
}
#endif
