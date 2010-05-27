#include "processStats.hh"
#include "globals.hh"
#include "resultSet.hh"

unordered_map<string, string> &setupDecoder()
{
	static unordered_map<string, string> theMap;

	theMap["SDHIPF"] = "SDRAM High Performance";
	theMap["SDBAS"] = "SDRAM Baseline";
	theMap["CPBAS"] = "Close Page Baseline";
	theMap["LOLOC"] = "Low Locality";
	theMap["HILOC"] = "High Locality";
	theMap["GRD"] = "Greedy";
	theMap["STR"] = "Strict";
	theMap["BRR"] = "Bank Round Robin";
	theMap["RRR"] = "Rank Round Robin";
	theMap["CLSO"] = "Close Page Aggressive";
	theMap["OPEN"] = "Open Page";
	theMap["CPA"] = "Close Page Aggressive";
	theMap["CLOS"] = "Close Page";
	theMap["OPA"] = "Open Page Aggressive";
	theMap["CPBOPT"] = "Close Page Baseline Opt";
	theMap["FRSTA"] = "First Available (Age)";
	theMap["FRSTR"] = "First Available (RIFF)";
	theMap["FRSTQ"] = "First Available (Queue)";
	theMap["CPRH"] = "Command Pair Rank Hop";

	return theMap;
}

string matchMap(const string &match)
{
	if (decoder.find(match) == decoder.end())
		return match;
	else
		return decoder[match];
}

void thumbNailWorker(const string &filename)
{
	string commandLine2 = "gzip -c -9 -f " + filename + " > " + filename + "z";
	
	if (system(commandLine2.c_str()) != 0)
		cerr << "Failed to compress " << filename << endl;
	
	bf::remove(bf::path(filename));
}

bool regexSearch(const char *input, const char *regex)
{
	boost::regex currentRegex(regex);
	boost::cmatch match;
	return boost::regex_search(input, match, currentRegex);
}


////////////////////////////////////////////////
int generateResultHtml(map<string,ResultSet> &results, bf::path &outputDir)
{
	// then generate result.html
	bf::path openfile = executableDirectory / "template2.html";

	if (!fileExists(openfile.native_directory_string()))
	{
		cerr << "cannot open template file: "
			<< openfile.native_directory_string();
		return -1;
	}

	ifstream instream(openfile.directory_string().c_str());
	stringstream entirefile;
	entirefile << instream.rdbuf();

	string fileList;
	string csvOutput = results.begin()->second.getCsvHeader();
	for (map<string, ResultSet>::const_iterator x = results.begin(); x
		!= results.end(); ++x)
	{
		pair<string,string> output = x->second.generateResultLine();
		csvOutput += output.first + "\n";
		fileList += output.second + "\n";
	}

	// write the result html file
	bf::path outputHtml((separateOutputDir ? outputDir : ".") / "result.html");
	ofstream out(outputHtml.native_file_string().c_str());
	string outString = entirefile.str();
	replace_all(outString, "@@@", fileList);
	out.write(outString.c_str(), outString.length());
	out.close();

	// write the result csv file
	bf::path outputCsv((separateOutputDir ? outputDir : ".") / "result.csv");	
	ofstream outCsv(outputCsv.native_file_string().c_str());
	outCsv.write(csvOutput.c_str(), csvOutput.length());
	outCsv.close();

	// create the js directory 
	bf::path jsDirectory((separateOutputDir ? outputDir : ".") / "js");	

	if (!exists(jsDirectory))
	{
		bf::create_directories(jsDirectory);
	}
	else if (!is_directory(jsDirectory))
	{
		cerr << "error: 'js' exists but is not a directory" << endl;
		return -1;
	}

	path sourceJsDirectory = executableDirectory / "js";

	// copy all the files from the js directory to the result js directory
	bf::directory_iterator endIter;
	for (bf::directory_iterator iter(sourceJsDirectory); iter != endIter; ++iter)
	{
		if (!is_directory(*iter))
		{
			path result = jsDirectory / iter->leaf();
			if (!exists(result) || !bf::is_regular_file(*iter) || !exists(*iter) || result.empty())
			{
				copy_file(*iter, result);
			}
		}
	}

	return 0;
}


const string hitRateVsEnergyGraph = "set xrange [0 : 1] noreverse nowriteback\n\
									set xlabel 'Hit Rate' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
									set style line 1\n\
									set style data points\n\
									set key rmargin center vertical reverse Right\n\
									plot '-' using 1:2 t '8MB, 64B block' w points linewidth 2 pt 6 ps 2.5 lc 1,\
									'-' using 1:2 t '8MB, 256B block' w points linewidth 2 pt 2 ps 2.5 lc 2,\
									'-' using 1:2 t '16MB, 64B block' w points linewidth 2 pt 6 ps 2.5 lc 3,\
									'-' using 1:2 t '16MB, 256B block' w points linewidth 2 pt 2 ps 2.5 lc 4\n";


