#include <map>
#include <string>
#include <sstream>

#include "dramTimingSpecification.h"

using namespace std;
using namespace DRAMSimII;

ostream &DRAMSimII::operator<<(ostream &os, const dramTimingSpecification &this_a)
{
	os << "rtrs[" << this_a.t_rtrs << "] ";
	return os;
}


dramTimingSpecification::dramTimingSpecification(const dramSettings *settings)
{
	switch(settings->dramType)
	{
	case DDR:
		// @ 200 MHz (400 Mbps)
		t_al = 0;		// no such thing in DDR 
		t_burst = 8;	// depending on system config! can be 2, 4, or 8
		t_cas = 6;
		t_cmd = 2;		// protocol specific, cannot be changed
		t_cwd = 2;		// protocol specific, cannot be changed
		t_int_burst = 2;// protocol specific, cannot be changed
		t_faw = 0;		// no such thing in DDR
		t_ras = 16;		// 40 ns @ 2.5 ns per beat == 16 beats
		t_rc = 22;		// 55 ns t_rc
		t_rcd = 6;
		t_rfc = 28;		// 70 ns @ 2.5 ns per beat == 28 beats
		t_rp = 6;		// 15 ns @ 2.5ns per beat = 6 beats
		t_rrd = 0;
		t_rtp = 2;
		t_rtrs = 2;
		t_wr = 6;		// 15 ns @ 2.5 ns per beat = 6 beats
		t_wtr = 4;
		t_rtrs = settings->tRTRS;
		break;
	case DDR2:
		t_al = settings->tAL;
		t_burst = settings->tBurst;
		t_cas = settings->tCAS;
		t_cmd = 2;	// protocol specific, cannot be changed
		t_cwd = t_cas - 2;	// protocol specific, cannot be changed
		t_int_burst = 4;	// protocol specific, cannot be changed
		t_faw = settings->tFAW;
		t_ras = settings->tRAS;
		t_rp = settings->tRP;
		t_rc = settings->tRC;
		t_rcd = settings->tRCD;
		t_rfc = settings->tRFC;
		t_rrd = settings->tRRD;
		t_rtp = settings->tRTP;
		t_rtrs = settings->tRTRS;
		t_wr = settings->tWR;
		t_wtr = settings->tWTR;
		break;
	case DDR3:
		// defaults to @ 1.33 Gbps = 0.75 ns per beat
		t_al = 0;
		t_burst = 8;	// protocol specific, cannot be changed 
		t_cas = 10;
		t_cmd = 2;		// protocol specific, cannot be changed
		t_cwd = t_cas - 2;
		t_int_burst = 8;	// protocol specific, cannot be changed
		t_faw = 30;
		t_ras = 36;	// 27 ns @ 0.75ns per beat = 36 beats
		t_rc = 48;	// 36 ns @ 0.75ns per beat = 48 beats
		t_rcd = 12;
		t_rfc = 280;
		t_rp = 12;	// 9 ns @ 0.75ns per beat = 12 beats
		t_rrd = 8;
		t_rtrs = 2;
		t_rtp = 8;
		t_wr = 12;
		t_wtr = 8;
		break;
	case SDRAM:
		// @ 100 MHz 
		t_al = 0;		// no such thing as posted CAS in SDRAM 
		t_burst = 8;	// depending on system config! can be 1, 2, 4, or 8 
		t_cas = 2;
		t_cmd = 1;		// protocol specific, cannot be changed 
		t_cwd = 0;		//no such thing in SDRAM 
		t_int_burst = 1;// prefetch length is 1 
		t_faw = 0;		// no such thing in SDRAM 
		t_ras = 5;		 
		t_rc = 7;		
		t_rcd = 2;
		t_rfc = 7;		// same as t_rc 
		t_rp = 2;		// 12 ns @ 1.25ns per cycle = 9.6 cycles 
		t_rrd = 0;		// no such thing in SDRAM 
		t_rtp = 1;
		t_rtrs = 0;		// no such thing in SDRAM 
		t_wr = 2;		
		break;
	case DRDRAM:
		cerr << "DRDRAM not yet supported." << endl;
		exit(-12);
		break;
	default:
		break;
	}
	t_rtrs = settings->tRTRS;
	t_buffer_delay = settings->tBufferDelay;
}

