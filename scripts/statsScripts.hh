#ifndef STATSSCRIPTS_HH
#define STATSSCRIPTS_HH

#include "processStats.hh"
#include "globals.hh"

class StatsScripts
{
private:
	std::list<string> filesGenerated;

	bool foundEpoch;

	unsigned scaleIndex;
	unsigned scaleFactor;
	bool throughOnce;

	// vectors to keep track of various stats
	vector<vector<vector<vector<unsigned> > > > channelDistribution;
	vector<vector<vector<uint64_t> > > channelDistributionBuffer;
	vector<vector<vector<vector<unsigned> > > > channelLatencyDistribution;
	vector<vector<vector<uint64_t> > > channelLatencyDistributionBuffer;

	std::tr1::unordered_map<boost::uint64_t, pair<uint64_t, uint64_t> > latencyVsPcLow;
	std::tr1::unordered_map<boost::uint64_t, pair<uint64_t, uint64_t> > latencyVsPcHigh;

	vector<tuple<unsigned, unsigned, double, unsigned> > transactionLatency;
	
	vector<unsigned> transactionCount;

	uint64_t transactionCountBuffer;

	unordered_map<unsigned, unsigned> distTransactionLatency;

	vector<float> hitMissValues;

	double hitMissValueBuffer;

	vector<unsigned> hitMissTotals;

	unsigned hitMissTotalBuffer;

	vector<unsigned> iCacheHits;

	uint64_t iCacheHitBuffer;

	vector<unsigned> iCacheMisses;

	unsigned iCacheMissBuffer;

	vector<long> iCacheMissLatency;

	uint64_t iCacheMissLatencyBuffer;

	vector<unsigned> dCacheHits;

	uint64_t dCacheHitBuffer;

	vector<unsigned> dCacheMisses;

	uint64_t dCacheMissBuffer;

	vector<long> dCacheMissLatency;

	uint64_t dCacheMissLatencyBuffer;

	vector<unsigned> l2CacheHits;

	uint64_t l2CacheHitBuffer;

	vector<unsigned> l2CacheMisses;

	uint64_t l2CacheMissBuffer;

	vector<long> l2CacheMissLatency;

	uint64_t l2CacheMissLatencyBuffer;

	vector<unsigned> l2MshrHits;

	uint64_t l2MshrHitBuffer;

	vector<unsigned> l2MshrMisses;

	uint64_t l2MshrMissBuffer;

	vector<long> l2MshrMissLatency;

	uint64_t l2MshrMissLatencyBuffer;

	vector<pair<uint64_t, uint64_t> > bandwidthValues;

	pair<uint64_t, uint64_t> bandwidthValuesBuffer;

	vector<pair<uint64_t, uint64_t> > cacheBandwidthValues;

	pair<uint64_t, uint64_t> cacheBandwidthValuesBuffer;

	vector<pair<unsigned, unsigned> > cacheHitMiss;

	pair<unsigned, unsigned> cacheHitMissBuffer;

	// DIMM cache stuff
	pair<unsigned, unsigned> readHitsMisses;
	pair<unsigned, unsigned> hitsMisses;

	vector<float> ipcValues;

	double ipcValueBuffer;

	vector<unsigned> workingSetSize;

	uint64_t workingSetSizeBuffer;

	bool foundCommandline;
	unsigned ipcLinesWritten;
	double epochTime;
	double periodInNs;

	unsigned tRC;
	unsigned tRAS;

	unsigned channelCount;
	unsigned rankCount;
	unsigned bankCount;
	unsigned epochCounter;
	double totalLatency;
	unsigned totalCount;
	vector<string> commandLine;
	string rawCommandLine;

	bf::path givenfilename;

	bool usingCache;

public:


