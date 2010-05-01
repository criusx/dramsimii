#include "processStats.hh"
#include "globals.hh"

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


void thumbNailWorker()
{
	//using namespace Magick;

	string filename;
	string baseFilename;
	//Image second;


	while (!doneEntering || !fileList.empty())
	{
		if (fileList.empty())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
		else
		{
			{
				boost::mutex::scoped_lock lock(fileListMutex);
				filename = fileList.front();
				baseFilename = fileList.front().substr(0,
					fileList.front().find(extension) - 1);
				fileList.pop_front();
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


void process(const string i, const list<pair<string,string> > &powerParams)
{
	if (regexSearch(i.c_str(), (const char *)"power[CN]?[.](gz|bz2)?$"))
	{		
		if (separateOutputDir)
		{
			bf::path filepath(i.substr(0, i.find("-power")));
			bf::path path = outputDir / filepath.filename();
			bf::path filename(i);
			processPower(path, filename.native_file_string(), powerParams);
		}
		else
		{
			bf::path filepath(i);
			bf::path path = filepath.branch_path()
				/ filepath.leaf().substr(0, filepath.leaf().find(
				"-power"));
			processPower(path, i, powerParams);
		}
	}
	else if (regexSearch(i.c_str(), (const char *)"stats[CN]?[.](gz|bz2)?$"))

	{
		cerr << i << endl;
		return;
		if (separateOutputDir)
		{
			bf::path filepath(i.substr(0, i.find("-stats")));
			bf::path path = outputDir / filepath.filename();
			bf::path filename(i);
			processStats(path, filename.native_file_string());
		}
		else
		{
			bf::path filepath(i);
			bf::path path = filepath.branch_path()
				/ filepath.leaf().substr(0, filepath.leaf().find(
				"-stats"));
			processStats(path, i);
		}
	}
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
		("create,f","Force creation of the index file only")
		("power-params,r",opt::value<string>(),
		"Update power parameters to different values than what are in the power files")
		("cypress,c","Generate only select graphs for Cypress study")
		("process,r","Only process the files, do not regenerate the html file")
		("output,o",opt::value<string>(),		
		"Choose an output directory different from the current directory")
		("pairs,p","Results for DIMM and non-DIMM are generated as pairs and should be processed accordingly");

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

	if (vm.count("output"))
	{
		outputDir = vm["output"].as<string> ();
		separateOutputDir = true;
		cerr << outputDir.native_directory_string() << endl;
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

	map<string, list<string> > results;

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
		int currentFileNumber = 0;
		// gather stats from all the files to generate the html file
#pragma omp parallel for shared(files) 
		for (vector<pair<string, string> >::const_iterator currentFile = filePairs.begin(); 
			currentFile < filePairs.end(); ++currentFile)
		{
			// go through the stats file
			if (regexSearch(currentFile->first.c_str(), (const char *)"stats[CN]?[.](gz|bz2)?$"))
			{
				processStatsForPair(*currentFile, results);		
			}
			// go through the power file
			else if (regexSearch(currentFile->first.c_str(), (const char *)"power[CN]?[.](gz|bz2)?$"))
			{		
				processPowerForPair(*currentFile, results, powerParams);
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

		string fileList;
		string csvOutput;
		for (map<string, list<string> >::const_iterator x = results.begin(); x
			!= results.end(); ++x)
		{
			fileList += "<tr>";

			for (list<string>::const_iterator i = x->second.begin(), end =
				x->second.end(); i != end;)
			{
				csvOutput += *i;

				fileList += "<td>" + ireplace_all_copy(ireplace_all_copy(urlString,
					"%2", *i), "%1", x->first) + "</td>";

				if (++i != end)
					csvOutput += ',';
				else
					csvOutput += '\n';

			}

			fileList += "</tr>";
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
		outCsv.write(csvHeader.c_str(), csvHeader.length());
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

	bf::path sourceJsDirectory = executableDirectory / "js";

	// copy all the files from the js directory to the result js directory
	bf::directory_iterator endIter;
	for (bf::directory_iterator iter(sourceJsDirectory); iter != endIter; ++iter)
	{
		if (!is_directory(*iter))
		{
			bf::path result = jsDirectory / iter->leaf();
			if (!exists(result))
				copy_file(*iter, result);
		}
	}

	// finally, process the files to generate results
	if (!generateResultsOnly)
	{
		boost::thread threadA(thumbNailWorker);
		
#pragma omp parallel for shared(files)
		for (vector<string>::const_iterator currentFile = files.begin(); currentFile < files.end(); ++currentFile)
			process(*currentFile, powerParams);
		
		doneEntering = true;

		boost::thread threadB(thumbNailWorker);

		cerr << "Waiting for post-processing to finish." << endl;

		threadA.join();
		threadB.join();
	}

	return 0;
}

