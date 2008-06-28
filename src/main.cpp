#include <map>
#include <iostream>
#include <fstream>

#include "System.h"
#include "fbdSystem.h"
#include "Rank.h"
#include "globals.h"
#include "Settings.h"

using namespace std;
using namespace DRAMSimII;

int main(int argc,const char **argv, char *envp[])
{
	map<enum FileIOToken,string> parameters;

	const Settings settings(argc, argv);
	//create_input_map(argc, argv, parameters);
	//simulation_parameters_t	*simulation_parameters;
	//dram_statistics_t	*dram_stats;

	if (settings.systemType == FBD_CONFIG)
	{
		fbdSystem fbds(settings);
		fbds.runSimulations();
	}
	else
	{
		System ds(settings); // combines read_command line and other inits
		//ds.run_simulations2();
		ds.runSimulations();
		timingOutStream << ds << endl;
	}

	
	//cerr << ds.timing_specification;
	//cerr << (dram_statistics) ds;
	return 0;
}
