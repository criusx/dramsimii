#ifndef M5_DRAMSYSTEM
#endif
#pragma once

#include "mem/physical.hh"
#include "dramSystem.h"

// this is a wrapper class to allow DRAMSimII
// to integrate with M5
class M5dramSystem: public PhysicalMemory
{
protected:
	dramSystem *ds = NULL;
	Tick calculateLatency(Packet *);

public:
	struct Params: public PhysicalMemory::Params
	{
		std::string outFilename;
		std::string dramType;
		std::string rowBufferManagmentPolicy;
		std::string autoPrecharge;
		std::string addrMappingScheme;
		std::string datarate;
		std::string refreshTime;
		std::string readWriteGrouping;
		std::string refreshPolicy;
		std::string seniorityAgeLimit;
		std::string postedCas;
		std::string clockGranularity;
		std::string rowCount;
		std::string cachelinesPerRow;
		std::string colCount;
		std::string colSize;
		std::string rowSize;
		std::string cachelineSize;
		std::string historyQueueDepth;
		std::string completionQueueDepth;
		std::string transactionQueueDepth;
		std::string eventQueueDepth;
		std::string perBankQueueDepth;
		std::string orderingAlgorithm;
		std::string configType;
		std::string chanCount;
		std::string rankCount;
		std::string bankCount;
		std::string refreshQueueDepth;
		std::string tAL;
		std::string tBURST;
		std::string tCAS;
		std::string tFAW;
		std::string tRAS;
		std::string tRP;
		std::string tRCD;
		std::string tRFC;
		std::string tRRD;
		std::string tRTP;
		std::string tRTRS;
		std::string tWR;
		std::string tWTR;
	};
	M5dramSystem(Params *);
};

#endif