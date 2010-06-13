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
* Declaration of a LRU tag store.
*/

#ifndef CACHE_HH
#define CACHE_HH

#include <list>
#include <vector>

#include "blk.hh"
#include "base.hh"
#include "../globals.hh"
#include "../command.hh"

namespace DRAMsimII
{
	class Packet;

	typedef std::list<Packet*> PacketList;

	class BaseCache;

	/**
	* LRU cache block.
	*/
	class LRUBlk : public CacheBlk
	{
	public:
		/** Has this block been touched? Used to aid calculation of warmup time. */
		bool isTouched;

		LRUBlk():
		isTouched(false)
		{}
	};

	/**
	* An associative set of cache blocks.
	*/
	class CacheSet
	{
		typedef DRAMsimII::PhysicalAddress Addr;

	public:
		/** The associativity of this set. */
		int assoc;

		/** Cache blocks in this set, maintained in LRU order 0 = MRU. */
		//LRUBlk **blks;
		std::vector<LRUBlk *> blks;

		CacheSet(unsigned associativity):
		assoc(associativity),
			blks(associativity)
		{
			//assoc = associativity;

			// 			for (unsigned j = 0; j < associativity; ++j)
			// 			{
			// 
			// 			}
		}

		/**
		* Find a block matching the tag in this set.
		* @param asid The address space ID.
		* @param tag The Tag to find.
		* @return Pointer to the block if found.
		*/
		LRUBlk* findBlk(Addr tag) const;

		/**
		* Move the given block to the head of the list.
		* @param blk The block to move.
		*/
		void moveToHead(LRUBlk *blk);
	};

	/**
	* A LRU cache tag store.
	*/
	class Cache : public BaseTags
	{
	public:

		enum ReplacementPolicy
		{
			LRU,
			NMRU,
			MRU,
			LFU,
			RAND,
			FIFO
		};

		typedef DRAMsimII::PhysicalAddress Addr;
		//typedef boost::uint8_t uint8_t;
		typedef DRAMsimII::tick tick;
		typedef DRAMsimII::Command Command;


		/** Typedef the block type used in this tag store. */
		typedef LRUBlk BlkType;
		/** Typedef for a list of pointers to the local block class. */
		typedef std::list<LRUBlk*> BlkList;

	protected:

		const unsigned numSets;					///< the number of sets
		const unsigned blkSize;					///< the block size of the cache
		const unsigned assoc;					///< the associativity of the cache
		const unsigned hitLatency;				///< the hit latency

		std::vector<CacheSet> sets;				///< The cache sets.
		std::vector<LRUBlk> blks;				///< The cache blocks.
		//std::vector<uint8_t> dataBlks;			///< The data blocks, 1 per cache block.

		const int setShift;						///< The amount to shift the address to get the set.
		const int tagShift;						///< The amount to shift the address to get the tag.
		const unsigned setMask;					///< Mask out all bits that aren't part of the set index.
		const unsigned blkMask;					///< Mask out all bits that aren't part of the block offset.
		const bool writeAllocate;				///< Allocate a block for writes upon a write miss
		const unsigned nmruCount;				///< the number of sets tracked when using the NMRU policy
		ReplacementPolicy replacementPolicy;	///< the policy that dictates what cache block is evicted when necessary

		// statistics
		std::pair<unsigned,unsigned> hitsMisses;	///< the number of hits and misses since the last reset to this cache
		std::pair<unsigned,unsigned> readHitsMisses;	///< the number of read hits and misses since the last reset to this cache
		std::pair<unsigned,unsigned> cumulativeHitsMisses; ///< hits and misses since the cache was created
		std::pair<unsigned,unsigned> cumulativeReadHitsMisses; ///< read hits and misses since the cache was created
		unsigned bandwidth;						///< bandwidth since the last reset to this cache


	public:
		/**
		* Construct and initialize this tag store.
		* @param _numSets The number of sets in the cache.
		* @param _blkSize The number of bytes in a block.
		* @param _assoc The associativity of the cache.
		* @param _hit_latency The latency in cycles for a hit.
		*/
		Cache(unsigned _numSets, unsigned _blkSize, unsigned _assoc,
			unsigned _hit_latency, unsigned _nmruCount, ReplacementPolicy _replacementPolicy);

		Cache(const Cache&);

		Cache(const DRAMsimII::Settings &);

		/**
		* Destructor
		*/
		virtual ~Cache();

		bool access(const Command *currentCommand, int &lat, BlkType *&blk, tick, PacketList &);

		void satisfyCpuSideRequest(const Command *, BlkType *blk);

		bool timingAccess(const Command *, tick);

