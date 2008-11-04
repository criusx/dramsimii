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

#ifndef DRAMSTATISTICS_H
#define DRAMSTATISTICS_H
#pragma once

#include "globals.h"
#include "transaction.h"
#include "command.h"
#include "Settings.h"

#include <fstream>
#include <map>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>

namespace DRAMSimII
{
	/// @brief stores statistics about this memory system, primarily relating to counts of transactions/commands 
	class Statistics
	{

	protected:
		unsigned validTransactionCount;
		unsigned startNumber;
		unsigned endNumber;
		unsigned burstOf8Count;
		unsigned burstOf4Count;
		unsigned columnDepth;
		unsigned readCount;
		unsigned writeCount;
		std::map<unsigned,unsigned> commandDelay;			///< stores the start time - enqueue time stats for commands
		std::map<unsigned,unsigned> commandExecution;		///< stores the finish time - start time stats for commands
		std::map<unsigned,unsigned> commandTurnaround;		///< stores the finish time - enqueue time stats for commands
		std::map<unsigned,unsigned> transactionDecodeDelay;	///< stores the decode time - enqueue time stats for transactions
		std::map<unsigned,unsigned> transactionExecution;	///< stores the finish time - start time stats for transactions
		std::map<PHYSICAL_ADDRESS, tick> workingSet;	///< stores all the addresses seen in an epoch to calculate the working set

	public:

		// constructors
		explicit Statistics(const Settings& settings);

		// functions
		void clear();
		void collectTransactionStats(const Transaction*);	
		void collectCommandStats(const Command*);
		inline void setValidTransactionCount(int vtc) {validTransactionCount = vtc;}
		friend std::ostream &operator<<(std::ostream &, const Statistics &);

		// overloads
		bool operator==(const Statistics& right) const;

	private:

		explicit Statistics();
		friend class boost::serialization::access;
		
		template<class Archive>
		void serialize( Archive & ar, const unsigned version )
		{
			ar & validTransactionCount & startNumber & endNumber & burstOf4Count & burstOf8Count & columnDepth & commandDelay & commandExecution &
				commandTurnaround & transactionDecodeDelay & transactionExecution & workingSet;
		}
	};
}
#endif
