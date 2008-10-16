#ifndef SSTDRAMSYSTEM
#define SSTDRAMSYSTEM

#pragma once

#include <enkidu/enkidu.h>
#include <global.h>
#include "level1/cacheInternals.h"
#include "globals.h"
#include <boost/random.hpp>
#include <boost/random/variate_generator.hpp>

#include "System.h"

//#define TESTNEW

class SSTdramSystem : public SW2
{
protected:
	DRAMSimII::System *ds;				///< the DRAMSimII object
	tick nextStats;						///< the next time at which stats should be collected

	// random number stuff
	boost::mt19937 randomNumberGenerator;
	boost::uniform_real<> rngDistributionModel;	///< generates a random distribution from the set of [0:1)
	boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rngGenerator;
	int cpuRatio;											///< the ratio of the cpu frequency to the memory frequency


	bool doAtomicAccess(parcel *p);
	void moveToTime(const tick now);

public:
	SSTdramSystem(std::string prefix);
	~SSTdramSystem();

	int load();
	virtual void setup();
	virtual void finish();
	virtual void handleParcel(parcel *p);
	virtual void preTic();
	virtual void postTic();
	int getCPURatio() const { return cpuRatio; }			///< returns the ratio of the cpu frequency to the memory frequency

};

#endif
