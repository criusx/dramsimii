#include "dramsim.h"

/* find the protocol gap between a command and current system state */

int min_protocol_gap(int system_type, 
		dram_controller_t *channel, 
		command_t *this_c, 
		dram_system_configuration_t *config,
		dram_timing_specification_t *timing){
	int 	min_gap;
	int 	debug;
	int	rank_id,bank_id,row_id;
	int	this_rank_id,this_bank_id;
	bank_t	*this_b;
	rank_t	*this_r;
	tick_t 	now;
	int     t_cas_gap;
	int     t_faw_gap;             
	int     t_ras_gap;              
	int     t_rcd_gap;               
	int     t_rfc_gap;                
	int     t_rp_gap;                  
	int     t_rrd_gap;                  
	int	casw_to_rp_gap;
	int	cas_to_rp_gap;
	tick_t	*last_ras_time;		/* for t_rrd computations */
	tick_t	*fourth_ras_time;	/* for t_faw computations */
	tick_t	temp_time;
	tick_t	this_r_last_cas_time;
	tick_t	this_r_last_casw_time;
	tick_t	other_r_last_cas_time;
	tick_t	other_r_last_casw_time;
	tick_t	last_cas_time;
	tick_t	last_casw_time;
	int	last_cas_length;
	int	last_casw_length;
	int	other_r_last_cas_length;
	int	other_r_last_casw_length;
	int	i,j;
	int	time_inserted;
	int	t_al;
	int	ras_q_count;
	int	t_burst;
																			
	debug 		= FALSE;
	now		= channel->time;
	this_rank_id	= this_c->addr.rank_id;
	this_bank_id	= this_c->addr.bank_id;
	this_r 		= &(channel->rank[this_rank_id]);
	this_b		= &(this_r->bank[this_bank_id]);
	if(this_c->posted_cas == TRUE){
		t_al 	= timing->t_al;
	} else {
		t_al	= 0;
	}
	t_cas_gap 	= 0;
	t_faw_gap 	= 0;             
	t_ras_gap 	= 0;              
	t_rcd_gap 	= 0;               
	t_rfc_gap 	= 0;                
	t_rp_gap 	= 0;                  
	t_rrd_gap 	= 0;                  

	switch(this_c->command){
		case RAS_COMMAND:
			/* respect t_rp of same bank*/
			t_rp_gap	= MAX(0,(int)(this_b->last_prec_time + timing->t_rp - now));
	
			/* respect t_rrd of all other banks of same rank*/
			ras_q_count	= q_count(this_r->last_ras_times);
			if(ras_q_count == 0){
				t_rrd_gap 	= 0;
			} else {
				last_ras_time	= q_read(this_r->last_ras_times, ras_q_count - 1);	/* read tail end of ras history */
				t_rrd_gap 	= MAX(t_rrd_gap,(int)(*last_ras_time + timing->t_rrd - now));
			}
			if(ras_q_count < 4){
				t_faw_gap	= 0;
			} else {
				fourth_ras_time= q_read(this_r->last_ras_times, 0);	/* read head of ras history */
				t_faw_gap 	= MAX(0,(int)(*fourth_ras_time + timing->t_faw - now));
			}

			min_gap 	= MAX(t_rp_gap,MAX(t_rrd_gap,t_faw_gap));
		break;
	
		case CAS_AND_PRECHARGE_COMMAND:
			/* Auto precharge will be issued as part of command,
			 * but DRAM devices are intelligent enough to delay the prec command
			 * until tRAS timing is met, so no need to check tRAS timing requirement here.
			 */
		case CAS_COMMAND:
			/* respect last ras of same rank */
			t_ras_gap	= MAX(0,(int)(this_b->last_ras_time + timing->t_rcd - t_al - now));

			other_r_last_cas_time	= now - 1000;
			other_r_last_cas_length	= timing->t_burst;
			other_r_last_casw_time	= now - 1000;
			other_r_last_casw_length= timing->t_burst;
			for(rank_id = 0;rank_id< config->rank_count;rank_id++){
				if(rank_id != this_rank_id){
					if((channel->rank[rank_id]).last_cas_time > other_r_last_cas_time){
						other_r_last_cas_time   = (channel->rank[rank_id]).last_cas_time;
						other_r_last_cas_length = (channel->rank[rank_id]).last_cas_length;
					}
					if((channel->rank[rank_id]).last_casw_time > other_r_last_casw_time){
						other_r_last_casw_time   = (channel->rank[rank_id]).last_casw_time;
						other_r_last_casw_length = (channel->rank[rank_id]).last_casw_length;
					}
				}
			}
	
			/* respect last cas of same rank */
			t_cas_gap	= MAX(0,(int)(this_r->last_cas_time + timing->t_burst - now));
			/* respect last cas write of same rank */
			t_cas_gap	= MAX(t_cas_gap,(int)(this_r->last_casw_time + timing->t_cwd + timing->t_burst + timing->t_wtr - now));

			if(config->rank_count > 1) {
				/* respect most recent cas of different rank */
				t_cas_gap	= MAX(t_cas_gap,(int)(other_r_last_cas_time + other_r_last_cas_length + timing->t_rtrs - now));
				/* respect timing of READ follow WRITE, different ranks.*/
				t_cas_gap	= MAX(t_cas_gap,(int)(other_r_last_casw_time + timing->t_cwd + other_r_last_casw_length + timing->t_rtrs - timing->t_cas - now));
			}
			min_gap		= MAX(t_ras_gap,t_cas_gap);
			/*
			fprintf(stderr,"   [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",
					(int)now,
					(int)this_r_last_cas_time,
					(int)this_r_last_casw_time,
					(int)other_r_last_cas_time,
					(int)other_r_last_casw_time,
					min_gap);
					*/
		break;
		case CAS_WRITE_AND_PRECHARGE_COMMAND:
			/* Auto precharge will be issued as part of command, so
			 * Since commodity DRAM devices are write-cycle limited, we don't have to worry if
			 * the precharge will met tRAS timing or not. So CAS_WRITE_AND_PRECHARGE_COMMAND
			 * has the exact same timing requirements as a simple CAS COMMAND.
			 */
		case CAS_WRITE_COMMAND:
			/* respect last ras of same rank */
			t_ras_gap	= MAX(0,(int)(this_b->last_ras_time + timing->t_rcd - t_al - now));

			other_r_last_cas_time 	= now - 1000;
			other_r_last_cas_length	= timing->t_burst;
			other_r_last_casw_time	= now - 1000;
			other_r_last_casw_length= timing->t_burst;
			for(rank_id = 0;rank_id< config->rank_count;rank_id++){
				if (rank_id != this_rank_id){
					if((channel->rank[rank_id]).last_cas_time > other_r_last_cas_time){
						other_r_last_cas_time   = (channel->rank[rank_id]).last_cas_time;
						other_r_last_cas_length = (channel->rank[rank_id]).last_cas_length;
					}
					if((channel->rank[rank_id]).last_casw_time > other_r_last_casw_time){
						other_r_last_casw_time   = (channel->rank[rank_id]).last_casw_time;
						other_r_last_casw_length = (channel->rank[rank_id]).last_casw_length;
					}
				}
			}
			/* respect last cas to same rank */
			t_cas_gap	= MAX(0,(int)(this_r->last_cas_time + timing->t_cas + timing->t_burst + timing->t_rtrs - timing->t_cwd - now));
			/* respect last cas to different ranks */
			t_cas_gap	= MAX(t_cas_gap,(int)(other_r_last_cas_time + timing->t_cas + other_r_last_cas_length + timing->t_rtrs - timing->t_cwd - now));
			/* respect last cas write to same rank */
			t_cas_gap	= MAX(t_cas_gap,(int)(this_r->last_casw_time + timing->t_burst - now));
			/* respect last cas write to different ranks */
			t_cas_gap	= MAX(t_cas_gap,(int)(other_r_last_casw_time + other_r_last_casw_length - now));
			min_gap		= MAX(t_ras_gap,t_cas_gap);
		break;
		case RETIRE_COMMAND:
		break;
		case PRECHARGE_COMMAND:
			/* respect t_ras of same bank */
			t_ras_gap	= MAX(0,(int)(this_b->last_ras_time + timing->t_ras - now));

			/* respect t_cas of same bank */
			t_cas_gap	= MAX(0,(int)(this_b->last_cas_time + t_al + timing->t_cas + timing->t_burst + 
											MAX(timing->t_rtp - timing->t_cmd, 0)- now));
			/* respect t_casw of same bank */
			t_cas_gap	= MAX(t_cas_gap,(int)(this_b->last_casw_time + t_al + timing->t_cwd + timing->t_burst + timing->t_wr - now));
			min_gap		= MAX(t_ras_gap,t_cas_gap);
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
	debug = FALSE;
	if(debug == TRUE){
		/*
		print_command_type(this_c->command);
		*/
		fprintf(stderr,"       ras[%2d] rrd[%2d] faw[%2d] cas[%2d] rrd[%2d] rp[%2d] min[%2d]\n",
				t_ras_gap,t_rrd_gap,t_faw_gap,t_cas_gap,t_rrd_gap,t_rp_gap,min_gap);
	}
	return MAX(min_gap,timing->t_cmd);
}
