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

#ifndef EVENT_H
#define EVENT_H
#pragma once

#include "Address.hh"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

namespace DRAMsimII
{
	/// @brief pending event queue
	class Event
	{
	protected:
		tick arrivalTime;						///< the time that this event was created
		tick enqueueTime;						///< the time that this event was put into a queue
		tick startTime;							///< when this actually started
		tick completionTime;					///< when this finished

		const Address address;					///< the address that this event involves

		// constructors
		Event():
		arrivalTime(0),
			enqueueTime(0),
			startTime(0),
			completionTime(0),
			address(0x0)
		{}

		Event(const Address &add, const tick enqTime):
		arrivalTime(0),
			enqueueTime(enqTime),
			startTime(0),
			completionTime(0),
			address(add)
		{}

		Event(const tick arrTime,const Address &add):
		arrivalTime(arrTime),
			enqueueTime(0),
			startTime(0),
			completionTime(0),
			address(add)
		{}

		Event(const tick arrTime,const PhysicalAddress add):
		arrivalTime(arrTime),
			enqueueTime(0),
			startTime(0),
			completionTime(0),
			address(add)
		{}

		Event(const Event &rhs):
		arrivalTime(rhs.arrivalTime),
			enqueueTime(rhs.enqueueTime),
			startTime(rhs.startTime),
			completionTime(rhs.completionTime),
			address(rhs.address)
		{}

	public:
		// destructors
		virtual ~Event() {}

		// accessors
		tick getArrivalTime() const { return arrivalTime; }
		tick getEnqueueTime() const { return enqueueTime; }
		tick getStartTime() const { return startTime; }
		tick getCompletionTime() const { return completionTime; }
		const Address &getAddress() const { return address; }

		// functions
		tick getExecuteTime() const { return completionTime - startTime; }
		tick getDelayTime() const { return startTime - enqueueTime; }
		tick getLatency() const { return completionTime - enqueueTime; }

		// mutators
		void setArrivalTime(const tick at) { arrivalTime = at; }
		void setEnqueueTime(const tick et) { enqueueTime = et; }
		void setStartTime(const tick st) { startTime = st; }
		void setCompletionTime(const tick ct) { completionTime = ct; }

		// friends
		friend std::ostream &DRAMsimII::operator<<(std::ostream &, const Event &);		

		// overloads
		bool operator==(const Event& right) const
		{
			return (arrivalTime == right.arrivalTime && enqueueTime == right.enqueueTime && startTime == right.startTime && completionTime == right.completionTime && address == right.getAddress());
		}
	private:
		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive & ar, const unsigned version)
		{
			if (version == 0)
			{
ar & startTime & enqueueTime & completionTime & arrivalTime & const_cast<Address&>(address);
			}
			
		}


	};
}
#endif
