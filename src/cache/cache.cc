/*
* Copyright (c) 2003-2005 The Regents of The University of Michigan
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met: redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer;
* redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution;
* neither the name of the copyright holders nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Erik Hallnor
*/

/**
* @file
* Definitions of LRU tag store.
*/

#include <string>
#include <cassert>
#include <cstring>

#include "base.hh"
#include "cache.hh"
#include "../Settings.hh"

#include <boost/assert.hpp>

using DRAMsimII::Cache;
using DRAMsimII::CacheSet;
using DRAMsimII::LRUBlk;
using DRAMsimII::PacketList;
using DRAMsimII::Command;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;

using DRAMsimII::isPowerOf2;
using DRAMsimII::floorLog2;

// create and initialize a LRU/MRU cache structure
Cache::Cache(unsigned _numSets, unsigned _blkSize, unsigned _assoc,unsigned _hit_latency, unsigned _nmruCount, ReplacementPolicy _replacementPolicy):
numSets(_numSets),
blkSize(_blkSize),
assoc(_assoc),
hitLatency(_hit_latency),
sets(numSets, CacheSet(_assoc)),
blks(numSets * assoc),
dataBlks(numSets * assoc * blkSize),
setShift(floorLog2(blkSize)),
tagShift(floorLog2(blkSize) + floorLog2(numSets)),
setMask(numSets - 1),
blkMask(blkSize - 1),
writeAllocate(false),
nmruCount(_nmruCount),
replacementPolicy(_replacementPolicy)
{
	// Check parameters
	BOOST_ASSERT(blkSize >= 4 && isPowerOf2(blkSize));
	BOOST_ASSERT(numSets > 0 && isPowerOf2(numSets));
	BOOST_ASSERT(assoc > 0);
	BOOST_ASSERT(hitLatency > 0);

	if (replacementPolicy == Cache::NMRU)
	{
		if (nmruCount < 1)
		{
			cerr << "NMRU caches must be used with a count of >= 1" << endl;
			exit(-2);
		}
	}

	if (blkSize < 4 || !isPowerOf2(blkSize)) 
	{
		cerr << "Block size must be at least 4 and a power of 2" << endl;
		exit(-2);
	}
	if (numSets <= 0 || !isPowerOf2(numSets)) 
	{
		cerr << "# of sets must be non-zero and a power of 2" << endl;
		exit(-2);
	}
	if (assoc <= 0) 
	{
		cerr << "associativity must be greater than zero" << endl;
		exit(-2);
	}
	if (hitLatency <= 0) 
	{
		cerr << "access latency must be greater than zero" << endl;
		exit(-2);
	}

	//blkMask = blkSize - 1;
	//setShift = floorLog2(blkSize);
	//setMask = numSets - 1;
	//tagShift = setShift + floorLog2(numSets);
	warmedUp = false;
	/** @todo Make warmup percentage a parameter. */
	warmupBound = numSets * assoc;

	// 	sets = new CacheSet[numSets];
	// 	blks = new LRUBlk[numSets * assoc];
	// allocate data storage in one big chunk
	//dataBlks = new uint8_t[numSets*assoc*blkSize];

	unsigned blkIndex = 0;       // index into blks array
	for (unsigned i = 0; i < numSets; ++i) 
	{ // TODO switch this to work for copies
		// done in constructor
		//sets[i].assoc = assoc;

		// done in constructor
		//sets[i].blks = new LRUBlk*[assoc];

		// link in the data blocks
		for (unsigned j = 0; j < assoc; ++j) 
		{
			// locate next cache block
			LRUBlk *blk = &blks[blkIndex];
			blk->data = &dataBlks[blkSize*blkIndex];
			++blkIndex;

			// invalidate new cache block
			blk->status = 0;

			//EGH Fix Me : do we need to initialize blk?

			// Setting the tag to j is just to prevent long chains in the hash
			// table; won't matter because the block is invalid
			blk->tag = j;
			blk->whenReady = 0;
			blk->isTouched = false;
			blk->size = blkSize;
			sets[i].blks[j]=blk;
			blk->set = i;
		}
	}	
}