void generateHitRateVsEnergyGraph(const bf::path &outFilename, const map<string,ResultSet > &results, opstream &p, const bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	vector<string> commandLine;
	printTitle("Hit Rate vs. Energy Ratio", commandLine, p);
	p << "set ylabel 'Energy Ratio (%)'" << endl;

	double maxY = 0.0F;
	for (map<string,ResultSet>::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{		
		maxY = max(maxY, i->second.getEnergyRatio());
	}
	p << "set yrange[0:" << 1.1 * maxY << "]" << endl;
	p << hitRateVsEnergyGraph;


	for (unsigned h = 8; h <=16; h+=8)
	{
		for (unsigned j = 64; j <= 256; j*=4)
		{
			for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
				i != end; i++)
			{
				if (i->second.cacheSize == h)
				{
					if (i->second.blockSize == j)
					{
						p << i->second.readHitRate << " " << i->second.getEnergyRatio() << endl;
					}
				}
			}
			p << "e" << endl;
		}
	}

	p << "unset output" << endl;
}

void generateHitRateVsEnergyGraph2(const bf::path &outFilename, const map<string,ResultSet > &results, opstream &p, const bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	vector<string> commandLine;
	printTitle("Hit Rate vs. Energy Reduction", commandLine, p);
	p << "set ylabel 'Energy Reduction (mJ)'" << endl;

	double maxY = 0.0F;
	double minY = 0.0F;
	for (map<string,ResultSet>::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{		
		maxY = max(maxY, i->second.getEnergyDifference());
		minY = min(minY, i->second.getEnergyDifference());
	}
	p << "set yrange[" << 1.1 * minY << ":" << 1.1 * maxY << "]" << endl;
	p << hitRateVsEnergyGraph;


	for (unsigned h = 8; h <=16; h+=8)
	{
		for (unsigned j = 64; j <= 256; j*=4)
		{
			for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
				i != end; i++)
			{
				if (i->second.cacheSize == h)
				{
					if (i->second.blockSize == j)
					{
						p << i->second.readHitRate << " " << i->second.getEnergyDifference() << endl;
					}
				}
			}
			p << "e" << endl;
		}
	}

	p << "unset output" << endl;
}

void generateHitRateVsEnergyGraph3(const bf::path &outFilename, const map<string,ResultSet > &results, opstream &p, const bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	vector<string> commandLine;
	printTitle("Hit Rate vs. Energy", commandLine, p);
	p << "set ylabel 'Energy (mJ)'" << endl;

	double maxY = 0.0F;
	double minY = 0.0F;
	//double minY = 1.0F;
	for (map<string,ResultSet>::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{		
		maxY = max(maxY, max(i->second.energyUsed,i->second.energyUsedTheoretical));
		//minY = min(minY, min(i->second.energyUsed,i->second.energyUsedTheoretical));
	}
	p << "set yrange[" << 0.9 * minY << ":" << 1.1 * maxY << "]" << endl;
	//cerr << minY << " " << maxY << endl;
	//p << "set logscale y" << endl;

	p << hitRateVsEnergyGraph;


	for (unsigned h = 8; h <=16; h+=8)
	{
		for (unsigned j = 64; j <= 256; j*=4)
		{
			for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
				i != end; i++)
			{
				if (i->second.cacheSize == h)
				{
					if (i->second.blockSize == j)
					{
						p << i->second.readHitRate << " " << i->second.energyUsedTheoretical << endl;
						p << "0 " << i->second.energyUsed << endl;
					}
				}
			}
			p << "e" << endl;
		}
	}

	p << "unset output" << endl;
}

const string hitRateVsEnergyGraph3 = "set xrange [0 : 1] noreverse nowriteback\n\
									set xlabel 'Hit Rate' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
									set style line 1\n\
									set style data points\n\
									unset key\n\
									plot '-' using 1:2 w points linewidth 2 pt 6 ps 2.5 lc 1\n";


