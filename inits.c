#include "dramsim.h"

/*
 * Init DRAM system
 */

dram_system_configuration_t *init_dram_system_configuration(){
	dram_system_configuration_t 	*this_c;

	this_c = (dram_system_configuration_t *)calloc(1,sizeof(dram_system_configuration_t));
	this_c->spd_file_ptr 			= NULL;
	this_c->config_type			= BASELINE_CONFIG;
	this_c->dram_type			= DDR2;
	this_c->datarate			= 1000; 		/* 1 Gbit per sec */
	this_c->row_buffer_management_policy	= CLOSE_PAGE;
	this_c->auto_precharge			= TRUE;
	this_c->addr_mapping_scheme		= CLOSE_PAGE_BASELINE;
	this_c->command_ordering_algorithm	= BANK_ROUND_ROBIN;
	this_c->read_write_grouping		= TRUE;
	this_c->refresh_policy			= NO_REFRESH;
	this_c->seniority_age_limit		= 200;			/* prevent against starvation */
	this_c->posted_cas			= FALSE;
	this_c->clock_granularity		= 2;			
	this_c->chan_count			= 1;
	this_c->rank_count			= 2;
	this_c->bank_count			= 16;
	this_c->row_count			= 16384;
	this_c->cachelines_per_row		= INVALID;		/* not used now, set to INVALID */
	this_c->col_count			= 512;
	this_c->col_size			= 8;			/* width of data bus */
	this_c->row_size			= INVALID;
	this_c->cacheline_size			= 64;
	this_c->per_bank_queue_depth		= 8;
	this_c->history_queue_depth		= 8;
	this_c->completion_queue_depth		= 8;
	this_c->transaction_queue_depth		= 32;
	this_c->event_queue_depth		= 32;			
	this_c->refresh_time			= 64000;
	this_c->short_burst_ratio		= 0.0; 
	this_c->read_percentage			= 0.5;
	return this_c;
}

/* all timing parameters in terms of cycles */

dram_timing_specification_t *init_dram_timing_specification(int dram_type){
	dram_timing_specification_t 	*timing;
	timing = (dram_timing_specification_t *)calloc(1,sizeof(dram_timing_specification_t));
	set_dram_timing_specification(timing, dram_type);
	return timing;
}

void set_dram_timing_specification(dram_timing_specification_t *timing, int dram_type){
	if(dram_type == SDRAM){						/* @ 100 MHz */
		timing->t_al				= 0;		/* no such thing as posted CAS in SDRAM */
		timing->t_burst				= 8;		/* depending on system config! can be 1, 2, 4, or 8 */
		timing->t_cas				= 2;
		timing->t_cmd 				= 1;		/* protocol specific, cannot be changed */
		timing->t_cwd				= 0;		/* no such thing in SDRAM */
		timing->t_int_burst			= 1;		/* prefetch length is 1 */
		timing->t_faw				= 0;		/* no such thing in SDRAM */
		timing->t_ras				= 5;		/* */
		timing->t_rc				= 7;		
		timing->t_rcd				= 2;
		timing->t_rfc				= 7;		/* same as t_rc */
		timing->t_rp				= 2;		/* 12 ns @ 1.25ns per cycle = 9.6 cycles */
		timing->t_rrd 				= 0;		/* no such thing in SDRAM */
		timing->t_rtp 				= 1;
		timing->t_rtrs				= 0;		/* no such thing in SDRAM */
		timing->t_wr 				= 2;		

	} else if (dram_type == DDR){					/* @ 200 MHz (400 Mbps) */
		timing->t_al				= 0;		/* no such thing in DDR */
		timing->t_burst				= 8;		/* depending on system config! can be 2, 4, or 8 */
		timing->t_cas				= 6;
		timing->t_cmd 				= 2;		/* protocol specific, cannot be changed */
		timing->t_cwd				= 2;		/* protocol specific, cannot be changed */
		timing->t_int_burst			= 2;		/* protocol specific, cannot be changed */
		timing->t_faw				= 0;		/* no such thing in DDR */
		timing->t_ras				= 16;		/* 40 ns @ 2.5 ns per beat == 16 beats */
		timing->t_rc				= 22;		/* 55 ns t_rc */
		timing->t_rcd				= 6;
		timing->t_rfc				= 28;		/* 70 ns @ 2.5 ns per beat == 28 beats */
		timing->t_rp				= 6;		/* 15 ns @ 2.5ns per beat = 6 beats */
		timing->t_rrd 				= 0;
		timing->t_rtp 				= 2;
		timing->t_rtrs				= 2;
		timing->t_wr 				= 6;		/* 15 ns @ 2.5 ns per beat = 6 beats */
		timing->t_wtr 				= 4;

	} else if (dram_type == DDR2){					/* @ 800 Mbps */
		timing->t_al				= 0;
		timing->t_burst				= 8;		/* depending on system config! can be 4, or 8 */
		timing->t_cas				= 10;
		timing->t_cmd 				= 2;		/* protocol specific, cannot be changed */
		timing->t_cwd				= timing->t_cas - 2;	/* protocol specific, cannot be changed */
		timing->t_int_burst			= 4;		/* protocol specific, cannot be changed */
		timing->t_faw				= 30;
		timing->t_ras				= 36;		/* 45 ns @ 1.25ns per beat = 36 beats */
		timing->t_rc				= 46;		/* 57 ns @ 1.25ns per beat = 45.6 beats */
		timing->t_rcd				= 10;
		timing->t_rfc				= 102;		/* 128 ns @ 1.25ns per beat ~= 102 beats */
		timing->t_rp				= 10;		/* 12 ns @ 1.25ns per beat = 9.6 beats */
		timing->t_rrd 				= 6;		/* 7.5 ns */
		timing->t_rtp 				= 6;
		timing->t_rtrs				= 2;
		timing->t_wr 				= 12;
		timing->t_wtr 				= 6;

	} else if (dram_type == DDR3){					/* @ 1.33 Gbps = 0.75 ns per beat */
		timing->t_al				= 0;
		timing->t_burst				= 8;		/* protocol specific, cannot be changed */
		timing->t_cas				= 10;
		timing->t_cmd 				= 2;		/* protocol specific, cannot be changed */
		timing->t_cwd				= timing->t_cas - 2;
		timing->t_int_burst			= 8;		/* protocol specific, cannot be changed */
		timing->t_faw				= 30;
		timing->t_ras				= 36;		/* 27 ns @ 0.75ns per beat = 36 beats */
		timing->t_rc				= 48;		/* 36 ns @ 0.75ns per beat = 48 beats */
		timing->t_rcd				= 12;
		timing->t_rfc				= 280;
		timing->t_rp				= 12;		/* 9 ns @ 0.75ns per beat = 12 beats */
		timing->t_rrd 				= 8;
		timing->t_rtrs				= 2;
		timing->t_rtp 				= 8;
		timing->t_wr 				= 12;
		timing->t_wtr 				= 8;

	} else if (dram_type == DRDRAM){					/* FIXME */
		/* not currently supported */
	}
}

