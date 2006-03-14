#include "dramsim.h"

void    print_dram_stats(FILE *fout, dram_statistics_t *stats){
	double BW_efficiency;
	double request_rate;

	BW_efficiency = (stats->bo8_count * 8.0 + stats->bo4_count * 4.0) * 100.0 / MAX(stats->end_time,1);
	request_rate  = stats->end_time / MAX(1,(stats->bo4_count + stats->bo8_count));
	fprintf(fout,"RR[%6.3lf] BWE[%6.3lf]\n",request_rate,BW_efficiency);
}

int main(int argc, char *argv[]){
	int rank_id, bank_id;
	dram_system_t		*dram_system;
	input_stream_t		*input_stream;
	simulation_parameters_t	*simulation_parameters;
	dram_statistics_t	*dram_stats;

	/* set defaults */
	dram_system		= init_dram_system();
	dram_system->config 	= init_dram_system_configuration();
	dram_system->timing 	= init_dram_timing_specification(DDR3);
	simulation_parameters 	= init_simulation_parameters();
	input_stream 		= init_input_stream(dram_system->config);
	dram_stats 		= init_dram_stats();

	/* override defaults */
	read_command_line_config(argc, argv, dram_system, simulation_parameters, input_stream, dram_stats);

	/* system parameters are now stable */
	dram_system->channel 	= init_dram_controllers(dram_system->config);
	dram_system->algorithm 	= init_algorithm(dram_system);

	/*
	fprintf(stderr,"Starting Simulations\n");
	*/
	run_simulations(dram_system,input_stream,simulation_parameters,dram_stats);
	dram_stats->system_config_type = dram_system->config->config_type;
	print_system_config_summary(stderr, dram_system->config);
	print_system_timing_summary(stderr, dram_system->timing);
	print_dram_stats(stderr, dram_stats);
}

