#include <iostream>

#include "dramChannel.h"

using namespace std;
using namespace DRAMSimII;

/// <summary>
/// find the protocol gap between a command and current system state
/// </summary>
int dramChannel::minProtocolGap(const command *this_c) const
{ 
	int min_gap;

	const unsigned this_rank = this_c->getAddress().rank;
	
	const rank_c &currentRank = rank[this_rank];
	
	const bank_c &currentBank = currentRank.bank[this_c->getAddress().bank];
	
	int t_al = this_c->isPostedCAS() ? timingSpecification.tAL() : 0;

	switch(this_c->getCommandType())
	{
	case RAS_COMMAND:
		{
			// respect t_rp of same bank
			int tRPGap = (int)(currentBank.lastPrechargeTime - time) + timingSpecification.tRP();

			int ras_q_count = currentRank.lastRASTimes.size();
			
			// respect tRRD and tRC of all other banks of same rank
			int tRRDGap;

			if (ras_q_count == 0)
			{
				tRRDGap = 0;
			}
			else 
			{
				// read tail end of ras history
				tick_t *last_ras_time = currentRank.lastRASTimes.read(ras_q_count - 1); 
				// respect the row-to-row activation delay
				tRRDGap = (int)(*last_ras_time - time) + timingSpecification.tRRD();				
			}

			// respect the row cycle time limitation
			int tRCGap = (int)(currentBank.lastRASTime - time) + timingSpecification.tRC();

			// respect the t_faw value for DDR2 and beyond
			int tFAWGap;

			if (ras_q_count < 4)
			{
				tFAWGap = 0;
			}
			else
			{
				// read head of ras history
				tick_t *fourth_ras_time = currentRank.lastRASTimes.front(); 
				tFAWGap = (int)(*fourth_ras_time - time) + timingSpecification.tFAW();
			}

			// respect tRFC
			int tRFCGap = (int)(currentRank.lastRefreshTime - time) + timingSpecification.tRFC();
			
			min_gap = max(max(max(tRFCGap,tRCGap) , tRPGap) , max(tRRDGap , tFAWGap));
		}
		break;

	case CAS_AND_PRECHARGE_COMMAND:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case CAS_COMMAND:
		{
			//respect last ras of same rank
			int t_ras_gap = (int)((currentBank.lastRASTime - time) + timingSpecification.tRCD() - t_al);

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			tick_t otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick_t otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent cas(w) time and length
			for (vector<rank_c>::const_iterator thisRank = rank.begin(); thisRank != rank.end(); thisRank++)
			{
				if (thisRank->getRankID() != currentRank.getRankID())
				{
					if (thisRank->lastCASTime > otherRankLastCASTime)
					{
						otherRankLastCASTime = thisRank->lastCASTime;
						otherRankLastCASLength = thisRank->lastCASLength;
					}
					if (thisRank->lastCASWTime > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = thisRank->lastCASWTime;
						otherRankLastCASWLength = thisRank->lastCASWLength;
					}
				}
			}			

			//respect last cas of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			int t_cas_gap = (int)((currentRank.lastCASTime - time) + timingSpecification.tBurst());
			
			//respect last cas write of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			t_cas_gap = max(t_cas_gap,(int)((currentRank.lastCASWTime - time) + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR()));

			if (rank.size() > 1) 
			{
				//respect most recent cas of different rank
				t_cas_gap = max(t_cas_gap,(int)(otherRankLastCASTime + otherRankLastCASLength + timingSpecification.tRTRS() - time));
				//respect timing of READ follow WRITE, different ranks.*/
				t_cas_gap = max(t_cas_gap,(int)(otherRankLastCASWTime + timingSpecification.tCWD() + otherRankLastCASWLength + timingSpecification.tRTRS() - timingSpecification.tCAS() - time));
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
			int t_ras_gap = (int)((currentBank.lastRASTime - time) + timingSpecification.tRCD() - t_al);

			tick_t otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick_t otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent CAS/CASW time and length
			// FIXME: change to use iterators
			for (unsigned rank_id = 0; rank_id < rank.size() ; rank_id++)
			{
				if (rank_id != this_rank)
				{
					if (rank[rank_id].lastCASTime > otherRankLastCASTime)
					{
						otherRankLastCASTime = rank[rank_id].lastCASTime;
						otherRankLastCASLength = rank[rank_id].lastCASLength;
					}
					if (rank[rank_id].lastCASWTime > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = rank[rank_id].lastCASWTime;
						otherRankLastCASWLength = rank[rank_id].lastCASWLength;
					}
				}
			}
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			
			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			int t_cas_gap = max(0,(int)(currentRank.lastCASTime + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD() - time));
			
			// respect last cas to different ranks
			t_cas_gap = max(t_cas_gap,(int)(otherRankLastCASTime + timingSpecification.tCAS() + otherRankLastCASLength + timingSpecification.tRTRS() - timingSpecification.tCWD() - time));
			
			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			t_cas_gap = max(t_cas_gap,(int)(currentRank.lastCASWTime + timingSpecification.tBurst() - time));
			
			// respect last cas write to different ranks
			t_cas_gap = max(t_cas_gap,(int)(otherRankLastCASWTime + otherRankLastCASWLength - time));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case RETIRE_COMMAND:
		break;

	case PRECHARGE_COMMAND:
		{
			// respect t_ras of same bank
			int t_ras_gap = (int)(currentBank.lastRASTime - time) + timingSpecification.tRAS();

			// respect t_cas of same bank
			int t_cas_gap = max(0,(int)((currentBank.lastCASTime - time) + t_al + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD())));

			// respect t_casw of same bank
			t_cas_gap = max(t_cas_gap,(int)((currentBank.lastCASWTime - time) + t_al + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR()));

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
		// respect tRFC and tRP
		min_gap = max((int)((currentRank.lastRefreshTime - time) + timingSpecification.tRFC()),(int)((currentRank.lastPrechargeTime - time) + timingSpecification.tRP()));
		break;
	}

#ifdef DEBUG_MIN_PROTOCOL_GAP
	outStream << this_c->commandType;
	outStream << " ras[" << setw(2) << t_ras_gap << "] rrd[" << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]" << endl;
#endif

	return max(min_gap,timingSpecification.tCMD());
}
