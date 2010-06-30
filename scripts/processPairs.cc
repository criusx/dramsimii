#include "processStats.hh"
#include "globals.hh"
#include "statsScripts.hh"
#include "powerScripts.hh"
#include "resultSet.hh"

void processStatsForPair(const pair<string, string> &filePair, map<string, ResultSet > &results, path &outputDir, const bool generateResultsOnly)
{
	StatsScripts ssCache, ssNoCache;
	
	// with the DIMM cache
	bool found0 = ssCache.processStatsForFile(filePair.first);
	// without the DIMM cache
	bool found1 = ssNoCache.processStatsForFile(filePair.second);

	if (found0 && found1)
	{
		ResultSet rs;
		rs.parseCommandLine(ssCache.getRawCommandLine().c_str(), filePair.first);

		rs.runtime = ssCache.getRunTime();
		rs.noCacheRuntime = ssNoCache.getRunTime();
		rs.cacheRuntime = ssCache.getRunTime();
		rs.readHitRate = ((double) ssCache.getReadHitsMisses().first
			/ ((double) ssCache.getReadHitsMisses().first + ssCache.getReadHitsMisses().second));
		rs.hitRate = ((double) ssCache.getReadHitsMisses().first
			/ ((double) ssCache.getReadHitsMisses().first + ssCache.getReadHitsMisses().second));
		rs.averageLatency = ssNoCache.getAverageLatency();
		rs.averageTheoreticalLatency = ssCache.getAverageLatency();
		rs.reuseRate = ssNoCache.getReuseRate();
		rs.averageBandwidth = ssNoCache.getAverageBandwidth();
		rs.averageIpc = ssCache.getAverageIpcValue();
		rs.withCacheLatency = ssCache.getTotalLatency();
		rs.withoutCacheLatency = ssNoCache.getTotalLatency();
		rs.withCacheRequestCount = ssCache.getTotalCount();
		rs.withoutCacheRequestCount = ssNoCache.getTotalCount();

		const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));

#pragma omp critical
		results[basefilename].setStats(rs, true);

		if (!generateResultsOnly)
		{
			try
			{
				string basename = regexMatch<string>(filePair.first.c_str(), "(.*)-(stats|power)(C|N)?.*");
				//cerr << "bn: " << basename << " od: " << outputDir << endl;;
				ssNoCache.generateGraphs(outputDir / basename);

				ssNoCache.generateJointGraphs(outputDir / basename, ssCache);
			}
			catch (std::exception e)
			{
				return;
			}
		}		
	}
}

void processStats(const pair<string, string> &filePair, map<string, ResultSet > &results, path &outputDir, const bool generateResultsOnly)
{
	StatsScripts statScript;
	
	// without the DIMM cache
	bool found1 = statScript.processStatsForFile(filePair.second);

	if (found1)
	{
		ResultSet rs;
		rs.parseCommandLine(statScript.getRawCommandLine().c_str(), filePair.second);

		rs.runtime = statScript.getRunTime();
		rs.noCacheRuntime = statScript.getRunTime();
		rs.cacheRuntime = 0;
		rs.readHitRate = 0;
		rs.hitRate = 0;
		rs.averageLatency = statScript.getAverageLatency();
		rs.averageTheoreticalLatency = 0;
		rs.withCacheLatency = 0;
		rs.withoutCacheLatency = statScript.getTotalLatency();
		rs.withCacheRequestCount = 0;
		rs.withoutCacheRequestCount = statScript.getTotalCount();
		rs.reuseRate = statScript.getReuseRate();
		rs.averageIpc = statScript.getAverageIpcValue();
		rs.averageBandwidth = statScript.getAverageBandwidth();

		const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));

#pragma omp critical
		results[basefilename].setStats(rs, true);

		if (!generateResultsOnly)
		{
			try
			{
				string basename = regexMatch<string>(filePair.second.c_str(), "(.*)-(stats|power)(C|N)?.*");
				statScript.generateGraphs(outputDir / basename);
			}
			catch (std::exception e)
			{
				return;
			}
		}		
	}
}

