#ifndef DRAMTIMINGSPECIFICATION
#define DRAMTIMINGSPECIFICATION
#pragma once

#include <map>
#include "enumTypes.h"
#include "Settings.h"
#include "globals.h"

namespace DRAMSimII
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

		// friends
		friend std::ostream &operator<<( std::ostream&, const TimingSpecification&);
	};
}
#endif
