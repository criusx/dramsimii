#include "processStats.hh"
#include "globals.hh"

#include <boost/algorithm/string/regex.hpp>

void powerGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
				const vector<vector<float> > &values,
				float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << totalPowerScript << endl;
	p << "set title \"{/=18 Power vs. Time}\\n{/=14 "
		<< commandLine << "}\"  offset character 0, -1, 0 font \"Arial," << (isThumbnail ? "14" : "6") << "\" norotate\n";
	p << "plot ";

	unsigned channelCount = values.size() / 5;
	for (unsigned a = 0; a < channelCount; a++)
	{
		for (unsigned b = 0; b < 5; b++)
		{
			p << "'-' using 1 axes x2y1 title \"P_{sys}("
				<< powerTypes[b] << ") ch[" << a << "]\",";
		}
	}
	p << "'-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n";
	for (vector<vector<float> >::const_iterator i = values.begin(); i
		!= values.end(); ++i)
	{
		for (vector<float>::const_iterator j = i->begin(); j != i->end(); ++j)
		{
			p << *j << endl;
		}
		p << "e" << endl;
	}
	p << "0 0" << endl << (!values.empty() ? values.back().size() : 0.0)
		* epochTime << " 0.2" << endl << "e" << endl;

	p << averagePowerScript;

	// make the total power bar graphs
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];
		p << i * epochTime << " " << totalPowerPerEpoch << endl;
	}
	p << "e" << endl;

	// make the average power line
	CumulativePriorMovingAverage cumulativePower;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float total = 0;

		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			total += values[j][i];
		cumulativePower.add(1.0, total);
		p << i * epochTime << " " << cumulativePower.getAverage() << endl;
	}
	p << "e" << endl;

	PriorMovingAverage powerMovingAverage(WINDOW);

	// moving window average
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float total = 0;

		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			total += values[j][i];
		powerMovingAverage.append(total);
		p << i * epochTime << " " << powerMovingAverage.getAverage() << endl;
	}
	p << "e" << endl << "unset multiplot" << endl << "unset output" << endl;
}

void energyGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
				 const vector<vector<float> > &values,
				 float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << energyScript << endl;
	p << "set title \"{/=18 Energy vs. Time}\\n{/=14 " << commandLine
		<< "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
	p << "plot '-' u 1:2 sm csp t \"Energy (P t)\" w lines lw 2.00, '-' u 1:2 sm csp t \"IBM Energy (P^{2} t^{2})\" w lines lw 2.00\n";

	// various energy graphs
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p << i * epochTime << " " << totalPowerPerEpoch * epochTime << endl;
	}
	p << "e" << endl;

	double cumulativeEnergy = 0.0F;
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p << i * epochTime << " " << totalPowerPerEpoch * totalPowerPerEpoch
			* epochTime * epochTime << endl;
		cumulativeEnergy += totalPowerPerEpoch * epochTime;
	}

	p << "e" << endl << energy2Script << endl;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p << i * epochTime << " " << totalPowerPerEpoch * epochTime
			* epochTime << endl;
	}
	p << "e" << endl;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p << i * epochTime << " " << totalPowerPerEpoch * totalPowerPerEpoch
			* epochTime * epochTime * epochTime << endl;
	}
	p << "e" << endl << "unset multiplot" << endl << "unset output" << endl;
}

void bigEnergyGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
					const vector<vector<float> > &values,
					float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << bigEnergyScript << endl;
	p << "set title \"{/=24 Energy vs. Time}\\n{/=18 "
		<< commandLine
		<< "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
	p << "plot '-' u 1:2 axes x1y1 t \"Energy (P t)\" w boxes lt rgb \"#66CF03\" , '-' u 1:2 axes x1y2 t \"Cumulative Energy\" w lines lw 6.00 lt rgb \"#387400\", '-' u 1:2 axes x1y1 notitle with points pointsize 0.01"
		<< endl;
	// various energy graphs
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p << i * epochTime << " " << totalPowerPerEpoch * epochTime << endl;
	}
	p << "e" << endl;

	double cumulativeEnergy = 0.0F;
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		cumulativeEnergy += totalPowerPerEpoch * epochTime;
		p << i * epochTime << " " << cumulativeEnergy << endl;
	}

	p << "e" << endl << "0 0" << endl << values.front().size() * epochTime << " 1E-5" << endl << "e" << endl
		<< "unset output" << endl;
}

void bigPowerGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
				   const vector<vector<float> > &values,
				   float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"{/=24 Power vs. Time}\\n{/=18 "
		<< commandLine
		<< "}\"  offset character 0, -1, 0 font \"Arial,15\" norotate\n";

	p << bigPowerScript << endl;
	p << "plot ";

	unsigned channelCount = values.size() / 5;

	for (unsigned a = 0; a < channelCount; a++)
	{
		for (unsigned b = 0; b < 5; b++)
		{
			p << "'-' using 1 axes x2y1 title \"P_{sys}("
				<< powerTypes[b] << ") ch[" << a << "]\",";
		}
	}

	//p << "'-' u 1:2 axes x2y1 notitle with points pointsize 0.01,";
	p << "'-' u 1:2 axes x1y1 t \"Cumulative Average\" w lines lw 6.00 lt rgb \"#225752\",";
	p << "'-' u 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;
	for (vector<vector<float> >::const_iterator i = values.begin(); i
		!= values.end(); ++i)
	{
		for (vector<float>::const_iterator j = i->begin(), end = i->end(); j != end; ++j)
		{
			//cerr << *j << " ";
			p << *j << endl;
		}
		p << "e" << endl;
	}
	CumulativePriorMovingAverage cumulativePower;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float total = 0;

		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			total += values[j][i];
		cumulativePower.add(1.0, total);
		p << i * epochTime << " " << cumulativePower.getAverage() << endl;
	}
	p << "e" << endl;
	p << "0 0" << endl << values.front().size() * epochTime << " 1e-5" << endl << "e" << endl
		<< "unset output" << endl;
}

void cumulativeEnergyGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
						   vector<pair<float, float> > &energyValues,
						   float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << "Cumulative Energy\\n" << commandLine << "\""
		<< endl << cumulPowerScript;

	float time = 0.0F;
	float totalPower = 0.0F;
	for (vector<pair<float, float> >::const_iterator i = energyValues.begin(); i
		!= energyValues.end(); ++i)
	{
		totalPower += i->first;

		p << time << " " << totalPower << endl;

		time += epochTime;
	}

	p << "e" << endl;

	time = 0.0F;
	totalPower = 0.0F;
	for (vector<pair<float, float> >::const_iterator i = energyValues.begin(); i
		!= energyValues.end(); ++i)
		//for (vector<unsigned>::size_type i = 0; i < energyValues.back().size(); ++i)
	{
		//for (vector<unsigned>::size_type j = 0; j < alternateValues.size(); ++j)
		//	totalPower += alternateValues[j][i];
		totalPower += i->first - i->second;

		p << time << " " << totalPower << endl;
		//cerr << time << " " << totalPower << endl;

		//cerr << time << " " << i->first + i->second << endl;
		time += epochTime;
	}
	//////////////////////////////////////////////////////////////////////////
	p << "e" << endl << "unset output" << endl;
}

#define POWER_VALUES_PER_CHANNEL 5

