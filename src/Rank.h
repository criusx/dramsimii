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

#ifndef RANK_C_H
#define RANK_C_H
#pragma once

#include "globals.h"
#include "Bank.h"
#include "Settings.h"
#include "TimingSpecification.h"

#include <vector>

#include <boost/circular_buffer.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

namespace DRAMsimII
{
	/// @brief represents a logical rank and associated statistics
	class Rank
	{
	private:

		const TimingSpecification& timing;	///< reference to the timing information, used in calculations	

	protected:

		tick lastRefreshTime;				///< the time of the last refresh
		tick lastPrechargeAnyBankTime;		///< the time of the last precharge
		tick lastCASTime;					///< the time of the last CAS
		tick lastCASWTime;					///< the time of the last CASW

		tick otherLastCASTime;				///< the time of the most recent CAS from any other rank on this channel
		tick otherLastCASWTime;				///< the time of the most recent CASW from any other rank on this channel

		tick prechargeTime;					///< total time that all banks in this rank are precharged in this epoch
		tick totalPrechargeTime;			///< total time that all banks are precharged, all time

		tick nextActivateTime;				///< the time at which an ACT may be sent to this rank
		tick nextReadTime;					///< the time at which a CAS may be sent to this rank
		tick nextWriteTime;					///< the time at which a CASW may be sent to this rank
		//tick nextPrechargeTime;				///< the time at which a Pre may be sent to this rank
		tick nextRefreshTime;				///< the time at which a Ref may be sent to this rank

		tick lastCalculationTime;			///< the time at which the last power calculation was done

		unsigned lastCASLength;				///< the length of the last CAS
		unsigned lastCASWLength;			///< the length of the last CASW
		unsigned otherLastCASLength;		///< the length of the last CAS on any other rank
		unsigned otherLastCASWLength;		///< the length of the last CASW on any other rank
		unsigned CASLength;					///< total cycles the bus spent sending data
		unsigned CASWLength;				///< the total cycles the bus spent receiving data
		unsigned rankID;					///< the ordinal number of this rank
		unsigned lastBankID;				///< id of the last accessed bank of this rank
		unsigned banksPrecharged;			///< the number of banks in the precharge state

	public:

		boost::circular_buffer<tick> lastActivateTimes; ///< ras time queue. useful to determine if t_faw is met
		std::vector<Bank> bank;							///< the banks within this rank

		// functions
		bool refreshAllReady() const;
		Command *getCommand(const unsigned bank);
		void issueRAS(const tick currentTime, const Command *currentCommand);
		void issuePRE(const tick currentTime, const Command *currentCommand);
		void issueCAS(const tick currentTime, const Command *currentCommand);
		void issueCASother(const tick currentTime, const Command *currentCommand);
		void issueCASW(const tick currentTime, const Command *currentCommand);
		void issueCASWother(const tick currentTime, const Command *currentCommand);
		void issueREF(const tick currentTime);
		void resetToTime(const tick time);
		tick next(Command::CommandType nextCommandType) const;

		// constructors
		explicit Rank(const Rank &, const TimingSpecification &, const SystemConfiguration &);
		explicit Rank(const Settings& settings, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal);
		Rank(const Rank &);

		// accessors
		unsigned getRankID() const { return rankID; }		
		tick getTotalPrechargeTime(const tick currentTime) const;  
		tick getPrechargeTime(const tick currentTime) const;
		tick getLastRefreshTime() const { return lastRefreshTime; }
		tick getLastCASTime() const { return lastCASTime; }
		tick getLastCASWTime() const { return lastCASWTime; }
		tick getOtherLastCASTime() const { return otherLastCASTime; }
		tick getOtherLastCASWTime() const { return otherLastCASWTime; }
		tick getLastPrechargeTime() const { return lastPrechargeAnyBankTime; }
		unsigned getLastBankID() const { return lastBankID; }
		unsigned getLastCASLength() const { return lastCASLength; }
		unsigned getLastCASWLength() const { return lastCASWLength; }
		unsigned getOtherLastCASLength() const { return otherLastCASLength; }
		unsigned getOtherLastCASWLength() const { return otherLastCASWLength; }
		unsigned getReadCycles() const { return CASLength; }
		unsigned getWriteCycles() const { return CASWLength; }
		bool isEmpty() const;


