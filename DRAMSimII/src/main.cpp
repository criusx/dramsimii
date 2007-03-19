
#ifdef WIN32
#ifndef RELEASE
#ifndef VLD_CONFIG_AGGREGATE_DUPLICATES
#define VLD_CONFIG_AGGREGATE_DUPLICATES
#endif
//#include <vld.h>
#endif
#endif

#include <map>
#include <iostream>
#include <fstream>

#include "dramSystem.h"
#include "rank_c.h"
#include "globals.h"
#include "dramSettings.h"

using namespace std;

int main(int argc,const char **argv, char *envp[])
{

	map<enum file_io_token_t,string> parameters;

	dramSettings *settings = new dramSettings(argc, argv);
	create_input_map(argc, argv, parameters);
	//simulation_parameters_t	*simulation_parameters;
	//dram_statistics_t	*dram_stats;

	dramSystem ds(settings); // combines read_command line and other inits
	delete settings;
	//input_stream	is(ds);
	//simulation_parameters sp;
	//dram_statistics stats;
	//ds.read_command_line_config(argc,argv)
	//ds.run_simulations2();
	ds.run_simulations3();

	outStream << ds << endl;
	//cerr << ds.timing_specification;
	//cerr << (dram_statistics) ds;
	
	return 0;
}
