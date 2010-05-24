#include "processStats.hh"
#include "globals.hh"
#include "powerScripts.hh"

#include <boost/algorithm/string/regex.hpp>

#define POWER_VALUES_PER_CHANNEL 10

const string PowerScripts::energyScript = "unset border\n\
										  set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
										  set autoscale xfixmin\n\
										  set autoscale xfixmax\n\
										  set yrange [0:*] noreverse nowriteback\n\
										  set title\n\
										  set ytics out\n\
										  set xtics out\n\
										  set mxtics\n\
										  set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
										  unset x2tics\n\
										  set multiplot\n\
										  set size 1.0, 0.5\n\
										  set origin 0.0, 0.5\n\
										  set ylabel \"Energy\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
										  set xlabel \"Time (s)\"\n";

const string PowerScripts::energy2Script = "set size 1.0, 0.5\n\
										   set origin 0.0, 0.0\n\
										   set title\n\
										   plot '-' u 1:2 t \"Energy Delay Prod (P t^{2})\" w lines lw 2.00,\
										   '-' u 1:2 t \"IBM Energy2 (P^{2}t^{3})\" w lines lw 2.00\n";

const string PowerScripts::cumulPowerScript = "unset border\n\
											  set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
											  set autoscale xfixmin\n\
											  set autoscale xfixmax\n\
											  set yrange [0:*] noreverse nowriteback\n\
											  unset x2tics\n\
											  set mxtics\n\
											  set xrange [0:*]\n\
											  set xlabel \"Time (s)\"\n\
											  set ylabel \"Energy (mJ)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
											  set ytics out\n\
											  set origin 0.0, 0.0\n\
											  plot '-' u 1:2 t \"Cumulative Energy\" w lines lw 1.5,\
											  '-' u 1:2 t \"Theoretical Cumulative Energy\" w lines lw 1.5\n";


const string PowerScripts::bigPowerScript = "set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
											set yrange [0:*] noreverse nowriteback\n\
											unset x2tics\n\
											set mxtics\n\
											set xrange [0:*]\n\
											set yrange [0:*]\n\
											set xlabel \"Time (s)\"\n\
											set ylabel \"Power Dissipated (mW)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
											set ytics out\n\
											set boxwidth 1.00 absolute\n\
											set style fill  solid 1.00 noborder\n\
											set style data histograms\n\
											#set style data filledcurves below x1\n\
											set style histogram rowstacked title offset 0,0,0\n";

const string PowerScripts::comparativePowerScript = "set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
													set yrange [0:*] noreverse nowriteback\n\
													unset x2tics\n\
													set mxtics\n\
													set xrange [0:*]\n\
													set yrange [0:*]\n\
													set xlabel \"Time (s)\"\n\
													set ylabel \"Power Dissipated (mW)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
													set ytics out\n\
													set boxwidth 1.00 absolute\n\
													set style fill  solid 1.00 noborder\n";

const string PowerScripts::bigEnergyScript = "set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
											 set autoscale xfixmin\n\
											 set autoscale xfixmax\n\
											 set yrange [0:*] noreverse nowriteback\n\
											 set ytics out\n\
											 set xtics out\n\
											 set mxtics\n\
											 set yrange [0:*]\n\
											 set y2range [0:*]\n\
											 set ytics nomirror\n\
											 set y2tics\n\
											 unset x2tics\n\
											 set boxwidth 1.00 relative\n\
											 #set logscale y2\n\
											 set ylabel \"Energy (mJ)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
											 set y2label \"Cumulative Energy (mJ)\"\n\
											 set xlabel \"Time (s)\"\n";

bool PowerScripts::processStatsForFile(const string &file)
{
	givenfilename = file;
	filtering_istream inputStream;

	if (ends_with(file, ".gz"))
		inputStream.push(boost::iostreams::gzip_decompressor());
	else if (ends_with(file, ".bz2"))
		inputStream.push(boost::iostreams::bzip2_decompressor());

	inputStream.push(file_source(file));

	char newLine[NEWLINE_LENGTH];

	if (!inputStream.is_complete())
		return false;

	for (inputStream.getline(newLine, NEWLINE_LENGTH);
		(newLine[0] != NULL) && (!userStop);
		inputStream.getline(newLine, NEWLINE_LENGTH))
	{			
		processLine(newLine);
	}

	return working();
}

