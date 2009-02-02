// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
// 
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#ifndef COMMAND_H
#define COMMAND_H
#pragma once

#include "globals.h"
#include "Address.h"
#include "queue.h"
#include "transaction.h"
#include "event.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

namespace DRAMsimII
{
	/// @brief represents a DRAM command from the memory controller to the DRAMs
	class Command: public Event
	{
	protected:
		static Queue<Command> freeCommandPool; ///< command objects are stored here to avoid allocating memory after initialization

		mutable CommandType commandType;///< what type of command this is
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
		~Command();

		// accessors
		CommandType getCommandType() const { return commandType; }
		Transaction *getHost() const { return hostTransaction; }
		unsigned getLength() const { return length; }
		bool isPostedCAS() const { return postedCAS; }
		bool isRead() const { return ((commandType == READ) || (commandType == READ_AND_PRECHARGE) || (commandType == IFETCH_TRANSACTION)); }
		bool isWrite() const { return ((commandType == WRITE) || (commandType == WRITE_AND_PRECHARGE)); }
		bool isReadOrWrite() const { return isRead() || isWrite(); }

		// mutators
		Transaction *removeHost() { Transaction* host = hostTransaction; hostTransaction = NULL; return host; }
		void setAutoPrecharge(const bool autoPrecharge) const;

		// friends
		friend std::ostream &DRAMsimII::operator<<(std::ostream &, const DRAMsimII::Command &);	

		// overloads
		bool operator==(const Command& right) const;
		bool operator!=(const Command& right) const;
		void *operator new(size_t size);
		void operator delete(void *);

	private:
		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned version)
		{
			// @todo serialize inherited types
			//ar & freeCommandPool;
			//ar & commandType & startTime & enqueueTime & completionTime & addr & hostTransaction & postedCAS & length;
			ar & commandType & hostTransaction & postedCAS & length;
		}
	};	
}
#endif