/* init simulation parameters */

simulation_parameters_t *init_simulation_parameters(){
	simulation_parameters_t *this_sp;
	this_sp = (simulation_parameters_t *)calloc(1,sizeof(simulation_parameters_t));
	this_sp->debug_flag				= FALSE;
	this_sp->request_count				= 0;
	this_sp->input_type				= RANDOM;
	return this_sp;
}

bank_t *init_banks(dram_system_configuration_t *this_c){
	int i,bank_count;
	bank_t *this_b;
	bank_t *temp_b;
	bank_count = this_c->bank_count;
	this_b = (bank_t *)calloc(bank_count,sizeof(bank_t));
	temp_b = this_b;
	for(i=0;i<bank_count;i++){
		temp_b = &(this_b[i]);
		/* set these time to way in the past. won't impact timing of 0th command */
		temp_b->last_ras_time = -100;
		temp_b->last_cas_time = -100;
		temp_b->last_casw_time = -100;
		temp_b->last_prec_time = -100;
		temp_b->last_refresh_all_time = -100;
		temp_b->row_id = 0;
		temp_b->per_bank_q = init_q(this_c->per_bank_queue_depth);
		temp_b->ras_count = 0;
		temp_b->cas_count = 0;
	}
	return this_b;
}

rank_t *init_ranks(dram_system_configuration_t *this_c){
	int i,rank_count;
	rank_t *this_r;
	rank_t *temp_r;
	rank_count = this_c->rank_count;
	this_r = (rank_t *)calloc(rank_count,sizeof(rank_t));
	temp_r = this_r;
	for(i=0;i<rank_count;i++){
		temp_r = &(this_r[i]);
		temp_r->bank_count = this_c->bank_count;
		temp_r->last_bank_id = this_c->bank_count - 1;
		temp_r->last_refresh_time = 0;
		temp_r->last_cas_time = -100;
		temp_r->last_casw_time = -100;
		temp_r->bank = init_banks(this_c);
		temp_r->last_ras_times 	= init_q(4);	/* used for tfaw keep track of last 4 ras times */
	}
	return this_r;
}

dram_controller_t *init_dram_controllers(dram_system_configuration_t *this_c){
	int i,chan_count;
	rank_t *this_r;
	dram_controller_t *this_dc;
	dram_controller_t *temp_dc;

	chan_count = this_c->chan_count;
	this_dc = (dram_controller_t *)calloc(chan_count,sizeof(dram_controller_t));
	/* initialize all controllers */
	for(i=0;i<chan_count;i++){
		temp_dc = &(this_dc[i]);
		temp_dc->time 			= 0;
		temp_dc->last_refresh_time	= 0;
		temp_dc->refresh_row_index	= 0;
		temp_dc->last_rank_id 		= 0;
		temp_dc->rank = init_ranks(this_c);
		temp_dc->channel_q 	= init_q(this_c->transaction_queue_depth);
		temp_dc->history_q 	= init_q(this_c->history_queue_depth);
		temp_dc->completion_q 	= init_q(this_c->completion_queue_depth);
		temp_dc->refresh_q 	= init_q(this_c->refresh_queue_depth);
	}
	return this_dc;
}