Cache::Cache(const Cache&rhs):
numSets(rhs.numSets),
blkSize(rhs.blkSize),
assoc(rhs.assoc),
hitLatency(rhs.hitLatency),
sets(rhs.sets),
blks(rhs.blks),
dataBlks(rhs.dataBlks),
setShift(rhs.setShift),
tagShift(rhs.tagShift),
setMask(rhs.setMask),
blkMask(rhs.blkMask),
writeAllocate(rhs.writeAllocate),
nmruCount(rhs.nmruCount),
replacementPolicy(rhs.replacementPolicy)
{
	unsigned blkIndex = 0;       // index into blks array
	for (unsigned i = 0; i < numSets; ++i) 
	{ // TODO switch this to work for copies
		// done in constructor
		//sets[i].assoc = assoc;

		// done in constructor
		//sets[i].blks = new LRUBlk*[assoc];

		// link in the data blocks
		for (unsigned j = 0; j < assoc; ++j) 
		{
			// locate next cache block
			LRUBlk *blk = &blks[blkIndex];
			blk->data = &dataBlks[blkSize*blkIndex];
			++blkIndex;

			// invalidate new cache block
			blk->status = 0;

			//EGH Fix Me : do we need to initialize blk?

			// Setting the tag to j is just to prevent long chains in the hash
			// table; won't matter because the block is invalid
			blk->tag = j;
			blk->whenReady = 0;
			blk->isTouched = false;
			blk->size = blkSize;
			sets[i].blks[j]=blk;
			blk->set = i;
		}
	}

}

Cache::Cache(const DRAMsimII::Settings &settings):
numSets((settings.cacheSize * 1024)/ settings.blockSize / settings.associativity),
blkSize(settings.blockSize),
assoc(settings.associativity),
hitLatency(settings.hitLatency),
sets((settings.cacheSize * 1024) / settings.blockSize / settings.associativity, CacheSet(settings.associativity)),
blks(numSets * assoc),
dataBlks(numSets * assoc * blkSize),
setShift(floorLog2(settings.blockSize)),
tagShift(floorLog2((settings.cacheSize * 1024) / settings.blockSize / settings.associativity) + floorLog2(settings.blockSize)), 
setMask((settings.cacheSize * 1024) / settings.blockSize / settings.associativity - 1),
blkMask(settings.blockSize - 1),
writeAllocate(false),
nmruCount(settings.nmruTrackingCount),
replacementPolicy(settings.replacementPolicy)
{
	if (blkSize < 4 || !isPowerOf2(blkSize)) 
	{
		cerr << "Block size must be at least 4 and a power of 2" << endl;
		exit(-2);
	}
	if (numSets <= 0 || !isPowerOf2(numSets)) 
	{
		cerr << "# of sets must be non-zero and a power of 2" << endl;
		exit(-2);
	}
	if (assoc <= 0) 
	{
		cerr << "associativity must be greater than zero" << endl;
		exit(-2);
	}
	if (hitLatency <= 0) 
	{
		cerr << "access latency must be greater than zero" << endl;
		exit(-2);
	}

	unsigned blkIndex = 0;       // index into blks array
	for (unsigned i = 0; i < numSets; ++i) 
	{ // TODO switch this to work for copies
		// done in constructor
		//sets[i].assoc = assoc;

		// done in constructor
		//sets[i].blks = new LRUBlk*[assoc];

		// link in the data blocks
		for (unsigned j = 0; j < assoc; ++j) 
		{
			// locate next cache block
			LRUBlk *blk = &blks[blkIndex];
			blk->data = &dataBlks[blkSize*blkIndex];
			blkIndex++;

			// invalidate new cache block
			blk->status = 0;

			//EGH Fix Me : do we need to initialize blk?

			// Setting the tag to j is just to prevent long chains in the hash
			// table; won't matter because the block is invalid
			blk->tag = j;
			blk->whenReady = 0;
			blk->isTouched = false;
			blk->size = blkSize;
			sets[i].blks[j]=blk;
			blk->set = i;
		}
	}
}

Cache::~Cache()
{
	for (unsigned i = 0; i < numSets; ++i) 
	{ 
		for (unsigned j = 0; j < assoc; ++j) 
		{	
			sets[i].blks[j] = NULL;
		}
	}
}