		bool isHit(const Command *) const;

		Addr blockAlign(Addr addr) const { return (addr & ~(Addr(blkSize - 1))); }

		BlkType *handleFill(const Command *currentCommand, BlkType *blk, tick time);

		BlkType *allocateBlock(const Addr &addr, PacketList &);
		
		std::pair<unsigned,unsigned> getHitsMisses() const { return hitsMisses; }
		std::pair<unsigned,unsigned> getReadHitsMisses() const { return readHitsMisses; }
		std::pair<unsigned,unsigned> getCumulativeHitsMisses() const { return cumulativeHitsMisses; }
		std::pair<unsigned,unsigned> getCumulativeReadHitsMisses() const { return cumulativeReadHitsMisses; }
		unsigned getReadHits() const { return readHitsMisses.first; }
		unsigned getWriteHits() const { return hitsMisses.first - readHitsMisses.first; }
		unsigned getReadMisses() const { return readHitsMisses.second; }
		unsigned getWriteMisses() const { return hitsMisses.second - readHitsMisses.second; }

		void resetStats()
		{
			hitsMisses.first = hitsMisses.second = bandwidth = 
				readHitsMisses.first = readHitsMisses.second = 0U;
		}

		/**
		* Return the block size.
		* @return the block size.
		*/
		unsigned getBlockSize() const
		{
			return blkSize;
		}

		/**
		* Return the subblock size. In the case of LRU it is always the block
		* size.
		* @return The block size.
		*/
		unsigned getSubBlockSize() const
		{
			return blkSize;
		}

		/**
		* Invalidate the given block.
		* @param blk The block to invalidate.
		*/
		void invalidateBlk(BlkType *blk);

		/**
		* Access block and update replacement data.  May not succeed, in which case
		* NULL pointer is returned.  This has all the implications of a cache
		* access and should only be used as such. Returns the access latency as a side effect.
		* @param addr The address to find.
		* @param asid The address space ID.
		* @param lat The access latency.
		* @return Pointer to the cache block if found.
		*/
		LRUBlk* accessBlock(const Addr addr, int &lat, int context_src, tick curTick);

		LRUBlk* accessBlock(const Addr addr) const;

		/**
		* Finds the given address in the cache, do not update replacement data.
		* i.e. This is a no-side-effect find of a block.
		* @param addr The address to find.
		* @param asid The address space ID.
		* @return Pointer to the cache block if found.
		*/
		LRUBlk* findBlock(Addr addr) const;

		/**
		* Find a block to evict for the address provided.
		* @param addr The addr to a find a replacement candidate for.
		* @param writebacks List for any writebacks to be performed.
		* @return The candidate block.
		*/
		LRUBlk* findVictim(Addr addr, PacketList &writebacks);

		/**
		* Insert the new block into the cache.  For LRU this means inserting into
		* the MRU position of the set.
		* @param addr The address to update.
		* @param blk The block to update.
		*/
		void insertBlock(Addr addr, BlkType *blk, int context_src, tick curTick);

		/**
		* Generate the tag from the given address.
		* @param addr The address to get the tag from.
		* @return The tag of the address.
		*/
		inline Addr extractTag(Addr addr) const
		{
			return (addr >> tagShift);
		}

		/**
		* Calculate the set index from the address.
		* @param addr The address to get the set from.
		* @return The set index of the address.
		*/
		inline Addr extractSet(Addr addr) const
		{
			return ((addr >> setShift) & setMask);
		}

		/**
		* Get the block offset from an address.
		* @param addr The address to get the offset of.
		* @return The block offset.
		*/
		inline Addr extractBlkOffset(Addr addr) const
		{
			return (addr & blkMask);
		}

		/**
		* Align an address to the block size.
		* @param addr the address to align.
		* @return The block address.
		*/
		Addr blkAlign(Addr addr) const
		{
			return (addr & ~(Addr)blkMask);
		}

		/**
		* Regenerate the block address from the tag.
		* @param tag The tag of the block.
		* @param set The set of the block.
		* @return The block address.
		*/
		Addr regenerateBlkAddr(Addr tag, unsigned set) const
		{
			return ((tag << tagShift) | ((Addr)set << setShift));
		}

		/**
		* Return the hit latency.
		* @return the hit latency.
		*/
		int getHitLatency() const
		{
			return hitLatency;
		}

		/**
		* Called at end of simulation to complete average block reference stats.
		*/
		virtual void cleanupRefs();

		Cache &operator=(const Cache& rhs);		
	};

	std::ostream& operator<<(std::ostream&, const DRAMsimII::Cache::ReplacementPolicy);
}
#endif // __CACHE_HH__
