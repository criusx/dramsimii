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
#if 0
		//string currentUrlString = ireplace_all_copy(urlString,"%1",basefilename);
		string modUrlString = ssCache.getRawCommandLine().substr(ssCache.getRawCommandLine().find(':') + 2, ssCache.getRawCommandLine().length());
		vector<string> splitLine;
		erase_all(modUrlString, "_");
		erase_all(modUrlString, "{");
		erase_all(modUrlString, "}");
		replace_first(modUrlString,"  ]","]");
		split(splitLine, modUrlString, is_any_of(" "), token_compress_on);

		for (vector<string>::const_iterator x = splitLine.begin(); x
			!= splitLine.end(); ++x)
		{
			string::size_type start = x->find("[");
			string::size_type end = x->find("]");
			string benchmarkName;

			if (start == string::npos || end == string::npos)
				benchmarkName = *x;
			else
				benchmarkName = x->substr(start + 1, end - start - 1);

			if (decoder.find(benchmarkName) != decoder.end())
				benchmarkName = decoder[benchmarkName];

			currentLine.push_back(benchmarkName);
		}

		stringstream current;

		current << std::dec << std::fixed << std::setprecision(2) << ssNoCache.getRunTime();
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << ((double) ssNoCache.getReadHitsMisses().first
			/ ((double) ssNoCache.getReadHitsMisses().first + ssNoCache.getReadHitsMisses().second));
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< ((float) ssNoCache.getHitsMisses().first / ((float) ssNoCache.getHitsMisses().first
			+ ssNoCache.getHitsMisses().second));
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< ssNoCache.getAverageLatency();
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< ssCache.getAverageLatency();
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< ssNoCache.getAverageLatency() - ssCache.getAverageLatency();
		currentLine.push_back(current.str());
		current.str("");
#endif
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


#if 0
		while (!currentLine.empty())
		{
			string element = currentLine.back();
#pragma omp critical
			results[basefilename].push_front(element);
			currentLine.pop_back();
		}
#endif

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

#if 0
		stringstream current;
		current << std::dec << std::fixed << std::setprecision(2) << energyNormal;
#pragma omp critical 
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << energyCache;
#pragma omp critical
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << energyCache / energyNormal * 100.0;
#pragma omp critical
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << psNoCache.getRunTime();
#pragma omp critical
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << psCache.getRunTime();
#pragma omp critical
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << ((double) psCache.getAverageInUseTime() * 100);
#pragma omp critical
		results[basefilename].push_back(current.str());
#endif
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