bool Cache::access(const Command *currentCommand, int &lat, BlkType *&blk, tick time, PacketList &writebacks)
{
	blk = accessBlock(currentCommand->getAddress().getPhysicalAddress(), lat, 0, time);

	if (blk != NULL)
	{
		BOOST_ASSERT(blk->isReadable());
		// address is not in the cache at this point
		//if (pkt->needsExclusive() ? blk->isWritable() : blk->isReadable()) 
		if (blk->isReadable())
		{
			// OK to satisfy access
			//hits[currentCommand->cmdToIndex()][0/*currentCommand->req->threadId()*/]++;
			satisfyCpuSideRequest(currentCommand, blk);
			return true;
		}
	}

	// Writeback handling is special case.  We can write the block
	// into the cache without having a writable copy (or any copy at
	// all).
	//if (pkt->cmd == MemCmd::Writeback)

	// handle a write miss with write allocate
	if (currentCommand->isWrite())
	{
		//assert(blkSize == 8 * currentCommand->getLength());
		if (blk == NULL) 
		{
			// need to do a replacement
			blk = allocateBlock(currentCommand->getAddress().getPhysicalAddress(), writebacks);
			BOOST_ASSERT(blk != NULL);

			if (blk == NULL) 
			{
				// no replaceable block available, give up.
				// writeback will be forwarded to next level.
				//incMissCount(pkt);
				return false;
			}
			//int id = pkt->req->hasContextId() ? pkt->req->contextId() : -1;
			int id = 0;
			insertBlock(currentCommand->getAddress().getPhysicalAddress(), blk, id, time);
			blk->status = BlkValid | BlkReadable;
		}
		//std::memcpy(blk->data, pkt->getPtr<uint8_t>(), blkSize);
		blk->status |= BlkDirty;
		// nothing else to do; writeback doesn't expect response
		//assert(!pkt->needsResponse());
		//hits[pkt->cmdToIndex()][0/*currentCommand->req->threadId()*/]++;

		return false;
		//return true;
	}

	// 	incMissCount(pkt);
	// 
	// 	if (blk == NULL && pkt->isLLSC() && pkt->isWrite()) {
	// 		// complete miss on store conditional... just give up now
	// 		pkt->req->setExtraData(0);
	// 		return true;
	// 	}

	return false;
}

void Cache::satisfyCpuSideRequest(const Command *currentCommand, BlkType *blk)
{
	BOOST_ASSERT(blk);
	// Occasionally this is not true... if we are a lower-level cache
	// satisfying a string of Read and ReadEx requests from
	// upper-level caches, a Read will mark the block as shared but we
	// can satisfy a following ReadEx anyway since we can rely on the
	// Read requester(s) to have buffered the ReadEx snoop and to
	// invalidate their blocks after receiving them.
	// assert(currentCommand->needsExclusive() ? blk->isWritable() : blk->isValid());

	//assert(currentCommand->getOffset(blkSize) + currentCommand->getSize() <= blkSize);

	// Check RMW operations first since both isRead() and
	// isWrite() will be true for them
	// 	if (currentCommand->cmd == MemCmd::SwapReq)
	// 	{
	// 		cmpAndSwap(blk, currentCommand);
	// 	}
	//} else if (pkt->isWrite()) {
	if (currentCommand->isWrite()) 
	{
		blk->status |= BlkDirty;
		// 		if (blk->checkWrite(currentCommand))
		// 		{
		// 			currentCommand->writeDataToBlock(blk->data, blkSize);
		// 		}
	}
	//} else if (pkt->isRead()) {
	else if (currentCommand->isRead())
	{
		// 		if (currentCommand->isLLSC()) 
		// 		{
		// 			blk->trackLoadLocked(currentCommand);
		// 		}
		// not actually moving data
		//currentCommand->setDataFromBlock(blk->data, blkSize);
		//BOOST_ASSERT(8 * currentCommand->getLength() == blkSize);
		if (8 * currentCommand->getLength() == blkSize)
		{
			// special handling for coherent block requests from
			// upper-level caches
			// 			if (currentCommand->needsExclusive()) 
			// 			{
			// 				// on ReadExReq we give up our copy
			// 				tags->invalidateBlk(blk);
			// 			} 
			// 			else
			{
				// on ReadReq we create shareable copies here and in
				// the requester
				//currentCommand->assertShared();
				blk->status &= ~BlkWritable;
			}
		}
	} 
	// 	else 
	// 	{
	// 		// Not a read or write... must be an upgrade.  it's OK
	// 		// to just ack those as long as we have an exclusive
	// 		// copy at this level.
	// 		assert(currentCommand->cmd == MemCmd::UpgradeReq);
	// 		invalidateBlk(blk);
	// 	}
}