	pair<unsigned, unsigned> getReadHitsMisses() const { return readHitsMisses; }
	pair<unsigned, unsigned> getHitsMisses() const { return hitsMisses; }
	unsigned getEpochCounter() const { return epochCounter; }
	unsigned getEpochTime() const { return epochTime; }
	double getAverageLatency() const { return totalLatency / (double)totalCount; }
	double getTotalLatency() const { return totalLatency; }
	unsigned getTotalCount() const { return totalCount; }
	string getRawCommandLine() const { return rawCommandLine; }
	double getRunTime() const { return (double)epochCounter * epochTime; }
	bool isUsingCache() const { return usingCache; }

public:
	StatsScripts():
	  foundEpoch(false),
		  scaleIndex(0),
		  scaleFactor(1),
		  throughOnce(false),
		  transactionCountBuffer(0ULL),
		  hitMissValueBuffer(0.0),
		  hitMissTotalBuffer(0U),
		  iCacheHitBuffer(0ULL),
		  iCacheMissBuffer(0U),
		  iCacheMissLatencyBuffer(0ULL),
		  dCacheHitBuffer(0ULL),
		  dCacheMissBuffer(0ULL),
		  dCacheMissLatencyBuffer(0ULL),
		  l2CacheHitBuffer(0ULL),
		  l2CacheMissBuffer(0ULL),
		  l2CacheMissLatencyBuffer(0ULL),
		  l2MshrHitBuffer(0ULL),
		  l2MshrMissBuffer(0ULL),
		  l2MshrMissLatencyBuffer(0ULL),
		  bandwidthValuesBuffer(0ULL, 0ULL),
		  cacheBandwidthValuesBuffer(0ULL, 0ULL),
		  cacheHitMissBuffer(0, 0),
		  ipcValueBuffer(0.0),
		  foundCommandline(false),
		  epochTime(0.0F),
		  periodInNs(0.0F),
		  tRC(0),
		  tRAS(0),
		  channelCount(0),
		  rankCount(0),
		  bankCount(0),
		  epochCounter(0),
		  totalLatency(0.0),
		  totalCount(0.0),
		  usingCache(false)
	  {
		  transactionCount.reserve(MAXIMUM_VECTOR_SIZE);
		  hitMissValues.reserve(MAXIMUM_VECTOR_SIZE);
		  hitMissTotals.reserve(MAXIMUM_VECTOR_SIZE);
		  iCacheHits.reserve(MAXIMUM_VECTOR_SIZE);
		  iCacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
		  iCacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
		  dCacheHits.reserve(MAXIMUM_VECTOR_SIZE);
		  dCacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
		  dCacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
		  l2CacheHits.reserve(MAXIMUM_VECTOR_SIZE);
		  l2CacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
		  l2CacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
		  l2MshrHits.reserve(MAXIMUM_VECTOR_SIZE);
		  l2MshrMisses.reserve(MAXIMUM_VECTOR_SIZE);
		  l2MshrMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
		  bandwidthValues.reserve(MAXIMUM_VECTOR_SIZE);
		  cacheBandwidthValues.reserve(MAXIMUM_VECTOR_SIZE);
		  cacheHitMiss.reserve(MAXIMUM_VECTOR_SIZE);
		  ipcValues.reserve(MAXIMUM_VECTOR_SIZE);
		  workingSetSize.reserve(MAXIMUM_VECTOR_SIZE);
	  }

	  void generateGraphs(const bf::path &outputDir);

	  void generateJointGraphs(const bf::path &outputDir, const StatsScripts &alternateStats);

	  bool working()
	  {
		  return foundCommandline && foundEpoch;
	  }

	  bool processStatsForFile(const string &file);

	  const vector<pair<unsigned, unsigned> > &getCacheHitMiss() const { return cacheHitMiss; }

protected:

	void processLine(char *newLine);

	void pushStats();

	void compressStats();

	void addressLatencyDistributionPerChannelGraph(const bf::path &outFilename, opstream &p, unsigned channelID, bool isThumbnail);

	void addressDistributionPerChannelGraph(const bf::path &outFilename, opstream &p, unsigned channelID, bool isThumbnail);

	void overallAddressDistributionGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void pcVsLatencyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void pcVsAverageLatencyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void transactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void zoomedTransactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void bandwidthGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void cacheGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void averageIpcAndLatencyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void hitMissGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void workingSetGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void bigIpcGraph(const bf::path &outFilename, opstream &p, bool isThumbnail);

	void cacheHitMissGraph(const bf::path &outFilename, opstream &p, const vector<pair<unsigned,unsigned> > &alternateCacheHitMiss, bool isThumbnail);

	void transactionLatencyCumulativeDistributionGraph(const bf::path &outFilename, opstream &p, const char* title,bool isThumbnail);
};

#endif