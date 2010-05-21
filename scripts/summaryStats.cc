#include "processStats.hh"
#include "globals.hh"
#include "statsScripts.hh"
#include "powerScripts.hh"
#include "resultSet.hh"

void processStatsForPair(const pair<string, string> &filePair, map<string, ResultSet > &results, path &outputDir, const bool generateResultsOnly)
{
	const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));
	cerr << basefilename << endl;

	StatsScripts ssCache, ssNoCache;

	bool found0, found1;

	// with the DIMM cache
	found0 = ssCache.processStatsForFile(filePair.first);
	// without the DIMM cache
	found1 = ssNoCache.processStatsForFile(filePair.second);
	if (found0 && found1)
	{
		vector<string> currentLine;

		/// @TODO process both of these?
		//toBeProcessed.push_back(filePair->first);

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


#pragma omp critical
		results[basefilename].setStats(rs, true);

		if (!generateResultsOnly)
		{
			try
			{
				string basename = regexMatch<string>(filePair.first.c_str(), "(.*)-(stats|power).*");
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

void processPowerForPair(const pair<string, string> &filePair, map<string, ResultSet > &results, list<pair<string, string> > &powerParams, path &outputDir, const bool generateResultsOnly)
{
	bool found0, found1;
	PowerScripts psCache(powerParams), psNoCache(powerParams);
	
	// with the cache
	found0 = psCache.processStatsForFile(filePair.first);
	// without the cache
	found1 = psNoCache.processStatsForFile(filePair.second);

	const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));
	cerr << basefilename << endl;

	if (found0 && found1)
	{		
		double energyCache = psCache.getTotalEnergy().first + psCache.getTotalEnergy().second;
		double energyNormal = psNoCache.getTotalEnergy().second;

		ResultSet rs;
		rs.parseCommandLine(psNoCache.getRawCommandLine().c_str(), filePair.first);
		rs.energyUsed = energyNormal;
		rs.energyUsedTheoretical = energyCache;
		rs.percentCacheTimeInUse = ((double) psCache.getAverageInUseTime() * 100);
		rs.noCacheRuntime = psNoCache.getRunTime();
		rs.cacheRuntime = psCache.getRunTime();

#pragma omp critical
		results[basefilename].setStats(rs, false);

		//cerr << "c " << psCache.getRunTime() << " nc " << psNoCache.getRunTime() << endl;
		if (!generateResultsOnly)
		{
			try
			{
				string basename = regexMatch<string>(filePair.first.c_str(), "(.*)-(stats|power).*");

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
