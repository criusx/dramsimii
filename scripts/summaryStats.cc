#include "processStats.hh"
#include "globals.hh"
#include "statsScripts.hh"

void processStatsForPair(const pair<string, string> &filePair, map<string, list<string> > &results)
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

		current << std::dec << std::fixed << std::setprecision(2) << ((double) ssCache.getEpochCounter() * ssCache.getEpochTime());
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

		while (!currentLine.empty())
		{
			string element = currentLine.back();
#pragma omp critical
			results[basefilename].push_front(element);
			currentLine.pop_back();
		}
	}
}

void processPowerForPair(const pair<string, string> &filePair, map<string, list<string> > &results, list<pair<string, string> > &powerParams)
{

	bool found0, found1;
	unsigned epochCounter0 = 0, epochCounter1 = 0;
	pair<double, double> totalEnergy0, totalEnergy1;
	double epoch0 = 0.0, epoch1 = 0.0;
	double averageInUseTime0, averageInUseTime1;

	// with the cache
	found0 = processPowerForFile(filePair.first, powerParams, epochCounter0, epoch0, totalEnergy0, averageInUseTime0);
	// without the cache
	found1 = processPowerForFile(filePair.second, powerParams, epochCounter0, epoch0, totalEnergy1, averageInUseTime1);

	const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));
	cerr << basefilename << endl;

	if (found0 && found1)
	{
		stringstream current;
		current << std::dec << std::fixed << std::setprecision(2) << ((double) totalEnergy1.second);
#pragma omp critical 
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << ((double) totalEnergy0.first + totalEnergy0.second);
#pragma omp critical
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << ((double) (totalEnergy0.first + totalEnergy0.second) / totalEnergy1.second * 100);
#pragma omp critical
		results[basefilename].push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << ((double) averageInUseTime0 * 100);
#pragma omp critical
		results[basefilename].push_back(current.str());
	}
}

bool processPowerForFile(const string file, list<pair<string, string> > &powerParams, unsigned &epochCounter, double &epochTime, pair<double,double> &totalEnergy, double &averageInUseTime)
{
	bool foundCommandLine = false, foundEpoch = false;

	filtering_istream inputStream;

	if (ends_with(file, ".gz"))
		inputStream.push(boost::iostreams::gzip_decompressor());
	else if (ends_with(file, ".bz2"))
		inputStream.push(boost::iostreams::bzip2_decompressor());

	inputStream.push(file_source(file));
	
	PowerParameters<double> params;

	char newLine[NEWLINE_LENGTH];

	if (!inputStream.is_complete())
		return false;

	WeightedAverage<double> inUseTime;

	for (inputStream.getline(newLine, NEWLINE_LENGTH);
		(newLine[0] != NULL) && (!userStop);
		inputStream.getline(newLine, NEWLINE_LENGTH))
	{
		
		if (starts_with(newLine,"+ch"))
		{
			PowerCalculations pc = params.calculateSystemPower(newLine,epochTime);
			totalEnergy.first += pc.dimmEnergy;
			totalEnergy.second += pc.energy;
			inUseTime.add(pc.inUseTime,1);
			//cerr << pc.inUseTime << endl;
			//cerr << pc.energy << " " << pc.reducedEnergy << endl;
		}
		else if (!foundCommandLine && starts_with(newLine, "----Command Line:"))
		{
			foundCommandLine = true;
			params.setParameters(newLine, powerParams);
		}
		else if (!foundEpoch && starts_with(newLine, "----Epoch"))
		{
			foundEpoch = true;

			char *position = strchr(newLine, ' ');
			if (position == NULL)
				break;
			epochTime = atof(position + 1);
		}
	}

	averageInUseTime = inUseTime.average();

	return foundEpoch && foundCommandLine;
}