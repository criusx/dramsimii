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

#ifndef DIMM_HH
#define DIMM_HH
#pragma once

#include "globals.hh"
#include "Settings.hh"
#include "TimingSpecification.hh"
#include "Rank.hh"

namespace DRAMsimII
{
	class DIMM
	{
	private:

		const TimingSpecification& timing;	///< reference to the timing information, used in calculations	
		const SystemConfiguration& systemConfig; ///< reference to system configuration information
		Statistics& statistics;				///< reference for collecting statistics

	protected:
		Cache cache;							///< the tagstore of the emulated cache
		std::vector<Rank> rank;							///< vector of the array of ranks
		unsigned dimmId;						///< the ordinal number of this DIMM

	public:
		explicit DIMM(const Settings&_settings, const TimingSpecification &_timing, const SystemConfiguration &_systemConfig, Statistics& _stats);
		explicit DIMM(const DIMM &rhs, const TimingSpecification &_timing, const SystemConfiguration &_systemConfig, Statistics &_stats);
		DIMM(const DIMM &);

		// functions
		void setDimmId(const unsigned channelId, const unsigned dimmId);
		void issueRAS(const tick currentTime, const Command *currentCommand);
		void issuePRE(const tick currentTime, const Command *currentCommand);
		void issueCAS(const tick currentTime, const Command *currentCommand);
		void issueCASW(const tick currentTime, const Command *currentCommand);
		void issueREF(const tick currentTime);
		void resetToTime(const tick time);

		tick getLastRasTime(Address &addr) { return (rank.begin() + (addr.getRank() - dimmId * systemConfig.getRankCount()))->getLastRasTime(addr); }

		// accessors
		Rank& getRank(const unsigned rankNum) { return rank[rankNum]; }								///< get a reference to this channel's rank n
		const Rank& getRank(const unsigned rankNum) const { return rank[rankNum]; }					///< get a const reference to this channel's rank n
		std::vector<Rank>& getRank() { return rank; }												///< get a reference to this channel's ranks
		const std::vector<Rank>& getRank() const { return rank; }									///< get a const reference to this channel's ranks


		// overloads
		DIMM& operator=(const DIMM &rhs);
	};
}

#endif