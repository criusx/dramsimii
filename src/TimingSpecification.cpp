#include <map>
#include <string>
#include <sstream>

#include "TimingSpecification.h"

using std::ostream;
using std::cerr;
using std::endl;
using namespace DRAMSimII;

ostream &DRAMSimII::operator<<(ostream &os, const TimingSpecification &this_a)
{
	os << "rtrs[" << this_a.t_rtrs << "] ";
	return os;
}


TimingSpecification::TimingSpecification(const Settings& settings)
{
	switch(settings.dramType)
	{
	case DDR:

		t_ccd = 2;					// internal fetch is 1 cycle, 2 beats for DDR
		t_al = 0;					// no such thing in DDR 
		t_burst = settings.tBurst;	// depending on system config! can be 2, 4, or 8
		t_cas = settings.tCAS;
		t_cmd = 2;					// protocol specific, cannot be changed
		t_cwd = 2;					// protocol specific, cannot be changed
		t_int_burst = 2;			// protocol specific, cannot be changed
		t_faw = 0;					// no such thing in DDR
		t_ras = settings.tRAS;		// 40 ns @ 2.5 ns per beat == 16 beats
		t_rc = settings.tRC;		// 55 ns t_rc
		t_rcd = settings.tRCD;
		t_rfc = settings.tRFC;		// 70 ns @ 2.5 ns per beat == 28 beats
		t_rp = settings.tRP;		// 15 ns @ 2.5ns per beat = 6 beats
		t_rrd = settings.tRRD;
		t_rtp = settings.tRTP;
		t_rtrs = settings.tRTRS;
		t_wr = settings.tWR;		// 15 ns @ 2.5 ns per beat = 6 beats
		t_wtr = settings.tWTR;
		t_rtrs = settings.tRTRS;
		break;

	case DDR2:

		t_ccd = 4;					// two cycles, 4 beats in DDR2
		t_al = settings.postedCAS ? settings.tAL : 0; // if posted CAS is disabled, tAL should be zero		
		t_rcd = settings.tRCD;
		t_burst = settings.tBurst; // can be 4 or 8
		t_cas = settings.tCAS;
		t_cmd = 2;					// protocol specific, cannot be changed
		t_rtp = settings.tRTP;
		t_rc = settings.tRC;
		t_ras = settings.tRAS;		
		//assert(t_rcd + t_burst + t_rtp - t_ccd == t_rc);
		t_cwd = t_cas - 2;			// protocol specific, cannot be changed
		t_int_burst = 4;			// protocol specific, cannot be changed
		t_faw = settings.tFAW;		
		t_rp = settings.tRP;
		t_rfc = settings.tRFC;
		t_rrd = settings.tRRD;		
		t_rtrs = settings.tRTRS;
		t_wr = settings.tWR;
		t_wtr = settings.tWTR;

		assert(t_rcd + t_burst + t_rtp - t_ccd <= t_ras);
		assert(t_al <= t_rcd);
		assert(t_al >= 0 && t_al <= 8); // must be 0..4 cycles, or 0..8 beats
		assert(t_al + t_cmd == t_rcd);
		break;

	case DDR3:

		t_ccd = 8;					// four cycles, eight beats in DDR3
		t_al = settings.postedCAS ? settings.tAL : 0; // if posted CAS is disabled, tAL should be zero
		t_burst = 8;				// protocol specific, cannot be changed 
		t_cas = settings.tCAS;
		t_cmd = 2;					// protocol specific, cannot be changed
		t_cwd = t_cas - 2;			// fixed
		t_int_burst = 8;			// protocol specific, cannot be changed
		t_faw = settings.tFAW;
		t_ras = settings.tRAS;		// 27 ns @ 0.75ns per beat = 36 beats
		t_rc = settings.tRC;		// 36 ns @ 0.75ns per beat = 48 beats
		t_rcd = settings.tRCD;
		t_rfc = settings.tRFC;
		t_rp = settings.tRP;		// 9 ns @ 0.75ns per beat = 12 beats
		t_rrd = settings.tRRD;
		t_rtrs = settings.tRTRS;
		t_rtp = settings.tRTP;
		t_wr = settings.tWR;
		t_wtr = settings.tWTR;
		break;

	case SDRAM:

		t_ccd = 1;					// one cycle, one beat in SDR
		t_al = 0;					// no such thing as posted CAS in SDRAM 
		t_burst = settings.tBurst;	// depending on system config, can be 1, 2, 4, or 8 
		t_cas = settings.tCAS;
		t_cmd = 1;					// protocol specific, cannot be changed 
		t_cwd = 0;					//no such thing in SDRAM 
		t_int_burst = 1;			// prefetch length is 1 
		t_faw = 0;					// no such thing in SDRAM 
		t_ras = settings.tRAS;		 
		t_rc = settings.tRC;		
		t_rcd = settings.tRCD;
		t_rfc = settings.tRC;		// same as t_rc 
		t_rp = settings.tRP;		// 12 ns @ 1.25ns per cycle = 9.6 cycles 
		t_rrd = 0;					// no such thing in SDRAM 
		t_rtp = settings.tRTP;
		t_rtrs = settings.tRTRS;	// no such thing in SDRAM 
		t_wr = settings.tWR;		
		break;

	case DRDRAM:
		cerr << "DRDRAM not yet supported." << endl;
		exit(-12);
		break;

	default:
		break;
	}

	// set the minimum length of time a command must take to be decoded
	t_buffer_delay = settings.tBufferDelay;
	// set the refresh interval, issue a refresh command every tREFI
	t_refi = settings.tREFI;
}

// no arg constructor for deserialization and unit testing
TimingSpecification::TimingSpecification():
t_al(-1),
t_burst(-1),
t_cas(-1),
t_ccd(-1),
t_cmd(-1),
t_cwd(-1),
t_faw(-1),
t_ras(-1),
t_rc(-1),
t_rcd(-1),
t_rfc(-1),
t_rp(-1),
t_rrd(-1),
t_rtp(-1),
t_rtrs(-1),
t_wr(-1),
t_wtr(-1),
t_int_burst(-1),
t_buffer_delay(-1),
t_refi(-1)
{}


bool TimingSpecification::operator==(const TimingSpecification &right) const 
{
	return (t_al == right.t_al && t_burst == right.t_burst && t_cas == right.t_cas && t_ccd == right.t_ccd && t_cmd == right.t_cmd &&
		t_cwd == right.t_cwd && t_faw == right.t_faw && t_ras == right.t_ras && t_rc == right.t_rc && t_rcd == right.t_rcd && 
		t_rfc == right.t_rfc && t_rp == right.t_rp && t_rrd == right.t_rrd && t_rtp == right.t_rtp && t_rtrs == right.t_rtrs && 
		t_wr == right.t_wr && t_wtr == right.t_wtr && t_int_burst == right.t_int_burst && t_buffer_delay == right.t_buffer_delay &&
		t_refi == right.t_refi);

}
