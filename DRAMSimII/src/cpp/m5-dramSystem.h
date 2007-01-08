#ifndef M5_DRAMSYSTEM
#define M5_DRAMSYSTEM
#pragma once

#include "mem/physical.hh"
#include "mem/tport.hh"
#include "dramSystem.h"

// this is a wrapper class to allow DRAMSimII
// to integrate with M5
class M5dramSystem: public PhysicalMemory
{
	// taken from m5 interface
private:
	class MemPort : public SimpleTimingPort
	{
		// backward pointer to the memory system
		M5dramSystem *memory;

	public:

		MemPort(const std::string &_name, M5dramSystem *_memory);

	protected:

		virtual Tick recvAtomic(PacketPtr pkt);

		virtual void recvFunctional(PacketPtr pkt);

		virtual void recvStatusChange(Status status);

		virtual void getDeviceAddressRanges(AddrRangeList &resp,
			AddrRangeList &snoop);

		virtual int deviceBlockSize();

		virtual bool recvTiming(PacketPtr pkt);
	};

	

protected:
	MemPort *memoryPort;
	// the whole point of the wrapper class
	dramSystem *ds;
	int cpuRatio;
	void getAddressRanges(AddrRangeList &resp, AddrRangeList &snoop);
	//virtual Tick calculateLatency(Packet *);
	virtual Tick recvTiming(PacketPtr pkt);
	void virtual init();
	

public:
	// allows other components to get a port which they can send packets to
	virtual Port *getPort(const std::string &if_name, int idx = -1);


	struct Params: public PhysicalMemory::Params
	{
		int			cpu_ratio;
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
	virtual ~M5dramSystem();
};

#endif