void PowerScripts::processLine(char *newLine)
{
	if (!foundCommandLine || !foundEpoch)
	{
		if (newLine[0] == '-')
		{
			if (starts_with(newLine, "----Command Line"))
			{
				foundCommandLine = true;

				rawCommandLine = newLine;

				powerParameters.setParameters(newLine, powerParams);		

				channelCount = regexMatch<unsigned>(newLine,"ch\\[([0-9]+)\\]");

				std::string cache = regexMatch<std::string>(newLine,"usingCache\\[([TF]+)\\]");

				usingCache = (cache == "T");

				values.reserve(channelCount * POWER_VALUES_PER_CHANNEL);

				energyValues.reserve(channelCount * POWER_VALUES_PER_CHANNEL);

				// setup the buffer to be the same size as the value array
				valueBuffer.resize(channelCount * POWER_VALUES_PER_CHANNEL);

				for (int i = channelCount * POWER_VALUES_PER_CHANNEL; i > 0; --i)
				{
					values.push_back(vector<float>());
					values.back().reserve(MAXIMUM_VECTOR_SIZE);
				}

				// determine the commandline name
				char *position = strchr(newLine, ':');
				//commandLine = position + 2;
				//commandLine = getCommandLine(commandLine);				
				commandLine = getCommandLine(string(position + 2));				
			}
			else if (starts_with(newLine, "----Epoch"))
			{
				foundEpoch = true;

				char *position = strchr(newLine, ' ');
				if (position == NULL)
					return;
				position++;
				epochTime = lexical_cast<float> (position);
			}
		}
	}
	if (newLine[0] == '+')
	{
		// per channel power numbers
		try
		{
			PowerCalculations pc = powerParameters.calculateSystemPower(newLine, (double)epochTime);

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
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 7] += pc.PsysDQ;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 8] += pc.PsysTermRoth + pc.PsysTermW + pc.PsysTermWoth;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 9] += pc.sramActivePower + pc.sramIdlePower;

			averageInUseTime.add(pc.inUseTime, 1);
			//cerr << pc.inUseTime << endl;

			/// @TODO fix this too
			energyValueBuffer.first += pc.energy;
			energyValueBuffer.second += pc.dimmEnergy;

			totalEnergy.first += pc.dimmEnergy;
			totalEnergy.second += pc.energy;

			if (currentChannel + 1 == channelCount)
			{
				pushStats();
			}
		}
		catch (std::exception &ex)
		{

		}


	}
}

void PowerScripts::pushStats()
{
	epochCount++;
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

	if (values.front().size() >= MAXIMUM_VECTOR_SIZE)
	{
		compressStats();
	}
}

void PowerScripts::compressStats()
{
	// try to compress the array by half and double the scaleFactor

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

void PowerScripts::generateGraphs(const bf::path &outputDir)
{
	if (!ensureDirectoryExists(outputDir))
		exit(-1);

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

	list<pair<string, string> > graphs;


	//////////////////////////////////////////////////////////////////////////
	// make the power graph
	path outFilename = outputDir / ("power." + extension);
	powerGraph(outFilename,p,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("power","Overall Power"));
	outFilename = outputDir / ("power-thumb." + thumbnailExtension);
	powerGraph(outFilename,p2,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the big power graph
	outFilename = outputDir / ("bigPower." + extension);
	bigPowerGraph(outFilename,p3, values, false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigPower","Power"));
	outFilename = outputDir / ("bigPower-thumb." + thumbnailExtension);
	bigPowerGraph(outFilename,p3, values, true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the other big power graph
	outFilename = outputDir / ("bigPower2." + extension);
	bigPowerGraph2(outFilename,p3,values, false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigPower2","Combined Power"));
	outFilename = outputDir / ("bigPower2-thumb." + thumbnailExtension);
	bigPowerGraph2(outFilename,p3,values, true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the energy graph
	outFilename = outputDir / ("energy." + extension);
	energyGraph(outFilename,p2,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("energy","Energy"));
	outFilename = outputDir / ("energy-thumb." + thumbnailExtension);
	energyGraph(outFilename,p2,true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the big energy graph
	outFilename = outputDir / ("bigEnergy." + extension);
	bigEnergyGraph(outFilename,p,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("bigEnergy","Simple Energy"));
	outFilename = outputDir / ("bigEnergy-thumb." + thumbnailExtension);
	bigEnergyGraph(outFilename,p3,true);
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

	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, graphs);
}

#if 0
void PowerScripts::evenRunTime(const double newRunTime)
{
	int numberOfEpochs = (newRunTime - getRunTime()) / epochTime;

	if (numberOfEpochs < 0)
		return;

	for (;numberOfEpochs > 0; --numberOfEpochs)
	{
		for (int currentChannel = 0; currentChannel < channelCount; ++currentChannel)
		{
			PowerCalculations pc = powerParameters.calculateSystemPowerIdle(epochTime);

			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 0] += pc.PsysACT_STBY;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 1] += pc.PsysACT;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 2] += pc.PsysPRE_STBY;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 3] += pc.PsysRD;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 4] += pc.PsysWR;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 5] += pc.PsysACT_PDN;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 6] += pc.PsysPRE_PDN;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 9] += pc.sramActivePower + pc.sramIdlePower;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 8] += pc.PsysTermRoth + pc.PsysTermW + pc.PsysTermWoth;
			valueBuffer[currentChannel * POWER_VALUES_PER_CHANNEL + 7] += pc.PsysDQ;

			energyValueBuffer.first += pc.energy;
			energyValueBuffer.second += pc.dimmEnergy;

			totalEnergy.first += pc.dimmEnergy;
			totalEnergy.second += pc.energy;

			if (currentChannel + 1 == channelCount)
			{
				pushStats();
			}
		}
	}
}
#endif

