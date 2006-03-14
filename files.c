#include "dramsim.h"

/* file io #defines */

#define         EOL                     10
#define         CR                      13
#define         SPACE                   32
#define         TAB                     9
#define         PERCENT_SIGN            37

int get_next_bus_event(bus_event_t *this_e) {
	int		address;
	int		control;
	int		base_control;
	int		attributes;
	int		burst_length;
	int 		burst_count;
	int		bursting;
	char		input[1024];
	int		EOF_reached;
	double		timestamp;
	double 		multiplier;

	burst_length = 4;	/* Socket 7 cachelines are 32 byte long, burst of 4 */
	bursting = TRUE;
	EOF_reached = FALSE;
	if(trace_type == K6_TRACE){
		while((bursting == TRUE) && (EOF_reached == FALSE)){
			if(fscanf(fin,"%X",&address) == EOF) { /* found starting Hex address */
				printf("EOF Reached\n");
				return FAILURE;
			} 
			if(fscanf(fin,"%s",input) == EOF) {
				printf("Unexpected EOF, Please fix input trace file \n");
				return FAILURE;
			} 
			if((control = file_io_token(input)) == unknown_token){
				printf("Unknown Token Found [%s]\n",input);
				return FAILURE;
			} 
	
			if(control == MEM_WR){
				attributes = WRITE_TRANSACTION;
			} else {
				attributes = READ_TRANSACTION;
			}
			if(fscanf(fin,"%s",input) == EOF) {
				printf("Unexpected EOF, Please fix input trace file \n");
				return FAILURE;
			}
			timestamp = ascii2double(input);
	
			if(fscanf(fin,"%s",input) == EOF) {
				printf("Unexpected EOF, Please fix input trace file \n");
				return FAILURE;
			}
			if((this_e->attributes != control) ||
				(((this_e->address ^ address) & 0xFFFFFFE0) != 0) ||
				(burst_count == burst_length)){
				bursting = FALSE;
				timestamp = timestamp * ascii2multiplier(input);
				this_e->address 	= 0x3FFFFFFF & address;		/* mask out top addr bit */
				this_e->attributes 	= control;
				this_e->timestamp 	= timestamp;
				burst_count 		= 1;
			} else {
				burst_count++;
			}
		}
	} else if (trace_type == MASE_TRACE){
		if(fscanf(fin,"%X",&address) == EOF) { /* found starting Hex address */
			printf("Unexpected EOF, Please fix input trace file \n");
			return FAILURE;
		} 
		if(fscanf(fin,"%s",input) == EOF) {
			printf("Unexpected EOF, Please fix input trace file \n");
			return FAILURE;
		} 
		if((control = file_io_token(input)) == unknown_token){
			printf("Unknown Token Found [%s]\n",input);
			return FAILURE;
		} 
		if(fscanf(fin,"%X",&timestamp) == EOF) { /* found starting Hex address */
			printf("Unexpected EOF, Please fix input trace file \n");
			return FAILURE;
		} 
		this_e->address 	= address;
		this_e->attributes 	= control;
		this_e->timestamp 	= timestamp;
	}
	return SUCCESS;
}

int	is_valid_hex_number(char *input){
	int result;
	int length,i;

	result = TRUE;
	length = strlen(input);

	if(i>8){
		return FALSE;
	}

	for(i=0;i<length;i++){ /* 0-9 or A-F */
		if((input[i]> 47 && input[i] < 58) || (input[i] > 64 && input[i] < 71)){
			result = TRUE;
		} else {
			result = FALSE;
			return result;
		}
	}
	return result;
}

/* hand coded ascii to double routine.  */

double ascii2double(char *input){
	double 	result,decimal_multiplier;
	int  	decimal_flag,i,length;

	length = strlen(input);

	result = 0.0;
	decimal_multiplier = 1.0;
	decimal_flag = FALSE;

	for(i=0;i<length;i++){
		if(input[i] < 58 && input[i] > 47){
			if(!decimal_flag){
				result = result * 10 + (input[i] - 48);
			} else {
				decimal_multiplier *= 0.1;
				result = result + (input[i]-48) * decimal_multiplier;
			}
		} else if(input[i] == 46){  /* this is the decimal, set decimal_flag */
			decimal_flag = TRUE;
		} else if(input[i] == 44){  /* This is the comma, do nothing */

		} else if(input[i] == 58){ /*  This is the Colon denotes minute */
			result = result * 60;
		} else {
			printf("unrecognized character within number[%s]\n",input);
		}
	}
	return result;
}

