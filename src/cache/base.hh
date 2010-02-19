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
*          Ron Dreslinski
*/

/**
* @file
* Declaration of a common base class for cache tagstore objects.
*/

#ifndef __BASE_HH__
#define __BASE_HH__

#include <string>
#include <boost/cstdint.hpp>
#include <map>

using boost::int64_t;

namespace DRAMsimII
{

	class BaseCache;

	/**
	* A common base class of Cache tagstore objects.
	*/
	class BaseTags
	{
	protected:
		/** Pointer to the parent cache. */
		BaseCache *cache;

		/** Local copy of the parent cache name. Used for DPRINTF. */
		std::string objName;

		/**
		* The number of tags that need to be touched to meet the warmup
		* percentage.
		*/
		int warmupBound;
		/** Marked true when the cache is warmed up. */
		bool warmedUp;

		// Statistics
		/**
		* @addtogroup CacheStatistics
		* @{
		*/

		/** Number of replacements of valid blocks per thread. */
		//Stats::Vector replacements;
		std::map<int,int> replacements;
		/** Per cycle average of the number of tags that hold valid data. */
		int64_t tagsInUse;
		//Stats::Average tagsInUse;

		/** The total number of references to a block before it is replaced. */
		int64_t totalRefs;
		//Stats::Scalar totalRefs;

		/**
		* The number of reference counts sampled. This is different from
		* replacements because we sample all the valid blocks when the simulator
		* exits.
		*/
		int64_t sampledRefs;
		//Stats::Scalar sampledRefs;

		/**
		* Average number of references to a block before is was replaced.
		* @todo This should change to an average stat once we have them.
		*/
		//Stats::Formula avgRefs;

		/** The cycle that the warmup percentage was hit. */
		//Stats::Scalar warmupCycle;
		int64_t warmupCycle;
		/**
		* @}
		*/

	public:


		BaseTags():
		cache(NULL),
			objName(""),
			warmupBound(500),
			warmedUp(false),
			tagsInUse(0),
			totalRefs(0),
			sampledRefs(0),
			warmupCycle(5000)
		{}

		/**
		* Destructor.
		*/
		virtual ~BaseTags() {}

		/**
		* Set the parent cache back pointer. Also copies the cache name to
		* objName.
		* @param _cache Pointer to parent cache.
		*/
		void setCache(BaseCache *_cache);

		/**
		* Return the parent cache name.
		* @return the parent cache name.
		*/
		const std::string &name() const
		{
			return objName;
		}

		/**
		* Register local statistics.
		* @param name The name to preceed each statistic name.
		*/
		void regStats(const std::string &name);

		/**
		* Average in the reference count for valid blocks when the simulation
		* exits.
		*/
		virtual void cleanupRefs() {}
	};

	// class BaseTagsCallback : public Callback
	// {
	//     BaseTags *tags;
	//   public:
	//     BaseTagsCallback(BaseTags *t) : tags(t) {}
	//     virtual void process() { tags->cleanupRefs(); };
	// };

}
#endif //__BASE_HH__