bool Cache::timingAccess(const Command *currentCommand, tick time)
{
	int lat = 3; // hit latency
	BlkType *blk = NULL;
	PacketList writebacks;

	bool satisfied = access(currentCommand, lat, blk, time,  writebacks);

	if (writebacks.size() > 0)
	{
		cout << endl << writebacks.size() << endl;
	}

	if (!satisfied) 
	{
		hitsMisses.second++;
		cumulativeHitsMisses.second++;

		if (currentCommand->isRead())
		{
			readHitsMisses.second++;
			cumulativeReadHitsMisses.second++;
		}
		// MISS
		//PacketPtr bus_pkt = getBusPacket(pkt, blk, pkt->needsExclusive());

		//bool is_forward = (bus_pkt == NULL);

		// 		if (is_forward) {
		// 			// just forwarding the same request to the next level
		// 			// no local cache operation involved
		// 			bus_pkt = pkt;
		// 		}

		// 		DPRINTF(Cache, "Sending an atomic %s for %x\n",
		// 			bus_pkt->cmdString(), bus_pkt->getAddr());
		// 
		// #if TRACING_ON
		// 		CacheBlk::State old_state = blk ? blk->status : 0;
		// #endif

		//lat += memSidePort->sendAtomic(bus_pkt);

		// 		DPRINTF(Cache, "Receive response: %s for addr %x in state %i\n",
		// 			bus_pkt->cmdString(), bus_pkt->getAddr(), old_state);
		// 
		// 		assert(!bus_pkt->wasNacked());

		// If packet was a forward, the response (if any) is already
		// in place in the bus_pkt == pkt structure, so we don't need
		// to do anything.  Otherwise, use the separate bus_pkt to
		// generate response to pkt and then delete it.
		// 		if (pkt->needsResponse())
		// 		{
		// 			assert(bus_pkt->isResponse());
		// 			if (bus_pkt->isError()) 
		// 			{
		// 				pkt->makeAtomicResponse();
		// 				pkt->copyError(bus_pkt);
		// 			} 
		//else 
		// allocate a spot for the read
		if (currentCommand->isRead())
		{
			blk = handleFill(currentCommand,blk,time);
			satisfyCpuSideRequest(currentCommand,blk);
		}
		// 				if (bus_pkt->isRead() ||
		// 				bus_pkt->cmd == MemCmd::UpgradeResp) 
		// 			{
		// 					// we're updating cache state to allow us to
		// 					// satisfy the upstream request from the cache
		// 					blk = handleFill(bus_pkt, blk, writebacks);
		// 					satisfyCpuSideRequest(pkt, blk);
		// 			} 
		// 			else 
		// 			{
		// 				// we're satisfying the upstream request without
		// 				// modifying cache state, e.g., a write-through
		// 				pkt->makeAtomicResponse();
		// 			}
		//}
		//delete bus_pkt;

	}
	else
	{
		if (currentCommand->isRead())
		{
			cumulativeReadHitsMisses.first++;
			readHitsMisses.first++;
		}
		cumulativeHitsMisses.first++;
		hitsMisses.first++;
		bandwidth += currentCommand->getLength();
	}

	return satisfied;
}

Cache::BlkType *Cache::handleFill(const Command *currentCommand, BlkType *blk, tick time)
{
	Addr addr = currentCommand->getAddress().getPhysicalAddress();
	// #if TRACING_ON
	// 	CacheBlk::State old_state = blk ? blk->status : 0;
	//#endif

	if (blk == NULL) 
	{
		// better have read new data...
		//assert(currentCommand->hasData());
		// need to do a replacement
		PacketList writebacks;
		blk = allocateBlock(addr, writebacks);
		BOOST_ASSERT(blk != NULL);
		if (blk == NULL) {
			BOOST_ASSERT(false);
			// No replaceable block... just use temporary storage to
			// complete the current request and then get rid of it
			// 			assert(!tempBlock->isValid());
			// 			blk = tempBlock;
			// 			tempBlock->set = tags->extractSet(addr);
			// 			tempBlock->tag = tags->extractTag(addr);
			// 			DPRINTF(Cache, "using temp block for %x\n", addr);
		} 
		else
		{
			//int id = currentCommand->req->hasContextId() ? currentCommand->req->contextId() : -1;
			int id = 0;
			insertBlock(currentCommand->getAddress().getPhysicalAddress(), blk, id, time);
		}
	} 
	else
	{
		BOOST_ASSERT(false);
		// existing block... probably an upgrade
		assert(blk->tag == extractTag(addr));
		// either we're getting new data or the block should already be valid

		//assert(currentCommand->hasData() || blk->isValid());
	}

	//if (!currentCommand->sharedAsserted()) {
	blk->status = BlkValid | BlkReadable | BlkWritable;
	// 	} else {
	// 		assert(!currentCommand->needsExclusive());
	// 		blk->status = BlkValid | BlkReadable;
	// 	}

	// 	DPRINTF(Cache, "Block addr %x moving from state %i to %i\n",
	// 		addr, old_state, blk->status);

	// if we got new data, copy it in
	// 	if (currentCommand->isRead()) {
	// 		std::memcpy(blk->data, currentCommand->getPtr<uint8_t>(), blkSize);
	// 	}

	//blk->whenReady = currentCommand->finishTime;

	return blk;
}