double ascii2multiplier(char *input){
	double 	result;
	int  	length;
	int 	token;

	token = file_io_token(input);
	
	switch(token){
		case PICOSECOND:
			result = 0.001;
		break;
		case NANOSECOND:
			result = 1.0;
		break;
		case MICROSECOND:
			result = 1000.0;
		break;
		case MILLISECOND:
			result = 1000000.0;
		break;
		case SECOND:
			result = 1000000000.0;
		break;
		default:
			result = 0.0;
			fprintf(stderr,"unknown multipler[%s]\n",input);
		break;
	}
	return result;
}

void read_dram_config_from_file(dram_system_t *this_ds){

	FILE 	*fin;
	char 	c;
	char 	input_string[256];
	int	input_int;
	int	dram_config_token;
	dram_system_configuration_t *config;
	dram_timing_specification_t *timing;
	size_t length;

	config = this_ds->config;
	timing = this_ds->timing;
	fin = config->spd_file_ptr;

	while ((c = fgetc(fin)) != EOF){
		if((c != EOL) && (c != CR) && (c != SPACE) && (c != TAB)){
			fscanf(fin,"%s",&input_string[1]);
			input_string[0] = c;
		} else {
			fscanf(fin,"%s",&input_string[0]);
		}
		dram_config_token = file_io_token(&input_string[0]);
		switch(dram_config_token){
			case dram_type_token:
				fscanf(fin,"%s",&input_string[0]);
				if (!strncmp(input_string, "sdram", 5)){
					config->dram_type = SDRAM;	
				} else if (!strncmp(input_string, "ddrsdram", 8)){
					config->dram_type = DDR;	
				} else if (!strncmp(input_string, "drdram", 6)){
					config->dram_type = DRDRAM;	
				} else if (!strncmp(input_string, "ddr2", 4)){
					config->dram_type = DDR2;	
				} else if (!strncmp(input_string, "ddr3", 4)){
					config->dram_type = DDR3;	
				} else {
					config->dram_type = SDRAM;	
				}
				set_dram_timing_specification(timing, config->dram_type);
			break;
			case clock_granularity_token:
				fscanf(fin,"%d",&input_int);
			break;
			case datarate_token:				/* aka memory_frequency: units is MBPS */
				fscanf(fin,"%d",&input_int);
				config->datarate = input_int;	
			break;
			case row_buffer_management_policy_token:
				fscanf(fin,"%s",&input_string[0]);
				if (!strncmp(input_string, "open_page", 9)){
					config->row_buffer_management_policy = OPEN_PAGE;
				} else if (!strncmp(input_string, "close_page", 10)){
					config->row_buffer_management_policy = CLOSE_PAGE;
				} else {
					fprintf(stderr,"\n\n\nExpecting buffer management policy, found [%s] instead\n\n\n",input_string);
					config->row_buffer_management_policy = OPEN_PAGE;
				}
			break;
			case auto_precharge_token:
				fscanf(fin,"%s",&input_string[0]);
				if (!strncmp(input_string, "TRUE", 4)){
					config->auto_precharge = TRUE;
				} else {
					config->auto_precharge = FALSE;
				}
			break;
			case addr_mapping_scheme_token:
				fscanf(fin,"%s",&input_string[0]);
				if (!strncmp(input_string, "close_page_baseline", 19)){
					config->addr_mapping_scheme = CLOSE_PAGE_BASELINE;
				} else if (!strncmp(input_string, "open_page_baseline", 18)){
					config->addr_mapping_scheme = OPEN_PAGE_BASELINE;
				} else if (!strncmp(input_string, "sdram_hiperf_map", 16)){
					config->addr_mapping_scheme = OPEN_PAGE_BASELINE;
				} else {
					fprintf(stderr,"\n\n\nExpecting mapping policy, found [%s] instead\n\n\n",input_string);
					config->addr_mapping_scheme = OPEN_PAGE_BASELINE;
				}
			break;
			case transaction_ordering_policy_token:
				fscanf(fin,"%s",&input_string[0]);
				if (!strncmp(input_string, "strict_order", 12)){
					config->command_ordering_algorithm = STRICT_ORDER;
				} else if (!strncmp(input_string, "bank_round_robin", 16)){
					config->command_ordering_algorithm = BANK_ROUND_ROBIN;
				} else if (!strncmp(input_string, "rank_round_robin", 16)){
					config->command_ordering_algorithm = RANK_ROUND_ROBIN;
				} else if (!strncmp(input_string, "wang_hop", 8)){
					config->command_ordering_algorithm = WANG_RANK_HOP;
				}
			break;
			case refresh_time_token:
				fscanf(fin,"%d",&input_int);
				/* refresh time is specified in spd file in terms of microseconds. Expect datarate expressed in MHz.
				 * convert refresh time to total number of cycles per refresh loop */
				config->refresh_time = input_int * config->datarate;	
			break;
			case refresh_policy_token:
				fscanf(fin,"%s",&input_string[0]);
				if (!strncmp(input_string, "no_refresh", 10)){
					config->refresh_policy = NO_REFRESH;
				} else if (!strncmp(input_string, "refresh_one_chan_all_rank_all_bank", 30)){
					config->refresh_policy = BANK_CONCURRENT;
				} else if (!strncmp(input_string, "bank_concurrent", 15)){
					config->refresh_policy = BANK_CONCURRENT;
				} else if (!strncmp(input_string, "bank_staggered", 14)){
					config->refresh_policy = BANK_STAGGERED_HIDDEN;
				}
			break;
			case posted_cas_token:
				fscanf(fin,"%s",&input_string[0]);
				if (!strncmp(input_string, "TRUE", 4)){
					config->posted_cas = TRUE;
				} else {
					config->posted_cas = FALSE;
				}
			break;
			case chan_count_token:
				fscanf(fin,"%d",&input_int);
				config->chan_count = input_int;
			break;
			case channel_width_token:
				fscanf(fin,"%d",&input_int);
				if(config->dram_type != DRDRAM){
					config->col_size = input_int;
				}
			break;
			case rank_count_token:
				fscanf(fin,"%d",&input_int);
				config->rank_count = input_int;
			break;
			case bank_count_token:
				fscanf(fin,"%d",&input_int);
				config->bank_count = input_int;
			break;
			case row_count_token:
				fscanf(fin,"%d",&input_int);
				config->row_count = input_int;
			break;
			case col_count_token:
				fscanf(fin,"%d",&input_int);
				config->col_count = input_int;
			break;
			case col_size_token:
				fscanf(fin,"%d",&input_int);
				config->col_size = input_int;
			break;
			case row_size_token:
				fscanf(fin,"%d",&input_int);
				config->row_size = input_int;
			break;
			case cacheline_size_token:
				fscanf(fin,"%d",&input_int);
				config->cacheline_size = input_int;
			break;
			case per_bank_queue_depth_token:
				fscanf(fin,"%d",&input_int);
				config->per_bank_queue_depth = input_int;
			break;
			case t_al_token:
				fscanf(fin,"%d",&input_int);
				timing->t_al = input_int;
			break;
			case t_burst_token:
				fscanf(fin,"%d",&input_int);
				timing->t_burst = input_int;
			break;
			case t_cas_token:
				fscanf(fin,"%d",&input_int);
				timing->t_cas = input_int;
				switch(config->dram_type){		/* also set t_cwd */
				 	case SDRAM:
						timing->t_cwd = 0;
					break;	
				 	case DDR:
						timing->t_cwd = 2;
					break;	
				 	case DDR2:
				 	case DDR3:
						timing->t_cwd = timing->t_cas - 2;
					break;	
				}	
			break;
			case t_cmd_token:
				fscanf(fin,"%d",&input_int);		/* t_cmd is protocol specific, not settable here */
				fprintf(stderr,"(mild warning) t_cmd is protocol specific, not settable here\n");
			break;
			case t_cwd_token:
				fscanf(fin,"%d",&input_int);
				fprintf(stderr,"(mild warning) t_cwd is protocol specific, not settable here\n");
			break;
			case t_faw_token:
				fscanf(fin,"%d",&input_int);
				timing->t_faw = input_int;
			break;
			case t_ras_token:
				fscanf(fin,"%d",&input_int);
				timing->t_ras = input_int;
			break;
			case t_rc_token:
				fscanf(fin,"%d",&input_int);
				timing->t_rc = input_int;
			break;
			case t_rcd_token:
				fscanf(fin,"%d",&input_int);
				timing->t_rcd = input_int;
			break;
			case t_rfc_token:
				fscanf(fin,"%d",&input_int);
				timing->t_rfc = input_int;
			break;
			case t_rp_token:
				fscanf(fin,"%d",&input_int);
				timing->t_rp = input_int;
			break;
			case t_rrd_token:
				fscanf(fin,"%d",&input_int);
				timing->t_rrd = input_int;
			break;
			case t_rtp_token:
				fscanf(fin,"%d",&input_int);
				timing->t_rtp = input_int;
			break;
			case t_rtrs_token:
				fscanf(fin,"%d",&input_int);
				timing->t_rtrs = input_int;
			break;
			case t_wr_token:
				fscanf(fin,"%d",&input_int);
				timing->t_wr = input_int;
			break;
			case t_wtr_token:
				fscanf(fin,"%d",&input_int);
				timing->t_wtr = input_int;
			break;
			case comment_token:
				while (((c = fgetc(fin)) != EOL) && (c != EOF)){
					/*comment, to be ignored */
				}
			break;
			default:
			case unknown_token:
				fprintf(stderr,"Unknown Token [%s]\n",input_string);
			break;
			case deprecated_ignore_token:
				fscanf(fin,"%d",&input_int);
				fprintf(stderr,"[%s] no longer supported\n",input_string);
			break;
		}
	}
}

