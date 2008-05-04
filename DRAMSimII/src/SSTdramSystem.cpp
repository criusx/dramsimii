#include "SSTdramSystem.h"
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>



SSTdramSystem::SSTdramSystem():
generator(42u),
uni_dist(0,65535),
uni(generator, uni_dist)
{
	
}

SSTdramSystem::~SSTdramSystem()
{
	delete ds;
}

void SSTdramSystem::setup() {}

void SSTdramSystem::finish() {}

#define TESTNEW

void SSTdramSystem::handleParcel(parcel *p)
{
#ifdef TESTNEW

	// do the read or write for this transaction
	doAtomicAccess(p);
	uni();
	DEBUG_TIMING_LOG("sending packet back at " << std::dec << static_cast<Tick>(curTick + 95996));
	schedSendTiming(pkt,curTick + 200000 + randomGen.random((Tick)0, (Tick)200000));
	
#endif
}