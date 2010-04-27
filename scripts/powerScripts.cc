#include "processStats.hh"
#include "globals.hh"

#include <boost/algorithm/string/regex.hpp>

#define POWER_VALUES_PER_CHANNEL 8

void powerGraph(const bf::path &outFilename, opstream &p, const vector<string>& commandLine,
				const vector<vector<float> > &values,
				float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << totalPowerScript << endl;

	printTitle("Power vs. Time", commandLine, p);
	
	p << "plot ";

	unsigned channelCount = values.size() / POWER_VALUES_PER_CHANNEL;
	for (unsigned a = 0; a < channelCount; a++)
	{
		for (unsigned b = 0; b < POWER_VALUES_PER_CHANNEL; b++)
		{
			p << "'-' using 1 axes x2y1 title \"P_{sys}("
				<< powerTypes[b] << ") ch[" << a << "]\",";
		}
	}
	p << "'-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n";
	assert(values.size() > 0);
	for (vector<vector<float> >::const_iterator i = values.begin(); i
		!= values.end(); ++i)
	{
		assert(i->size() > 0);
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

void energyGraph(const bf::path &outFilename, opstream &p, const vector<string>& commandLine,
				 const vector<vector<float> > &values,
				 float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << energyScript << endl;
	//p << "set title \"{/=18 Energy vs. Time}\\n{/=14 " << commandLine
	//	<< "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
	printTitle("Energy vs. Time", commandLine, p);

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

void bigEnergyGraph(const bf::path &outFilename, opstream &p, const vector<string>& commandLine,
					const vector<vector<float> > &values,
					float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << bigEnergyScript << endl;
	//p << "set title \"{/=24 Energy vs. Time}\\n{/=18 "
	//	<< commandLine
	//	<< "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
	printTitle("Energy vs. Time", commandLine, p);

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

void bigPowerGraph(const bf::path &outFilename, opstream &p, const vector<string>& commandLine,
				   const vector<vector<float> > &values,
				   float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	//p << "set title \"{/=24 Power vs. Time}\\n{/=18 "
	//	<< commandLine
	//	<< "}\"  offset character 0, -1, 0 font \"Arial,15\" norotate\n";
	printTitle("Power vs. Time", commandLine, p);

	p << bigPowerScript << endl;
	p << "plot ";

	unsigned channelCount = values.size() / POWER_VALUES_PER_CHANNEL;

	for (unsigned a = 0; a < channelCount; a++)
	{
		for (unsigned b = 0; b < POWER_VALUES_PER_CHANNEL; b++)
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

void bigPowerGraph2(const bf::path &outFilename, opstream &p, const vector<string>& commandLine,
					const vector<vector<float> > &values,
					float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	//p << "set title \"{ Power vs. Time}\\n{ "
	//	<< commandLine
	//	<< "}\"  offset character 0, -1, 0 font \"Arial,15\" norotate\n";
	printTitle("Power vs. Time", commandLine, p);

	p << bigPowerScript << endl;
	p << "plot ";

	unsigned channelCount = values.size() / POWER_VALUES_PER_CHANNEL;

	for (unsigned b = 0; b < POWER_VALUES_PER_CHANNEL; b++)
	{
		p << "'-' using 1 axes x2y1 title \"P_{sys}("
			<< powerTypes[b] << ")\",";
	}

	//p << "'-' u 1:2 axes x2y1 notitle with points pointsize 0.01,";
	p << "'-' u 1:2 axes x1y1 t \"Cumulative Average\" w lines lw 6.00 lt rgb \"#225752\",";
	p << "'-' u 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

	for (int i = 0; i < POWER_VALUES_PER_CHANNEL; i++)
	{
		for (int j = 0; j < values.front().size(); j++)
		{
			double value = 0.0;
			for (int k = 0; k < channelCount; k++)
				value += values[i + POWER_VALUES_PER_CHANNEL * k][j];

			p << value << endl;
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

//////////////////////////////////////////////////////////////////////////
void comparativePowerGraph(const bf::path &outFilename, opstream &p, const vector<string>& commandLine,
						   const vector<vector<float> > &values, const vector<vector<float> > &alternateValues,
						   float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	printTitle("Power vs. Time", commandLine, p);

	p << bigPowerScript << endl;
	
	unsigned channelCount = values.size() / POWER_VALUES_PER_CHANNEL;

	p << "plot '-' u 1:2 w lines lw 2.00 t \"Normal System\",'-' u 1:2 w lines lw 2.00 t \"Theoretical System\"" << endl;

	double time = 0.0;

	vector<vector<float> >::size_type columns = values.size();
	vector<float>::size_type epochs = values.front().size();
	
	for (vector<float>::size_type i = 0; i < epochs; ++i)
	{
		double total = 0.0;

		for (vector<vector<float> >::size_type j = 0; j < columns; ++j)
		{
			total += values[j][i];
		}

		p << time << " " << total << endl;
		
		time += epochTime;
	}

	p << "e" << endl;
	time = 0.0;

	for (vector<float>::size_type i = 0; i < epochs; ++i)
	{
		double total = 0.0;

		for (vector<vector<float> >::size_type j = 0; j < columns; ++j)
		{
			total += alternateValues[j][i];
		}

		p << time << " " << total << endl;

		time += epochTime;
	}

	p << "e" << endl;
	
	p << "unset output" << endl;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void cumulativeEnergyGraph(const bf::path &outFilename, opstream &p, const vector<string>& commandLine,
						   vector<pair<float, float> > &energyValues,
						   float epochTime, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	//p << "set title \"" << "Cumulative Energy\\n" << commandLine << "\""
	//	<< endl << cumulPowerScript;
	printTitle("Cumulative Energy", commandLine, p);

	p << cumulPowerScript;

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
	{
		totalPower += i->second;

		p << time << " " << totalPower << endl;
		
		time += epochTime;
	}
	//////////////////////////////////////////////////////////////////////////
	p << "e" << endl << "unset output" << endl;
}

///////////////////////////////////////////////////////////////////////////////
void processPower(const bf::path &outputDir, const string &filename, const list<pair<string,string> > &updatedPowerParams)
{
	if (!fileExists(filename))
	{
		cerr << "cannot find " << filename << endl;
		return;
	}

	if (!ensureDirectoryExists(outputDir))
		exit(-1);

	filtering_istream inputStream;
	inputStream.push(boost::iostreams::gzip_decompressor());
	inputStream.push(file_source(filename));

	unsigned writing = 0;

	list<string> filesGenerated;

	unsigned channelCount;
	float epochTime = 0.0F;

	vector<vector<float> > values;
	vector<float> valueBuffer;

	vector<vector<float> > alternateValues;
	vector<float> alternateValueBuffer;

	vector<pair<float, float> > energyValues;
	pair<float, float> energyValueBuffer;

	unsigned scaleFactor = 1;
	unsigned scaleIndex = 0;

	vector<string> commandLine;

	// power values

	PowerParameters<double> powerParams;	

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
				powerParams.setParameters(newLine, updatedPowerParams);		

				channelCount = regexMatch<unsigned>(newLine,"ch\\[([0-9]+)\\]");

				values.reserve(channelCount * POWER_VALUES_PER_CHANNEL);
				alternateValues.reserve(channelCount * POWER_VALUES_PER_CHANNEL);

				energyValues.reserve(channelCount * POWER_VALUES_PER_CHANNEL);

				// setup the buffer to be the same size as the value array
				valueBuffer.resize(channelCount * POWER_VALUES_PER_CHANNEL);
				alternateValueBuffer.resize(channelCount * POWER_VALUES_PER_CHANNEL);

				for (int i = channelCount * POWER_VALUES_PER_CHANNEL; i > 0; --i)
				{
					values.push_back(vector<float>());
					alternateValues.push_back(vector<float>());
					values.back().reserve(MAXIMUM_VECTOR_SIZE);
					alternateValues.back().reserve(MAXIMUM_VECTOR_SIZE);
				}

				// determine the commandline name
				char *position = strchr(newLine, ':');
				//commandLine = position + 2;
				//commandLine = getCommandLine(commandLine);				
				commandLine = getCommandLine(string(position + 2));				
			}		
		}		
		// a line with all the power components for one channel
		else if (newLine[0] == '+')
		{	
			// per channel power numbers

			PowerCalculations pc = powerParams.calculateSystemPower(newLine, (double)epochTime);

			//cerr << PsysACT_STBY <<endl;
			//cerr << PsysACT <<endl;
			//cerr << PsysPRE_STBY <<endl;
			//cerr << PsysRD <<endl;
			//cerr << PsysWR <<endl;

			//cerr << PsysRdAdjusted <<endl;
			//cerr << PsysPRE_PDN <<endl;
			//cerr << PsysACT_PDN <<endl;
			//cerr << PsysACTAdjusted <<endl;
			unsigned currentChannel = regexMatch<unsigned>(newLine,"ch\\[([0-9]+)\\]");

			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 0] += pc.PsysACT_STBY;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 1] += pc.PsysACT;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 2] += pc.PsysPRE_STBY;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 3] += pc.PsysRD;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 4] += pc.PsysWR;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 5] += pc.PsysACT_PDN;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 6] += pc.PsysPRE_PDN;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 7] += 0; //pc.sramActivePower + pc.sramIdlePower;

			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 0] += pc.PsysACT_STBYAdjusted;
			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 1] += pc.PsysACTAdjusted;
			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 2] += pc.PsysPRE_STBY;
			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 3] += pc.PsysRdAdjusted;
			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 4] += pc.PsysWR;
			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 5] += pc.PsysACT_PDNAdjusted;
			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 6] += pc.PsysPRE_PDN;
			alternateValueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 7] += pc.sramActivePower + pc.sramIdlePower;
 
			energyValueBuffer.first += pc.energy;
			energyValueBuffer.second += pc.reducedEnergy;

			if (currentChannel + 1 == channelCount)
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
						alternateValues[i].push_back(alternateValueBuffer[i] / scaleFactor);
						alternateValueBuffer[i] = 0;
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
					for (vector<vector<float> >::iterator i = values.begin(), end = values.end();
						i < end; ++i)
					{
						for (unsigned j = 0; j < MAXIMUM_VECTOR_SIZE / 2; ++j)
						{
							(*i)[j] = ((*i)[2 * j] + (*i)[2 * j + 1]) / 2;
						}
						i->resize(MAXIMUM_VECTOR_SIZE / 2);

						assert(i->size() == MAXIMUM_VECTOR_SIZE / 2);
					}

					for (vector<vector<float> >::iterator i = alternateValues.begin(), end = alternateValues.end();
						i < end; ++i)
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

	assert(values.size() > 0);

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
	// make the big power graph
	outFilename = outputDir / ("bigAlternatePower." + extension);
	bigPowerGraph(outFilename,p3,commandLine,alternateValues,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigAlternatePower","Theoretical Power"));
	outFilename = outputDir / ("bigAlternatePower-thumb." + thumbnailExtension);
	bigPowerGraph(outFilename,p3,commandLine,values,epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the other big power graph
	outFilename = outputDir / ("bigPower2." + extension);
	bigPowerGraph2(outFilename,p3,commandLine,values,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigPower2","Combined Power"));
	outFilename = outputDir / ("bigPower2-thumb." + thumbnailExtension);
	bigPowerGraph2(outFilename,p3,commandLine,values,epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the other big power graph
	outFilename = outputDir / ("comparativePower." + extension);
	comparativePowerGraph(outFilename,p3,commandLine,values, alternateValues, epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("comparativePower","Comparative Power"));
	outFilename = outputDir / ("comparativePower-thumb." + thumbnailExtension);
	comparativePowerGraph(outFilename,p3,commandLine,values, alternateValues, epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the other big power graph
	outFilename = outputDir / ("bigTheoreticalPower2." + extension);
	bigPowerGraph2(outFilename,p3,commandLine,alternateValues,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigTheoreticalPower2","Combined Theoretical Power"));
	outFilename = outputDir / ("bigTheoreticalPower2-thumb." + thumbnailExtension);
	bigPowerGraph2(outFilename,p3,commandLine,alternateValues,epochTime,true);
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

	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, graphs);
}