int file_io_token(char *input){
	size_t length;
	length = strlen(input);
	if(strncmp(input, "//",2) == 0) {
		return comment_token;
	} else if (strncmp(input, "type",length) == 0) {
		return dram_type_token;
	} else if (strncmp(input, "datarate",length) == 0) {
		return datarate_token;
	} else if (strncmp(input, "clock_granularity",length) == 0) {
		return clock_granularity_token;
	} else if (strncmp(input, "row_buffer_policy",length) == 0) {
		return row_buffer_management_policy_token;
	} else if (strncmp(input, "auto_precharge",length) == 0) {
		return auto_precharge_token;
	} else if (strncmp(input, "PA_mapping_policy",length) == 0) {
		return addr_mapping_scheme_token;
	} else if (strncmp(input, "addr_mapping_scheme",length) == 0) {
		return addr_mapping_scheme_token;
	} else if (strncmp(input, "transaction_ordering_policy",length) == 0) {
		return transaction_ordering_policy_token;
	} else if (strncmp(input, "command_ordering_algorithm",length) == 0) {
		return command_ordering_algorithm_token;
	} else if (strncmp(input, "command_ordering_policy",length) == 0) {
		return command_ordering_algorithm_token;
	} else if (strncmp(input, "auto_refresh_policy",length) == 0) {
		return refresh_policy_token;
	} else if (strncmp(input, "refresh_policy",length) == 0) {
		return refresh_policy_token;
	} else if (strncmp(input, "refresh_time",length) == 0) {
		return refresh_time_token;
	} else if (strncmp(input, "posted_cas",length) == 0) {
		return posted_cas_token;
	} else if (strncmp(input, "channel_count",length) == 0) {
		return chan_count_token;
	} else if (strncmp(input, "channel_width",length) == 0) {
		return channel_width_token;
	} else if (strncmp(input, "rank_count",length) == 0) {
		return rank_count_token;
	} else if (strncmp(input, "bank_count",length) == 0) {
		return bank_count_token;
	} else if (strncmp(input, "row_count",length) == 0) {
		return row_count_token;
	} else if (strncmp(input, "col_count",length) == 0) {
		return col_count_token;
	} else if (strncmp(input, "col_size",length) == 0) {
		return col_size_token;
	} else if (strncmp(input, "row_size",length) == 0) {
		return row_size_token;
	} else if (strncmp(input, "cacheline_size",length) == 0) {
		return cacheline_size_token;
	} else if (strncmp(input, "per_bank_queue_depth",length) == 0) {
		return per_bank_queue_depth_token;
	} else if (strncmp(input, "t_cmd",length) == 0) {
		return t_cmd_token;
	} else if (strncmp(input, "t_faw",length) == 0) {
		return t_faw_token;
	} else if (strncmp(input, "t_rfc",length) == 0) {
		return t_rfc_token;
	} else if (strncmp(input, "t_rrd",length) == 0) {
		return t_rrd_token;
	} else if (strncmp(input, "t_rtp",length) == 0) {
		return t_rtp_token;
	} else if (strncmp(input, "t_wr",length) == 0) {
		return t_wr_token;
	} else if (strncmp(input, "t_wtr",length) == 0) {
		return t_wtr_token;
	} else if (strncmp(input, "t_ras",length) == 0) {
		return t_ras_token;
	} else if (strncmp(input, "t_rcd",5) == 0) {
		return t_rcd_token;
	} else if (strncmp(input, "t_cas",length) == 0) {
		return t_cas_token;
	} else if (strncmp(input, "t_cac",length) == 0) {
		return deprecated_ignore_token;
	} else if (strncmp(input, "t_rp",length) == 0) {
		return t_rp_token;
	} else if (strncmp(input, "t_rcd",5) == 0) {
		return t_rcd_token;
	} else if (strncmp(input, "t_rc",4) == 0) {
		return t_rc_token;
	} else if (strncmp(input, "t_cwd",length) == 0) {
		return t_cwd_token;
	} else if (strncmp(input, "t_al",length) == 0) {
		return t_al_token;
	} else if (strncmp(input, "t_rl",length) == 0) {
		return t_rl_token;
	} else if (strncmp(input, "t_dqs",length) == 0) {
		return t_rtrs_token;
	} else if (strncmp(input, "t_rtrs",length) == 0) {
		return t_rtrs_token;
	} else if (strncmp(input, "t_burst",length) == 0) {
		return t_burst_token;
	} else if(strncmp(input, "FETCH",length) == 0) {
		return FETCH;
	} else if (strncmp(input, "IFETCH",length) == 0) {
		return FETCH;
	} else if (strncmp(input, "P_FETCH",length) == 0) {
		return FETCH;
	} else if (strncmp(input, "P_LOCK_RD",length) == 0) {
		return LOCK_RD;                    
	} else if (strncmp(input, "P_LOCK_WR",length) == 0) {
		return LOCK_WR;                    
	} else if (strncmp(input, "LOCK_RD",length) == 0) {
		return LOCK_RD;                    
	} else if (strncmp(input, "LOCK_WR",length) == 0) {
		return LOCK_WR;                    
	} else if (strncmp(input, "MEM_RD",length) == 0) {
		return MEM_RD;                    
	} else if (strncmp(input, "WRITE",length) == 0) {
		return MEM_WR;                    
	} else if (strncmp(input, "MEM_WR",length) == 0) {
		return MEM_WR;                    
	} else if (strncmp(input, "READ",length) == 0) {
		return MEM_RD;                    
	} else if (strncmp(input, "P_MEM_RD",length) == 0) {
		return MEM_RD;                    
	} else if (strncmp(input, "P_MEM_WR",length) == 0) {
		return MEM_WR;                    
	} else if (strncmp(input, "P_I/O_RD",length) == 0) {
		return IO_RD;                    
	} else if (strncmp(input, "P_I/O_WR",length) == 0) {
		return IO_WR;                    
	} else if (strncmp(input, "IO_RD",length) == 0) {
		return IO_RD;                    
	} else if (strncmp(input, "I/O_RD",length) == 0) {
		return IO_RD;                    
	} else if (strncmp(input, "IO_WR",length) == 0) {
		return IO_WR;                    
	} else if (strncmp(input, "I/O_WR",length) == 0) {
		return IO_WR;                    
	} else if (strncmp(input, "P_INT_ACK",length) == 0) {
		return INT_ACK;                    
	} else if (strncmp(input, "INT_ACK",length) == 0) {
		return INT_ACK;                    
	} else if (strncmp(input, "BOFF",length) == 0) {
		return BOFF;                    
	} else if (strncmp(input, "ps",length) == 0) {
		return PICOSECOND;                    
	} else if (strncmp(input, "ns",length) == 0) {
		return NANOSECOND;                    
	} else if (strncmp(input, "us",length) == 0) {
		return MICROSECOND;                    
	} else if (strncmp(input, "ms",length) == 0) {
		return MILLISECOND;                    
	} else if (strncmp(input, "s",length) == 0) {
		return SECOND;                    
	} else if (strncmp(input, "k6",length) == 0) {
		return K6_TRACE;                    
	} else if (strncmp(input, "mase",length) == 0) {
		return MASE_TRACE;                    
	} else if (strncmp(input, "random",length) == 0) {
		return RANDOM;                    
	} else {
		printf("Unknown %s\n",input);
		return unknown_token;
	}
}

