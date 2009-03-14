#include "pstream.h"
#include <boost/circular_buffer.hpp>
#include <cstdio>
#include <string>
#include <list>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <signal.h>
#ifdef WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif
#include <sys/wait.h>

namespace bf = boost::filesystem;

using std::string;
using std::cerr;
using std::endl;
using std::pair;
using boost::iostreams::bzip2_decompressor;
using boost::iostreams::gzip_decompressor;
using boost::iostreams::gzip_params;
using boost::iostreams::file_source;
using boost::iostreams::filtering_istream;
using boost::iostreams::filtering_istreambuf;
using boost::iostreams::file_source;
using boost::split;
using boost::starts_with;
using boost::ends_with;
using boost::token_compress_on;
using boost::trim;
using boost::is_any_of;
using boost::lexical_cast;
using boost::regex;
using boost::cmatch;
using std::max;
using std::min;
using std::vector;
using std::endl;
using std::ifstream;
using redi::opstream;
using std::ios;
using std::list;

void prepareOutputDir(const bf::path &outputDir, const string &filename, const string &commandLine, unsigned channelCount);

// globals
bf::path executableDirectory;

#define WINDOW 5

std::string thumbnailResolution = "640x400";

#include "processStats.hh"

std::string terminal = "set terminal svg size 1920,1200 dynamic enhanced fname \"Arial\" fsize 10\n";

std::string extension = "svg";

bool userStop = false;

template <class T>
T toNumeric(const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	T t;
	iss >> f >> t;
	return t;
}	

class CumulativePriorMovingAverage
{
	double average;
	double count;

public:
	CumulativePriorMovingAverage():average(0.0),count(0.0)
	{}

	void add(double count, double value)
	{
		count += 1.0;
		average += ((count * value - average) / count);
	}

	double getAverage() const { return average; }

	void clear()
	{
		average = 0.0;
		count = 0.0;
	}
};

class WeightedAverage
{
	unsigned count;
	unsigned total;

public:
	WeightedAverage():count(0), total(0)
	{}

	void add(unsigned value, unsigned count)
	{
		this->total += value * count;
		this->count += count;
	}

	void clear()
	{
		total = 0;
		count = 0;
	}

	double average()
	{
		return (double)total / ((count > 0) ? (double)count : 1.0);
	}
};

class PriorMovingAverage
{
	boost::circular_buffer<float> data;
	float average;

public:
	PriorMovingAverage(unsigned size):data(size,size,0),average(0)
	{}

	double getAverage() const { return average; }

	void append(float x)
	{
		float oldestItem = data.front();
		average = average - (oldestItem / data.size()) + (x / data.size());
		data.push_back(x);
	}

	void clear()
	{
		average = 0.0;
		for (boost::circular_buffer<float>::iterator i = data.begin(); i != data.end(); i++)
			*i = 0;
	}
};

bool fileExists(const string& fileName)
{
	bf::path newPath(fileName.c_str());
	return bf::exists(newPath);
}

void thumbNail(std::list<string>& fileList)
{
	for (std::list<string>::const_iterator i = fileList.begin();
		i != fileList.end(); i++)
	{
		string filename = (*i).substr(0,(*i).find(extension) - 1);
		string commandLine = "convert -limit memory 512mb -resize " + thumbnailResolution + " " + *i + " " + filename + "-thumb.png";
		string commandLine2 = "gzip -c -9 -f " +  *i + " > " + *i + "z";


		system(commandLine.c_str());
		system(commandLine2.c_str());

		bf::remove(bf::path(*i));

	}
}