dramTimingSpecification::dramTimingSpecification(map<file_io_token_t,string> &parameter)
{
	map<file_io_token_t,string>::iterator temp;
	if ((temp=parameter.find(dram_type_token))==parameter.end())
		exit(9);

	if(temp->second == "sdram")
	{					// @ 100 MHz 
		t_al = 0;		// no such thing as posted CAS in SDRAM 
		t_burst = 8;	// depending on system config! can be 1, 2, 4, or 8 
		t_cas = 2;
		t_cmd = 1;		// protocol specific, cannot be changed 
		t_cwd = 0;		//no such thing in SDRAM 
		t_int_burst = 1;// prefetch length is 1 
		t_faw = 0;		// no such thing in SDRAM 
		t_ras = 5;		 
		t_rc = 7;		
		t_rcd = 2;
		t_rfc = 7;		// same as t_rc 
		t_rp = 2;		// 12 ns @ 1.25ns per cycle = 9.6 cycles 
		t_rrd = 0;		// no such thing in SDRAM 
		t_rtp = 1;
		t_rtrs = 0;		// no such thing in SDRAM 
		t_wr = 2;		

	} 
	// @ 200 MHz (400 Mbps)
	else if (temp->second == "ddr" || temp->second == "ddrsdram")					
	{
		t_al = 0;		// no such thing in DDR 
		t_burst = 8;	// depending on system config! can be 2, 4, or 8
		t_cas = 6;
		t_cmd = 2;		// protocol specific, cannot be changed
		t_cwd = 2;		// protocol specific, cannot be changed
		t_int_burst = 2;// protocol specific, cannot be changed
		t_faw = 0;		// no such thing in DDR
		t_ras = 16;		// 40 ns @ 2.5 ns per beat == 16 beats
		t_rc = 22;		// 55 ns t_rc
		t_rcd = 6;
		t_rfc = 28;		// 70 ns @ 2.5 ns per beat == 28 beats
		t_rp = 6;		// 15 ns @ 2.5ns per beat = 6 beats
		t_rrd = 0;
		t_rtp = 2;
		t_rtrs = 2;
		t_wr = 6;		// 15 ns @ 2.5 ns per beat = 6 beats
		t_wtr = 4;

	}
	else if (temp->second == "ddr2")					/* @ 800 Mbps */
	{
		if ((temp=parameter.find(t_al_token))==parameter.end())
			t_al = 0;
		else
		{
			toNumeric<int>(t_al,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_burst_token))==parameter.end())
			t_burst = 8;/* depending on system config! can be 4, or 8 */
		else
		{
			toNumeric<int>(t_burst,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_cas_token))==parameter.end())
			t_cas = 10;
		else
		{
			toNumeric<int>(t_cas,temp->second,std::dec);
		}

		t_cmd = 2;	// protocol specific, cannot be changed

		t_cwd = t_cas - 2;	// protocol specific, cannot be changed

		t_int_burst = 4;	// protocol specific, cannot be changed

		if ((temp=parameter.find(t_faw_token))==parameter.end())
		{
			t_faw = 30;
		}
		else
		{
			toNumeric<int>(t_faw,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_ras_token))==parameter.end())
		{
			t_ras = 36;	// 45 ns @ 1.25ns per beat = 36 beats
		}
		else
		{
			toNumeric<int>(t_ras,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_rp_token))==parameter.end())
		{
			t_rp = 10;	// 12 ns @ 1.25ns per beat = 9.6 beats
		}
		else
		{
			toNumeric<int>(t_rp,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_rc_token))==parameter.end())
		{
			t_rc = t_ras + t_rp; // if it is not defined as anything else, set it to this defined value
		}
		else
		{
			toNumeric<int>(t_rc,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_rcd_token))==parameter.end())
		{
			t_rcd = 10;
		}
		else
		{
			toNumeric<int>(t_rcd,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_rfc_token))==parameter.end())
		{
			t_rfc = 102;	// 128 ns @ 1.25ns per beat ~= 102 beats
		}
		else
		{
			toNumeric<int>(t_rfc,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_rrd_token))==parameter.end())
		{
			t_rrd = 6;	// 7.5 ns
		}
		else
		{
			toNumeric<int>(t_rrd,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_rtp_token))==parameter.end())
		{
			t_rtp = 6;
		}
		else
		{
			toNumeric<int>(t_rtp,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_rtrs_token))==parameter.end())
		{
			t_rtrs = 2;
		}
		else
		{
			toNumeric<int>(t_rtrs,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_wr_token))==parameter.end())
		{
			t_wr = 12;
		}
		else
		{
			toNumeric<int>(t_wr,temp->second,std::dec);
		}

		if ((temp=parameter.find(t_wtr_token))==parameter.end())
		{
			t_wtr = 6;
		}
		else
		{
			toNumeric<int>(t_wtr,temp->second,std::dec);
		}
	}
	else if (temp->second == "ddr3")
	{
		// defaults to @ 1.33 Gbps = 0.75 ns per beat
		t_al = 0;
		t_burst = 8;	// protocol specific, cannot be changed 
		t_cas = 10;
		t_cmd = 2;		// protocol specific, cannot be changed
		t_cwd = t_cas - 2;
		t_int_burst = 8;	// protocol specific, cannot be changed
		t_faw = 30;
		t_ras = 36;	// 27 ns @ 0.75ns per beat = 36 beats
		t_rc = 48;	// 36 ns @ 0.75ns per beat = 48 beats
		t_rcd = 12;
		t_rfc = 280;
		t_rp = 12;	// 9 ns @ 0.75ns per beat = 12 beats
		t_rrd = 8;
		t_rtrs = 2;
		t_rtp = 8;
		t_wr = 12;
		t_wtr = 8;

	}
	else if (temp->second == "drdram")
	{
		;// FIXME: DRDRAM not currently supported
	}

	if ((temp=parameter.find(t_rtrs_token))!=parameter.end())
	{
		toNumeric<int>(t_rtrs,temp->second,std::dec);
	}

	// set the delay for the transaction to command buffer delay
	if ((temp=parameter.find(t_buffer_delay_token)) != parameter.end())
	{
		toNumeric<int>(t_buffer_delay,temp->second,std::dec);
	}
	else
	{
		t_buffer_delay = 2; // just a default value
	}
}