void print_command_type(int command){
	switch(command){
		case	RAS_COMMAND:
			fprintf(stderr,"RAS    ");
		break;
		case 	CAS_COMMAND:
			fprintf(stderr,"CAS    ");
		break;
		case 	CAS_AND_PRECHARGE_COMMAND:
			fprintf(stderr,"CAS+P  ");
		break;
		case 	CAS_WRITE_COMMAND:
			fprintf(stderr,"CASW   ");
		break;
		case 	CAS_WRITE_AND_PRECHARGE_COMMAND:
			fprintf(stderr,"CASW+P ");
		break;
		case 	RETIRE_COMMAND:
			fprintf(stderr,"RETIRE ");
		break;
		case 	PRECHARGE_COMMAND:
			fprintf(stderr,"PREC   ");
		break;
		case 	PRECHARGE_ALL_COMMAND:
			fprintf(stderr,"PREC_A ");
		break;
		case 	RAS_ALL_COMMAND:
			fprintf(stderr,"RAS_A  ");
		break;
		case 	DRIVE_COMMAND:
			fprintf(stderr,"DRIVE  ");
		break;
		case 	DATA_COMMAND:
			fprintf(stderr,"DATA   ");
		break;
		case 	CAS_WITH_DRIVE_COMMAND:
			fprintf(stderr,"CAS+D  ");
		break;
		case 	REFRESH_ALL_COMMAND:
			fprintf(stderr,"REF_A  ");
		break;
	}
}