void PowerScripts::generateJointGraphs(const bf::path &outputDir, PowerScripts &alternatePower) 
{
	if (!ensureDirectoryExists(outputDir))
		exit(-1);

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

	list<pair<string, string> > graphs;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// make the big power graph
	path outFilename = outputDir / ("bigAlternatePower." + extension);
	bigPowerGraph(outFilename,p3,alternatePower.getValues(), false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigAlternatePower","Theoretical Power"));
	outFilename = outputDir / ("bigAlternatePower-thumb." + thumbnailExtension);
	bigPowerGraph(outFilename,p3,alternatePower.getValues(), true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the other big power graph
	outFilename = outputDir / ("comparativePower." + extension);
	comparativePowerGraph(outFilename,p3,alternatePower.getValues(), false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("comparativePower","Comparative Power"));
	outFilename = outputDir / ("comparativePower-thumb." + thumbnailExtension);
	comparativePowerGraph(outFilename,p3,alternatePower.getValues(), true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the other big power graph
	outFilename = outputDir / ("bigTheoreticalPower2." + extension);
	bigPowerGraph2(outFilename,p3,alternatePower.getValues(),false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bigTheoreticalPower2","Combined Theoretical Power"));
	outFilename = outputDir / ("bigTheoreticalPower2-thumb." + thumbnailExtension);
	bigPowerGraph2(outFilename,p3,alternatePower.getValues(),true);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// the cumulative energy graph
	outFilename = outputDir / ("cumulativeEnergy." + extension);
	cumulativeEnergyGraph(outFilename,p4,alternatePower.getEnergyValues(), false);
	graphs.push_back(pair<string, string> ("cumulativeEnergy","Cumulative Energy"));
	filesGenerated.push_back(outFilename.native_directory_string());
	outFilename = outputDir / ("cumulativeEnergy-thumb." + thumbnailExtension);
	cumulativeEnergyGraph(outFilename,p4,alternatePower.getEnergyValues(), true);
	//////////////////////////////////////////////////////////////////////////

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

	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, graphs);
}

const string PowerScripts::totalPowerScript = "unset border\n\
											  set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
											  set autoscale xfixmin\n\
											  set autoscale xfixmax\n\
											  set yrange [0:*] noreverse nowriteback\n\
											  unset x2tics\n\
											  set mxtics\n\
											  set xrange [0:*]\n\
											  set xlabel \"Time (s)\"\n\
											  set ylabel \"Power Dissipated (mW)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
											  set ytics out\n\
											  set multiplot\n\
											  set size 1.0, 0.66\n\
											  set origin 0.0, 0.34\n\
											  set boxwidth 0.95 relative\n\
											  set style fill  solid 1.00 noborder\n\
											  set style data histograms\n\
											  #set style data filledcurves below x1\n\
											  set style histogram rowstacked title offset 0,0,0\n";

const string PowerScripts::averagePowerScript = "set size 1.0, 0.35\n\
												set origin 0.0, 0.0\n\
												set title \"Power Dissipated\"\n\
												set boxwidth 0.95 relative\n\
												plot \
												'-' u 1:2 t \"Total Power\" w boxes,\
												'-' u 1:2 t \"Running Average\" w lines lw 1.00,\
												'-' u 1:2 t \"Cumulative Average\" w lines lw 1.00\n";

void PowerScripts::powerGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const
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
	p << "e" << endl;

	p << "unset multiplot" << endl << "unset output" << endl;
}

void PowerScripts::energyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const
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

void PowerScripts::bigEnergyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail) const
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

void PowerScripts::bigPowerGraph(const bf::path &outFilename, opstream &p, const vector<vector<float> > &alternateValues, bool isThumbnail) const
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("Theoretical Power vs. Time", commandLine, p);

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
	for (vector<vector<float> >::const_iterator i = alternateValues.begin(), end = alternateValues.end(); 
		i < end; ++i)
	{
		for (vector<float>::const_iterator j = i->begin(), end = i->end(); j != end; ++j)
		{
			p << *j << endl;
		}
		p << "e" << endl;
	}

	CumulativePriorMovingAverage cumulativePower;

	for (vector<unsigned>::size_type i = 0; i < alternateValues.back().size(); ++i)
	{
		float total = 0;

		for (vector<unsigned>::size_type j = 0; j < alternateValues.size(); ++j)
			total += alternateValues[j][i];
		cumulativePower.add(1.0, total);
		p << i * epochTime << " " << cumulativePower.getAverage() << endl;
	}
	p << "e" << endl;
	p << "0 0" << endl << alternateValues.back().size() * epochTime << " 1e-5" << endl << "e" << endl
		<< "unset output" << endl;
}