dram_system_t *init_dram_system(){
	dram_system_t 	*dram_system;
	dram_system 		= (dram_system_t *)calloc(1,sizeof(dram_system_t));
	dram_system->time  	= 0;                  /* start the clock */
	dram_system->free_command_pool   	= init_q(32);           /* place to temporarily dump unused command_t structures */
	dram_system->free_transaction_pool 	= init_q(32);           /* ditto, but for transactions */
	dram_system->event_q             	= init_q(32);           /* create system wide event queue structure */
	return	dram_system;
}

input_stream_t *init_input_stream(dram_system_configuration_t *config){
	input_stream_t *this_i;
	long	seed;

	this_i = (input_stream_t *)calloc(1,sizeof(input_stream_t));

	this_i->type 			= RANDOM;
	this_i->trace_file_ptr 		= NULL;
	this_i->chan_locality 		= 1.0 / (double) (config->chan_count);
	this_i->rank_locality 		= 1.0 / (double) (config->rank_count);
	this_i->bank_locality 		= 1.0 / (double) (config->bank_count);
	this_i->row_locality 		= 0.2;
	this_i->read_percentage 	= config -> read_percentage;
	this_i->short_burst_ratio	= config -> short_burst_ratio;
	this_i->average_interarrival_cycle_count 	= 10;		
	this_i->interarrival_distribution_model 	= UNIFORM_DISTRIBUTION;

        if (this_i->interarrival_distribution_model == UNIFORM_DISTRIBUTION) {
                this_i->arrival_thresh_hold = 1.0 - (1.0 / (double) this_i->average_interarrival_cycle_count);
        } else if (this_i->interarrival_distribution_model == GAUSSIAN_DISTRIBUTION) {
                this_i->arrival_thresh_hold = 1.0 - (1.0 / box_muller((double)this_i->average_interarrival_cycle_count, 10));
	}

	seed	= (long) (1010321 + 9763099);
	srand48(seed);
	return this_i;
}

void print_WHCC(queue_t *WHCC){
	int i, q_depth;
	command_t 	*temp_c;
	q_depth = q_count(WHCC);
	for(i = 0;i<q_depth;i++){
		temp_c = q_read(WHCC,i);
		fprintf(stderr,"[%2d] ",i);
		if(temp_c->command == RAS_COMMAND){
			fprintf(stderr,"R ");
		} else {
			fprintf(stderr,"C ");
		}
		fprintf(stderr,"[%2d] [%2d]\n",temp_c->addr.rank_id,temp_c->addr.bank_id);
	}
}

dram_command_algorithm_t *init_algorithm(dram_system_t *dram_system){
	queue_t 	*WHCC;
	command_t	*temp_c;
	dram_system_configuration_t *config;
	queue_t         *free_command_pool;
	dram_command_algorithm_t *algorithm;

	algorithm 	= (dram_command_algorithm_t *)calloc(1,sizeof(dram_command_algorithm_t));

	config = dram_system->config;
	free_command_pool = dram_system->free_command_pool;
	algorithm->rank_id[0] = 0;
	algorithm->rank_id[1] = 3;
	algorithm->ras_bank_id[0] = 0;
	algorithm->ras_bank_id[1] = 0;
	algorithm->ras_bank_id[2] = 0;
	algorithm->ras_bank_id[3] = 0;
	algorithm->cas_count[0] = 0;
	algorithm->cas_count[1] = 0;
	algorithm->cas_count[2] = 0;
	algorithm->cas_count[3] = 0;

	algorithm->WHCC_offset[0] = 0;
	algorithm->WHCC_offset[1] = 0;
	algorithm->transaction_type[0] = READ_TRANSACTION;
	algorithm->transaction_type[1] = READ_TRANSACTION;
	algorithm->transaction_type[2] = READ_TRANSACTION;
	algorithm->transaction_type[3] = READ_TRANSACTION;
	WHCC = init_q(config->rank_count * config->bank_count * 2);
	algorithm->WHCC = WHCC;

	if((config->config_type == BASELINE_CONFIG) && (config->rank_count == 2)){
		if(config->bank_count == 8){
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		} else if (config->bank_count == 16){
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  8; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  9; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 10; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 11; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  8; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 12; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  9; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 10; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 13; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 11; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 12; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 14; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 13; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 14; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 15; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 15; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  8; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  9; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 10; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 11; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 12; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  8; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  9; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 13; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 10; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 11; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 14; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 12; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 13; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 15; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 14; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
			temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 15; enqueue(WHCC, temp_c);
		}
	} else if((config->config_type == BASELINE_CONFIG) && (config->rank_count == 4)){
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  0; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  3; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  1; enqueue(WHCC, temp_c);
		temp_c = acquire_item(free_command_pool, COMMAND); temp_c->command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; enqueue(WHCC, temp_c);
	}
	return algorithm;
}
