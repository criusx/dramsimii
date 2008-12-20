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

#include <iostream>

#include "Channel.h"

using std::vector;
using std::max;
using namespace DRAMsimII;

/// <summary>
/// find the protocol gap between a command and current system state
/// </summary>
tick Channel::minProtocolGap(const Command *currentCommand) const
{ 
	tick min_gap = 0;

	const unsigned currentRankID = currentCommand->getAddress().getRank();

	const Rank &currentRank = rank[currentRankID];

	const Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	switch(currentCommand->getCommandType())
	{
	case ACTIVATE:
		{
			// refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk

			// respect the row cycle time limitation
			int tRCGap = (int)(currentBank.getLastRASTime() - time) + timingSpecification.tRC();

			// respect tRRD and tRC of all other banks of same rank
			int tRRDGap;
		
			if (currentRank.lastActivateTimes.empty())
			{
				tRRDGap = 0;
			}
			else 
			{
				// read tail end of ras history
				const tick lastRASTime = currentRank.lastActivateTimes.front();
				// respect the row-to-row activation delay
				tRRDGap = (int)(lastRASTime - time) + timingSpecification.tRRD();				
			}

			// respect tRP of same bank
			int tRPGap = (int)(currentBank.getLastPrechargeTime() - time) + timingSpecification.tRP();

			// respect the t_faw value for DDR2 and beyond
			int tFAWGap;

			if (!currentRank.lastActivateTimes.full())
			{
				tFAWGap = 0;
			}
			else
			{
				// read head of ras history
				const tick fourthRASTime = currentRank.lastActivateTimes.back(); 
				tFAWGap = (fourthRASTime - time) + timingSpecification.tFAW();
			}

			// respect tRFC
			int tRFCGap = (currentRank.getLastRefreshTime() - time) + timingSpecification.tRFC();

			min_gap = max(max(max(tRFCGap,tRCGap) , tRPGap) , max(tRRDGap , tFAWGap));
		}
		break;

	case READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case READ:
		{
			//respect last ras of same rank
			int tRCDGap = ((currentBank.getLastRASTime() - time) + timingSpecification.tRCD() - timingSpecification.tAL());

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent cas(w) time and length
			for (vector<Rank>::const_iterator thisRank = rank.begin(); thisRank != rank.end(); thisRank++)
			{
				if (thisRank->getRankID() != currentRank.getRankID())
				{
					if (thisRank->getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = thisRank->getLastCASTime();
						otherRankLastCASLength = thisRank->getLastCASLength();
					}
					if (thisRank->getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = thisRank->getLastCASWTime();
						otherRankLastCASWLength = thisRank->getLastCASWLength();
					}
				}
			}			

			//respect last cas of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			int t_cas_gap = ((currentRank.getLastCASTime() - time) + timingSpecification.tBurst());

			//respect last cas write of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			t_cas_gap = max((tick)t_cas_gap,((currentRank.getLastCASWTime() - time) + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR()));

			if (rank.size() > 1) 
			{
				//respect most recent cas of different rank
				t_cas_gap = max((tick)t_cas_gap, (otherRankLastCASTime - time) + otherRankLastCASLength + timingSpecification.tRTRS());
				//respect timing of READ follow WRITE, different ranks
				t_cas_gap = max((tick)t_cas_gap, (otherRankLastCASWTime - time) + timingSpecification.tCWD() + otherRankLastCASWLength + timingSpecification.tRTRS() - timingSpecification.tCAS());
			}
			min_gap = max(tRCDGap,t_cas_gap);

			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will met tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple CAS COMMAND.

	case WRITE:
		{
			//respect last ras of same rank
			int t_ras_gap = ((currentBank.getLastRASTime() - time) + timingSpecification.tRCD() - timingSpecification.tAL());

			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent CAS/CASW time and length
			// FIXME: change to use iterators
			for (unsigned rank_id = 0; rank_id < rank.size() ; rank_id++)
			{
				if (rank_id != currentRankID)
				{
					if (rank[rank_id].getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = rank[rank_id].getLastCASTime();
						otherRankLastCASLength = rank[rank_id].getLastCASLength();
					}
					if (rank[rank_id].getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = rank[rank_id].getLastCASWTime();
						otherRankLastCASWLength = rank[rank_id].getLastCASWLength();
					}
				}
			}
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);

			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			int t_cas_gap = max(0,(int)(currentRank.getLastCASTime() - time) + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas to different ranks
			t_cas_gap = max((tick)t_cas_gap,(otherRankLastCASTime - time) + timingSpecification.tCAS() + otherRankLastCASLength + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			t_cas_gap = max((tick)t_cas_gap,(currentRank.getLastCASWTime() - time) + currentRank.getLastCASWLength());

			// respect last cas write to different ranks
			t_cas_gap = max((tick)t_cas_gap,(otherRankLastCASWTime - time) + otherRankLastCASWLength + timingSpecification.tOST());

			min_gap = max((tick)t_ras_gap,(tick)t_cas_gap);
		}
		break;

	case RETIRE_COMMAND:
		break;

	case PRECHARGE:
		{
			// respect t_ras of same bank
			int t_ras_gap = (currentBank.getLastRASTime() - time) + timingSpecification.tRAS();

			// respect t_cas of same bank
			int t_cas_gap = max(0,((int)(currentBank.getLastCASTime() - time) + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD())));

			// respect t_casw of same bank
			t_cas_gap = max((tick)t_cas_gap,((currentBank.getLastCASWTime() - time) + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR()));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case PRECHARGE_ALL:
		break;

	case ACTIVATE_ALL:
		break;

	case DRIVE_COMMAND:
		break;

	case DATA_COMMAND:
		break;

	case CAS_WITH_DRIVE_COMMAND:
		break;

	case REFRESH_ALL:
		// respect tRFC and tRP
		min_gap = max((currentRank.getLastRefreshTime() + timingSpecification.tRFC()),(currentRank.getLastPrechargeTime() + timingSpecification.tRP())) - time;
		break;

	case INVALID_COMMAND:
		break;

	default:
		break;
	}

	//return max(min_gap,timingSpecification.tCMD());
	return max(min_gap,max(lastCommandIssueTime - time + (tick)timingSpecification.tCMD(),(tick)0));
}

/// @brief Returns the soonest time that this command may execute
/// @details Looks at all of the timing parameters and decides when the this command may soonest execute
tick Channel::earliestExecuteTime(const Command *currentCommand) const
{ 
	tick nextTime;

	const unsigned thisRank = currentCommand->getAddress().getRank();

	const Rank &currentRank = rank[thisRank];

	const Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	switch(currentCommand->getCommandType())
	{
	case ACTIVATE:
		{
			// refer to Table 11.4 in Memory Systems: Cache, DRAM, Disk

			// respect the row cycle time limitation
			tick tRCLimit = currentBank.getLastRASTime() + timingSpecification.tRC();

			// respect tRRD and tRC of all other banks of same rank
			tick tRRDLimit;

			const tick lastRASTime = currentRank.lastActivateTimes.front(); 

			// respect the row-to-row activation delay
			tRRDLimit = lastRASTime + timingSpecification.tRRD();				

			// respect tRP of same bank
			tick tRPLimit = currentBank.getLastPrechargeTime() + timingSpecification.tRP();

			// respect the t_faw value for DDR2 and beyond
			tick tFAWLimit;

			const tick fourthRASTime = currentRank.lastActivateTimes.back(); 
			tFAWLimit = (int)(fourthRASTime - time) + timingSpecification.tFAW();

			tFAWLimit = time;

			// respect tRFC, refresh cycle time
			tick tRFCLimit = currentRank.getLastRefreshTime() + timingSpecification.tRFC();

			nextTime = max(max(max(tRFCLimit,tRCLimit) , tRPLimit) , max(tRRDLimit , tFAWLimit));

			//DEBUG_TIMING_LOG(currentCommand->getCommandType() << " ras[" << setw(2) << t_ras_gap << "] rrd[" << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]");
		}
		break;

	case READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met (thanks to tAL), so no need to check tRAS timing requirement here.

	case READ:
		{
			//respect last RAS of same rank
			tick tRCDLimit = currentBank.getLastRASTime() + (timingSpecification.tRCD() - timingSpecification.tAL());

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent cas(w) time and length
			for (vector<Rank>::const_iterator thisRank = rank.begin(); thisRank != rank.end(); thisRank++)
			{
				if (thisRank->getRankID() != currentRank.getRankID())
				{
					if (thisRank->getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = thisRank->getLastCASTime();
						otherRankLastCASLength = thisRank->getLastCASLength();
					}
					if (thisRank->getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = thisRank->getLastCASWTime();
						otherRankLastCASWLength = thisRank->getLastCASWLength();
					}
				}
			}			

			// respect last CAS of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			tick tCASLimit = currentRank.getLastCASTime() + timingSpecification.tBurst();

			// respect last CASW of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			tCASLimit = max(tCASLimit,currentRank.getLastCASWTime() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR());

			if (rank.size() > 1) 
			{
				//respect most recent CAS of different rank
				tCASLimit = max(tCASLimit, otherRankLastCASTime + otherRankLastCASLength + timingSpecification.tRTRS());
				//respect timing of READ follow WRITE, different ranks
				tCASLimit = max(tCASLimit, otherRankLastCASWTime + timingSpecification.tCWD() + otherRankLastCASWLength + timingSpecification.tRTRS() - timingSpecification.tCAS());
			}

			nextTime = max(tRCDLimit,tCASLimit);

			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will meet tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple WRITE.

	case WRITE:
		{
			//respect last RAS of same rank
			tick tRASLimit = currentBank.getLastRASTime() + timingSpecification.tRCD() - timingSpecification.tAL();

			tick otherRankLastCASTime = time - 1000;
			int otherRankLastCASLength = timingSpecification.tBurst();
			tick otherRankLastCASWTime = time - 1000;
			int otherRankLastCASWLength = timingSpecification.tBurst();

			// find the most recent CAS/CASW time and length
			// FIXME: change to use iterators
			for (unsigned rankID = 0; rankID < rank.size() ; rankID++)
			{
				if (rankID != thisRank)
				{
					if (rank[rankID].getLastCASTime() > otherRankLastCASTime)
					{
						otherRankLastCASTime = rank[rankID].getLastCASTime();
						otherRankLastCASLength = rank[rankID].getLastCASLength();
					}
					if (rank[rankID].getLastCASWTime() > otherRankLastCASWTime)
					{
						otherRankLastCASWTime = rank[rankID].getLastCASWTime();
						otherRankLastCASWLength = rank[rankID].getLastCASWLength();
					}
				}
			}
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);

			// respect last cas to same rank
			// DW 3/9/2006 replace the line after next with the next line
			// t_cas_gap = max(0,(int)(this_r.last_cas_time + timing_specification.t_cas + cas_length + timing_specification.t_rtrs - timing_specification.t_cwd - now));
			tick tCASLimit = max(time,currentRank.getLastCASTime() + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas to different ranks
			tCASLimit = max(tCASLimit,otherRankLastCASTime + timingSpecification.tCAS() + otherRankLastCASLength + timingSpecification.tRTRS() - timingSpecification.tCWD());

			// respect last cas write to same rank
			// DW 3/9/2006 replace the line after next with the next line			
			// t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + casw_length - now));
			tCASLimit = max(tCASLimit,currentRank.getLastCASWTime() + currentRank.getLastCASWLength());

			// respect last CASW to different ranks
			// TODO: should this not also be -tAL?
			tCASLimit = max(tCASLimit,otherRankLastCASWTime + otherRankLastCASWLength + timingSpecification.tOST());

			nextTime = max(tRASLimit,tCASLimit);
		}
		break;

	case PRECHARGE:
		{
			// respect t_ras of same bank
			tick tRASLimit = currentBank.getLastRASTime() + timingSpecification.tRAS();

			// respect t_cas of same bank
			// TODO: do not need tAL for these
			//tick tCASLimit = max(time,currentBank.getLastCASTime() + timingSpecification.tAL() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tick tCASLimit = max(time,currentBank.getLastCASTime() + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD()));

			// respect t_casw of same bank
			//tCASLimit = max(tCASLimit,currentBank.getLastCASWTime() + timingSpecification.tAL() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());
			// tAL is accounted for by measuring the execution time internal to the DRAM
			tCASLimit = max(tCASLimit,currentBank.getLastCASWTime() + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR());

			nextTime = max(tRASLimit,tCASLimit);
		}
		break;

	case REFRESH_ALL:
		// respect tRFC and tRP
		nextTime = max(currentRank.getLastRefreshTime() + timingSpecification.tRFC(), currentRank.getLastPrechargeTime() + timingSpecification.tRP());
		break;

	case RETIRE_COMMAND:
	case PRECHARGE_ALL:
	case ACTIVATE_ALL:
	case DRIVE_COMMAND:
	case DATA_COMMAND:
	case CAS_WITH_DRIVE_COMMAND:
	case SELF_REFRESH:
	case DESELECT:
	case NOOP:
	case INVALID_COMMAND:
	default:
		nextTime = 0;
		break;
	}

	//return max(nextTime, time + timingSpecification.tCMD());
	//return max(nextTime, max(time, lastCommandIssueTime + timingSpecification.tCMD()));
	return max(nextTime, max(time , lastCommandIssueTime + timingSpecification.tCMD()));
}
