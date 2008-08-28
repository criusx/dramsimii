#include <iostream>
#include <fstream>


#include "globals.h"
#include "System.h"

using std::cerr;
using std::endl;

using namespace DRAMSimII;

int main(int argc,const char **argv, char *envp[])
{
	const Settings settings(argc, argv);

#ifdef DEBUG
	cerr << TICK_MAX << endl;
	cerr << PHYSICAL_ADDRESS_MAX << endl;
	//DRAMSimII::unitTests(settings);
#endif

	if (settings.systemType == FBD_CONFIG)
	{
		//fbdSystem fbds(settings);
		//fbds.runSimulations();
	}
	else
	{
		System ds(settings);
		ds.runSimulations();
	}


	//cerr << ds.timing_specification;
	//cerr << (dram_statistics) ds;
	return 0;
}