void PowerScripts::bigPowerGraph2(const bf::path &outFilename, opstream &p, const vector<vector<float> > &alternateValues, bool isThumbnail) const
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	//p << "set title \"{ Power vs. Time}\\n{ "
	//	<< commandLine
	//	<< "}\"  offset character 0, -1, 0 font \"Arial,15\" norotate\n";
	printTitle("Theoretical Power vs. Time", commandLine, p);

	p << bigPowerScript << endl;
	p << "plot ";

	unsigned channelCount = alternateValues.size() / POWER_VALUES_PER_CHANNEL;

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
		for (int j = 0; j < alternateValues.front().size(); j++)
		{
			double value = 0.0;
			for (int k = 0; k < channelCount; k++)
				value += alternateValues[i + POWER_VALUES_PER_CHANNEL * k][j];

			p << value << endl;
		}
		p << "e" << endl;
	}

	CumulativePriorMovingAverage cumulativePower;

	for (vector<unsigned>::size_type i = 0; i < alternateValues.back().size(); ++i)
	{
		float total = 0;

		for (vector<unsigned>::size_type j = 0; j < alternateValues.size(); ++j)
			total += alternateValues[j][i];
		cumulativePower.add(1.0, total);
		p << i * epochTime << " " << cumulativePower.getAverage() << endl;
	}
	p << "e" << endl;
	p << "0 0" << endl << alternateValues.front().size() * epochTime << " 1e-5" << endl << "e" << endl
		<< "unset output" << endl;
}

//////////////////////////////////////////////////////////////////////////
void PowerScripts::comparativePowerGraph(const bf::path &outFilename, opstream &p, const vector<vector<float> > &alternateValues, bool isThumbnail) const
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	printTitle("Power/Theoretical Power vs. Time", commandLine, p);

	p << bigPowerScript << endl;

	unsigned channelCount = values.size() / POWER_VALUES_PER_CHANNEL;

	p << "plot '-' u 1:2 w lines lw 2.00 t \"Normal System\",'-' u 1:2 w lines lw 2.00 t \"Theoretical System\"" << endl;

	double time = 0.0;

	vector<vector<float> >::size_type columns = values.size();

	for (vector<float>::size_type i = 0; i < values.front().size(); ++i)
	{
		if (i < values.front().size())
		{
			double total = 0.0;

			for (vector<vector<float> >::size_type j = 0; j < columns; ++j)
			{
				total += values[j][i];
			}

			p << time << " " << total << endl;
		}
		else
		{
			p << time << " " << 0.0 << endl;
		}

		time += epochTime;
	}

	p << "e" << endl;
	time = 0.0;

	for (vector<float>::size_type i = 0; i < alternateValues.front().size(); ++i)
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
void PowerScripts::cumulativeEnergyGraph(const bf::path &outFilename, opstream &p, const vector<pair<float,float> > &alternateValues, bool isThumbnail) const
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	printTitle("Cumulative Energy", commandLine, p);

	p << cumulPowerScript;

	double time = 0.0;
	double totalPower = 0.0;

	for (vector<pair<float, float> >::const_iterator i = energyValues.begin(), end = energyValues.end();
		i < end; ++i)
	{
		totalPower += i->first + i->second;

		p << time << " " << totalPower << endl;

		time += epochTime;
	}

	cerr << "normal " << totalPower << endl;	

	p << "e" << endl;

	time = 0.0;
	totalPower = 0.0;

	for (vector<pair<float, float> >::const_iterator i = alternateValues.begin(), end = alternateValues.end();
		i < end; ++i)
	{
		totalPower += i->first + i->second;

		p << time << " " << totalPower << endl;

		time += epochTime;
	}

	cerr << "alt " << totalPower << endl;

	p << "e" << endl << "unset output" << endl;
}
