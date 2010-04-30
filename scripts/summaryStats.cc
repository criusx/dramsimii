#include "processStats.hh"
#include "globals.hh"

void processStatsForPair(const pair<string, string> &filePair, map<string, list<string> > &results)
{
	const string basefilename = filePair.first.substr(0, filePair.first.find_last_of('-'));
	cerr << basefilename << endl;

	unsigned epochCounter0 = 0, epochCounter1 = 0;
	double epoch0 = 0.0, epoch1 = 0.0;
	pair<unsigned, unsigned> readHitsMisses0, readHitsMisses1;
	pair<unsigned, unsigned> hitsMisses0, hitsMisses1;
	double averageLatency, averageAdjustedLatency;
	bool found0, found1;
	string commandLine0, commandLine1;

	// with the DIMM cache
	found0 = processStatsForFile(filePair.first, epochCounter0, epoch0, averageAdjustedLatency, readHitsMisses0, hitsMisses0, commandLine0);
	// without the DIMM cache
	found1 = processStatsForFile(filePair.second, epochCounter1, epoch1, averageLatency, readHitsMisses1, hitsMisses1, commandLine1);

	if (found0 && found1)
	{
		vector<string> currentLine;

		/// @TODO process both of these?
		//toBeProcessed.push_back(filePair->first);

		//string currentUrlString = ireplace_all_copy(urlString,"%1",basefilename);
		string modUrlString = commandLine0.substr(commandLine0.find(':') + 2, commandLine0.length());
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

		current << std::dec << std::fixed << std::setprecision(2) << ((double) epochCounter1 * epoch1);
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2) << ((float) readHitsMisses1.first
			/ ((float) readHitsMisses1.first + readHitsMisses1.second));
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< ((float) hitsMisses0.first / ((float) hitsMisses0.first
			+ hitsMisses0.second));
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< averageLatency;
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< averageAdjustedLatency;
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(2)
			<< averageLatency - averageAdjustedLatency;
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

bool processStatsForFile(string file, unsigned &epochCounter, double &epoch, double &averageLatency, pair<unsigned, unsigned> &readHitsMisses, pair<unsigned, unsigned> &hitsMisses, string &commandLine)
{
	filtering_istream inputStream;

	if (ends_with(file, ".gz"))
		inputStream.push(boost::iostreams::gzip_decompressor());
	else if (ends_with(file, ".bz2"))
		inputStream.push(boost::iostreams::bzip2_decompressor());

	inputStream.push(file_source(file));


	char newLine[NEWLINE_LENGTH];

	if (!inputStream.is_complete())
		return false;

	bool foundCommandline = false;
	bool foundEpoch = false;

	unsigned lineCounter = 0;

	WeightedAverage<double> averageTransactionLatency;

	for (inputStream.getline(newLine, NEWLINE_LENGTH);
	(newLine[0] != NULL) && (!userStop);
	inputStream.getline(newLine, NEWLINE_LENGTH))
	{						
		if (starts_with(newLine,
			"----Cumulative DIMM Cache Read Hits/Misses"))
		{
			epochCounter++;

			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				break;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				break;
			*secondBracket = (char) NULL;
			readHitsMisses.first = max(atoi(firstBracket + 1), 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL)
				break;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL)
				break;
			*secondBracket = (char) NULL;
			readHitsMisses.second = max(atoi(firstBracket + 1), 1);
		}
		else if (starts_with(newLine,
			"----Cumulative DIMM Cache Hits/Misses"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				break;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				break;
			*secondBracket = (char) NULL;
			hitsMisses.first = max(atoi(firstBracket + 1), 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL)
				break;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL)
				break;
			*secondBracket = (char) NULL;
			hitsMisses.second = max(atoi(firstBracket + 1), 1);
		}
		else if (starts_with(newLine, "----Transaction Latency"))
		{
			// continue adding to the StdDev object to get a cumulative value
			char *position = newLine;

			while (position != NULL)
			{
				char *firstBracket = strchr(position, '{');
				if (firstBracket == NULL) break;

				char *secondBracket = strchr(position, '}');
				if (secondBracket == NULL) break;

				char *comma = strchr(position, ',');
				if (comma == NULL) break;

				*comma = *secondBracket = NULL;

				unsigned latency = atoi(firstBracket + 1);
				unsigned count = atoi(comma + 1);

				averageTransactionLatency.add(latency, count);

				position = secondBracket + 1;
			}
		}
#if 0
		else if (starts_with(newLine,
			"----Average Cumulative Transaction Latency"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				break;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				break;
			*secondBracket = (char) NULL;
			averageLatency = atof(firstBracket + 1);
		}
#endif
#if 0
		else if (starts_with(newLine,
			"----Average Cumulative Adjusted Transaction Latency"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				break;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				break;
			*secondBracket = (char) NULL;
			averageAdjustedLatency = atof(firstBracket + 1);
		}
#endif
		else if (!foundEpoch && starts_with(newLine, "----Epoch"))
		{
			foundEpoch = true;

			char *position = strchr(newLine, ' ');
			if (position == NULL)
				break;
			epoch = atof(position + 1);
		}
		else if (!foundCommandline && starts_with(newLine, "----Command Line:"))
		{
			foundCommandline = true;

			commandLine = newLine;
		}
	}

	averageLatency = averageTransactionLatency.average();

	return foundCommandline && foundEpoch;
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

	for (inputStream.getline(newLine, NEWLINE_LENGTH);
		(newLine[0] != NULL) && (!userStop);
		inputStream.getline(newLine, NEWLINE_LENGTH))
	{
		
		if (starts_with(newLine,"+ch"))
		{
			PowerCalculations pc = params.calculateSystemPower(newLine,epochTime);
			totalEnergy.first += pc.dimmEnergy;
			totalEnergy.second += pc.energy;
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

	return foundEpoch && foundCommandLine;
}