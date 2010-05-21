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
	//using namespace Magick;

	//string filename;
	string baseFilename;
	//Image second;


	//while (!doneEntering || !fileList.empty())
	{
		//if (fileList.empty())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
		//else
		{
			{
				//boost::mutex::scoped_lock lock(fileListMutex);
				//filename = fileList.front();
				//baseFilename = filename.front().substr(0,
				//	fileList.front().find(extension) - 1);
				//fileList.pop_front();
			}
			//string commandLine0 = CONVERT_COMMAND + " " + filename + "[" + thumbnailResolution + "] " + baseFilename + "-thumb.png";
			//string commandLine1 = "mogrify -resize 3840 -format png -limit memory 1gb " + filename;

			//second = first;

			//second.sample("3840");

			//second.write(baseFilename + ".png");

			//string commandLine0 = string(CONVERT_COMMAND) + " " + filename + "'[" + thumbnailResolution + "]' " + baseFilename + "-thumb.png";
			// 			string commandLine0 = string(CONVERT_COMMAND) + " " + filename
			// 				+ " -resize " + thumbnailResolution + " " + baseFilename
			// 				+ "-thumb.png";
			string commandLine2 = "gzip -c -9 -f " + filename + " > "
				+ filename + "z";
#ifndef NDEBUG
			//cerr << commandLine0 << endl;
#endif
			// 			if (system(commandLine0.c_str()) != 0)
			// 				cerr << "Failed to create thumbnail for " << filename << endl;
			if (system(commandLine2.c_str()) != 0)
				cerr << "Failed to compress " << filename << endl;

			//Image first(filename.c_str());
			// 			Image first(baseFilename + "-thumb.png");
			// 			first.magick("png");
			// 			first.zoom(thumbnailResolution);
			// 			first.write(baseFilename + "-thumb2.png");
			
			bf::remove(bf::path(filename));
		}
	}
}

bool regexSearch(const char *input, const char *regex)
{
	boost::regex currentRegex(regex);
	boost::cmatch match;
	return boost::regex_search(input, match, currentRegex);
}


////////////////////////////////////////////////
int main(int argc, char** argv)
{
	bf::path dir(argv[0]);
	executableDirectory = dir.branch_path();

	signal(SIGUSR1, sigproc);

	opt::options_description desc("Basic options");
	string settingsFile;
	string extraSettings;
	desc.add_options()
		("help", "help message")
		("create,i","Force creation of the index file only")
		("power-params,m",opt::value<string>(),"Update power parameters to different values than what are in the power files")
		("cypress,c","Generate only select graphs for Cypress study")
		("process,p","Only process the files, do not regenerate the html file")
		("output,o",opt::value<string>(),"Choose an output directory different from the current directory")
		("pairs,r","Results for DIMM and non-DIMM are generated as pairs and should be processed accordingly");

	opt::variables_map vm;

	try
	{
		opt::store(opt::parse_command_line(argc, argv, desc), vm);
	} 
	catch (opt::unknown_option uo)
	{
		cerr << uo.what() << endl;
		exit(-1);
	}

	opt::notify(vm);

	if (vm.count("help"))
	{
		cout << "Usage: " << argv[0] << "(--help | -f | -p)" << endl;
	}

	list<pair<string,string> > powerParams;
	if (vm.count("power-params"))
	{
		string params = vm["power-params"].as<string>();
		vector<string> splitLine;
		split(splitLine,params,is_any_of(","));
		for (vector<string>::const_iterator i = splitLine.begin(), end = splitLine.end();
			i != end; ++i)
		{
			vector<string> splitParam;
			split(splitParam, *i, is_any_of("="));
			powerParams.push_back(pair<string,string>(splitParam[0],splitParam[1]));
		}
	}

	bf::path outputDir;
	if (vm.count("output"))
	{
		outputDir = vm["output"].as<string> ();
		separateOutputDir = true;
		cerr << outputDir.native_directory_string() << endl;
	}
	else
	{
		outputDir = ".";
	}

	if (vm.count("create") > 0 && vm.count("process") > 0)
	{
		cerr << "Error, cannot use --create and --process together, as they exclude one another" << endl;
		exit(-1);
	}

	bool generateResultsOnly = vm.count("create") > 0;

	bool processFilesOnly = vm.count("process") > 0;

	cypressResults = vm.count("cypress") > 0;

	list<string> toBeProcessed;

	map<string, ResultSet> results;

	vector<string> files;

	// figure out which in the command line are viable files to analyze
	for (int i = 0; i < argc; ++i)
	{
		if (regexSearch(argv[i], (const char *)"(power|stats)[CN]?[.](gz|bz2)?$"))
		{
			files.push_back(string(argv[i]));
		}
	}

	vector<pair<string,string> > filePairs;

	if (vm.count("pairs"))
	{
		for (vector<string>::const_iterator currentFile = files.begin(); 
			currentFile < files.end(); ++currentFile)
		{
			// find the cache file
			if (regexSearch(currentFile->c_str(), (const char *)"(power|stats)C.(gz|bz2)?$"))
			{
				// then find the non-cache file
				for (vector<string>::const_iterator currentFile2 = files.begin(); 
					currentFile2 < files.end(); ++currentFile2)
				{
					if (starts_with(*currentFile2, currentFile->substr(0,currentFile->length() - 4)) &&
						ends_with(*currentFile2,"N.gz"))
					{
						filePairs.push_back(pair<string,string>(*currentFile,*currentFile2));
						//cerr << *currentFile << " " << *currentFile2 << endl;
					}
				}
			}
		}
	}

	if (!processFilesOnly)
	{
		// gather stats from all the files to generate the html file
#pragma omp parallel for shared(files) 
		for (vector<pair<string, string> >::const_iterator currentFile = filePairs.begin(); 
			currentFile < filePairs.end(); ++currentFile)
		{
			// go through the stats file
			if (regexSearch(currentFile->first.c_str(), "stats[CN]?[.](gz|bz2)?$"))
			{
				processStatsForPair(*currentFile, results, outputDir, generateResultsOnly);		
			}
			// go through the power file
			else if (regexSearch(currentFile->first.c_str(), "power[CN]?[.](gz|bz2)?$"))
			{		
				processPowerForPair(*currentFile, results, powerParams, outputDir, generateResultsOnly);
			}
		}

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

		//////////////////////////////////////////////////////////////////////////
		generateOverallGraphs(outputDir, results);
		//////////////////////////////////////////////////////////////////////////

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
	}

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

	// finally, process the files to generate results

	cerr << "Waiting for post-processing to finish." << endl;

#pragma omp parallel for
	for (vector<string>::const_iterator i = fileList.begin(); i < fileList.end(); i++)
	{
		thumbNailWorker(*i);
	}

	doneEntering = true;


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
		cerr << name << "|" ;
		string basename = regexMatch<string>(name,"^([A-Za-z]+)");
		cerr << basename << "|"<<endl;
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