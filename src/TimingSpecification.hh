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

#ifndef DRAMTIMINGSPECIFICATION
#define DRAMTIMINGSPECIFICATION
#pragma once

#include "globals.h"
#include "Settings.h"

#include <map>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

namespace DRAMsimII
{
	// t_pp (min prec to prec of any bank) ignored
	/// @brief contains all the specs for this channel's DIMMs
	class TimingSpecification
	{
	protected:
		
		int t_al;			///< additive latency, used with posted cas
		int t_burst;		///< number of cycles utilized per cacheline burst
		int t_cas;			///< delay between start of CAS and start of burst
		int t_ccd;			///< column-to-column delay, the minimum column command timing, determined by internal burst (prefetch) length.
		int t_cmd;			///< command bus duration...
		int t_cwd;			///< delay between end of CASW and start of burst
		int t_faw;			///< four bank activation
		int t_ras;			///< interval between ACT and PRECHARGE to same bank
		int t_rc;			///< t_rc is simply t_ras + t_rp
		int t_rcd;			///< RAS to CAS delay of same bank
		int t_rfc;			///< refresh cycle time
		int t_rp;			///< interval between PRECHARGE and ACT to same bank
		int t_rrd;			///< row to row activation delay
		int t_rtp;			///< read to precharge delay
		int t_rtrs;			///< rank hand off penalty, also t_dqs
		int t_wr;			///< write recovery time , time to restore data
		int t_wtr;			///< write to read turnaround time
		int t_ost;			///< on-die termination switching time
		int t_int_burst;	///< internal prefetch length of DRAM devices, 4 for DDR2, 8 for DDR3
		int t_buffer_delay;	///< the delay a transaction experiences before it can be converted to a series of commands
		int t_refi;			///< refresh interval, should send one refresh every n ticks to a rank

	public:
		// constructors		
		explicit TimingSpecification(const Settings& settings);

		// accessors
		int tAL() const { return t_al; }
		int tCAS() const { return t_cas; }
		int tCCD() const { return t_ccd; }
		int tBurst() const { return t_burst; }
		int tRTP() const { return t_rtp; }
		int tRAS() const { return t_ras; }
		int tCWD() const { return t_cwd; }
		int tWR() const { return t_wr; }
		int tBufferDelay() const { return t_buffer_delay; }
		int tCMD() const { return t_cmd; }
		int tREFI() const { return t_refi; }
		int tRCD() const { return t_rcd; }
		int tRP() const { return t_rp; }
		int tRRD() const { return t_rrd; }
		int tFAW() const { return t_faw; }
		int tRFC() const { return t_rfc; }
		int tWTR() const { return t_wtr; };
		int tRTRS() const { return t_rtrs; }
		int tRC() const { return t_rc; }
		int tOST() const { return t_ost; }

		// friends
		friend std::ostream &operator<<( std::ostream&, const TimingSpecification&);

		// overloads
		bool operator==(const TimingSpecification& right) const;

	private:
		// serialization
		friend class boost::serialization::access;
		friend void unitTests(const Settings &settings);

		TimingSpecification();

		template<class Archive>
		void serialize( Archive & ar, const unsigned version)
		{
			if (version == 0)
			{
				ar & t_al & t_burst & t_cas & t_ccd & t_cmd & t_cwd &  t_faw & t_ras & t_rc & t_rcd & t_rfc & t_rp & t_rrd & t_rtp &
					t_rtrs & t_wr & t_wtr & t_int_burst & t_buffer_delay & t_refi & t_ost;
			}			
		}
	};
}
#endif
