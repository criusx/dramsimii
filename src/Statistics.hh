// Copyright (C) 2010 University of Maryland.
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

#ifndef STATISTICS_HH
#define STATISTICS_HH
#pragma once

#include "globals.hh"
#include "transaction.hh"
#include "command.hh"
#include "Settings.hh"

#include <fstream>
#include <map>
#ifdef _MSC_VER
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

namespace DRAMsimII
{
	template <typename A, typename B>
	bool operator==(const std::tr1::unordered_map<A, B> &first, const std::tr1::unordered_map<A, B> &second)
	{
		if (first.size() != second.size()) return false;

		typename std::tr1::unordered_map<A, B>::const_iterator firstEnd = first.end();
		typename std::tr1::unordered_map<A, B>::const_iterator secondEnd = second.end();
		for (typename std::tr1::unordered_map<A, B>::const_iterator it1 = first.begin();it1 != firstEnd; it1++) 
		{
			typename std::tr1::unordered_map<A, B>::const_iterator it2 = second.find(it1->first);

			if(it2 == secondEnd || (it1->second != it2->second)) 
				return false;
		} 
		return true;
	}


	/// @brief stores statistics about this memory system, primarily relating to counts of transactions/commands
	class Statistics
	{

	public:

		template <typename T>
		class WeightedAverage
		{
			uint64_t count;
			T total;

		public:
			WeightedAverage():count(0), total(0)
			{}

			void add(T _value, uint64_t _count)
			{
				total += _value * (T)_count;
				count += _count;
			}

			void clear()
			{
				total = 0;
				count = 0;
			}

			T average() const
			{
				return total / ((count > 0) ? (T)count : (T)1);
			}
		};

		class DelayCounter
		{
		private:
			tick accumulatedLatency;				///< the total latency for this address
			unsigned count;								///< the total number of times this address has been seen
		public:
			DelayCounter():
			  accumulatedLatency(0),
				  count(0) {}

			  void countUp()
			  {
				  count++;
			  }

			  void delay(tick value)
			  {
				  accumulatedLatency += value;
			  }

			  tick getAccumulatedLatency() const { return accumulatedLatency; }
			  unsigned getCount() const { return count; }

			  bool operator==(const DelayCounter &right) const
			  {
				  return accumulatedLatency == right.accumulatedLatency &&
					  count == right.count;
			  }

		};

	protected:

		const std::vector<Channel> &channel;

		const unsigned channels;
		const unsigned ranks;
		const unsigned banks;
		unsigned validTransactionCount;
		unsigned startNumber;
		unsigned endNumber;
		unsigned burstOf8Count;
		unsigned burstOf4Count;
		unsigned columnDepth;
		unsigned readCount;
		unsigned writeCount;
		std::vector<std::pair<unsigned,unsigned> > bandwidthData;		///< per channel count of bytes read and written
		float timePerEpoch;												///< the number of seconds that have elapsed per epoch
		std::vector<std::vector<std::pair<unsigned,unsigned> > > rowBufferAccesses;												///< the number of row hits this epoch
		std::vector<std::vector<unsigned> > rasReduction;				///< the number of unnecessary RAS commands
		unsigned issuedAtTFAW;											///< the number of commands executed at exactly tFAW
		std::tr1::unordered_map<tick,unsigned> commandDelay;			///< stores the start time - enqueue time stats for commands
		std::tr1::unordered_map<tick,unsigned> commandExecution;		///< stores the finish time - start time stats for commands
		std::tr1::unordered_map<tick,unsigned> commandTurnaround;		///< stores the finish time - enqueue time stats for commands
		std::tr1::unordered_map<tick,unsigned> transactionDecodeDelay;	///< stores the decode time - enqueue time stats for transactions
		std::tr1::unordered_map<tick,unsigned> transactionExecution;	///< stores the finish time - start time stats for transactions
		// still some bugs supporting 64-bit numbers
		std::map<PhysicalAddress, DelayCounter> pcOccurrence;	///< stores the PC address, number of times it was seen and total latency
		std::map<PhysicalAddress, unsigned> workingSet;		///< stores all the addresses seen in an epoch to calculate the working set
		std::vector<unsigned> aggregateBankUtilization; ///< the bank usage per bank
		std::vector<tick> bankLatencyUtilization;	///< the latency due to each bank per unit time
	
	public:

		// constructors
		explicit Statistics(const Settings& settings, const std::vector<Channel> &);

		// functions
		std::pair<unsigned,unsigned> getReadWriteBytes() const { return std::pair<unsigned,unsigned>(getReadBytesTransferred(),getWriteBytesTransferred()); }
		void clear();
		void collectTransactionStats(const Transaction*);	
		void collectCommandStats(const Command*);
		inline void setValidTransactionCount(int vtc) {validTransactionCount = vtc;}
		inline void reportTFawCommand() { issuedAtTFAW++; }
		void reportRowBufferAccess(const Transaction *currentTransaction, bool isHit);
		
		// accessors
		const std::vector<std::vector<std::pair<unsigned,unsigned> > > &getRowBufferAccesses() const { return rowBufferAccesses; }
		const std::vector<std::pair<unsigned,unsigned> >& getBandwidthData() const { return bandwidthData;}
		
		unsigned getReadBytesTransferred() const
		{
			unsigned value = 0;

			for (std::vector<std::pair<unsigned,unsigned> >::const_iterator i = bandwidthData.begin(), end = bandwidthData.end();
				i != end; ++i)
			{
				value += i->first;
			}

			return value;
		}
			
		unsigned getWriteBytesTransferred() const
		{
			unsigned value = 0;

			for (std::vector<std::pair<unsigned,unsigned> >::const_iterator i = bandwidthData.begin(), end = bandwidthData.end();
				i != end; ++i)
			{
				value += i->second;
			}

			return value;
		}

		//const std::vector<std::vector<std::pair<std::pair<uint64_t, uint64_t>, std::pair<uint64_t, uint64_t> > > > &getHitRate() const { return hitRate; }
		friend std::ostream &operator<<(std::ostream &, const Statistics &);

		// overloads
		bool operator==(const Statistics& right) const;

	private:

		explicit Statistics(const std::vector<Channel> &);
	
	};	
}

#endif
