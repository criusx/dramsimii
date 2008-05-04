#ifndef SSTDRAMSYSTEM
#define SSTDRAMSYSTEM

#pragma once

#include <enkidu/enkidu.h>
#include <global.h>
#include "level1/cacheInternals.h"
#include "globals.h"
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>


class SSTdramSystem : public component
{
protected:
	DRAMSimII::System ds;			///< the DRAMSimII object
	tick nextStats;					///< the next time at which stats should be collected

	// random number stuff
	base_generator_type generator;		///< the random generator base type
	boost::uniform_int<> uni_dist;		///< the type of random variable
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > uni;	///< the actual generator

	bool doAtomicAccess(parcel *p);

public:
	SSTdramSystem();
	~SSTdramSystem();
	virtual void setup();
	virtual void finish();
	virtual void handleParcel(parcel *p);
	virtual void preTic();
	virtual void postTic();
};

#endif