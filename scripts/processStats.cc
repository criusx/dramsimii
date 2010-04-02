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


			if (generatePngFiles)
			{
// 				string commandLine1 = string(MOGRIFY_COMMAND)
// 					+ " -resize 3840 -format png " + filename;
// 				system(commandLine1.c_str());
			}

			bf::remove(bf::path(filename));
		}
	}
}


void process(const string i)
{
	cerr << i << endl;

	if (ends_with(i, "power.gz") || ends_with(i, "power.bz2"))
	{
		if (separateOutputDir)
		{
			bf::path filepath(i.substr(0, i.find("-power")));
			bf::path path = outputDir / filepath.filename();
			bf::path filename(i);
			processPower(path, filename.native_file_string());
		}
		else
		{
			bf::path filepath(i);
			bf::path path = filepath.branch_path()
				/ filepath.leaf().substr(0, filepath.leaf().find(
				"-power"));
			processPower(path, i);
		}
	}
	else if (ends_with(i, "stats.gz") || ends_with(i, "stats.bz2"))
	{
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
		("png,p","Generate PNG versions of the files")
		("cypress,c","Generate only select graphs for Cypress study")
		("process,r","Only process the files, do not regenerate the html file")
		("output,o",opt::value<string>(),		
		"Choose an output directory different from the current directory");

	opt::variables_map vm;

	try
	{
		opt::store(opt::parse_command_line(argc, argv, desc), vm);
	} catch (opt::unknown_option uo)
	{
		cerr << uo.what() << endl;
		exit(-1);
	}

	opt::notify(vm);

	if (vm.count("help"))
	{
		cout << "Usage: " << argv[0] << "(--help | -f | -p)" << endl;
	}

	bf::path outputDir;
	
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

	generatePngFiles = vm.count("png") > 0;

	cypressResults = vm.count("cypress") > 0;

	list<string> toBeProcessed;

	map<string, vector<string> > results;

	vector<string> files;

	// figure out which in the command line are viable files to analyze
	for (int i = 0; i < argc; ++i)
	{
		if (ends_with(argv[i], "power.gz") || ends_with(argv[i], "power.bz2")
			|| ends_with(argv[i], "stats.gz") || ends_with(argv[i],
			"stats.bz2"))
		{
			files.push_back(string(argv[i]));
		}
	}

	if (!processFilesOnly)
	{
		int currentFileNumber = 0;
		// gather stats from all the files to generate the html file
#pragma omp parallel 
		{
			for (vector<string>::const_iterator currentFile = files.begin(); 
				currentFile != files.end(); ++currentFile)
//			exit(-1);
//#pragma omp parallel for
//			for (int i = 0; i < files.size(); i++)
			{
				//string *currentFile = &files[i];

				filtering_istream inputStream;

				if (ends_with(*currentFile, "stats.gz"))
					inputStream.push(boost::iostreams::gzip_decompressor());
				else if (ends_with(*currentFile, "stats.bz2"))
					inputStream.push(boost::iostreams::bzip2_decompressor());
				else
					continue;

				inputStream.push(file_source(*currentFile));

				char newLine[NEWLINE_LENGTH];

				if (!inputStream.is_complete())
					continue;

				pair<unsigned, unsigned> readHitsMisses;
				pair<unsigned, unsigned> hitsMisses;
				unsigned epochCounter = 0;
				double epoch = 0.0;
				float averageLatency;
				float averageAdjustedLatency;
				vector<string> currentLine;
				string basefilename;

				bool foundCommandline = false;
				bool foundEpoch = false;

				inputStream.getline(newLine, NEWLINE_LENGTH);

				unsigned lineCounter = 0;

				while ((newLine[0] != NULL) && (!userStop))
				{
					const string filename(*currentFile);

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
					else if (!foundEpoch && starts_with(newLine, "----Epoch"))
					{
						foundEpoch = true;

						char *position = strchr(newLine, ' ');
						if (position == NULL)
							break;
						epoch = atof(position + 1);
					}
					else if (!foundCommandline && starts_with(newLine,
						"----Command Line:"))
					{
						foundCommandline = true;

						toBeProcessed.push_back(*currentFile);

						basefilename = filename.substr(0, filename.find_last_of('-'));
						//string currentUrlString = ireplace_all_copy(urlString,"%1",basefilename);
						const string commandline(newLine);
						string modUrlString = commandline.substr(commandline.find(':')
							+ 2, commandline.length());
						vector<string> splitLine;
						erase_all(modUrlString, "_");
						erase_all(modUrlString, "{");
						erase_all(modUrlString, "}");
						replace_first(modUrlString,"  ]","]");
						split(splitLine, modUrlString, is_any_of(" "),
							token_compress_on);

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
							//"<td>" + ireplace_all_copy(currentUrlString,"%2",benchmarkName) + "</td>";
						}

						// then calculate the runtime for the last column
						if (!ends_with(*currentFile, "stats.gz") && !ends_with(
							*currentFile, "stats.bz2"))
						{
							if (results.find(basefilename) == results.end())
								results[basefilename] = currentLine;

							// do not read power files all the way through
							break;
						}
					}

					if ((++lineCounter % 7500) == 0)
					{
						char *position = strchr(newLine, '\n');
						if (position != NULL)
							*position = (char) NULL;
						newLine[50] = NULL;

						cerr << "\r" << setiosflags(ios::right) << std::setw(4)
							<< ++currentFileNumber << "/"
							<< setiosflags(ios::left) << std::setw(4)
							<< files.size() << " " << std::setw(9)
							<< lineCounter << " " << newLine << "\r";
					}

					inputStream.getline(newLine, NEWLINE_LENGTH);
				}
				boost::iostreams::close(inputStream);

				stringstream current;
				current << std::dec << std::fixed << std::setprecision(6)
					<< ((double) epochCounter * epoch);
				currentLine.push_back(current.str());
				current.str("");

				current << std::dec << std::fixed << std::setprecision(6)
					<< ((float) readHitsMisses.first
					/ ((float) readHitsMisses.first + readHitsMisses.second));
				currentLine.push_back(current.str());
				current.str("");

				current << std::dec << std::fixed << std::setprecision(4)
					<< ((float) hitsMisses.first / ((float) hitsMisses.first
					+ hitsMisses.second));
				currentLine.push_back(current.str());
				current.str("");

				current << std::dec << std::fixed << std::setprecision(4)
					<< averageLatency;
				currentLine.push_back(current.str());
				current.str("");

				current << std::dec << std::fixed << std::setprecision(4)
					<< averageAdjustedLatency;
				currentLine.push_back(current.str());

				current << std::dec << std::fixed << std::setprecision(4)
					<< averageLatency - averageAdjustedLatency;
				currentLine.push_back(current.str());

#pragma omp critical
				results[basefilename] = currentLine;
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
		for (map<string, vector<string> >::const_iterator x = results.begin(); x
			!= results.end(); ++x)
		{
			fileList += "<tr>";

			for (vector<string>::const_iterator i = x->second.begin(), end =
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
		bf::create_directories(jsDirectory);
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

		// TODO: make this parallel?
		for_each(files.begin(), files.end(), process);
		
		doneEntering = true;

		boost::thread threadB(thumbNailWorker);

		cerr << "Waiting for post-processing to finish." << endl;

		threadA.join();
		threadB.join();
	}

	return 0;
}