void processPower(const string filename)
{
	bf::path outputDir(filename.substr(0,filename.find("-power")));

	if (!fileExists(filename))
	{
		cerr << "cannot find " << filename << endl;
		return;
	}

	if (!bf::exists(outputDir))
	{
		if (!bf::create_directory(outputDir))
		{
			cerr << "Could not create dir " << outputDir.leaf();
			exit(-1);
		}
	}
	else
	{
		if (!bf::is_directory(outputDir))
		{
			cerr << "Directory " << outputDir.leaf() << " exists, but is not a directory." << endl;
			exit(-1);
		}
	}

	unsigned writing = 0;

	// get a couple copies of gnuplot running
	opstream p("gnuplot");
	p << terminal;
	opstream p2("gnuplot");
	p2 << terminal;

	list<string> filesGenerated;

	unsigned channelCount;
	float epochTime;
	vector<vector<float> > values;

	string commandLine;

	filtering_istream inputStream;
	inputStream.push(boost::iostreams::gzip_decompressor());
	inputStream.push(file_source(filename));


	char newLine[256];
	string line = "";
	boost::regex powerRegex("\\{([0-9.e\\-\\+]+)\\}");

	if (!inputStream.is_complete())
		return;

	inputStream.getline(newLine,256); 

	while ((newLine[0] != 0) && (!userStop))
	{
		if (newLine[0] == '-')
		{
			if (newLine[1] == 'P')
			{
				char *position = strchr(newLine,'{') + 1;
				char *position2 = strchr(newLine,'}');
				*position2 = 0;

				float thisPower = atof(position);
				values[writing].push_back(thisPower);

				writing = (writing + 1) % values.size();
			}
			else
			{
				line = newLine;
				if (boost::starts_with(line,"----Epoch"))
				{
					vector<string> splitLine;
					split(splitLine,line, is_any_of(" "),token_compress_on);
					epochTime = lexical_cast<float>(splitLine[1]);
				}
				else if (starts_with(line,"----Command Line"))
				{
					trim(line);
					vector<string> splitLine;
					split(splitLine,line,is_any_of(":"));
					commandLine = splitLine[1];
					cerr << commandLine << endl;

					p << "set title \"{/=12 Power Consumed vs. Time}\\n{/=10 " << commandLine << "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";

					bf::path fileName = outputDir / ("energy." + extension);
					filesGenerated.push_back(fileName.native_directory_string());
					cerr << fileName.native_directory_string();

					p2 << "set output \"" << fileName.native_directory_string() << "\"\n";
					p2 << powerScripts[2] << endl;
					p2 << "set title \"{/=12 Energy vs. Time}\\n{/=10 " << commandLine << "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
					p2 << "plot '-' u 1:2 sm csp t \"Energy (P t)\" w lines lw 2.00, '-' u 1:2 sm csp t \"IBM Energy (P^{2} t^{2})\" w lines lw 2.00\n";
				}
				else if (line[1] == '+')
				{
					bf::path fileName = outputDir / ("power." + extension);
					filesGenerated.push_back(fileName.native_directory_string());
					p << "set output \"" << fileName.native_directory_string() << "\"\n";
					p << powerScripts[0] << endl;

					vector<string> splitLine;
					split(splitLine,line,is_any_of("[]"));
					channelCount = lexical_cast<unsigned>(splitLine[1]);
					//unsigned ranks = lexical_cast<unsigned>(splitLine[3]);

					p << "plot ";

					for (unsigned a = 0; a < channelCount; a++)
					{
						for (unsigned b = 0; b < 5; b++)
						{
							p << "'-' using 1 axes x2y1 title \"P_{sys}(" << powerTypes[b] << ") ch[" << a << "]\",";
						}
					}
					p << "'-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n";

					values.reserve(channelCount * 5);

					for (int i  = channelCount * 5; i > 0; --i)
					{
						values.push_back(vector<float>());
					}	
				}
			}
		}
		inputStream.getline(newLine,256);
	}

	userStop = false;
	
	// make the main power graph
	for (vector<vector<float> >::const_iterator i = values.begin(); i != values.end(); i++)
	{
		for (vector<float>::const_iterator j = i->begin(); j != i->end(); j++)
		{
			p << *j << endl;
		}
		p << "e" << endl;
	}
	p << "0 0" << endl << values.size() * epochTime << " 0.2" << endl << "e" << endl;

	p << powerScripts[1];

	// make the average power line
	CumulativePriorMovingAverage cumulativePower;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); i++)
	{
		for (vector<unsigned>::size_type j = 0; j < values.size(); j++)
			cumulativePower.add(1.0,values[j][i]);
		p << i * epochTime << " " << cumulativePower.getAverage() << endl;
	}
	p << "e" << endl;

	// make the total power bar graphs
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); i++)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); j++)
			totalPowerPerEpoch += values[j][i];
		p << i * epochTime << " " << totalPowerPerEpoch << endl;
	}
	p << "e" << endl;

	PriorMovingAverage powerMovingAverage(WINDOW);

	// moving window average
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); i++)
	{
		for (vector<unsigned>::size_type j = 0; j < values.size(); j++)
			powerMovingAverage.append(values[j][i]);
		p << i * epochTime << " " << powerMovingAverage.getAverage() << endl;
	}
	p << "e" << endl << "unset multiplot" << endl << "unset output" << endl << "exit" << endl;

	// various energy graphs
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); i++)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); j++)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * epochTime << endl;
	}
	p2 << "e" << endl;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); i++)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); j++)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * totalPowerPerEpoch * epochTime * epochTime << endl;
	}

	p2 << "e" << endl << powerScripts[3] << endl;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); i++)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); j++)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * epochTime * epochTime << endl;
	}
	p2 << "e" << endl;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); i++)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); j++)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * totalPowerPerEpoch * epochTime * epochTime * epochTime << endl;
	}
	p2 << "e" << endl << "unset multiplot" << endl << "unset output" << endl << "exit" << endl;

	p.close();
	p2.close();

	thumbNail(filesGenerated);

	bf::path givenfilename(filename);
	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, channelCount);
}

