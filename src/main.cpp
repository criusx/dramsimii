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
//#define _CRTDBG_MAP_ALLOC
#include <iostream>
#include <fstream>

//#include <stdlib.h>
//#include <crtdbg.h>

#include "globals.h"
#include "System.h"

using std::cerr;
using std::endl;

using namespace DRAMsimII;

int main(int argc,char **argv, char *envp[])
{
	const Settings settings(argc, argv);

	//////////////////////////////////////////////////////////////////////////
	// CPRH test
	//////////////////////////////////////////////////////////////////////////
#if 0
	const unsigned ranks = 4;
	const unsigned banks = 16;

	std::pair<unsigned,unsigned> lastRAS = std::pair<unsigned,unsigned>(0,banks / 2);
	std::pair<unsigned,unsigned> lastCAS = std::pair<unsigned,unsigned>(0,0);
	std::map<std::pair<unsigned,unsigned>,std::pair<unsigned,unsigned> > alreadyVisitedRas;
	std::map<std::pair<unsigned,unsigned>,unsigned> alreadyVisitedCas;

	for (unsigned i = 2* ranks * banks; i > 0; --i)
	{
		std::cerr << lastRAS.first << " " << lastRAS.second << " , " << lastCAS.first << " " << lastCAS.second << "\t" << 			
			(ranks - ((2 * (lastRAS.second % (banks / 2)) + !(lastRAS.second / (banks / 2))) % ranks) + lastRAS.first) % ranks << " " <<
		2 * (lastRAS.second % (banks / 2)) + !(lastRAS.second / (banks / 2)) << endl;
		if (alreadyVisitedRas.find(lastRAS) != alreadyVisitedRas.end())
		{
			//std::cerr << "too many" << endl;
		}
		else
		{
			alreadyVisitedRas[lastRAS] = lastCAS;
		}
		alreadyVisitedCas[lastCAS]++;
		

		unsigned nextCasBank = (lastCAS.second + 1) % banks;
		unsigned nextCasRank = (nextCasBank == 0) ? (lastCAS.first + 1) % ranks : lastCAS.first;

		unsigned nextRasRank =  (lastRAS.first + 1 + ((nextCasBank == 0) ? 1 : 0)) % ranks;
		unsigned nextRasBank = (nextCasBank == 0) ? banks / 2 : (lastRAS.second >= banks / 2 ? lastRAS.second - banks / 2 : lastRAS.second + banks / 2 + 1);
		lastCAS.first = nextCasRank;
		lastCAS.second = nextCasBank;
		lastRAS.first = nextRasRank;
		lastRAS.second = nextRasBank;
	}

	for (std::map<std::pair<unsigned,unsigned>,std::pair<unsigned,unsigned> >::const_iterator i = alreadyVisitedRas.begin(); i != alreadyVisitedRas.end(); i++)
	{
		std::cerr << "(" << i->first.first << "," << i->first.second << ")" << endl;
	}
	for (std::map<std::pair<unsigned,unsigned>,unsigned>::const_iterator i = alreadyVisitedCas.begin(); i != alreadyVisitedCas.end(); i++)
	{
		std::cerr << "(" << i->first.first << "," << i->first.second << ") " << i->second << endl;
	}

	// second effort
	std::pair<unsigned,unsigned> lastCommand = std::pair<unsigned,unsigned>(0,banks / 2);
	bool isActivate = true;


	for (unsigned i = 2* ranks * banks; i > 0; --i)
	{
		
		std::cerr << lastCommand.first << " " << lastCommand.second;
		if (isActivate)
			std::cerr << " , " ;
		else
			std::cerr << endl;

		if (isActivate)
		{
			// set the rank of the next CAS
			unsigned newBank = 2 * (lastCommand.second % (banks / 2)) + !(lastCommand.second / (banks / 2));
			unsigned newRank = (ranks - ((2 * (lastCommand.second % (banks / 2)) + !(lastCommand.second / (banks / 2))) % ranks) + lastCommand.first) % ranks;
			lastCommand = std::pair<unsigned,unsigned>(newRank,newBank);
		}
		else
		{
			unsigned newBank, newRank;
			if (lastCommand.second == banks - 1)
			{
				newRank = (lastCommand.first + 1) % ranks;
				newBank = banks / 2;
			}
			else
			{				
				newBank = (lastCommand.second % 2) * (banks / 2) + ((lastCommand.second + 1) / 2);
				newRank = (lastCommand.first + lastCommand.second + 1) % ranks;
			}
			lastCommand = std::pair<unsigned,unsigned>(newRank,newBank);
		}

		isActivate = !isActivate;
	}
#endif

#ifdef DEBUG
	//cerr << TICK_MAX << endl;
	//cerr << PHYSICAL_ADDRESS_MAX << endl;
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
	//_CrtDumpMemoryLeaks();
	return 0;
}