Cache::BlkType *Cache::allocateBlock(const Addr &addr, PacketList &writebacks)
{
	//PacketList writebacks;

	BlkType *blk = findVictim(addr, writebacks);

	if (blk->isValid()) 
	{
		//Addr repl_addr = regenerateBlkAddr(blk->tag, blk->set);
		// 		MSHR *repl_mshr = mshrQueue.findMatch(repl_addr);
		// 		if (repl_mshr)
		// 		{
		// 			// must be an outstanding upgrade request on block
		// 			// we're about to replace...
		// 			assert(!blk->isWritable());
		// 			assert(repl_mshr->needsExclusive());
		// 			// too hard to replace block with transient state
		// 			// allocation failed, block not inserted
		// 			return NULL;
		// 		}
		// 		else
		{
			// 			DPRINTF(Cache, "replacement: replacing %x with %x: %s\n",
			// 				repl_addr, addr,
			// 				blk->isDirty() ? "writeback" : "clean");

			// writethrough cache at this time
			// 			if (blk->isDirty()) 
			// 			{
			// 				// Save writeback packet for handling by caller
			// 				writebacks.push_back(writebackBlk(blk));
			// 			}
		}
	}

	return blk;
}

LRUBlk *Cache::accessBlock(Addr addr, int &lat, int context_src, tick curTick)
{
	Addr tag = extractTag(addr);

	unsigned set = extractSet(addr);

	LRUBlk *blk = sets[set].findBlk(tag);

	lat = hitLatency;

	if (blk != NULL)
	{
		// move this block to head of the MRU list
		sets[set].moveToHead(blk);
		//         DPRINTF(CacheRepl, "set %x: moving blk %x to MRU\n",
		//                 set, regenerateBlkAddr(tag, set));
		if (blk->whenReady > curTick
			&& blk->whenReady - curTick > hitLatency) 
		{
			lat = blk->whenReady - curTick;
		}
		blk->refCount += 1;
	}

	return blk;
}


LRUBlk *Cache::findBlock(Addr addr) const
{
	Addr tag = extractTag(addr);
	unsigned set = extractSet(addr);
	LRUBlk *blk = sets[set].findBlk(tag);
	return blk;
}

LRUBlk *Cache::findVictim(Addr addr, PacketList &writebacks)
{
	unsigned set = extractSet(addr);

	LRUBlk *blk;

	switch (replacementPolicy)
	{
	case Cache::LRU:
		// grab a replacement candidate
		blk = sets[set].blks[assoc-1];

		break;

	case Cache::NMRU:
		{
			int index = rand() % (assoc - nmruCount);
			blk = sets[set].blks[nmruCount + index];
		}
		break;
	case Cache::MRU:
		blk = sets[set].blks[0];
		break;
	case Cache::LFU:
		{
			blk = sets[set].blks[0];
			int lowestRefCount = sets[set].blks[0]->refCount;
			int greatestRefCount = sets[set].blks[0]->refCount;
			for (unsigned i = 1; i < assoc; ++i)
			{
				LRUBlk *currentBlock = sets[set].blks[i];
				if (currentBlock->refCount < lowestRefCount)
				{
					lowestRefCount = currentBlock->refCount;
					blk = currentBlock;
				}
				if (currentBlock->refCount > greatestRefCount)
					greatestRefCount = currentBlock->refCount;
			}
			//cerr << greatestRefCount << endl;
		}

	case Cache::RAND:
		blk = sets[set].blks[rand() % assoc];
		break;

	default:
		cerr << "Not implemented yet" << endl;
		exit(-3);
		break;
	}

	if (blk->isValid()) 
	{
		replacements[0]++;
		totalRefs += blk->refCount;
		++sampledRefs;
		blk->refCount = 0;

		//         DPRINTF(CacheRepl, "set %x: selecting blk %x for replacement\n",
		//                 set, regenerateBlkAddr(blk->tag, set));
	}
	return blk;
}

