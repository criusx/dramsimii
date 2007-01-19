#include <iostream>

#include "dramSystem.h"

using namespace std;

/// <summary>
/// find the protocol gap between a command and current system state
/// </summary>
int dramSystem::minProtocolGap(const unsigned channel_id,const command *this_c) const
{ 
	//FIXME: some max() functions take uninit values
	int min_gap;

	const dramChannel &channel = dramSystem::channel[channel_id];

	const unsigned this_rank = this_c->addr.rank_id;
	const unsigned this_bank = this_c->addr.bank_id;
	rank_c &this_r = channel.get_rank(this_rank);
	bank_c &this_b = this_r.bank[this_bank];
	tick_t now = channel.get_time();
	int t_ras_gap = 0;
	tick_t other_r_last_cas_time;
	tick_t other_r_last_casw_time;
	int other_r_last_cas_length;
	int other_r_last_casw_length;

	int t_al = this_c->posted_cas ? timing_specification.t_al : 0;

	switch(this_c->this_command)
	{
	case RAS_COMMAND:
		{
			// respect t_rp of same bank
			int t_rp_gap = (int)(this_b.last_prec_time - now) + timing_specification.t_rp;

			int ras_q_count = this_r.last_ras_times.get_count();
			
			// respect tRRD and tRC of all other banks of same rank
			int t_rrd_gap;

			if (ras_q_count == 0)
			{
				t_rrd_gap = 0;
			}
			else 
			{
				// read tail end of ras history
				tick_t *last_ras_time = this_r.last_ras_times.read(ras_q_count - 1); 
				// respect the row-to-row activation delay
				t_rrd_gap = (int)(*last_ras_time - now) + timing_specification.t_rrd;				
			}

			// respect the row cycle time limitation
			int tRcGap = (int)(this_b.last_ras_time - now) + timing_specification.t_rc;

			// respect the t_faw value for DDR2 and beyond
			int t_faw_gap;

			if (ras_q_count < 4)
			{
				t_faw_gap = 0;
			}
			else
			{
				// read head of ras history
				tick_t *fourth_ras_time = this_r.last_ras_times.read(0); 
				t_faw_gap = (int)(*fourth_ras_time - now) + timing_specification.t_faw;
			}
			
			min_gap = max(max(tRcGap , t_rp_gap) , max(t_rrd_gap , t_faw_gap));
		}
		break;

	case CAS_AND_PRECHARGE_COMMAND:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met, so no need to check tRAS timing requirement here.

	case CAS_COMMAND:
		{
			//respect last ras of same rank
			t_ras_gap = max(0,(int)((this_b.last_ras_time - now) + timing_specification.t_rcd - t_al));

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			other_r_last_cas_time = now - 1000;
			other_r_last_cas_length = timing_specification.t_burst;
			other_r_last_casw_time = now - 1000;
			other_r_last_casw_length = timing_specification.t_burst;

			// find the most recent cas(w) time and length
			for (unsigned rank_id = 0; rank_id < system_config.rank_count ; rank_id++)
			{
				if (rank_id != this_rank)
				{
					if (channel.get_rank(rank_id).last_cas_time > other_r_last_cas_time)
					{
						other_r_last_cas_time = channel.get_rank(rank_id).last_cas_time;
						other_r_last_cas_length = channel.get_rank(rank_id).last_cas_length;
					}
					if (channel.get_rank(rank_id).last_casw_time > other_r_last_casw_time)
					{
						other_r_last_casw_time = channel.get_rank(rank_id).last_casw_time;
						other_r_last_casw_length = channel.get_rank(rank_id).last_casw_length;
					}
				}
			}

			//respect last cas of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			int t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_burst - now));
			
			//respect last cas write of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wtr - now));

			if (system_config.rank_count > 1) 
			{
				//respect most recent cas of different rank
				t_cas_gap = max(t_cas_gap,(int)(other_r_last_cas_time + other_r_last_cas_length + timing_specification.t_rtrs - now));
				//respect timing of READ follow WRITE, different ranks.*/
				t_cas_gap = max(t_cas_gap,(int)(other_r_last_casw_time + timing_specification.t_cwd + other_r_last_casw_length + timing_specification.t_rtrs - timing_specification.t_cas - now));
			}
			min_gap = max(t_ras_gap,t_cas_gap);
			
			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will met tRAS timing or not. So CAS_WRITE_AND_PRECHARGE_COMMAND
		// has the exact same timing requirements as a simple CAS COMMAND.

	case CAS_WRITE_COMMAND:
		{
			//respect last ras of same rank
			t_ras_gap = max(0,(int)(this_b.last_ras_time + timing_specification.t_rcd - t_al - now));

			other_r_last_cas_time = now - 1000;
			other_r_last_cas_length = timing_specification.t_burst;
			other_r_last_casw_time = now - 1000;
			other_r_last_casw_length= timing_specification.t_burst;

			for (unsigned rank_id = 0 ; rank_id < system_config.rank_count ; rank_id++)
			{
				if (rank_id != this_rank)
				{
					if (channel.get_rank(rank_id).last_cas_time > other_r_last_cas_time)
					{
						other_r_last_cas_time = channel.get_rank(rank_id).last_cas_time;
						other_r_last_cas_length = channel.get_rank(rank_id).last_cas_length;
					}
					if ( channel.get_rank(rank_id).last_casw_time > other_r_last_casw_time)
					{
						other_r_last_casw_time = channel.get_rank(rank_id).last_casw_time;
						other_r_last_casw_length = channel.get_rank(rank_id).last_casw_length;
					}
				}
			}
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			
			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			int t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + timing_specification.t_burst + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			
			// respect last cas to different ranks
			t_cas_gap = max(t_cas_gap,(int)(other_r_last_cas_time + timing_specification.t_cas + other_r_last_cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			
			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_burst - now));
			
			// respect last cas write to different ranks
			t_cas_gap = max(t_cas_gap,(int)(other_r_last_casw_time + other_r_last_casw_length - now));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case RETIRE_COMMAND:
		break;

	case PRECHARGE_COMMAND:
		{
			// respect t_ras of same bank
			t_ras_gap = (int)(this_b.last_ras_time - now) + timing_specification.t_ras;

			// respect t_cas of same bank
			int t_cas_gap = max(0,(int)(this_b.last_cas_time + t_al + timing_specification.t_cas + timing_specification.t_burst + max(0,timing_specification.t_rtp - timing_specification.t_cmd)- now));

			// respect t_casw of same bank
			t_cas_gap = max(t_cas_gap,(int)(this_b.last_casw_time + t_al + timing_specification.t_cwd + timing_specification.t_burst + timing_specification.t_wr - now));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case PRECHARGE_ALL_COMMAND:
		break;

	case RAS_ALL_COMMAND:
		break;

	case DRIVE_COMMAND:
		break;

	case DATA_COMMAND:
		break;

	case CAS_WITH_DRIVE_COMMAND:
		break;

	case REFRESH_ALL_COMMAND:
		break;
	}

#ifdef DEBUG_MIN_PROTOCOL_GAP
	cerr << this_c->this_command;
	cerr << " ras[" << setw(2) << t_ras_gap << "] rrd[" << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]" << endl;
#endif

	return max(min_gap,timing_specification.t_cmd);
}