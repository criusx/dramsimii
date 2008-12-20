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
#include <fstream>


#include "globals.h"
#include "System.h"

using std::cerr;
using std::endl;

using namespace DRAMsimII;

int main(int argc,const char **argv, char *envp[])
{
	const Settings settings(argc, argv);

#ifdef DEBUG
	cerr << TICK_MAX << endl;
	cerr << PHYSICAL_ADDRESS_MAX << endl;
	//DRAMsimII::unitTests(settings);
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