void Cache::insertBlock(Addr addr, Cache::BlkType *blk, int context_src, tick curTick)
{
	if (!blk->isTouched) 
	{
		tagsInUse++;
		blk->isTouched = true;
		//if (!warmedUp && tagsInUse.value() >= warmupBound) {
		if (!warmedUp && tagsInUse >= warmupBound) 
		{
			warmedUp = true;
			warmupCycle = curTick;
		}
	}

	// Set tag for new block.  Caller is responsible for setting status.
	blk->tag = extractTag(addr);

	unsigned set = extractSet(addr);
	sets[set].moveToHead(blk);
}

// void
// LRU::invalidateBlk(LRU::BlkType *blk)
// {
//     if (blk) {
//         blk->status = 0;
//         blk->isTouched = false;
//         blk->clearLoadLocks();
//         tagsInUse--;
//     }
// }

void Cache::cleanupRefs()
{
	for (unsigned i = 0; i < numSets*assoc; ++i) {
		if (blks[i].isValid()) {
			totalRefs += blks[i].refCount;
			++sampledRefs;
		}
	}
}

Cache &Cache::operator =(const Cache& rhs)
{
	const_cast<unsigned&>(numSets) = rhs.numSets;
	const_cast<unsigned&>(blkSize) = rhs.blkSize;
	const_cast<unsigned&>(assoc) = rhs.assoc;
	const_cast<unsigned&>(hitLatency) = rhs.hitLatency;

	sets = rhs.sets;
	blks = rhs.blks;
	dataBlks = rhs.dataBlks;

	const_cast<int&>(setShift) = rhs.setShift;
	const_cast<int&>(tagShift) = rhs.tagShift;
	const_cast<unsigned&>(setMask) = rhs.setMask;
	const_cast<unsigned&>(blkMask) = rhs.blkMask;
	const_cast<bool&>(writeAllocate) = rhs.writeAllocate;
	const_cast<ReplacementPolicy&>(replacementPolicy) = rhs.replacementPolicy;
	const_cast<unsigned&>(nmruCount) = rhs.nmruCount;

	unsigned blkIndex = 0;       // index into blks array
	for (unsigned i = 0; i < numSets; ++i) 
	{ 
		// link in the data blocks
		for (unsigned j = 0; j < assoc; ++j) 
		{
			// locate next cache block
			LRUBlk *blk = &blks[blkIndex];
			blk->data = &dataBlks[blkSize*blkIndex];
			++blkIndex;

			blk->size = blkSize;
			sets[i].blks[j]=blk;
			blk->set = i;
		}
	}

	return *this;
}


LRUBlk *CacheSet::findBlk(Addr tag) const
{
	for (int i = 0; i < assoc; ++i) 
	{
		if (blks[i]->tag == tag && blks[i]->isValid()) 
		{
			return blks[i];
		}
	}
	return 0;
}


void CacheSet::moveToHead(LRUBlk *blk)
{
	// nothing to do if blk is already head
	if (blks[0] == blk)
		return;

	// write 'next' block into blks[i], moving up from MRU toward LRU
	// until we overwrite the block we moved to head.

	// start by setting up to write 'blk' into blks[0]
	int i = 0;
	LRUBlk *next = blk;

	do {
		assert(i < assoc);
		// swap blks[i] and next
		LRUBlk *tmp = blks[i];
		blks[i] = next;
		next = tmp;
		++i;
	} while (next != blk);
}


ostream &DRAMsimII::operator<<(ostream &os, const Cache::ReplacementPolicy type)
{
	switch (type)
	{
	case Cache::LRU:
		os << "LRU";
		break;
	case Cache::NMRU:
		os << "NMRU";
		break;
	case Cache::MRU:
		os << "MRU";
		break;
	case Cache::LFU:
		os << "LFU";
		break;
	case Cache::RAND:
		os << "RAND";
		break;
	case Cache::FIFO:
		os << "FIFO";
		break;
	default:
		os << "UNKWN";
		break;
	}
	return os;
}