///////////////////////////////////////////////////////////////////////////////
void processPower(const bf::path &outputDir, const string &filename)
{
	if (!fileExists(filename))
	{
		cerr << "cannot find " << filename << endl;
		return;
	}

	if (!ensureDirectoryExists(outputDir))
		exit(-1);

	unsigned writing = 0;

	list<string> filesGenerated;

	unsigned channelCount;
	float epochTime = 0.0F;

	vector<vector<float> > values;
	vector<float> valueBuffer;
	vector<pair<float, float> > energyValues;
	pair<float, float> energyValueBuffer;

	unsigned scaleFactor = 1;
	unsigned scaleIndex = 0;

	string commandLine;

	filtering_istream inputStream;
	inputStream.push(boost::iostreams::gzip_decompressor());
	inputStream.push(file_source(filename));

	// power values
	float pDsAct = 0.0F;
	float pDsActStby = 0.0F;
	float pDsActPdn = 0.0F;
	float pDsPreStby = 0.0F;
	float pDsPrePdn = 0.0F;
	double pDsRd = 0.0;
	double pDsWr = 0.0;
	float voltageScaleFactor = 0.0F;
	float frequencyScaleFactor = 0.0F;
	double tRc = 0.0;
	double tBurst = 0.0;
	double CKE_LO_PRE = 0.95F;
	double CKE_LO_ACT = 0.01F;

	/// @TODO make this generic
	float devicesPerRank = 8.0F;

	char newLine[NEWLINE_LENGTH];

	if (!inputStream.is_complete())
		return;

	list<pair<string, string> > graphs;	

	for (inputStream.getline(newLine, NEWLINE_LENGTH);
		(newLine[0] != 0) && (!userStop);
		inputStream.getline(newLine, NEWLINE_LENGTH))
	{
		if (newLine[0] == '-')
		{
			if (newLine[1] == 'P')
			{
				string backup(newLine);
				char *position = strchr(newLine, '{');
				char *position2 = strchr(newLine, '}');
				if (position == NULL || position2 == NULL)
					break;
				position++;
				*position2 = 0;

				float thisPower = atof(position);
				valueBuffer[writing] += (thisPower);

				// make sure to get multiple channels
				if (starts_with(newLine,"-Psys(ACT_STBY)"))
				{
					char *firstBrace = strchr(position2 + 1, '{');
					if (firstBrace == NULL) break;

					char *slash = strchr(firstBrace, '/');
					if (slash == NULL) break;

					char *secondBrace = strchr(firstBrace, '}');
					if (secondBrace == NULL) break;

					*slash = *secondBrace = NULL;

					energyValueBuffer.first += atof(firstBrace + 1);
					energyValueBuffer.second += atof(slash + 1);
				}

				writing = (writing + 1) % values.size();

				if (writing == 0)
				{
					// look to dump the buffer into the array
					scaleIndex = (scaleIndex + 1) % scaleFactor;

					// when the scale buffer is full
					if (scaleIndex == 0)
					{
						vector<float>::size_type limit = valueBuffer.size();
						for (vector<float>::size_type i = 0; i < limit; ++i)
						{
							values[i].push_back(valueBuffer[i] / scaleFactor);
							valueBuffer[i] = 0;
						}

						energyValues.push_back(pair<float, float> (
							energyValueBuffer.first / scaleFactor,
							energyValueBuffer.second / scaleFactor));

						energyValueBuffer.first = energyValueBuffer.second = 0.0F;
					}

					// try to compress the array by half and double the scaleFactor
					if (values.front().size() >= MAXIMUM_VECTOR_SIZE)
					{
						// scale the array back by half
						for (vector<vector<float> >::iterator i =
							values.begin(); i != values.end(); ++i)
						{
							for (unsigned j = 0; j < MAXIMUM_VECTOR_SIZE / 2; ++j)
							{
								(*i)[j] = ((*i)[2 * j] + (*i)[2 * j + 1]) / 2;
							}
							i->resize(MAXIMUM_VECTOR_SIZE / 2);

							assert(i->size() == MAXIMUM_VECTOR_SIZE / 2);
						}

						// scale the alternate array back by half
						for (unsigned j = 0; j < MAXIMUM_VECTOR_SIZE / 2; ++j)
						{
							energyValues[j] = pair<float, float> (
								(energyValues[2 * j].first + energyValues[2
								* j + 1].first) / 2.0F,
								(energyValues[2 * j].second
								+ energyValues[2 * j + 1].second)
								/ 2.0F);
						}

						energyValues.resize(MAXIMUM_VECTOR_SIZE / 2);

						assert(energyValues.size() == MAXIMUM_VECTOR_SIZE / 2);

						// double scaleFactor since each entry now represents twice what it did before
						scaleFactor *= 2;
						epochTime *= 2;
						cerr << "scaleFactor at " << scaleFactor << endl;
					}

				}
			}
			else
			{
				if (boost::starts_with(newLine, "----Epoch"))
				{
					char *position = strchr(newLine, ' ');
					if (position == NULL)
						break;
					position++;
					epochTime = lexical_cast<float> (position);
				}
				else if (starts_with(newLine, "----Command Line"))
				{
					tRc = regexMatch<double>(newLine, "\\{RC\\}\\[([0-9]+)\\]");

					tBurst = regexMatch<double>(newLine, "tBurst\\[([0-9]+)\\]");

					float idd0 = regexMatch<float>(newLine,"IDD0\\[([0-9]+)\\]");

					float idd3n = regexMatch<float>(newLine,"IDD3N\\[([0-9]+)\\]");

					float tRas = regexMatch<float>(newLine,"\\{RAS\\}\\[([0-9]+)\\]");

					float idd2n = regexMatch<float>(newLine,"IDD2N\\[([0-9]+)\\]");

					float vdd = regexMatch<float>(newLine,"VDD\\[([0-9.]+)\\]");

					float vddMax = regexMatch<float>(newLine,"VDDmax\\[([0-9.]+)\\]");

					float idd3p = regexMatch<float>(newLine,"IDD3P\\[([0-9]+)\\]");

					float idd2p = regexMatch<float>(newLine,"IDD2P\\[([0-9]+)\\]");

					float idd4r = regexMatch<float>(newLine,"IDD4R\\[([0-9]+)\\]");

					float idd4w = regexMatch<float>(newLine,"IDD4W\\[([0-9]+)\\]");

					float specFreq = regexMatch<float>(newLine,"spedFreq\\[([0-9]+)\\]");

					float freq = regexMatch<float>(newLine,"DR\\[([0-9]+)M\\]") * 1E6;

					int channelWidth = regexMatch<int>(newLine,"ChannelWidth\\[([0-9]+)\\]");

					int dqPerDram = regexMatch<int>(newLine,"DQPerDRAM\\[([0-9]+)\\]");

					devicesPerRank = channelWidth / dqPerDram;

					// update power values
					pDsAct = (idd0 - ((idd3n * tRas + idd2n * (tRc - tRas))/tRc)) * vdd;
					pDsActStby = idd3n * vdd;
					pDsActPdn = idd3p * vdd;
					pDsPreStby = idd2n * vdd;
					pDsPrePdn = idd2p * vdd;
					frequencyScaleFactor= freq / specFreq;
					voltageScaleFactor = (vdd / vddMax) * (vdd / vddMax);
					pDsRd = (idd4r - idd3n) * vdd;
					pDsWr = (idd4w - idd3n) * vdd;

					string line(newLine);
					trim(line);
					vector<string> splitLine;
					split(splitLine, line, is_any_of(":"));
					commandLine = splitLine[1];
				}
				else if (newLine[1] == '+')
				{
					string line(newLine);

					vector<string> splitLine;
					split(splitLine,line,is_any_of("[]"));
					channelCount = lexical_cast<unsigned>(splitLine[1]);

					values.reserve(channelCount * POWER_VALUES_PER_CHANNEL);
					energyValues.reserve(channelCount * POWER_VALUES_PER_CHANNEL);

					// setup the buffer to be the same size as the value array
					valueBuffer.resize(channelCount * POWER_VALUES_PER_CHANNEL);

					for (int i = channelCount * POWER_VALUES_PER_CHANNEL; i > 0; --i)
					{
						values.push_back(vector<float> ());
						values.back().reserve(MAXIMUM_VECTOR_SIZE);
					}
				}
			}
		}		
		else if (newLine[0] == '+')
		{
			string line(newLine);
			trim(line);
			vector<string> splitLine;
			boost::split_regex(splitLine, line, regex(" rk"));

			// per channel power numbers
			double PsysRD = 0.0;
			double PsysRdAdjusted = 0.0;
			double PsysWR = 0.0;
			double PsysACT_STBY = 0.0;
			double PsysPRE_STBY = 0.0;
			double PsysPRE_PDN = 0.0;
			double PsysACT_PDN = 0.0;
			double PsysACT = 0.0;
			double PsysACTAdjusted = 0.0;

			vector<string>::const_iterator currentRank = splitLine.begin(), end = splitLine.end();
			currentRank++;

			for (;currentRank != end; ++currentRank)
			{
				//cerr << *currentRank << endl;

				double duration = regexMatch<float>(currentRank->c_str(),"duration\\{([0-9]+)\\}");
				double thisRankRasCount = regexMatch<float>(currentRank->c_str(),"rasCount\\{([0-9]+)\\}");
				double thisRankAdjustedRasCount = regexMatch<float>(currentRank->c_str(),"adjRasCount\\{([0-9]+)\\}");
				double readCycles = regexMatch<float>(currentRank->c_str(),"read\\{([0-9]+)\\}");
				double writeCycles = regexMatch<float>(currentRank->c_str(),"write\\{([0-9]+)\\}");
				double readHits = regexMatch<float>(currentRank->c_str(),"readHits\\{([0-9]+)\\}");
				double prechargeTime = regexMatch<float>(currentRank->c_str(),"prechargeTime\\{([0-9]+)\\}");
				double percentActive = 1.0 - (prechargeTime / max((double)(duration), 0.00000001));
				assert(percentActive >= 0.0F && percentActive <= 1.0F);
				
				// background power analysis
				double PschACT_STBY = pDsActStby * percentActive * (1 - CKE_LO_ACT);
				PsysACT_STBY += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * PschACT_STBY;

				double PschPRE_STBY = pDsPreStby * (1.0 - percentActive) * (1 - CKE_LO_PRE);
				PsysPRE_STBY += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschPRE_STBY;

				double PschPRE_PDN = pDsPrePdn * (1.0 - percentActive) * (CKE_LO_PRE);
				PsysPRE_PDN += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschPRE_PDN;

				double PschACT_PDN = pDsActPdn * percentActive * CKE_LO_ACT;
				PsysACT_PDN += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschACT_PDN;

				// activate power analysis
				double tRRDsch = ((double)duration) / (thisRankRasCount > 0 ? thisRankRasCount : 0.00000001);
				double PschACT = pDsAct * tRc / tRRDsch;
				PsysACT += devicesPerRank * voltageScaleFactor * PschACT;

				// read power analysis
				double RDschPct = readCycles / duration;
				PsysRD += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsRd * RDschPct;

				// write power analysis
				double WRschPct = writeCycles / duration;
				PsysWR += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsWr * WRschPct;

				// accounting for cache effects
				double RDschPctAdjusted = (readCycles - tBurst * readHits) / duration;
				PsysRdAdjusted += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsRd * RDschPctAdjusted;

				double tRRDschAdjusted = duration / thisRankAdjustedRasCount;
				PsysACTAdjusted += devicesPerRank * (tRc / tRRDschAdjusted) * voltageScaleFactor * pDsAct;
			}			
		}
	}

	userStop = false;

	if (values.size() < 1)
		return;

	// get a couple copies of gnuplot running
	opstream p("gnuplot");
	p << basicSetup << terminal;
	opstream p2("gnuplot");
	p2 << basicSetup << terminal;
	opstream p3("gnuplot");
	p3 << basicSetup << terminal;
	opstream p4("gnuplot");
	p4 << basicSetup << terminal;

	//////////////////////////////////////////////////////////////////////////
	// make the power graph
	path outFilename = outputDir / ("power." + extension);
	powerGraph(outFilename,p,commandLine,values,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("power","Overall Power"));
	outFilename = outputDir / ("power-thumb." + thumbnailExtension);
	powerGraph(outFilename,p2,commandLine,values,epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the big power graph
	outFilename = outputDir / ("bigPower." + extension);
	bigPowerGraph(outFilename,p3,commandLine,values,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigPower","Power"));
	outFilename = outputDir / ("bigPower-thumb." + thumbnailExtension);
	bigPowerGraph(outFilename,p3,commandLine,values,epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the energy graph
	outFilename = outputDir / ("energy." + extension);
	energyGraph(outFilename,p2,commandLine,values,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("energy","Energy"));
	outFilename = outputDir / ("energy-thumb." + thumbnailExtension);
	energyGraph(outFilename,p2,commandLine,values,epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the big energy graph
	outFilename = outputDir / ("bigEnergy." + extension);
	bigEnergyGraph(outFilename,p,commandLine,values,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("bigEnergy","Simple Energy"));
	outFilename = outputDir / ("bigEnergy-thumb." + thumbnailExtension);
	bigEnergyGraph(outFilename,p3,commandLine,values,epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// the cumulative energy graph
	outFilename = outputDir / ("cumulativeEnergy." + extension);
	cumulativeEnergyGraph(outFilename,p4,commandLine,energyValues,epochTime,false);
	graphs.push_back(pair<string, string> ("cumulativeEnergy","Cumulative Energy"));
	filesGenerated.push_back(outFilename.native_directory_string());
	outFilename = outputDir / ("cumulativeEnergy-thumb." + thumbnailExtension);
	cumulativeEnergyGraph(outFilename,p4,commandLine,energyValues,epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	p << endl << "exit" << endl;
	p2 << endl << "exit" << endl;
	p3 << endl << "exit" << endl;
	p4 << endl << "exit" << endl;

	p.close();
	p2.close();
	p3.close();
	p4.close();

#pragma omp critical
	{
		boost::mutex::scoped_lock lock(fileListMutex);
		for (list<string>::const_iterator i = filesGenerated.begin(); i
			!= filesGenerated.end(); ++i)
			fileList.push_back(*i);
	}

	bf::path givenfilename(filename);
	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine,
		graphs);
}