void prepareOutputDir(const bf::path &outputDir, const string &filename, const string &commandLine, unsigned channelCount)
{
	bf::path openfile = executableDirectory / "template.html";

	if (!fileExists(openfile.native_directory_string()))
	{
		cerr << "cannot find template";
		return;
	}
	bf::path printFile = outputDir / (filename.substr(0,filename.find("-stats")) + ".html");

	if (!fileExists(printFile.native_directory_string()))
	{
		ifstream is(openfile.directory_string().c_str(), ios::in | ios::ate);
		ifstream::pos_type entireFileLength = is.tellg();
		char *entireFile = new char[entireFileLength];

		is.seekg(0,ios::beg);
		is.read(entireFile,entireFileLength);
		is.close();
		string templateFile(entireFile);
		string find("@@@");
		templateFile = templateFile.replace(templateFile.find(find),find.length(),commandLine);
		delete entireFile;
		string distroStrings;
		for (unsigned i = 0; i < channelCount; i++)
			distroStrings += "<h2>Address Distribution, Channel "+ lexical_cast<string>(i) + "</h2><a rel=\"lightbox\" href=\"addressDistribution" + lexical_cast<string>(i) + ".svgz\"><img class=\"fancyzoom\" src=\"addressDistribution"+ lexical_cast<string>(i) + "-thumb.png\" alt=\"\" /></a>";
		for (unsigned i = 0; i < channelCount; i++)
			distroStrings += "<h2>Address Latency Distribution, Channel "+ lexical_cast<string>(i) + "</h2><a rel=\"lightbox\" href=\"addressLatencyDistribution"+ lexical_cast<string>(i) + ".svgz\"><img class=\"fancyzoom\" src=\"addressLatencyDistribution"+ lexical_cast<string>(i) + "-thumb.png\" alt=\"\" /></a>";
		cerr << endl << printFile.native_directory_string() << endl;
		std::ofstream out(printFile.native_directory_string().c_str());

		find = "+++";
		templateFile = templateFile.replace(templateFile.find(find),find.length(),distroStrings);
		out << templateFile;
		out.close();
	}
	else
		cerr << "skipping creation of html file(" <<printFile.string() << "), exists" << endl;


	bf::path htaccessOut = outputDir / ".htaccess";
	if (!fileExists(htaccessOut.string()))
	{
		bf::path htaccessFile = executableDirectory / ".htaccess";
		bf::copy_file(htaccessFile, htaccessOut);
	}
	else
		cerr << "skipping creation of htaccess file, exists" << endl;
}
void processStats(const string filename)
{
	// create the output dir
	bf::path outputDir(filename.substr(0,filename.find("-stats")));

	if (!fileExists(filename))
	{
		cerr << "cannot find " << filename << endl;
		return;
	}

	if (!bf::exists(outputDir))
	{
		if (!bf::create_directory(outputDir))
		{
			cerr << "Could not create dir " << outputDir.leaf();
			exit(-1);
		}
	}
	else
	{
		if (!bf::is_directory(outputDir))
		{
			cerr << "Directory " << outputDir.leaf() << " exists, but is not a directory." << endl;
			exit(-1);
		}
	}

	//vector<vector<unsigned> > channels;
	//vector<unsigned> channelTotals;
	//vector<vector<unsigned> > ranks;
	//vector<unsigned> rankTotals;
	//vector<vector<unsigned> > banks;
	//vector<unsigned> bankTotals;

	//vector<vector<unsigned> > channelLatencies;
	//vector<unsigned> channelLatencyTotals;
	//vector<vector<unsigned> > rankLatencies;
	//vector<unsigned> rankLatencyTotals;
	//vector<vector<unsigned> > bankLatencies;
	//vector<unsigned> bankLatencyTotals;

	vector<vector<vector<vector<unsigned> > > > channelDistribution;
	vector<vector<vector<vector<unsigned> > > > channelLatencyDistribution;

	//unsigned commandTurnaroundCounter = 0;
	//unsigned cmdCounter = 0;
	unsigned workingSetCounter = 0;
	//unsigned rwTotalCounter = 0;
	//unsigned ipcCounter = 0;

	std::list<string> filesGenerated;

	opstream p0("gnuplot");
	p0 << terminal << basicSetup;
	opstream p1("gnuplot");
	p1 << terminal << basicSetup;
	opstream p2("gnuplot");
	p2 << terminal << basicSetup;
	opstream p3("gnuplot");
	p3 << terminal << basicSetup;

	//unsigned latencyVsPC = 0;
	std::tr1::unordered_map<boost::uint64_t,float> latencyVsPCDict;
	vector<float> transactionLatency;
	WeightedAverage averageTransactionLatency;
	vector<unsigned> transactionCount;
	unsigned perEpochTransactionCount;

	std::tr1::unordered_map<unsigned,unsigned> distTransactionLatency;
	unsigned transCounter = 0;

	vector<float> hitMissValues;

	vector<unsigned> iCacheHits;
	vector<unsigned> iCacheMisses;
	vector<long> iCacheMissLatency;
	vector<unsigned> dCacheHits;
	vector<unsigned> dCacheMisses;
	vector<long> dCacheMissLatency;
	vector<unsigned> l2CacheHits;
	vector<unsigned> l2CacheMisses;
	vector<long> l2CacheMissLatency;
	vector<unsigned> l2MshrHits;
	vector<unsigned> l2MshrMisses;
	vector<long> l2MshrMissLatency;

	vector<pair<unsigned,unsigned> > bandwidthValues;

	vector<float> ipcValues;
	PriorMovingAverage ipcBuffer(WINDOW);
	CumulativePriorMovingAverage ipcTotal;

	bool started = false;
	unsigned ipcLinesWritten = 0;
	//unsigned writeLines = 0;
	float epochTime = 0.0F;
	float period = 0.0F;
	unsigned channelCount = 0;
	unsigned rankCount = 0;
	unsigned bankCount = 0;
	unsigned writing = 0;
	string commandLine;


	filtering_istream inputStream;
	if (ends_with(filename,"gz"))
		inputStream.push(gzip_decompressor());
	else if (ends_with(filename,"bz2"))
		inputStream.push(bzip2_decompressor());

	inputStream.push(file_source(filename));


	char newLine[256];

	inputStream.getline(newLine,256);

	while ((newLine[0] != 0) && (!userStop))
	{
		if (newLine[0] == '-')
		{
			if (writing == 1)
			{
				// append the values for this time
				transactionLatency.push_back(averageTransactionLatency.average());
				transactionCount.push_back(perEpochTransactionCount);
				perEpochTransactionCount = 0;
			}
			else if (writing == 14)
			{
				for (unsigned i = 0; i < channelDistribution.size(); i++)
					for (unsigned j = 0; j < channelDistribution[i].size(); j++)
					{
						unsigned total = 0;
						for (unsigned k = 0; k < channelDistribution[i][j].size() - 1; k++)
							total += channelDistribution[i][j][k].back();
						channelDistribution[i][j].back().push_back((total > 0) ? total : 1);
					}
			}
			else if (writing == 15)
			{
				for (unsigned i = 0; i < channelLatencyDistribution.size(); i++)
					for (unsigned j = 0; j < channelLatencyDistribution[i].size(); j++)
					{
						unsigned total = 0;
						for (unsigned k = 0; k < channelLatencyDistribution[i][j].size() - 1; k++)
							total += channelLatencyDistribution[i][j][k].back();
						channelLatencyDistribution[i][j].back().push_back((total > 0) ? total : 1);
					}
			}
			writing = 0;

			if (!started)
			{
				if (starts_with(newLine,"----Command Line"))
				{
					string line(newLine);
					trim(line);
					vector<string> splitLine;
					split(splitLine,line,is_any_of(":"));
					cerr << splitLine[1] << endl;
					commandLine = splitLine[1];
					p3 << "set title 'Working Set Size vs Time\\n" << commandLine << "' offset character 0, -1, 0 font '' norotate" << endl;
					bf::path outFilename = outputDir / ("workingSet." + extension);
					p3 << "set output '" << outFilename.native_directory_string() << "'" << endl;
					filesGenerated.push_back(outFilename.native_directory_string());
					p3 << workingSetSetup << endl;

					started = true;

					regex channelSearch("ch\\[([0-9]+)\\]");
					cmatch what;
					
					if (regex_search(newLine,what,channelSearch))
					{
						string value(what[1].first,what[1].second);
						channelCount = lexical_cast<unsigned>(value);
					}
					else
						exit(-1);

					regex rankSearch("rk\\[([0-9]+)\\]");
					if (regex_search(newLine,what,rankSearch))
					{
						string value(what[1].first,what[1].second);
						rankCount = lexical_cast<unsigned>(value);
					}
					else
						exit(-1);

					regex bankSearch("bk\\[([0-9]+)\\]");
					if (regex_search(newLine,what,bankSearch))
					{
						string value(what[1].first,what[1].second);
						bankCount = lexical_cast<unsigned>(value);
					}
					else
						exit(-1);

					channelLatencyDistribution.reserve(channelCount);

					for (unsigned i = channelCount; i > 0; --i)
					{
						channelLatencyDistribution.push_back(vector<vector<vector<unsigned> > >());
						channelLatencyDistribution.back().reserve(rankCount);
						channelDistribution.push_back(vector<vector<vector<unsigned> > >());
						channelDistribution.back().reserve(rankCount);

						for (unsigned j = rankCount; j > 0; --j)
						{
							channelLatencyDistribution.back().push_back(vector<vector<unsigned> >());
							channelLatencyDistribution.back().back().reserve(bankCount + 1);
							channelDistribution.back().push_back(vector<vector<unsigned> >());
							channelDistribution.back().back().reserve(bankCount + 1);

							for (unsigned k = bankCount + 1; k > 0; --k)
							{
								channelLatencyDistribution.back().back().push_back(vector<unsigned>());
								channelLatencyDistribution.back().back().back().reserve(65536);
								channelDistribution.back().back().push_back(vector<unsigned>());
								channelDistribution.back().back().back().reserve(65536);
							}
						}
					}

// 					for (unsigned i = numberChannels; i > 0; --i)
// 						channels.push_back(vector<unsigned>());
// 					for (unsigned i = numberRanks; i > 0; --i)
// 						ranks.push_back(vector<unsigned>());
// 					for (unsigned i = numberBanks; i > 0; --i)
// 						banks.push_back(vector<unsigned>());
				}
			}
			else if (starts_with(newLine, "----M5 Stat:"))
			{
				char *position = strchr(newLine, ' ');
				position = strchr(position + 1, ' ');
				*position++ = 0;
				char* splitline2 = position;
				position = strchr(splitline2, ' ');
				//unsigned len = position - newLine;
				*position++ = 0;
				char *position2 = strchr(position, ' ');
				*position2 = 0;

				if (strcmp(splitline2,"system.cpu.dcache.overall_hits") == 0)
					dCacheHits.push_back(lexical_cast<unsigned>(position));
				else if (strcmp(splitline2,"system.cpu.dcache.overall_miss_latency") == 0)
					dCacheMissLatency.push_back(lexical_cast<float>(position));
				else if (strcmp(splitline2,"system.cpu.dcache.overall_misses") == 0)
					dCacheMisses.push_back(lexical_cast<unsigned>(position));
				else if (strcmp(splitline2,"system.cpu.icache.overall_hits") == 0)
					iCacheHits.push_back(lexical_cast<unsigned>(position));
				else if (strcmp(splitline2,"system.cpu.icache.overall_miss_latency") == 0)
					iCacheMissLatency.push_back(lexical_cast<float>(position));
				else if (strcmp(splitline2,"system.cpu.icache.overall_misses") == 0)
					iCacheMisses.push_back(lexical_cast<unsigned>(position));
				else if (strcmp(splitline2,"system.l2.overall_hits") == 0)
					l2CacheHits.push_back(lexical_cast<unsigned>(position));
				else if (strcmp(splitline2,"system.l2.overall_miss_latency") == 0)
					l2CacheMissLatency.push_back(lexical_cast<float>(position));
				else if (strcmp(splitline2,"system.l2.overall_misses") == 0)
					l2CacheMisses.push_back(lexical_cast<unsigned>(position));
				else if (strcmp(splitline2,"system.l2.overall_mshr_hits") == 0)
					l2MshrHits.push_back(lexical_cast<unsigned>(position));
				else if (strcmp(splitline2,"system.l2.overall_mshr_miss_latency") == 0)
					l2MshrMissLatency.push_back(lexical_cast<float>(position));
				else if (strcmp(splitline2,"system.l2.overall_mshr_misses") == 0)
					l2MshrMisses.push_back(lexical_cast<unsigned>(position));
				else
					cerr <<  "missed something: " << newLine << endl;
			}
			else if (newLine[4] == 'E')
			{
				epochTime = lexical_cast<float>(strchr(newLine, ' ') + 1);
			}
			else if (starts_with(newLine,"----Datarate"))
			{
				period = 1 / lexical_cast<float>(strchr(newLine, ' ') + 1) / 0.000000001F;
			}
			else
			{
				if (starts_with(newLine,"----Transaction Latency"))
				{
					averageTransactionLatency.clear();
					transCounter++;
					writing = 1;
				}
				else if (newLine[4] == 'W')
					writing = 2;
				else if (newLine[4] == 'D')
					writing = 3;
				else if (newLine[4] == 'C' && newLine[5] == 'M')
					writing = 4;
				else if (newLine[4] == 'C' && newLine[5] == 'o')
					writing = 6;
				else if (starts_with(newLine,"----Band"))
					writing = 7;
				else if (newLine[4] == 'A')
					writing = 8;
				else if (newLine[4] == 'I')
				{
					ipcLinesWritten = 0;
					writing = 9;
				}
				else if (starts_with(newLine,"----Row"))
					writing = 10;
				else if (starts_with(newLine,"----Channel"))
				{
// 					channelTotals.push_back(1);
// 					for (unsigned i = 0; i < channels.size(); i++)
// 						channels[i].push_back(0);
					writing = 11;
				}
				else if (starts_with(newLine,"----Rank"))
				{
// 					rankTotals.push_back(1);
// 					for (unsigned i = 0; i < ranks.size(); i++)
// 						ranks[i].push_back(0);
					writing = 12;
				}
				else if (starts_with(newLine,"----Bank"))
				{
// 					bankTotals.push_back(1);
// 					for (unsigned i = 0; i < banks.size(); i++)
// 						banks[i].push_back(0);
					writing = 13;
				}
				else if (starts_with(newLine,"----Utilization"))
				{
// 					channelTotals.push_back(1);
// 					rankTotals.push_back(1);
// 					bankTotals.push_back(1);
// 					for (unsigned i = 0; i < channels.size(); i++)
// 						channels[i].push_back(0);
// 					for (unsigned i = 0; i < ranks.size(); i++)
// 						ranks[i].push_back(0);
// 					for (unsigned i = 0; i < banks.size(); i++)
// 						banks[i].push_back(0);
					writing = 14; 
				}
				else if (starts_with(newLine,"----Latency Breakdown"))
					writing = 15;
				else
					writing = 0;
			}

		}
		// data in this section
		else
		{
			// transaction latency
			if (writing == 1)
			{
				char *position = strchr(newLine,' ');
				*position++ = 0;
				unsigned latency = atoi(newLine);
				unsigned count = atoi(position);
				averageTransactionLatency.add(latency,count);
				perEpochTransactionCount += count;
				distTransactionLatency[latency] += count;
			}
			// working set size
			else if (writing == 2)
			{
				string line(newLine);
				trim(line);
				p3 << workingSetCounter * epochTime << " " << line << endl;
				workingSetCounter++;
			}
			// bandwidth
			else if (writing == 7)
			{
				char *position = strchr(newLine,' ');
				*position++ = 0;

				unsigned readValue = atoi(newLine);
				unsigned writeValue = atoi(position);
				bandwidthValues.push_back(std::pair<unsigned,unsigned>(readValue,writeValue));				
			}
			// PC vs latency
			else if (writing == 8)
			{
				char *position = strchr(newLine, ' ') ;
				assert(position != NULL);
				*position++ = 0;
				// ignore stack addresses for now
				if ((position - newLine) < 12)
				{
					char *position2 = strchr(position,' ') ;
					*position2 = 0;
					string line(newLine);
					uint64_t PC = toNumeric<uint64_t>(line,std::hex);

					float numberAccesses = atof(position);

					latencyVsPCDict[PC] += numberAccesses;
				}
			}
			// IPC
			else if (writing == 9)
			{
				if (ipcLinesWritten < 1)
				{
					float currentValue = starts_with(newLine,"nan") ? 0.0F : atof(newLine);
					if (currentValue != currentValue)
						currentValue = 0.0F;

					ipcValues.push_back(currentValue);					
				}
				ipcLinesWritten++;
			}
			// hit and miss values
			else if (writing == 10)
			{
				char *position = strchr(newLine, ' ') ;
				*position++ = 0;
				unsigned hitCount = max(atoi(newLine),1);
				unsigned missCount = max(atoi(position),1);
				hitMissValues.push_back(hitCount / ((float)missCount + hitCount));
				}
			// channel breakdown
			else if (writing == 11)
			{
// 				char *position = strchr(newLine, ' ') + 1;
// 
// 				channels[atoi(newLine)].back() = atoi(position);
// 				channelTotals.back() += atoi(position);
			}
			// rank breakdown
			else if (writing == 12)
			{
// 				char *position = strchr(newLine, ' ') + 1;
// 
// 				ranks[atoi(newLine)].back() = atoi(position);
// 				rankTotals.back() += atoi(position);
			}
			// bank breakdown
			else if (writing == 13)
			{
// 				char *position = strchr(newLine, ' ') + 1;
// 
// 				banks[lexical_cast<unsigned>(newLine)].back() = lexical_cast<unsigned>(position);
// 				bankTotals.back() += atoi(position);
			}
			// new-style per bank breakdown
			else if (writing == 14)
			{
				char *splitLine1 = strchr(newLine,'(') + 1;
				char *splitLine2 = strchr(splitLine1,',');
				*splitLine2++ = 0;
				char *splitLine3 = strchr(splitLine2,',');
				*splitLine3++ = 0;
				char *splitLine5 = strchr(splitLine3,')');
				*splitLine5 = 0;
				splitLine5 += 2;
				unsigned channel = atoi(splitLine1);
				unsigned rank = atoi(splitLine2);
				unsigned bank = atoi(splitLine3);
				unsigned value = atoi(splitLine5);
			
				channelDistribution[channel][rank][bank].push_back(value);

				//channels[channel].back() += value;
				//ranks[rank].back() += value;
				//banks[bank].back() += value;
				//channelTotals.back() += value;
				//rankTotals.back() += value;
				//bankTotals.back() += value;
			}
			// latency distribution graphs
			else if (writing == 15)
			{
				char *splitLine1 = strchr(newLine,'(') + 1;
				char *splitLine2 = strchr(splitLine1,',');
				*splitLine2++ = 0;
				char *splitLine3 = strchr(splitLine2,',');
				*splitLine3++ = 0;
				char *splitLine5 = strchr(splitLine3,')');
				*splitLine5 = 0;
				splitLine5 += 2;
				unsigned channel = atoi(splitLine1);
				unsigned rank = atoi(splitLine2);
				unsigned bank = atoi(splitLine3);
				unsigned value = atoi(splitLine5);
				//unsigned channel = lexical_cast<unsigned>(splitLine1);
				//unsigned rank = lexical_cast<unsigned>(splitLine2);
				//unsigned bank = lexical_cast<unsigned>(splitLine3);
				//unsigned value = lexical_cast<unsigned>(splitLine5);

				channelLatencyDistribution[channel][rank][bank].push_back(value);
			}
		}

		inputStream.getline(newLine,256);
	} // end going through lines

	userStop = false;

	if (writing == 14)
	{
		for (unsigned i = 0; i < channelDistribution.size(); i++)
			for (unsigned j = 0; j < channelDistribution[i].size(); j++)
			{
				unsigned total = 0;
				for (unsigned k = 0; k < channelDistribution[i][j].size() - 1; k++)
					total += channelDistribution[i][j][k].back();
				channelDistribution[i][j].back().push_back((total > 0) ? total : 1);
			}
	}
	else if (writing == 15)
	{
		for (unsigned i = 0; i < channelLatencyDistribution.size(); i++)
			for (unsigned j = 0; j < channelLatencyDistribution[i].size(); j++)
			{
				unsigned total = 0;
				for (unsigned k = 0; k < channelLatencyDistribution[i][j].size() - 1; k++)
					total += channelLatencyDistribution[i][j][k].back();
				channelLatencyDistribution[i][j].back().push_back((total > 0) ? total : 1);
			}
	}

	p3 << "e" << endl << "unset output" << endl;

	// make the address latency distribution per channel graphs
	for (unsigned channelID = 0; channelID < channelLatencyDistribution.size(); channelID++)
	{
		bf::path filename = outputDir / ("addressLatencyDistribution" + lexical_cast<string>(channelID) + "." + extension);
		filesGenerated.push_back(filename.native_directory_string());
		p2 << "set output '" << filename.native_directory_string() << "'" << endl << subAddrDistroA;
		float rankFraction = 1.0F / channelLatencyDistribution[channelID].size();
		float offset = 1 - rankFraction;
		offset = 1.0F - rankFraction;
		for (unsigned rankID = 0; rankID < channelLatencyDistribution[channelID].size(); rankID++)
		{
			p2 << "set size 1.0, " << rankFraction << endl << "set origin 0.0, " << offset << endl;
			p2 << "set title \"Rank " << rankID << "Distribution Rate";
			if (rankID == 0)
				p2 << "\\n" << commandLine;
			p2 << "\" offset character 0, 0, 0 font \"\" norotate" << endl;
			offset -= rankFraction;
			if (rankID < channelLatencyDistribution[channelID].size() - 1)
				p2 << "unset key" << endl << "unset label" << endl;
			else
				p2 << "set xlabel 'Time (s)' offset 0,0.6" << endl << "set key outside center bottom horizontal reverse Left" << endl;
			p2 << "plot ";
			for (unsigned a = 0; a < channelLatencyDistribution[channelID][rankID].size() - 1; a++)
				p2 << "'-' using 1 axes x2y1 t 'bank_{" << a << "}  ',";
			p2 << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

			for (unsigned bankID = 0; bankID < channelLatencyDistribution[channelID][rankID].size() - 1; bankID++)
			{
				for (unsigned epoch = 0; epoch < channelLatencyDistribution[channelID][rankID][bankID].size(); epoch++)
				{
					p2 << channelLatencyDistribution[channelID][rankID][bankID][epoch] / ((float)channelLatencyDistribution[channelID][rankID].back()[epoch]) << endl;
					//cerr << channelLatencyDistribution[channelID][rankID][bankID][epoch] / ((float)channelLatencyDistribution[channelID][rankID].back()[epoch]) << endl;
				}
				p2 << "e" << endl;
			}
			p2 << channelLatencyDistribution[channelID][rankID][0].size() * epochTime << " " << "0.2" << endl << "e" << endl;
			//cerr << channelLatencyDistribution[channelID][rankID][0].size() * epochTime << endl << "e" << endl;
		}
		p2 << "unset multiplot" << endl << "unset output" << endl;
	}

	// make the address distribution per channel graphs
	for (unsigned channelID = 0; channelID < channelDistribution.size(); channelID++)
	{
		bf::path filename = outputDir / ("addressDistribution" + lexical_cast<string>(channelID) + "." + extension);
		filesGenerated.push_back(filename.native_directory_string());
		p1 << "set output '" << filename.native_directory_string() << "'" << endl << subAddrDistroA;
		float rankFraction = 1.0F / channelDistribution[channelID].size();
		float offset = 1 - rankFraction;
		offset = 1.0F - rankFraction;
		for (unsigned rankID = 0; rankID < channelDistribution[channelID].size(); rankID++)
		{
			p1 << "set size 1.0, " << rankFraction << endl << "set origin 0.0, " << offset << endl;
			p1 << "set title \"Rank " << rankID << " Distribution Rate";
			if (rankID == 0)
				p1 << "\\n" << commandLine;
			p1 << "\" offset character 0, 0, 0 font \"\" norotate" << endl;
			offset -= rankFraction;
			if (rankID < channelDistribution[channelID].size() - 1)
				p1 << "unset key" << endl << "unset label" << endl;
			else
				p1 << "set xlabel 'Time (s)' offset 0,0.6" << endl << "set key outside center bottom horizontal reverse Left" << endl;
			p1 << "plot ";
			for (unsigned a = 0; a < channelDistribution[channelID][rankID].size() - 1; a++)
				p1 << "'-' using 1 axes x2y1 t 'bank_{" << a << "}  ',";
			p1 << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

			for (unsigned bankID = 0; bankID < channelDistribution[channelID][rankID].size() - 1; bankID++)
			{
				for (unsigned epoch = 0; epoch < channelDistribution[channelID][rankID][bankID].size(); epoch++)
				{
					p1 << channelDistribution[channelID][rankID][bankID][epoch] / ((float)channelDistribution[channelID][rankID].back()[epoch]) << endl;
				}
				p1 << "e" << endl;
			}
			p1 << channelDistribution[channelID][rankID][0].size() * epochTime << " " << "0.2" << endl << "e" << endl;
		}
		p1 << "unset multiplot" << endl << "unset output" << endl;
	}

	// make the address distribution graphs
	bf::path outFilename = outputDir / ("addressDistribution." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p0 << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p0 << "set title \"" << commandLine << "\\nChannel Distribution Rate\"" << endl << addressDistroA << endl;
	p0 << "plot ";
	for (unsigned i = 0; i < channelDistribution.size(); i++)
		p0 << "'-' using 1 axes x2y1 t 'ch[" << i << "',";
	p0 << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

	for (unsigned channelID = 0; channelID < channelDistribution.size(); channelID++)
	{	
		for (unsigned epochID = 0; epochID < channelDistribution[0][0][0].size(); epochID++)
		{
			unsigned thisChannelTotal = 0;
			unsigned allChannelTotal = 1;
			for (unsigned rankID = 0; rankID < rankCount; rankID++)
			{
				for (unsigned bankID = 0; bankID < bankCount; bankID++)
				{
					thisChannelTotal += channelDistribution[channelID][rankID][bankID][epochID];
					for (unsigned channelID2 = 0; channelID2 < channelCount; channelID2++)
						allChannelTotal += channelDistribution[channelID2][rankID][bankID][epochID];
				}
			}
			p0 << thisChannelTotal / ((float)allChannelTotal) << endl;
		}
		p0 << "e" << endl;
	}

	p0 << channelDistribution[0][0][0].size() * epochTime << " " << "0.2" << endl << "e" << endl << addressDistroB << endl << "plot ";

	for (unsigned i = 0; i < rankCount; i++)
		p0 << "'-' using 1 axes x2y1 t 'rk[" << i << "',";
	p0 << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

	for (unsigned rankID = 0; rankID < rankCount; rankID++)
	{	
		for (unsigned epochID = 0; epochID < channelDistribution[0][0][0].size(); epochID++)
		{
			unsigned thisRankTotal = 0;
			unsigned allRankTotal = 1;
			for (unsigned channelID = 0; channelID < channelCount; channelID++)
			{
				for (unsigned bankID = 0; bankID < bankCount; bankID++)
				{
					thisRankTotal += channelDistribution[channelID][rankID][bankID][epochID];
					for (unsigned rankID2 = 0; rankID2 < rankCount; rankID2++)
						allRankTotal += channelDistribution[channelID][rankID2][bankID][epochID];
				}
			}
			p0 << thisRankTotal / ((float)allRankTotal) << endl;
		}
		p0 << "e" << endl;
	}
	p0 << channelDistribution[0][0][0].size() * epochTime << " " << "0.2" << endl << "e" << endl << addressDistroC << endl << "plot ";
	for (unsigned i = 0; i < bankCount; i++)
		p0 << "'-' using 1 axes x2y1 t 'bk[" << i << "',";
	p0 << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

	for (unsigned bankID = 0; bankID < channelDistribution.size(); bankID++)
	{	
		for (unsigned epochID = 0; epochID < channelDistribution[0][0][0].size(); epochID++)
		{
			unsigned thisBankTotal = 0;
			unsigned allBankTotal = 1;
			for (unsigned rankID = 0; rankID < rankCount; rankID++)
			{
				for (unsigned channelID = 0; channelID < channelCount; channelID++)
				{
					thisBankTotal += channelDistribution[channelID][rankID][bankID][epochID];
					for (unsigned bankID2 = 0; bankID2 < bankCount; bankID2++)
						allBankTotal += channelDistribution[channelID][rankID][bankID2][epochID];
				}
			}
			p0 << thisBankTotal / ((float)allBankTotal) << endl;
		}
		p0 << "e" << endl;
	}
	p0 << channelDistribution[0][0][0].size() * epochTime << " " << "0.2" << endl << "e" << endl << "unset multiplot" << endl << "unset output" << endl;

	// make the PC vs latency graph
	outFilename = outputDir / ("latencyVsPc." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p1 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p1 << "set title \"Total Latency Due to Reads vs. PC Value\\n" << commandLine << "\""<< endl << pcVsLatencyGraph0 << endl;
	std::list<uint64_t> highValues;
	for (std::tr1::unordered_map<uint64_t,float>::const_iterator i = latencyVsPCDict.begin(); i != latencyVsPCDict.end(); i++)
	{
		if (i->first < 0x100000000)
			p1 << i->first << " " << period * i->second << endl;
	}
	p1 << endl << "e" << endl << pcVsLatencyGraph1;
	for (std::tr1::unordered_map<uint64_t,float>::const_iterator i = latencyVsPCDict.begin(); i != latencyVsPCDict.end(); i++)
	{
		if (i->first >= 0x100000000)
			p1 << (i->first - 0x100000000) << " " << period * i->second << endl;
	}
	p1 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;

	// make the transaction latency distribution graph
	outFilename = outputDir / ("transactionLatencyDistribution." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title \"Read Transaction Latency\\n" << commandLine << "\""<< endl << transactionGraph << endl;
	for (std::tr1::unordered_map<unsigned,unsigned>::const_iterator i = distTransactionLatency.begin(); i != distTransactionLatency.end(); i++)
		p2 << i->first * period << " " << i->second << endl;
	p2 << "e" << endl << "unset output" << endl;

	// make the bandwidth graph
	outFilename = outputDir / ("bandwidth." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p3 << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p3 << "set title 'System Bandwidth\\n" << commandLine << "'"<< endl << bandwidthGraph << endl;

	for (vector<pair<unsigned,unsigned> >::const_iterator i = bandwidthValues.begin(); i != bandwidthValues.end(); i++)
		p3 << 1.0 * i->first << endl;
	p3 << "e" << endl;

	for (vector<pair<unsigned,unsigned> >::const_iterator i = bandwidthValues.begin(); i != bandwidthValues.end(); i++)
		p3 << 1.0 * i->second << endl;
	p3 << "e" << endl;

	float time = 0.0F;
	PriorMovingAverage bandwidthTotal(WINDOW);
	for (vector<pair<unsigned,unsigned> >::const_iterator i = bandwidthValues.begin(); i != bandwidthValues.end(); i++)
	{
		bandwidthTotal.append(1.0 * i->first + i->second);
		p3 << time << " " << bandwidthTotal.getAverage() << endl;
		time += epochTime;
	}
	p3 << "e" << endl;

	// make the small IPC graph
	p3 << smallIPCGraph << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); i++)
	{
		p3 << time << " " << *i << endl;
		time += epochTime;
	}
	p3 << "e" << endl;
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); i++)
	{
		ipcTotal.add(1,*i);
		p3 << time << " " << ipcTotal.getAverage() << endl;
		time += epochTime;
	}
	p3 << "e" << endl;
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); i++)
	{
		ipcBuffer.append(*i);
		p3 << time << " " << ipcBuffer.getAverage() << endl;
		time += epochTime;
	}
	p3 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;

	// make the cache graph
	outFilename = outputDir / ("cacheData." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p0 << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p0 << "set title 'Miss Rate of L1 ICache\\n" << commandLine << "'"<< endl << cacheGraphA << endl;

	for (unsigned i = 0; i < iCacheMisses.size(); i++)
		p0 << i * epochTime << " " << iCacheMisses[i] + iCacheHits[i] << endl;
	p0 << "e" << endl;
	for (unsigned i = 0; i < iCacheMisses.size(); i++)
		p0 << i * epochTime << " " << iCacheMisses[i] / ((float)max(iCacheMisses[i] + iCacheHits[i],1U)) << endl;
	p0 << "e" << endl;

	p0 << cacheGraphB << endl;
	for (unsigned i = 0; i < dCacheMisses.size(); i++)
		p0 << i * epochTime << " " << dCacheMisses[i] + dCacheHits[i] << endl;
	p0 << "e" << endl;
	for (unsigned i = 0; i < dCacheMisses.size(); i++)
		p0 << i * epochTime << " " << dCacheMisses[i] / ((float)max(dCacheMisses[i] + dCacheHits[i],1U)) << endl;
	p0 << "e" << endl;

	p0 << cacheGraphC << endl;
	for (unsigned i = 0; i < l2CacheMisses.size(); i++)
		p0 << i * epochTime << " " << l2CacheMisses[i] + l2CacheHits[i] << endl;
	p0 << "e" << endl;
	for (unsigned i = 0; i < l2CacheMisses.size(); i++)
		p0 << i * epochTime << " " << l2CacheMisses[i] / ((float)max(l2CacheMisses[i] + l2CacheHits[i],1U)) << endl;
	p0 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;

	// make the other IPC graph
	outFilename = outputDir / ("averageIPCandLatency." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p1 << "reset" << endl << terminal << endl << basicSetup << endl << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p1 << "set title 'Average IPC vs. Time\\n" << commandLine << "'"<< endl << otherIPCGraph << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); i++)
	{
		p1 << time << " " << *i << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	ipcTotal.clear();
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); i++)
	{
		ipcTotal.add(1, *i);
		p1 << time << " " << ipcTotal.getAverage() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	ipcBuffer.clear();
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); i++)
	{
		ipcBuffer.append(*i);
		p1 << time << " " << ipcBuffer.getAverage() << endl;
		time += epochTime;
	}

	// make the transaction latency graph
	p1 << "e" << endl << "set title 'Transaction Latency \\n" << commandLine << "'" << endl << averageTransactionLatencyScript << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = transactionLatency.begin(); i != transactionLatency.end(); i++)
	{
		p1 << time << " " << *i << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	time = 0.0F;
	PriorMovingAverage movingTransactionBuffer(WINDOW);
	for (vector<float>::const_iterator i = transactionLatency.begin(); i != transactionLatency.end(); i++)
	{
		movingTransactionBuffer.append(*i);
		p1 << time << " " << movingTransactionBuffer.getAverage() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	time = 0.0F;
	CumulativePriorMovingAverage average;
	for (vector<float>::const_iterator i = transactionLatency.begin(); i != transactionLatency.end(); i++)
	{
		average.add(1.0,*i);
		p1 << time << " " << average.getAverage() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	time = 0.0F;
	for (vector<unsigned>::const_iterator i = transactionCount.begin(); i != transactionCount.end(); i++)
	{
		p1 << time << " " << *i << endl;
		time += epochTime;
	}
	p1 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;

	// make the hit-miss graph
	outFilename = outputDir / ("rowHitRate." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title 'Reuse Rate of Open Rows vs. Time\\n" << commandLine << "'"<< endl << rowHitMissGraph << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = hitMissValues.begin(); i != hitMissValues.end(); i++)
	{
		p2 << time << " " << *i << endl;
		time += epochTime;
	}

	CumulativePriorMovingAverage hitMissTotal;
	time = 0.0F;
	for (vector<float>::const_iterator i = hitMissValues.begin(); i != hitMissValues.end(); i++)
	{
		hitMissTotal.add(1.0,*i);
		p2 << time << " " << hitMissTotal.getAverage() << endl;
		time += epochTime;
	}
	p2 << "e" << endl;

	PriorMovingAverage hitMissBuffer(WINDOW);
	time = 0.0F;
	for (vector<float>::const_iterator i = hitMissValues.begin(); i != hitMissValues.end(); i++)
	{
		hitMissBuffer.append(*i);
		p2 << time << " " << hitMissBuffer.getAverage() << endl;
		time += epochTime;
	}
	p2 << "e" << endl << "unset output" << endl;

	p0 << endl << "exit" << endl;
	p1 << endl << "exit" << endl;
	p2 << endl << "exit" << endl;
	p3 << endl << "exit" << endl;

	p0.close();
	p1.close();
	p2.close();
	p3.close();

	thumbNail(filesGenerated);
	bf::path givenfilename(filename);
	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, channelDistribution.size());
	return;
}

void sigproc(int i)
{
	userStop = true;
}

int main(int argc, char** argv)
{
	bf::path dir(argv[0]);
	executableDirectory = dir.branch_path();
	
	signal(SIGUSR1, sigproc);

	for (int i = 0; i < argc; i++)
	{
		string currentValue(argv[i]);
		if (ends_with(currentValue,"power.gz")|| ends_with(currentValue,"power.bz2"))
		{
			processPower(currentValue);
		}
		else if (ends_with(currentValue,"stats.gz")|| ends_with(currentValue,"stats.bz2"))
		{
			processStats(currentValue);
		}
	}
	return 0;
}

