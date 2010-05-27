#include "processStats.hh"
#include "globals.hh"
#include "resultSet.hh"

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
		map<string, ResultSet> results;

		// gather stats from all the files to generate the html file
#pragma omp parallel for shared(results)
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
		
		if (int i = generateResultHtml(results, outputDir))
			return i;

		// finally, process the files to generate results
		generateOverallGraphs(outputDir, results);
	}

	cerr << "Waiting for post-processing to finish." << endl;

#pragma omp parallel for
	for (vector<string>::const_iterator i = fileList.begin(); i < fileList.end(); ++i)
	{
		thumbNailWorker(*i);
	}

	return 0;
}