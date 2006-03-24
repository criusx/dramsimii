
#ifdef WIN32
#ifndef _DEBUG
#ifndef VLD_CONFIG_AGGREGATE_DUPLICATES
#define VLD_CONFIG_AGGREGATE_DUPLICATES
#endif
#include <vld.h>
#endif
#endif

#include <map>
#include <iostream>
#include <fstream>

#include "dramsim2.h"


int main(int argc, char *argv[])
{

  map<enum file_io_token_t,string> parameters;

  create_input_map(argc, argv, parameters);
  //simulation_parameters_t	*simulation_parameters;
  //dram_statistics_t	*dram_stats;

  dram_system ds(parameters); // combines read_command line and other inits
  //input_stream	is(ds);
  //simulation_parameters sp;
  //dram_statistics stats;
  //ds.read_command_line_config(argc,argv)
  ds.run_simulations();

  cerr << ds << endl;
  //cerr << ds.timing_specification;
  //cerr << (dram_statistics) ds;
}