		// mutators
		void setRankID(const unsigned value) { rankID = value; }
		void setLastBankID(const unsigned value) { lastBankID = value; }
		void resetPrechargeTime(const tick time); 
		void resetCycleCounts() { CASLength = CASWLength = 0; }

		// overloads
		Rank& operator=(const Rank &rs);
		bool operator==(const Rank& right) const;
		friend std::ostream& operator<<(std::ostream& os, const Rank& r);

	private:
		//serialization
		explicit Rank(const TimingSpecification &timing, const std::vector<Bank> & newBank);
		explicit Rank();

		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned version)
		{
			if (version == 0)
			{
				ar & lastRefreshTime & lastPrechargeAnyBankTime & lastCASTime & lastCASWTime & prechargeTime & totalPrechargeTime & lastCASLength &
					lastCASWLength & rankID & lastBankID & banksPrecharged & lastActivateTimes & CASWLength & CASLength & otherLastCASTime & 
					otherLastCASWTime & otherLastCASLength & otherLastCASWLength & nextActivateTime & nextReadTime & nextWriteTime & 
					nextRefreshTime & lastCalculationTime;
			}

		}

		template <class Archive>
		friend inline void save_construct_data(Archive& ar, const DRAMsimII::Rank* t, const unsigned version)
		{
			if (version == 0)
			{
				const DRAMsimII::TimingSpecification* const timing = &(t->timing);
				ar << timing;
				const std::vector<DRAMsimII::Bank>* const bank = &(t->bank);
				ar << bank;	
			}

		}

		template <class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMsimII::Rank *t, const unsigned version)
		{
			if (version == 0)
			{
				DRAMsimII::TimingSpecification* timing;
				ar >> timing;
				std::vector<DRAMsimII::Bank>* newBank;
				ar >> newBank;

				::new(t)DRAMsimII::Rank(*timing, *newBank);
			}

		}		
	};
}

namespace boost
{
	template<class Archive, class U, class Allocator>
	inline void serialize(Archive &ar, boost::circular_buffer<U, Allocator> &t, const unsigned version)
	{
		if (version == 0)
		{
			boost::serialization::split_free(ar,t,version);
		}
	}

	template<class Archive, class U, class Allocator>
	inline void save(Archive &ar, const boost::circular_buffer<U, Allocator> &t, const unsigned version)
	{
		if (version == 0)
		{
			//const circular_buffer<U>::size_type maxCount(t.capacity());
			//ar << BOOST_SERIALIZATION_NVP(maxCount);
			typename circular_buffer<U, Allocator>::capacity_type maxCount(t.capacity());
			ar << (maxCount);

			//const circular_buffer<U>::size_type count(t.size());
			//ar << BOOST_SERIALIZATION_NVP(count);
			typename circular_buffer<U, Allocator>::size_type count(t.size());
			ar << (count);

			for (typename circular_buffer<U, Allocator>::const_iterator i = t.begin(); i != t.end(); i++)
				//for (boost::circular_buffer<U,Allocator>::size_type i = 0; i < t.size(); i++)
			{
				ar << *i;
				//ar << t[i];
			}
		}

	}

	template<class Archive, class U, class Allocator>
	inline void load(Archive &ar, boost::circular_buffer<U, Allocator> &t, const unsigned version)
	{
		if (version == 0)
		{
			//ar >> BOOST_SERIALIZATION_NVP(maxCount);
			typename circular_buffer<U, Allocator>::capacity_type maxCount;
			ar >> (maxCount);
			t.set_capacity(maxCount);

			//circular_buffer<U>::size_type count;
			//ar >> BOOST_SERIALIZATION_NVP(count);
			unsigned count;
			ar >> (count);

			while (count-- > 0)
			{
				U value;
				ar >> value;
				//t.push_front(value);
				t.push_back(value);
			}
		}

	}

}

#endif
