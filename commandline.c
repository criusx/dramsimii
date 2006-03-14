#include "dramsim.h"

void read_command_line_config(	int argc, 
				char *argv[], 
				dram_system_t *this_ds, 
				simulation_parameters_t *this_sp, 
				input_stream_t *this_i, 
				dram_statistics_t *stats)
{
	FILE		*fout;
	int		length,i,j;
	char 		c;
	dram_system_configuration_t *config;
	dram_timing_specification_t *timing;
	int		type;
	int		analysisflag;
	char		filename[256];
	char		outfile[256];
	int		argc_index;

	/* set defaults */
	
	argc_index 	= 1;
	fout		= stdout;
	filename[0]	= '\0';

	config = this_ds->config;
	timing = this_ds->timing;
	
	if((argc < 2) || (strncmp(argv[1], "-help",5) == 0)){
		fprintf(stdout,"Usage: %s -options optionswitch\n",argv[0]);
		fprintf(stdout,"-input_type [k6|mase|random]\n");
		fprintf(stdout,"-trace_file TRACE_FILENAME\n");
		fprintf(stdout,"-dram:spd_input SPD_FILENAME\n");
		fprintf(stdout,"-output_file OUTPUT_FILENAME\n");
		fprintf(stdout,"-debug \n");
		_exit(0);
	}

	while(argc_index < argc){
		length = strlen(argv[argc_index]);
		if(strncmp(argv[argc_index], "-input_type",length) == 0) {
			if(((type = file_io_token(argv[argc_index+1])) != RANDOM) && (type !=K6_TRACE) && (type != MASE_TRACE)){
			    	printf("Unknown input type [%s]\n",argv[argc_index+1]);
			} else {
				this_i->type = type;
			}
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-trace_file",length) == 0) {
			length = strlen(argv[argc_index+1]);
			if((this_i->trace_file_ptr=fopen(argv[argc_index+1],"r"))==NULL){
				printf("Error in opening tracefile %s\n",argv[argc_index+1]);
				_exit(3);
			}
			length = strlen(argv[argc_index+1]);
			for(i=0;i<length-4;i++){
				filename[i] = argv[argc_index+1][i];
			}
			filename[length-4] = '\0';
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-dram:spd_input",length) == 0) {
			length = strlen(argv[argc_index+1]);
			if((config->spd_file_ptr=fopen(argv[argc_index+1],"r"))==NULL){
				printf("Error in opening spd file %s\n",argv[argc_index+1]);
				_exit(3);
			} else {
				read_dram_config_from_file(this_ds);
			}
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:channel",length) == 0) {
			sscanf(argv[argc_index+1],"%d",&(config->chan_count));
			this_i->chan_locality           = 1.0 / (double) (config->chan_count);	/* reset locality defaults */
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:rank",length) == 0) {
			sscanf(argv[argc_index+1],"%d",&(config->rank_count));
			this_i->rank_locality           = 1.0 / (double) (config->rank_count);	/* reset locality defaults */
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:bank",length) == 0) {
			sscanf(argv[argc_index+1],"%d",&(config->bank_count));
			this_i->bank_locality           = 1.0 / (double) (config->bank_count);	/* reset locality defaults */
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:queue_depth",length) == 0) {
			sscanf(argv[argc_index+1],"%d",&(config->per_bank_queue_depth));
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:ordering_algorithm",length) == 0) {
			if (!strncmp(argv[argc_index+1], "strict_order", 12)){
				config->command_ordering_algorithm = STRICT_ORDER;
			} else if (!strncmp(argv[argc_index+1], "bank_round_robin", 16)){
				config->command_ordering_algorithm = BANK_ROUND_ROBIN;
			} else if (!strncmp(argv[argc_index+1], "rank_round_robin", 16)){
				config->command_ordering_algorithm = RANK_ROUND_ROBIN;
			} else if (!strncmp(argv[argc_index+1], "wang_hop", 8)){
				config->command_ordering_algorithm = WANG_RANK_HOP;
			} else {
				fprintf(stderr,"Unrecognized Option[%s]\n",argv[argc_index+1]);
				config->command_ordering_algorithm = BANK_ROUND_ROBIN;
			}
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:system_config",length) == 0) {
			config->config_type = BASELINE_CONFIG;
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:read_percentage",length) == 0) {
			sscanf(argv[argc_index+1],"%lf",&(config->read_percentage));
			this_i->read_percentage 	= config->read_percentage;
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-config:short_burst_ratio",length) == 0) {
			sscanf(argv[argc_index+1],"%lf",&(config->short_burst_ratio));
			this_i->short_burst_ratio 	= config->short_burst_ratio;
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-debug",length) == 0) {
			this_sp->debug_flag = TRUE;
			argc_index ++;
		} else if(strncmp(argv[argc_index], "-request_count",length) == 0) {
			sscanf(argv[argc_index+1],"%d",&(this_sp->request_count));
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-output_file",length) == 0) {
			sprintf(filename,"%s",argv[argc_index+1]);
			argc_index += 2;
		} else if(strncmp(argv[argc_index], "-timing:rtrs",length) == 0) {
			sscanf(argv[argc_index+1],"%d",&(timing->t_rtrs));
			argc_index += 2;
		} else {
			printf("[%s] is not a recognized option\n",argv[argc_index+1]);
			argc_index ++;
		}
	}
}
