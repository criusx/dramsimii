#ifndef DRAMTIMINGSPECIFICATION
#define DRAMTIMINGSPECIFICATION
#pragma once

#include <map>
#include "enumTypes.h"
#include "dramSettings.h"
#include "globals.h"

namespace DRAMSimII
{
	// t_pp (min prec to prec of any bank) ignored
	class dramTimingSpecification
	{
	private:
		int t_rtrs;			// rank hand off penalty, also t_dqs
		int t_al;			// additive latency, used with posted cas
		int t_burst;		// number of cycles utilized per cacheline burst
		int t_cas;			// delay between start of CAS and start of burst
		int t_cwd;			// delay between end of CASW and start of burst
		int t_faw;			// four bank activation
		int t_ras;			// interval between ACT and PRECHARGE to same bank
		int t_rc;			// t_rc is simply t_ras + t_rp
		int t_rcd;			// RAS to CAS delay of same bank
		int t_rfc;			// refresh cycle time
		int t_rp;			// interval between PRECHARGE and ACT to same bank
		int t_rrd;			// row to row activation delay
		int t_rtp;			// read to precharge delay
		int t_wr;			// write recovery time , time to restore data
		int t_wtr;			// write to read turnaround time
		int t_cmd;			// command bus duration...
		int t_int_burst;	// internal prefetch length of DRAM devices, 4 for DDR2, 8 for DDR3
		int t_buffer_delay;	// the delay a transaction experiences before it can be converted to a series of commands
		int t_refi;			// refresh interval, should send one refresh every n ticks to a rank

	public:
		// constructors		
		explicit dramTimingSpecification(const dramSettings *settings);

		// friends
		friend std::ostream &operator<<( std::ostream&, const dramTimingSpecification &);
		friend class dramChannel; // TODO: change how this works, no need for friends when there's accessors
		friend class dramSystem;
	};
}
#endif