void processPower(const pair<string, string> &filePair, map<string, ResultSet > &results, list<pair<string, string> > &powerParams, path &outputDir, const bool generateResultsOnly)
{
	PowerScripts psCache(powerParams), psNoCache(powerParams);
	
	// without the cache
	bool found1 = psNoCache.processStatsForFile(filePair.second);

	if (found1)
	{		
		bool problem2 = psNoCache.isUsingCache();

		if (problem2)
		{
			cerr << "cache where there should not be " << filePair.second << endl;
			exit(-4);
		}

		double energyNormal = psNoCache.getTotalEnergy().first + psNoCache.getTotalEnergy().second;
		//cerr << "??? " << psNoCache.getTotalEnergy().first << endl;

		//cerr << "normalA " << psNoCache.getTotalEnergy().first + psNoCache.getTotalEnergy().second << endl << "altA " << psCache.getTotalEnergy().first + psCache.getTotalEnergy().second << endl;
		ResultSet rs;
		rs.parseCommandLine(psNoCache.getRawCommandLine().c_str(), filePair.first);
		rs.energyUsed = energyNormal;
		rs.energyUsedTheoretical = 0;
		rs.percentCacheTimeInUse = 0;
		rs.noCacheRuntime = psNoCache.getRunTime();
		rs.cacheRuntime = 0;
		rs.averageActStbyPower = psNoCache.getAverageActStbyPower();

		const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));

#pragma omp critical
		results[basefilename].setStats(rs, false);

#pragma omp critical
		results[basefilename].setPowerParameters(psCache.getPowerParameters());

		if (!generateResultsOnly)
		{
			try
			{
				string basename = regexMatch<string>(filePair.first.c_str(), "(.*)-(stats|power)(C|N)?.*");

				psNoCache.generateGraphs(outputDir / basename);
			}
			catch (std::exception e)
			{
				return;
			}
		}
	}
}

void processPowerForPair(const pair<string, string> &filePair, map<string, ResultSet > &results, list<pair<string, string> > &powerParams, path &outputDir, const bool generateResultsOnly)
{
	PowerScripts psCache(powerParams), psNoCache(powerParams);
	
	// with the cache
	bool found0 = psCache.processStatsForFile(filePair.first);
	// without the cache
	bool found1 = psNoCache.processStatsForFile(filePair.second);

	if (found0 && found1)
	{		
		bool problem = !psCache.isUsingCache();
		bool problem2 = psNoCache.isUsingCache();

		if (problem)
			cerr << "no cache where there should be " << filePair.first << endl;
		if (problem2)
			cerr << "cache where there should not be " << filePair.second << endl;

		if (problem2 || problem)
			exit(-4);

		double energyCache = psCache.getTotalEnergy().first + psCache.getTotalEnergy().second;
		double energyNormal = psNoCache.getTotalEnergy().first + psNoCache.getTotalEnergy().second;
		//cerr << "??? " << psNoCache.getTotalEnergy().first << endl;

		//cerr << "normalA " << psNoCache.getTotalEnergy().first + psNoCache.getTotalEnergy().second << endl << "altA " << psCache.getTotalEnergy().first + psCache.getTotalEnergy().second << endl;
		ResultSet rs;
		rs.parseCommandLine(psNoCache.getRawCommandLine().c_str(), filePair.first);
		rs.energyUsed = energyNormal;
		rs.energyUsedTheoretical = energyCache;
		rs.percentCacheTimeInUse = ((double) psCache.getAverageInUseTime() * 100);
		rs.noCacheRuntime = psNoCache.getRunTime();
		rs.cacheRuntime = psCache.getRunTime();
		rs.averageActStbyPower = psNoCache.getAverageActStbyPower();

		const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));

#pragma omp critical
		results[basefilename].setStats(rs, false);

#pragma omp critical
		results[basefilename].setPowerParameters(psCache.getPowerParameters());

		//cerr << "c " << psCache.getRunTime() << " nc " << psNoCache.getRunTime() << endl;
		if (!generateResultsOnly)
		{
			try
			{
				string basename = regexMatch<string>(filePair.second.c_str(), "(.*)-(stats|power)(C|N)?.*");

				psNoCache.generateGraphs(outputDir / basename);

				psNoCache.generateJointGraphs(outputDir / basename, psCache);
			}
			catch (std::exception e)
			{
				return;
			}
		}
	}
}