void generateHitRateVsEnergyGraph5(const bf::path &outFilename, const map<string,ResultSet > &results, opstream &p, const bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	vector<string> commandLine;
	printTitle("Hit Rate vs. Energy Ratio", commandLine, p);
	p << "set ylabel 'Energy Ratio (%)'" << endl;

	double maxY = 0.0F;
	for (map<string,ResultSet>::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{		
		maxY = max(maxY, i->second.getEnergyRatio());
	}
	p << "set yrange[0:" << 1.1 * maxY << "]" << endl;
	p << hitRateVsEnergyGraph3;

	for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{
		p << i->second.readHitRate << " " << i->second.getEnergyRatio() << endl;
	}
	p << "e" << endl;

	p << "unset output" << endl;
}

const string hitRateVsEnergyGraph2 = "set xrange [0 : 1] noreverse nowriteback\n\
									set xlabel 'Hit Rate' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
									set style line 1\n\
									set style data points\n\
									set key rmargin center vertical reverse Right\n\
									plot '-' using 1:2 t '8MB, 64B block' w points linewidth 2 pt 6 ps 2.5 lc 1,\
									'-' using 1:2 t '8MB, 256B block' w points linewidth 2 pt 2 ps 2.5 lc 2,\
									'-' using 1:2 t '16MB, 64B block' w points linewidth 2 pt 6 ps 2.5 lc 3,\
									'-' using 1:2 t '16MB, 256B block' w points linewidth 2 pt 2 ps 2.5 lc 4,\
									'-' using 1:2 t 'no cache' w points linewidth 2 pt 4 ps 2.5 lc 5\n";

void generateHitRateVsEnergyGraph4(const bf::path &outFilename, const string baseName, const map<string,ResultSet > &results, opstream &p, const bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	vector<string> commandLine;
	printTitle("Hit Rate vs. Theoretical/Actual Energy", commandLine, p);
	p << "set ylabel 'Energy (mJ)'" << endl;

	double maxY = 0.0F;
	double minY = 0.0F;
	//double minY = 1.0F;
	for (map<string,ResultSet>::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{		
		const char *name = i->first.c_str();
		string base = regexMatch<string>(name,"^([A-Za-z]+)");
		if (base == baseName)
		{
			maxY = max(maxY, max(i->second.energyUsed,i->second.energyUsedTheoretical));
			//minY = min(minY, min(i->second.energyUsed,i->second.energyUsedTheoretical));
		}
	}
	p << "set yrange[" << 0.9 * minY << ":" << 1.1 * maxY << "]" << endl;
	//cerr << minY << " " << maxY << endl;
	//p << "set logscale y" << endl;

	p << hitRateVsEnergyGraph2;


	for (unsigned h = 8; h <=16; h+=8)
	{
		for (unsigned j = 64; j <= 256; j*=4)
		{
			for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
				i != end; i++)
			{
				if (i->second.cacheSize == h)
				{
					if (i->second.blockSize == j)
					{
						const char *name = i->first.c_str();
						string base = regexMatch<string>(name,"^([A-Za-z]+)");
						if (base == baseName)
						{
							p << i->second.readHitRate << " " << i->second.energyUsedTheoretical << endl;
						}
					}
				}
			}
			p << "e" << endl;
		}
	}

	for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{
		const char *name = i->first.c_str();
		string base = regexMatch<string>(name,"^([A-Za-z]+)");
		if (base == baseName)
		{
			p << "0 " << i->second.energyUsed << endl;
		}
	}
	p << "e" << endl;

	p << "unset output" << endl;
}

void generateHitRateVsLatencyGraph(const bf::path &outFilename, const map<string,ResultSet > &results, opstream &p, const bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	vector<string> commandLine;
	printTitle("Read Hit Rate vs. Latency Ratio", commandLine, p);
	p << "set ylabel 'Latency Ratio (%)'" << endl;

	double maxY = 0.0F;
	for (map<string,ResultSet>::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{		
		maxY = max(maxY, i->second.getEnergyRatio());
	}
	p << "set yrange[0:" << 1.1 * maxY << "]" << endl;

	p << hitRateVsEnergyGraph;

	for (unsigned h = 8; h <=16; h+=8)
	{
		for (unsigned j = 64; j <= 256; j*=4)
		{
			for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
				i != end; i++)
			{
				if (i->second.cacheSize == h)
				{
					if (i->second.blockSize == j)
					{
						p << i->second.readHitRate << " " << i->second.getLatencyReduction() << endl;
					}
				}
			}
			p << "e" << endl;
		}
	}

	p << "unset output" << endl;
}

void generateOverallGraphs(const bf::path &outFilename, const map<string,ResultSet > &results)
{
	opstream p0("gnuplot");
	p0 << terminal << basicSetup;
	generateHitRateVsEnergyGraph(outFilename / "hitRateVsEnergy.png", results,p0,true);
	generateHitRateVsEnergyGraph2(outFilename / "hitRateVsEnergy2.png", results,p0,true);
	generateHitRateVsEnergyGraph3(outFilename / "hitRateVsEnergy3.png", results,p0,true);
	generateHitRateVsEnergyGraph5(outFilename / "hitRateVsEnergy4.png", results, p0, true);
	generateHitRateVsLatencyGraph(outFilename / "hitRateVsLatency5.png", results,p0,true);

	map<string,unsigned> uniqueBenchmarks;
	for (map<string,ResultSet >::const_iterator i = results.begin(), end = results.end();
		i != end; i++)
	{
		const char * name = i->first.c_str();
		//cerr << name << "|" ;
		string basename = regexMatch<string>(name,"^([A-Za-z]+)");
		//cerr << basename << "|"<<endl;
		uniqueBenchmarks[basename] = 1;
	}

	for (map<string,unsigned>::const_iterator i = uniqueBenchmarks.begin(), end = uniqueBenchmarks.end();
		i != end; i++)
	{
		generateHitRateVsEnergyGraph4(outFilename / ("hitRateVsEnergy-" + i->first + ".png"), i->first, results, p0, true);
	}

	p0 << "exit" << endl;
	p0.close();
}