#ifndef RESULTSET_HH
#define RESULTSET_HH
#include <string>

class ResultSet
{
	const static std::string urlString;
public:
	unsigned channels;
	unsigned dimms;
	unsigned ranks;
	unsigned banks;
	unsigned rows;
	unsigned columns;
	unsigned width;
	unsigned tRas;
	unsigned tCas;
	unsigned tRcd;
	unsigned tRc;
	bool postedCas;
	std::string addressMappingPolicy;
	std::string commandOrderingAlgorithm;
	std::string rowBufferManagementPolicy;
	std::string datarate;
	unsigned perBankQueueDepth;
	unsigned tFaw;
	unsigned cacheSize;
	unsigned blockSize;
	unsigned associativity;
	unsigned numberOfSets;
	std::string replacementPolicy;
	std::string title;
	std::string basename;
	double runtime;
	double readHitRate;
	double hitRate;
	double averageLatency;
	double averageTheoreticalLatency;
	double energyUsed;
	double energyUsedTheoretical;
	double noCacheRuntime;
	double cacheRuntime;
	double percentCacheTimeInUse;

	ResultSet():
	channels(0),
		dimms(0),
		ranks(0),
		banks(0),
		rows(0),
		columns(0),
		width(0),
		tRas(0),
		tCas(0),
		tRcd(0),
		tRc(0),
		postedCas(false),
		perBankQueueDepth(0),
		tFaw(0),
		cacheSize(0),
		blockSize(0),
		associativity(0),
		numberOfSets(0),
		runtime(0.0),
		readHitRate(0.0),
		hitRate(0.0),
		averageLatency(0.0),
		averageTheoreticalLatency(0.0),
		energyUsed(0.0),
		energyUsedTheoretical(0.0),
		noCacheRuntime(0.0),
		cacheRuntime(0.0),
		percentCacheTimeInUse(0.0)
	{}

	void parseCommandLine(const char *commandLine, const std::string &filename);

	std::pair<std::string,std::string> generateResultLine() const ;

	std::string generateTd(const std::string &str) const;

	std::string generateTd(const unsigned value) const;

	std::string generateTd(const double value) const;

	std::string generateTd(const char *value) const;

	void setStats(const ResultSet &rs, const bool isStats);

	double getEnergyReduction() const { return energyUsedTheoretical / energyUsed; } 

	double getEnergyDifference() const { return energyUsed - energyUsedTheoretical; }

	double getLatencyReduction() const { return averageTheoreticalLatency / averageLatency; }
	
};
#endif