void print_command(command_t *this_c){
	print_command_type(this_c->command);
	print_addresses(&(this_c->addr));
}

void print_transaction(transaction_t *this_t){
	fprintf(stderr,"[%8d] ",(int)this_t->arrival_time);
	switch(this_t->type){
		case IFETCH_TRANSACTION:
			fprintf(stderr,"IFETCH ");
		break;
		case WRITE_TRANSACTION:
			fprintf(stderr,"WRITE ");
		break;
		case READ_TRANSACTION:
			fprintf(stderr,"READ  ");
		break;
		case PREFETCH_TRANSACTION:
			fprintf(stderr,"PREF  ");
		break;
		case AUTO_REFRESH_TRANSACTION:
			fprintf(stderr,"AUTOR ");
		break;
		case PER_BANK_REFRESH_TRANSACTION:
			fprintf(stderr,"BANKR ");
		break;
		case AUTO_PRECHARGE_TRANSACTION:
			fprintf(stderr,"AUTOP ");
		break;
	}
	fprintf(stderr,"[%8d]\n",(int)this_t->completion_time);
}

void print_system_config_summary(FILE *fout, dram_system_configuration_t *config){
	fprintf(fout,"SYS[");
	switch(config->config_type){
		case BASELINE_CONFIG:
			fprintf(fout,"BASE] ");
		break;
		default:
			fprintf(fout,"UNKN] ");
		break;
	}
	fprintf(fout,"RC[%d] ",config->rank_count);
	fprintf(fout,"BC[%2d] ",config->bank_count);
	fprintf(fout,"ALG[");
	switch(config->command_ordering_algorithm){
		case STRICT_ORDER:
			fprintf(fout,"STRO] ");
		break;
		case RANK_ROUND_ROBIN:
			fprintf(fout,"RRR ] ");
		break;
		case BANK_ROUND_ROBIN:
			fprintf(fout,"BRR ] ");
		break;
		case WANG_RANK_HOP:
			fprintf(fout,"WANG] ");
		break;
		default:
			fprintf(fout,"UNKN] ");
		break;
	}
	fprintf(fout,"BQD[%2d] ",config->per_bank_queue_depth);
	fprintf(fout,"BLR[%2d] ",(int)(100 * (config->short_burst_ratio + 0.0001)));
	fprintf(fout,"RP[%2d] ",(int)(100 * config->read_percentage));
}

void    print_system_timing_summary(FILE *fout, dram_timing_specification_t *timing){
	fprintf(fout,"rtrs[%d] ",timing->t_rtrs);
}

void    print_system_config_detailed(FILE *fout, dram_system_configuration_t *config){

}

