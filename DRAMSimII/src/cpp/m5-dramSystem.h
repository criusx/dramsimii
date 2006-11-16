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
	Tick calculateLatency(Packet *pkt);

public:
	struct Params: public PhysicalMemory::Params
	{
		std::string outFilename;
		std::string dramType;
		std::string rowBufferManagmentPolicy;
		bool autoPrecharge;
		std::string addrMappingScheme;
		int datarate;
		int refreshTime;
		bool readWriteGrouping;
		std::string refreshPolicy;
		unsigned seniorityAgeLimit;
		bool postedCas;
		int clockGranularity;
		unsigned rowCount;
		unsigned cachelinesPerRow;
		unsigned colCount;
		unsigned colSize;
		unsigned rowSize;
		unsigned cachelineSize;
		unsigned historyQueueDepth;
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