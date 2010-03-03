#include "pstream.h"
#include <boost/circular_buffer.hpp>
#include <cstdio>
//#include <ImageMagick/Magick++.h>
#include <string>
#include <list>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <map>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
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
using std::cout;
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
using boost::erase_all;
using boost::ends_with;
using boost::replace_all;
using boost::token_compress_on;
using boost::trim;
using boost::ireplace_all_copy;
using boost::is_any_of;
using boost::lexical_cast;
using boost::numeric_cast;
using boost::regex;
using boost::cmatch;
using boost::tuple;
using boost::this_thread::sleep;
using boost::mutex;
using boost::filesystem::path;
using std::max;
using std::min;
using std::numeric_limits;
using std::vector;
using std::endl;
using std::map;
using std::ofstream;
using std::ifstream;
using redi::opstream;
using std::ios;
using std::list;
using std::stringstream;

#ifdef WIN32
#include <unordered_map>
using std::unordered_map;
#else
#include <tr1/unordered_map>
using std::tr1::unordered_map;
#endif

// the number of points to use as a maximum in the graph
#define MAXIMUM_VECTOR_SIZE 2 * 1024

#define NEWLINE_LENGTH 1024*1024

#define WINDOW 5

void prepareOutputDir(const bf::path &outputDir, const string &filename, const string &commandLine, unsigned channelCount, list<pair<string,string> > &graphs);

// globals
bf::path executableDirectory;

std::string thumbnailResolution = "800";

//std::string terminal = "set terminal svg size 1920,1200 dynamic enhanced fname \"Arial\" fsize 16\n";
//std::string terminal = "set terminal svg size 2048,1152 dynamic enhanced font \"Arial\" fsize 18\n";
std::string terminal = "set terminal svg size 1920,1200 enhanced font \"Arial\" fsize 14\n";

std::string extension = "svg";

string thumbnailExtenstion = "png";

bool generatePngFiles = false;

bool userStop = false;

bool cypressResults = false;

unordered_map<string,string> &setupDecoder();

unordered_map<string,string> decoder = setupDecoder();

#include "processStats.hh"


#if 0
template <typename T1, typename T2>
pair<T1,T2> operator/(const pair<T1,T2> &lhs, const float& right)
{
	return pair<T1,T2>(lhs.first / right, lhs.second / right );
}

template <typename T1,typename T2>
pair<T1,T2> operator+(pair<T1,T2> &left, const pair<T1,T2>& right)
{
	return pair<T1,T2>(left.first + right.first, left.second + right.second);
}

template <typename T>
class BufferAccumulator
{

protected:
	vector<T> values;
	T buffer;
	unsigned scaleFactor;
	unsigned count;

public:
	BufferAccumulator():
	  values(),
		  buffer(),
		  scaleFactor(1),
		  count(0)
	  {}

	  void push_back(const T& value)
	  {
		  buffer = value / (float)scaleFactor;
		  count++;
		  if (count == scaleFactor)
		  {
			  values.push_back(value);

			  if (values.size() > MAXIMUM_VECTOR_SIZE)
			  {
				  for (unsigned j = 0; j < MAXIMUM_VECTOR_SIZE / 2; ++j)
				  {
					  //values[j] = T((values[2 * j] + values[2 * j + 1]) / 2.0F);
					  values[j] = T((values[2 * j] + values[2 * j + 1]) );
				  }

				  values.resize(MAXIMUM_VECTOR_SIZE / 2);

				  scaleFactor *= 2;
			  }
		  }		
	  }
};
#endif
template <typename T>
class WeightedAverage
{
	unsigned count;
	T total;

public:
	WeightedAverage():count(0), total(0)
	{}

	void add(T value, unsigned count)
	{
		this->total += value * (T)count;
		this->count += count;
	}

	void clear()
	{
		total = 0;
		count = 0;
	}

	T average()
	{
		return total / ((count > 0) ? (T)count : (T)1);
	}
};

// using the method of Mendenhall and Sincich (1995)
template <typename T>
class BoxPlot
{
protected:
	map<T, unsigned> values;
	unsigned totalCount;
public:
	BoxPlot():
	  values(),
		  totalCount(0)
	  {}

	  void clear()
	  {
		  values.clear();
		  totalCount = 0;
	  }

	  void add(const T value, const unsigned count)
	  {
		  // shouldn't be any duplicates
		  if (values.find(value) != values.end())
			  throw;

		  values[value] = count;

		  totalCount += count;
	  }

	  tuple<T, T, T, T, T, T> getQuartiles() const
	  {
		  if (totalCount > 0)
		  {
			  unsigned median = max(round(totalCount / 2.0), 1.0);
			  unsigned firstQuartile = max(round((totalCount + 1.0) / 4.0), 1.0);
			  assert(firstQuartile <= median);
			  unsigned thirdQuartile = max(round((3.0 * totalCount + 3.0) / 4.0), 1.0);
			  if (thirdQuartile > totalCount) thirdQuartile = totalCount;
			  assert(median <= thirdQuartile);

			  T firstQuartileValue;
			  T thirdQuartileValue;
			  T medianValue;

			  unsigned cumulativeSum = 0;

			  typename map<T,unsigned>::const_iterator end = values.end();
			  WeightedAverage<double> wa;

			  // go determine which bin the quartile elements are in
			  for (typename map<T,unsigned>::const_iterator i = values.begin(); i != end; ++i)
			  {
				  wa.add(i->first, i->second);

				  if (firstQuartile > cumulativeSum && firstQuartile <= cumulativeSum + i->second)
					  firstQuartileValue = i->first;
				  if (thirdQuartile > cumulativeSum && thirdQuartile <= cumulativeSum + i->second)
					  thirdQuartileValue = i->first;
				  if (median > cumulativeSum && median <= cumulativeSum + i->second)
				  {
					  // only if it's the last element
					  if (totalCount % 2 == 1 && (cumulativeSum + i->second == median))
					  {
						  typename map<T,unsigned>::const_iterator j(i);
						  ++j;
						  if (j == end)
							  medianValue = i->first;
						  else
							  medianValue = (i->first + j->first) / 2;
					  }	
					  else
						  medianValue = i->first;

				  }
				  cumulativeSum += i->second;
			  }	

			  assert(firstQuartileValue <= medianValue);
			  assert(medianValue <= thirdQuartileValue);

			  return tuple<T, T, T, T, T, T>(values.begin()->first, firstQuartileValue, medianValue, wa.average(), thirdQuartileValue, values.rbegin()->first);
		  }
		  else
			  return tuple<T, T, T, T, T, T>((T)0, (T)0, (T)0, (T)0, (T)0, (T)0);
	  }
};

template <typename T>
class StdDev
{
protected:
	map<T, unsigned> values;
	unsigned totalCount;
public:
	StdDev():
	  values(),
		  totalCount(0)
	  {}

	  void clear()
	  {
		  values.clear();
		  totalCount = 0;
	  }

	  void add(const T value, const unsigned count)
	  {
		  // shouldn't be any duplicates
		  if (values.find(value) != values.end())
			  throw;

		  values[value] = count;

		  totalCount += count;
	  }

	  tuple<T, T, T, T> getStdDev() const
	  {
		  if (totalCount > 0)
		  {	  
			  typename map<T,unsigned>::const_iterator end = values.end();
			  WeightedAverage<double> wa;

			  // go determine which bin the quartile elements are in
			  for (typename map<T,unsigned>::const_iterator i = values.begin(); i != end; ++i)
			  {
				  wa.add(i->first, i->second);
			  }	

			  double mean = wa.average();

			  uint64_t sum = 0;

			  for (typename map<T,unsigned>::const_iterator i = values.begin(); i != end; ++i)
			  {
				  sum += (uint64_t)((double)i->second * ((double)i->first - mean) * ((double)i->first - mean));
			  }	

			  double stdDev = sqrt((double)sum / (double)totalCount);

			  return tuple<T, T, T, T>(values.begin()->first, mean, stdDev, values.rbegin()->first);
		  }
		  else
			  return tuple<T, T, T, T>((T)0, (T)0, (T)0, (T)0);
	  }
};

class CumulativePriorMovingAverage
{
	double average;
	double totalCount;

public:
	CumulativePriorMovingAverage():average(0.0),totalCount(0.0)
	{}

	void add(double count, double value)
	{
		totalCount += count;
		average += ((count * value - average) / totalCount);
	}

	double getAverage() const { return average; }

	void clear()
	{
		average = 0.0;
		totalCount = 0.0;
	}
};


class PriorMovingAverage
{
	boost::circular_buffer<float> data;
	float average;

public:
	PriorMovingAverage(unsigned size):data(size),average(0)
	{}

	double getAverage() const { return average; }

	void append(float x)
	{
		if (data.full())
		{
			float oldestItem = data.front();
			int size = data.size();
			average = average - (oldestItem / size) + (x / size);
		}
		else if (data.empty())
		{
			average = x;
		}
		else
		{
			float size = (float)data.size();
			average = average * size / (size + 1.0F) + x / (size + 1.0F);
		}
		data.push_back(x);
	}

	void clear()
	{
		average = 0.0;
		boost::circular_buffer<float>::size_type size = data.size();
		data.clear();
		data.resize(size, 0.0F);
		//for (boost::circular_buffer<float>::iterator i = data.begin(); i != data.end(); ++i)
		//	*i = 0;
	}
};

template <class T>
T toNumeric(const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	T t;
	iss >> f >> t;
	return t;
}	

bool fileExists(const string& fileName)
{
	bf::path newPath(fileName.c_str());
	return bf::exists(newPath);
}

unordered_map<string,string> &setupDecoder()
{
	static unordered_map<string,string> theMap;

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

bool doneEntering = false;
list<string> fileList;
mutex fileListMutex;

#define USE_USER_CONVERT

#ifdef USE_USER_CONVERT
#define CONVERT_COMMAND "/home/crius/ImageMagick/bin/convert"
#define MOGRIFY_COMMAND "/home/crius/ImageMagick/bin/mogrify"
#else
#define CONVERT_COMMAND "convert"
#define MOGRIFY_COMMAND "mogrify"
#endif


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
				baseFilename = fileList.front().substr(0,fileList.front().find(extension) - 1);
				fileList.pop_front();
			}
			//string commandLine0 = CONVERT_COMMAND + " " + filename + "[" + thumbnailResolution + "] " + baseFilename + "-thumb.png";
			//string commandLine1 = "mogrify -resize 3840 -format png -limit memory 1gb " + filename;

			//second = first;

			//second.sample("3840");

			//second.write(baseFilename + ".png");

			//string commandLine0 = string(CONVERT_COMMAND) + " " + filename + "'[" + thumbnailResolution + "]' " + baseFilename + "-thumb.png";
			string commandLine0 = string(CONVERT_COMMAND) + " " + filename + " -resize " + thumbnailResolution + " " + baseFilename + "-thumb.png";
			string commandLine2 = "gzip -c -9 -f " + filename + " > " + filename + "z";
#ifndef NDEBUG
			cerr << commandLine0 << endl;
#endif
			if (system(commandLine0.c_str()) != 0)
				cerr << "Failed to create thumbnail for " << filename << endl;
			if (system(commandLine2.c_str()) != 0)
				cerr << "Failed to compress " << filename << endl;

			//Image first(filename.c_str());
			// 			Image first(baseFilename + "-thumb.png");
			// 			first.magick("png");
			// 			first.zoom(thumbnailResolution);
			// 			first.write(baseFilename + "-thumb2.png");


			if (generatePngFiles)
			{
				string commandLine1 = string(MOGRIFY_COMMAND) + " -resize 3840 -format png " + filename;
				system(commandLine1.c_str());
			}

			bf::remove(bf::path(filename));
		}
	}
}

void processPower(const string &filename)
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

	list<string> filesGenerated;

	unsigned channelCount;
	float epochTime;

	vector<vector<float> > values;
	vector<float> valueBuffer;
	vector<pair<float,float> > energyValues;
	pair<float,float> energyValueBuffer;

	//BufferAccumulator<pair<float,float> > energy;

	unsigned scaleFactor = 1;
	unsigned scaleIndex = 0;

	string commandLine;

	filtering_istream inputStream;
	inputStream.push(boost::iostreams::gzip_decompressor());
	inputStream.push(file_source(filename));


	char newLine[NEWLINE_LENGTH];
	boost::regex powerRegex("\\{([0-9.e\\-\\+]+)\\}");

	if (!inputStream.is_complete())
		return;

	inputStream.getline(newLine,NEWLINE_LENGTH); 

	list<pair<string,string> > graphs;

	// get a couple copies of gnuplot running
	opstream p("gnuplot");
	p << basicSetup << terminal;
	opstream p2("gnuplot");
	p2 << basicSetup << terminal;
	opstream p3("gnuplot");
	p3 << basicSetup << terminal;
	opstream p4("gnuplot");
	p4 << basicSetup << terminal;


	while ((newLine[0] != 0) && (!userStop))
	{
		if (newLine[0] == '-')
		{
			if (newLine[1] == 'P')
			{
				string backup(newLine);
				char *position = strchr(newLine,'{');
				char *position2 = strchr(newLine,'}');
				if (position == NULL || position2 == NULL)
					break;
				position++;
				*position2 = 0;

				float thisPower = atof(position);
				valueBuffer[writing] += (thisPower);

				// Psys(ACT_STBY)
				if (writing == 0)
				{
					char *firstBrace = strchr(position2 + 1,'{');
					if (firstBrace == NULL) break;

					char *slash = strchr(firstBrace,'/');
					if (slash == NULL) break;

					char *secondBrace = strchr(firstBrace,'}');
					if (secondBrace == NULL) break;

					*slash = *secondBrace = NULL;

					energyValueBuffer.first = atof(firstBrace + 1);
					energyValueBuffer.second = atof(slash + 1);

					//energy.push_back(pair<float,float>(atof(firstBrace + 1),atof(slash + 1)));

					//cerr << energyValueBuffer.first << " " << energyValueBuffer.second << endl;
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

						energyValues.push_back(pair<float,float>(energyValueBuffer.first / scaleFactor, energyValueBuffer.second / scaleFactor));
					}

					// try to compress the array by half and double the scaleFactor
					if (values.front().size() >= MAXIMUM_VECTOR_SIZE)
					{
						// scale the array back by half
						for (vector<vector<float> >::iterator i = values.begin(); i != values.end(); ++i)
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
							energyValues[j] = pair<float,float>(
								(energyValues[2 * j].first + energyValues[2 * j + 1].first) / 2.0F,
								(energyValues[2 * j].second + energyValues[2 * j + 1].second) / 2.0F);
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
				if (boost::starts_with(newLine,"----Epoch"))
				{
					char *position = strchr(newLine, ' ');
					if (position == NULL)
						break;
					position++;
					epochTime = lexical_cast<float>(position);
				}
				else if (starts_with(newLine,"----Command Line"))
				{
					string line(newLine);
					trim(line);
					vector<string> splitLine;
					split(splitLine,line,is_any_of(":"));
					commandLine = splitLine[1];
					cerr << commandLine << endl;

					p << "set title \"{/=18 Power vs. Time}\\n{/=14 " << commandLine << "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
					p3 << "set title \"{/=24 Power vs. Time}\\n{/=18 " << commandLine << "}\"  offset character 0, -1, 0 font \"Arial,15\" norotate\n";

					bf::path fileName = outputDir / ("energy." + extension);
					filesGenerated.push_back(fileName.native_directory_string());
					//cerr << fileName.native_directory_string();

					p2 << "set output \"" << fileName.native_directory_string() << "\"\n";
					p2 << powerScripts[2] << endl;
					p2 << "set title \"{/=18 Energy vs. Time}\\n{/=14 " << commandLine << "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
					p2 << "plot '-' u 1:2 sm csp t \"Energy (P t)\" w lines lw 2.00, '-' u 1:2 sm csp t \"IBM Energy (P^{2} t^{2})\" w lines lw 2.00\n";

					fileName = outputDir / ("bigEnergy." + extension);
					filesGenerated.push_back(fileName.native_directory_string());

					p4 << "set output \"" << fileName.native_directory_string() << "\"\n";
					p4 << bigEnergyScript << endl;
					p4 << "set title \"{/=24 Energy vs. Time}\\n{/=18 " << commandLine << "}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n";
					p4 << "plot '-' u 1:2 axes x1y1 t \"Energy (P t)\" w boxes lt rgb \"#66CF03\" , '-' u 1:2 axes x1y2 t \"Cumulative Energy\" w lines lw 6.00 lt rgb \"#387400\", '-' u 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;
				}
				else if (newLine[1] == '+')
				{
					string line(newLine);
					bf::path fileName = outputDir / ("power." + extension);
					filesGenerated.push_back(fileName.native_directory_string());
					p << "set output \"" << fileName.native_directory_string() << "\"\n";
					p << powerScripts[0] << endl;

					fileName = outputDir / ("bigPower." + extension);
					filesGenerated.push_back(fileName.native_directory_string());
					p3 << "set output \"" << fileName.native_directory_string() << "\"" << endl;
					p3 << bigPowerScript << endl;

					vector<string> splitLine;
					split(splitLine,line,is_any_of("[]"));
					channelCount = lexical_cast<unsigned>(splitLine[1]);

					p << "plot ";
					p3 << "plot ";

					for (unsigned a = 0; a < channelCount; a++)
					{
						for (unsigned b = 0; b < 5; b++)
						{
							p << "'-' using 1 axes x2y1 title \"P_{sys}(" << powerTypes[b] << ") ch[" << a << "]\",";
							p3 << "'-' using 1 axes x2y1 title \"P_{sys}(" << powerTypes[b] << ") ch[" << a << "]\",";
						}
					}
					p << "'-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n";
					p3 << "'-' u 1:2 axes x2y1 notitle with points pointsize 0.01,";
					p3 << "'-' u 1:2 axes x1y1 sm csp t \"Cumulative Average\" w lines lw 6.00 lt rgb \"#225752\",";
					p3 << "'-' u 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

					values.reserve(channelCount * 5);
					energyValues.reserve(channelCount * 5);

					// setup the buffer to be the same size as the value array
					valueBuffer.resize(channelCount * 5);

					for (int i  = channelCount * 5; i > 0; --i)
					{
						values.push_back(vector<float>());
						values.back().reserve(MAXIMUM_VECTOR_SIZE);
					}	
				}
			}
		}
		inputStream.getline(newLine,NEWLINE_LENGTH);
	}

	userStop = false;

	if (values.size() < 1)
		return;

	// make the main power graph and big power graph
	for (vector<vector<float> >::const_iterator i = values.begin(); i != values.end(); ++i)
	{
		for (vector<float>::const_iterator j = i->begin(); j != i->end(); ++j)
		{
			//cerr << *j << " ";
			p << *j << endl;
			p3 << *j << endl;
		}
		p << "e" << endl;
		p3 << "e" << endl;
	}
	p << "0 0" << endl << (!values.empty() ? values.back().size() : 0.0) * epochTime << " 0.2" << endl << "e" << endl;
	p3 << "0 0" << endl << 3.31 / epochTime << " 1e-5" << endl << "e" << endl;

	p << powerScripts[1];

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
		cumulativePower.add(1.0,total);
		p << i * epochTime << " " << cumulativePower.getAverage() << endl;
		p3 << i * epochTime << " " << cumulativePower.getAverage() << endl;
	}
	p << "e" << endl;
	p3 << "e" << endl;
	p3 << "0 0" << endl << 3.31 << " 1e-5" << endl << "e" << endl << "unset output" << endl;


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
	p << "e" << endl << "unset multiplot" << endl << "unset output" << endl << "exit" << endl;

	// various energy graphs
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * epochTime << endl;
		p4 << i * epochTime << " " << totalPowerPerEpoch * epochTime << endl;
	}
	p2 << "e" << endl;
	p4 << "e" << endl;

	double cumulativeEnergy = 0.0F;
	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * totalPowerPerEpoch * epochTime * epochTime << endl;
		cumulativeEnergy += totalPowerPerEpoch * epochTime;
		p4 << i * epochTime << " " << cumulativeEnergy << endl;
	}

	p2 << "e" << endl << powerScripts[3] << endl;
	p4 << "e" << endl << "0 0" << endl << "3.31 1e-5" << endl << "e" << "unset output" << endl;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * epochTime * epochTime << endl;
	}
	p2 << "e" << endl;

	for (vector<unsigned>::size_type i = 0; i < values.back().size(); ++i)
	{
		float totalPowerPerEpoch = 0.0F;
		for (vector<unsigned>::size_type j = 0; j < values.size(); ++j)
			totalPowerPerEpoch += values[j][i];

		p2 << i * epochTime << " " << totalPowerPerEpoch * totalPowerPerEpoch * epochTime * epochTime * epochTime << endl;
	}
	p2 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;

	//////////////////////////////////////////////////////////////////////////
	// the cumulative energy graph
	path outFilename = outputDir / ("cumulativeEnergy." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	p4 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p4 << "set title \"" << "Cumulative Energy\\n" << commandLine << "\"" << endl << cumulPowerScript;

	float time = 0.0F;
	float totalPower = 0.0F;
	for (vector<pair<float,float> >::const_iterator i = energyValues.begin(); i != energyValues.end(); ++i)
	{
		totalPower += i->first;

		p4 << time << " " << totalPower << endl;

		time += epochTime;
	}

	p4 << "e" << endl;

	time = 0.0F;
	totalPower = 0.0F;
	for (vector<pair<float,float> >::const_iterator i = energyValues.begin(); i != energyValues.end(); ++i)
		//for (vector<unsigned>::size_type i = 0; i < energyValues.back().size(); ++i)
	{
		//for (vector<unsigned>::size_type j = 0; j < alternateValues.size(); ++j)
		//	totalPower += alternateValues[j][i];
		totalPower += i->first - i->second;

		p4 << time << " " << totalPower << endl;
		//cerr << time << " " << totalPower << endl;

		//cerr << time << " " << i->first + i->second << endl;
		time += epochTime;
	}
	//////////////////////////////////////////////////////////////////////////
	p4 << "e" << endl << "unset output" << endl;

	p2 << endl << "exit" << endl;
	p3 << endl << "exit" << endl;
	p4 << endl << "exit" << endl;

	p.close();
	p2.close();
	p3.close();
	p4.close();

	{
		boost::mutex::scoped_lock lock(fileListMutex);
		for (list<string>::const_iterator i = filesGenerated.begin(); i != filesGenerated.end(); ++i)
			fileList.push_back(*i);
	}

	bf::path givenfilename(filename);
	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, channelCount, graphs);
}


//////////////////////////////////////////////////////////////////////////
/// @brief setup the output directory
//////////////////////////////////////////////////////////////////////////
void prepareOutputDir(const bf::path &outputDir, const string &filename, const string &commandLine, unsigned channelCount, list<pair<string,string> > &graphs)
{
	bf::path templateFile = executableDirectory / "template.html";

	if (!fileExists(templateFile.native_directory_string()))
	{
		cerr << "cannot find template:" << templateFile.native_directory_string();
		return;
	}
	string basename;
	string::size_type position = filename.find("-stats");
	if (position != string::npos)
	{
		basename = filename.substr(0,position);
	}
	else if ((position = filename.find("-power")) != string::npos)
	{
		basename = filename.substr(0,position);
	}
	else
		return;

	bf::path printFile = outputDir / ("index.html");
	
	ifstream instream;

	bool alreadyExists;
	
	if (alreadyExists = fileExists(printFile.native_directory_string()))
	{
		instream.open(printFile.native_directory_string().c_str(), ios::in | ios::ate);
	}
	else
	{
		instream.open(templateFile.directory_string().c_str(), ios::in | ios::ate);
	}
		
	ifstream::pos_type entireFileLength = instream.tellg();
	char *entireFile = new char[entireFileLength];
	
	instream.seekg(0,ios::beg);
	instream.read(entireFile,entireFileLength);
	instream.close();

	string outputContent(entireFile);
	delete[] entireFile;

	bool changesMade = false;

	if (!alreadyExists)
	{
		string find("@@@");
		// update the title 
		outputContent = outputContent.replace(outputContent.find(find),find.length(),commandLine);
		changesMade = true;
	}


	for (list<pair<string, string> >::const_iterator i = graphs.begin(),
		end = graphs.end(); i != end; ++i)
	{
		string currentImageLink = "<h2>" + i->second
			+ "</h2><a rel=\"lightbox\" href=\"" + i->first + "." + extension
			+ "\"><img class=\"fancyzoom\" src=\"" + i->first + "-thumb." + thumbnailExtenstion +
			+ "\" alt=\"\" /></a>";

		if (outputContent.find(currentImageLink) == string::npos)
		{
			changesMade = true;
			outputContent = outputContent.insert(outputContent.find("</div>"),currentImageLink);
		}
	}

	if (changesMade)
	{
		cerr << endl << printFile.native_directory_string() << endl;
		std::ofstream out(printFile.native_directory_string().c_str());
		out << outputContent;
		out.close();
	}

	bf::path htaccessOut = outputDir / ".htaccess";
	if (!fileExists(htaccessOut.string()))
	{
		bf::path htaccessFile = executableDirectory / ".htaccess";
		bf::copy_file(htaccessFile, htaccessOut);
	}
}


//////////////////////////////////////////////////////////////////////////
/// @brief process the stats file
//////////////////////////////////////////////////////////////////////////
void processStats(const string &filename)
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

	std::list<string> filesGenerated;

	unsigned scaleIndex = 0;
	unsigned scaleFactor = 1;
	bool throughOnce = false;

	// vectors to keep track of various stats
	vector<vector<vector<vector<unsigned> > > > channelDistribution;
	vector<vector<vector<uint64_t> > > channelDistributionBuffer;
	vector<vector<vector<vector<unsigned> > > > channelLatencyDistribution;
	vector<vector<vector<uint64_t> > > channelLatencyDistributionBuffer;

	std::tr1::unordered_map<boost::uint64_t, pair<uint64_t,uint64_t> > latencyVsPcLow;
	std::tr1::unordered_map<boost::uint64_t, pair<uint64_t,uint64_t> > latencyVsPcHigh;

	vector<tuple<unsigned, unsigned, double, unsigned> > transactionLatency;
	StdDev<float> averageTransactionLatency;
	StdDev<float> averageAdjustedTransactionLatency;

	vector<unsigned> transactionCount;
	transactionCount.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t transactionCountBuffer = 0ULL;

	vector<unsigned> adjustedTransactionCount;
	adjustedTransactionCount.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t adjustedTransactionCountBuffer = 0ULL;

	unordered_map<unsigned,unsigned> distTransactionLatency;

	unordered_map<unsigned,unsigned> distAdjustedTransactionLatency;

	vector<float> hitMissValues;
	hitMissValues.reserve(MAXIMUM_VECTOR_SIZE);
	double hitMissValueBuffer  = 0.0;

	vector<unsigned> hitMissTotals;
	hitMissTotals.reserve(MAXIMUM_VECTOR_SIZE);
	unsigned hitMissTotalBuffer = 0U;

	vector<unsigned> iCacheHits;
	iCacheHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t iCacheHitBuffer = 0ULL;

	vector<unsigned> iCacheMisses;
	iCacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
	unsigned iCacheMissBuffer = 0U;

	vector<long> iCacheMissLatency;
	iCacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t iCacheMissLatencyBuffer = 0ULL;

	vector<unsigned> dCacheHits;
	dCacheHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t dCacheHitBuffer = 0ULL;

	vector<unsigned> dCacheMisses;
	dCacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t dCacheMissBuffer = 0ULL;

	vector<long> dCacheMissLatency;
	dCacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t dCacheMissLatencyBuffer = 0ULL;

	vector<unsigned> l2CacheHits;
	l2CacheHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2CacheHitBuffer = 0ULL;

	vector<unsigned> l2CacheMisses;
	l2CacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2CacheMissBuffer = 0ULL;

	vector<long> l2CacheMissLatency;
	l2CacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2CacheMissLatencyBuffer = 0ULL;

	vector<unsigned> l2MshrHits;
	l2MshrHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2MshrHitBuffer = 0ULL;

	vector<unsigned> l2MshrMisses;
	l2MshrMisses.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2MshrMissBuffer = 0ULL;

	vector<long> l2MshrMissLatency;
	l2MshrMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2MshrMissLatencyBuffer = 0ULL;

	vector<pair<uint64_t,uint64_t> > bandwidthValues;
	bandwidthValues.reserve(MAXIMUM_VECTOR_SIZE);
	pair<uint64_t,uint64_t> bandwidthValuesBuffer(0ULL,0ULL);

	vector<pair<uint64_t,uint64_t> > cacheBandwidthValues;
	cacheBandwidthValues.reserve(MAXIMUM_VECTOR_SIZE);
	pair<uint64_t,uint64_t> cacheBandwidthValuesBuffer(0ULL,0ULL);

	vector<pair<unsigned,unsigned> > cacheHitMiss;
	cacheHitMiss.reserve(MAXIMUM_VECTOR_SIZE);
	pair<unsigned,unsigned> cacheHitMissBuffer(0,0);

	vector<float> ipcValues;
	ipcValues.reserve(MAXIMUM_VECTOR_SIZE);
	double ipcValueBuffer = 0.0;

	vector<unsigned> workingSetSize;
	workingSetSize.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t workingSetSizeBuffer;

	bool started = false;
	unsigned ipcLinesWritten = 0;
	float epochTime = 0.0F;
	float period = 0.0F;

	unsigned tRC = 0;
	unsigned tRAS = 0;

	unsigned channelCount = 0;
	unsigned rankCount = 0;
	unsigned bankCount = 0;

	string commandLine;

	filtering_istream inputStream;
	if (ends_with(filename,"gz"))
		inputStream.push(gzip_decompressor());
	else if (ends_with(filename,"bz2"))
		inputStream.push(bzip2_decompressor());

	inputStream.push(file_source(filename));

	char newLine[1024*1024];

	inputStream.getline(newLine,NEWLINE_LENGTH);

	while ((newLine[0] != 0) && (!userStop))
	{
		if (!started)
		{
			if (starts_with(newLine,"----Command Line"))
			{
				string line(newLine);
				trim(line);
				vector<string> splitLine;
				split(splitLine,line,is_any_of(":"));
				commandLine = splitLine[1];
				trim(commandLine);
				cerr << commandLine << endl;

				started = true;

				// get the number of channels
				regex channelSearch("ch\\[([0-9]+)\\]");
				cmatch what;

				if (regex_search(newLine,what,channelSearch))
				{
					string value(what[1].first,what[1].second);
					channelCount = lexical_cast<unsigned>(value);
				}
				else
					exit(-1);

				// get the number of ranks
				regex rankSearch("rk\\[([0-9]+)\\]");
				if (regex_search(newLine,what,rankSearch))
				{
					string value(what[1].first,what[1].second);
					rankCount = lexical_cast<unsigned>(value);
				}
				else
					exit(-1);

				// get the number of banks
				regex bankSearch("bk\\[([0-9]+)\\]");
				if (regex_search(newLine,what,bankSearch))
				{
					string value(what[1].first,what[1].second);
					bankCount = lexical_cast<unsigned>(value);
				}
				else
					exit(-1);

				// get the value of tRC
				regex trcSearch("t_\\{RC\\}\\[([0-9]+)\\]");
				if (regex_search(newLine,what,trcSearch))
				{
					string value(what[1].first,what[1].second);
					tRC = lexical_cast<unsigned>(value);
#ifndef NDEBUG
					cerr << "got tRC as " << tRC << endl;
#endif
				}
				else
					exit(-1);

				// get the value of tRC
				regex trasSearch("t_\\{RAS\\}\\[([0-9]+)\\]");
				if (regex_search(newLine,what,trasSearch))
				{
					string value(what[1].first,what[1].second);
					tRAS = lexical_cast<unsigned>(value);
#ifndef NDEBUG
					cerr << "got tRAS as " << tRAS << endl;
#endif
				}
				else
					exit(-1);

				channelLatencyDistribution.reserve(channelCount);

				for (unsigned i = channelCount; i > 0; --i)
				{
					channelLatencyDistribution.push_back(vector<vector<vector<unsigned> > >());
					channelLatencyDistribution.back().reserve(rankCount);
					channelLatencyDistributionBuffer.push_back(vector<vector<uint64_t> >());
					channelLatencyDistributionBuffer.back().reserve(rankCount);

					channelDistribution.push_back(vector<vector<vector<unsigned> > >());
					channelDistribution.back().reserve(rankCount);
					channelDistributionBuffer.push_back(vector<vector<uint64_t> >());
					channelDistributionBuffer.back().reserve(rankCount);

					for (unsigned j = rankCount; j > 0; --j)
					{
						channelLatencyDistribution.back().push_back(vector<vector<unsigned> >());
						channelLatencyDistribution.back().back().reserve(bankCount + 1);
						channelLatencyDistributionBuffer.back().push_back(vector<uint64_t>());
						channelLatencyDistributionBuffer.back().back().reserve(bankCount + 1);

						channelDistribution.back().push_back(vector<vector<unsigned> >());
						channelDistribution.back().back().reserve(bankCount + 1);
						channelDistributionBuffer.back().push_back(vector<uint64_t>());
						channelDistributionBuffer.back().back().reserve(bankCount + 1);

						for (unsigned k = bankCount + 1; k > 0; --k)
						{
							channelLatencyDistribution.back().back().push_back(vector<unsigned>());
							channelLatencyDistribution.back().back().back().reserve(MAXIMUM_VECTOR_SIZE);
							channelLatencyDistributionBuffer.back().back().push_back(0ULL);

							channelDistribution.back().back().push_back(vector<unsigned>());
							channelDistribution.back().back().back().reserve(MAXIMUM_VECTOR_SIZE);
							channelDistributionBuffer.back().back().push_back(0ULL);
						}
					}
				}

			}
		}
		else if (starts_with(newLine, "----M5 Stat:"))
		{
			char *position = strchr(newLine, ' ');
			position = strchr(position + 1, ' ');
			if (position == NULL)
				break;
			*position++ = 0;
			char* splitline2 = position;
			position = strchr(splitline2, ' ');
			if (position == NULL)
				break;
			*position++ = 0;
			char *position2 = strchr(position, ' ');
			if (position2 == NULL)
				break;
			*position2 = 0;

			if (strcmp(splitline2,"system.cpu.dcache.overall_hits") == 0)
				dCacheHitBuffer = atoi(position);
			else if (strcmp(splitline2,"system.cpu.dcache.overall_miss_latency") == 0)
				dCacheMissLatencyBuffer = atoi(position);
			else if (strcmp(splitline2,"system.cpu.dcache.overall_misses") == 0)
				dCacheMissBuffer = atoi(position);
			else if (strcmp(splitline2,"system.cpu.icache.overall_hits") == 0)
				iCacheHitBuffer = atoi(position);
			else if (strcmp(splitline2,"system.cpu.icache.overall_miss_latency") == 0)
				iCacheMissLatencyBuffer = atof(position);
			else if (strcmp(splitline2,"system.cpu.icache.overall_misses") == 0)
				iCacheMissBuffer = atoi(position);
			else if (strcmp(splitline2,"system.l2.overall_hits") == 0)
				l2CacheHitBuffer = atoi(position);
			else if (strcmp(splitline2,"system.l2.overall_miss_latency") == 0)
				l2CacheMissLatencyBuffer = atof(position);
			else if (strcmp(splitline2,"system.l2.overall_misses") == 0)
				l2CacheMissBuffer = atoi(position);
			else if (strcmp(splitline2,"system.l2.overall_mshr_hits") == 0)
				l2MshrHitBuffer = atoi(position);
			else if (strcmp(splitline2,"system.l2.overall_mshr_miss_latency") == 0)
				l2MshrMissLatencyBuffer = atof(position);
			else if (strcmp(splitline2,"system.l2.overall_mshr_misses") == 0)
				l2MshrMissBuffer = atoi(position);
			else
				cerr <<  "missed something: " << newLine << endl;
		}
		else if (starts_with(newLine,"----Epoch"))
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
				// only if this is at least the second time around
				if (throughOnce)
				{
					// look to dump the buffers into arrays
					scaleIndex = (scaleIndex + 1) % scaleFactor;

					// when the scale buffer is full
					if (scaleIndex == 0)
					{
						// dump all the buffers into arrays, scaled correctly
						for (unsigned i = 0; i < channelCount; ++i)
						{
							for (unsigned j = 0; j < rankCount; ++j)
							{
								for (unsigned k = 0; k < bankCount + 1; k++)
								{
									channelDistribution[i][j][k].push_back(channelDistributionBuffer[i][j][k] / scaleFactor);
									channelDistributionBuffer[i][j][k] = 0;
									channelLatencyDistribution[i][j][k].push_back(channelLatencyDistributionBuffer[i][j][k] / scaleFactor);
									channelLatencyDistributionBuffer[i][j][k] = 0;
								}
							}
						}

						transactionLatency.push_back(tuple<unsigned,unsigned,double,unsigned>((tuple<unsigned,unsigned,double,unsigned>)averageTransactionLatency.getStdDev()));
						averageTransactionLatency.clear();

						transactionCount.push_back(transactionCountBuffer / scaleFactor);
						transactionCountBuffer = 0;

						adjustedTransactionCount.push_back(adjustedTransactionCountBuffer / scaleFactor);
						adjustedTransactionCountBuffer = 0;

						hitMissValues.push_back(hitMissValueBuffer / scaleFactor);
						hitMissValueBuffer = 0;

						hitMissTotals.push_back(hitMissTotalBuffer / scaleFactor);
						hitMissTotalBuffer = 0;

						iCacheHits.push_back(iCacheHitBuffer / scaleFactor);
						iCacheHitBuffer = 0;

						iCacheMisses.push_back(iCacheMissBuffer / scaleFactor);
						iCacheMissBuffer = 0;

						iCacheMissLatency.push_back(iCacheMissLatencyBuffer / scaleFactor);
						iCacheMissLatencyBuffer = 0;

						dCacheHits.push_back(dCacheHitBuffer / scaleFactor);
						dCacheHitBuffer = 0;

						dCacheMisses.push_back(dCacheMissBuffer / scaleFactor);
						dCacheMissBuffer = 0;

						dCacheMissLatency.push_back(dCacheMissLatencyBuffer / scaleFactor);
						dCacheMissLatencyBuffer = 0;

						l2CacheHits.push_back(l2CacheHitBuffer / scaleFactor);
						l2CacheHitBuffer = 0;

						l2CacheMisses.push_back(l2CacheMissBuffer / scaleFactor);
						l2CacheMissBuffer = 0;

						l2CacheMissLatency.push_back(l2CacheMissLatencyBuffer / scaleFactor);
						l2CacheMissLatencyBuffer = 0;

						l2MshrHits.push_back(l2MshrHitBuffer / scaleFactor);
						l2MshrHitBuffer = 0;

						l2MshrMisses.push_back(l2MshrMissBuffer / scaleFactor);
						l2CacheMissBuffer = 0;

						l2MshrMissLatency.push_back(l2MshrMissLatencyBuffer / scaleFactor);
						l2MshrMissLatencyBuffer = 0;

						bandwidthValues.push_back(pair<uint64_t,uint64_t>(bandwidthValuesBuffer.first / scaleFactor, bandwidthValuesBuffer.second / scaleFactor));
						bandwidthValuesBuffer.first = bandwidthValuesBuffer.second = 0;

						cacheBandwidthValues.push_back(pair<uint64_t,uint64_t>(cacheBandwidthValuesBuffer.first / scaleFactor, cacheBandwidthValuesBuffer.second / scaleFactor));
						cacheBandwidthValuesBuffer.first = cacheBandwidthValuesBuffer.second = 0;

						cacheHitMiss.push_back(pair<unsigned,unsigned>(cacheHitMissBuffer.first, cacheHitMissBuffer.second));
						cacheHitMissBuffer.first = cacheHitMissBuffer.second = 0;

						ipcValues.push_back(ipcValueBuffer / scaleFactor);
						ipcValueBuffer = 0;

						workingSetSize.push_back(workingSetSizeBuffer / scaleFactor);
						workingSetSizeBuffer = 0;

						if (ipcValues.size() >= MAXIMUM_VECTOR_SIZE)
						{
							// adjust the epoch time to account for the fact that each amount counts for more
							epochTime *= 2;
							// double the scale factor
							scaleFactor *= 2;

							cerr << "scaleFactor at " << scaleFactor << endl;

							// scale all the arrays by half
							for (unsigned epoch = 0; epoch < MAXIMUM_VECTOR_SIZE / 2; epoch++)
							{
								for (unsigned i = 0; i < channelCount; ++i)
								{
									for (unsigned j = 0; j < rankCount; ++j)
									{
										for (unsigned k = 0; k < bankCount + 1; k++)
										{
											channelDistribution[i][j][k][epoch] = (channelDistribution[i][j][k][2 * epoch] + channelDistribution[i][j][k][2 * epoch + 1]) / 2;
											channelLatencyDistribution[i][j][k][epoch] = (channelLatencyDistribution[i][j][k][2 * epoch] + channelLatencyDistribution[i][j][k][2 * epoch + 1]) / 2;
										}
									}
								}
								//fixit
								//transactionLatency[epoch] = (transactionLatency[2 * epoch] + transactionLatency[2 * epoch + 1]) / 2;
								transactionLatency[epoch].get<0>() = (transactionLatency[2 * epoch].get<0>() + transactionLatency[2 * epoch + 1].get<0>()) / 2;
								transactionLatency[epoch].get<1>() = (transactionLatency[2 * epoch].get<1>() + transactionLatency[2 * epoch + 1].get<1>()) / 2;
								transactionLatency[epoch].get<2>() = (transactionLatency[2 * epoch].get<2>() + transactionLatency[2 * epoch + 1].get<2>()) / 2;
								transactionLatency[epoch].get<3>() = (transactionLatency[2 * epoch].get<3>() + transactionLatency[2 * epoch + 1].get<3>()) / 2;
								//transactionLatency[epoch].get<4>() = (transactionLatency[2 * epoch].get<4>() + transactionLatency[2 * epoch + 1].get<4>()) / 2;
								//transactionLatency[epoch].get<5>() = (transactionLatency[2 * epoch].get<5>() + transactionLatency[2 * epoch + 1].get<5>()) / 2;

								transactionCount[epoch] = (transactionCount[2 * epoch] + transactionCount[2 * epoch + 1]) / 2;

								adjustedTransactionCount[epoch] = (adjustedTransactionCount[2 * epoch] + adjustedTransactionCount[2 * epoch + 1]) / 2;

								hitMissValues[epoch] = (hitMissValues[2 * epoch] + hitMissValues[2 * epoch + 1]) / 2;

								iCacheHits[epoch] = (iCacheHits[2 * epoch] + iCacheHits[2 * epoch + 1]) / 2;

								iCacheMisses[epoch] = (iCacheMisses[2 * epoch] + iCacheMisses[2 * epoch + 1]) / 2;

								iCacheMissLatency[epoch] = (iCacheMissLatency[2 * epoch] + iCacheMissLatency[2 * epoch + 1]) / 2;

								dCacheHits[epoch] = (dCacheHits[2 * epoch] + dCacheHits[2 * epoch + 1]) / 2;

								dCacheMisses[epoch] = (dCacheMisses[2 * epoch] + dCacheMisses[2 * epoch + 1]) / 2;

								dCacheMissLatency[epoch] = (dCacheMissLatency[2 * epoch] + dCacheMissLatency[2 * epoch + 1]) / 2;

								l2CacheHits[epoch] = (l2CacheHits[2 * epoch] + l2CacheHits[2 * epoch + 1]) / 2;

								l2CacheMisses[epoch] = (l2CacheMisses[2 * epoch] + l2CacheMisses[2 * epoch + 1]) / 2;

								l2CacheMissLatency[epoch] = (l2CacheMissLatency[2 * epoch] + l2CacheMissLatency[2 * epoch + 1]) / 2;

								l2MshrHits[epoch] = (l2MshrHits[2 * epoch] + l2MshrHits[2 * epoch + 1]) / 2;

								l2MshrMisses[epoch] = (l2MshrMisses[2 * epoch] + l2MshrMisses[2 * epoch + 1]) / 2;

								l2MshrMissLatency[epoch] = (l2MshrMissLatency[2 * epoch] + l2MshrMissLatency[2 * epoch + 1]) / 2;

								bandwidthValues[epoch].first = (bandwidthValues[2 * epoch].first + bandwidthValues[2 * epoch + 1].first) / 2;
								bandwidthValues[epoch].second = (bandwidthValues[2 * epoch].second + bandwidthValues[2 * epoch + 1].second) / 2;

								cacheBandwidthValues[epoch].first = (cacheBandwidthValues[2 * epoch].first + cacheBandwidthValues[2 * epoch + 1].first) / 2;
								cacheBandwidthValues[epoch].second = (cacheBandwidthValues[2 * epoch].second + cacheBandwidthValues[2 * epoch + 1].second) / 2;

								cacheHitMiss[epoch].first = (cacheHitMiss[2 * epoch].first + cacheHitMiss[2 * epoch + 1].first) / 2;
								cacheHitMiss[epoch].second = (cacheHitMiss[2 * epoch].second + cacheHitMiss[2 * epoch + 1].second) / 2;

								ipcValues[epoch] = (ipcValues[2 * epoch] + ipcValues[2 * epoch + 1]) / 2;

								workingSetSize[epoch] = (workingSetSize[2 * epoch] + workingSetSize[2 * epoch + 1]) / 2;
							}

							// resize all the vectors
							for (unsigned i = 0; i < channelCount; ++i)
							{
								for (unsigned j = 0; j < rankCount; ++j)
								{
									for (unsigned k = 0; k < bankCount + 1; k++)
									{
										channelDistribution[i][j][k].resize(MAXIMUM_VECTOR_SIZE / 2);
										channelLatencyDistribution[i][j][k].resize(MAXIMUM_VECTOR_SIZE / 2);
									}
								}
							}

							transactionLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

							transactionCount.resize(MAXIMUM_VECTOR_SIZE / 2);

							adjustedTransactionCount.resize(MAXIMUM_VECTOR_SIZE / 2);

							hitMissValues.resize(MAXIMUM_VECTOR_SIZE / 2);

							iCacheHits.resize(MAXIMUM_VECTOR_SIZE / 2);

							iCacheMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

							iCacheMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

							dCacheHits.resize(MAXIMUM_VECTOR_SIZE / 2);

							dCacheMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

							dCacheMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

							l2CacheHits.resize(MAXIMUM_VECTOR_SIZE / 2);

							l2CacheMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

							l2CacheMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

							l2MshrHits.resize(MAXIMUM_VECTOR_SIZE / 2);

							l2MshrMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

							l2MshrMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

							bandwidthValues.resize(MAXIMUM_VECTOR_SIZE / 2);

							cacheBandwidthValues.resize(MAXIMUM_VECTOR_SIZE / 2);

							cacheHitMiss.resize(MAXIMUM_VECTOR_SIZE / 2);

							ipcValues.resize(MAXIMUM_VECTOR_SIZE / 2);

							workingSetSize.resize(MAXIMUM_VECTOR_SIZE / 2);
						}
					}
				}
				else
				{
					throughOnce = true;
				}

				averageTransactionLatency.clear();
				char *position = newLine;
				while (position != NULL)
				{
					char *firstBracket = strchr(position,'{');
					if (firstBracket == NULL) break;

					char *secondBracket = strchr(position,'}');
					if (secondBracket == NULL) break;

					char *comma = strchr(position,',');
					if (comma == NULL) break;

					*comma = *secondBracket = NULL;

					unsigned latency = atoi(firstBracket + 1);
					unsigned count = atoi(comma + 1);

					averageTransactionLatency.add(latency,count);
					transactionCountBuffer += count;
					distTransactionLatency[latency] += count;

					position = secondBracket + 1;
				}
			}
			else if (starts_with(newLine,"----Adjusted Transaction Latency"))
			{
				averageAdjustedTransactionLatency.clear();

				char *position = newLine;
				while (position != NULL)
				{
					char *firstBracket = strchr(position,'{');
					if (firstBracket == NULL) break;

					char *secondBracket = strchr(position,'}');
					if (secondBracket == NULL) break;

					char *comma = strchr(position,',');
					if (comma == NULL) break;

					*comma = *secondBracket = NULL;

					unsigned latency = atoi(firstBracket + 1);
					unsigned count = atoi(comma + 1);

					averageAdjustedTransactionLatency.add(latency,count);
					//transactionCountBuffer += count;
					distAdjustedTransactionLatency[latency] += count;

					position = secondBracket + 1;
				}
			}
			else if (starts_with(newLine,"----Working Set"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;
				workingSetSizeBuffer = atoi(firstBracket + 1);
			}
			else if (starts_with(newLine,"----Bandwidth"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				// read bandwidth
				bandwidthValuesBuffer.first = atol(firstBracket + 1);

				firstBracket = strchr(secondBracket + 1,'{');
				if (firstBracket == NULL)
					break;

				secondBracket = strchr(secondBracket + 1,'}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				// write bandwidth
				bandwidthValuesBuffer.second = atol(firstBracket + 1);
			}
			else if (starts_with(newLine,"----Average Transaction Latency Per PC Value"))
			{
				char *position = newLine;

				while (position != NULL)
				{
					char *firstBracket = strchr(position,'{');
					if (firstBracket == NULL) break;

					char *secondBracket = strchr(position,'}');
					if (secondBracket == NULL) break;

					char *comma = strchr(position,',');
					if (comma == NULL) break;

					char *secondComma = strchr(comma + 1,',');
					if (secondComma == NULL) break;

					*comma = NULL;
					*secondComma = NULL;
					*secondBracket = NULL;

					uint64_t PC = strtol(firstBracket + 1,0,16);

					float averageAccessTime = atof(comma + 1);

					unsigned numberAccesses = atoi(secondComma + 1);

					if (PC < 0x100000000)
					{
						latencyVsPcLow[PC].first += numeric_cast<uint64_t>(averageAccessTime * (float)numberAccesses);
						latencyVsPcLow[PC].second += numberAccesses;
					}
					else
					{
						latencyVsPcHigh[PC].first += numeric_cast<uint64_t>(averageAccessTime * (float)numberAccesses);
						latencyVsPcHigh[PC].second += numberAccesses;
					}

					position = secondBracket + 1;
				}
			}
			else if (starts_with(newLine, "----DIMM Cache Bandwidth"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				// read bandwidth
				cacheBandwidthValuesBuffer.first = atol(firstBracket + 1);

				firstBracket = strchr(secondBracket + 1,'{');
				if (firstBracket == NULL)
					break;

				secondBracket = strchr(secondBracket + 1,'}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				// write bandwidth
				cacheBandwidthValuesBuffer.second = atol(firstBracket + 1);
			}
			else if (starts_with(newLine,"----IPC"))
			{
				if (ipcLinesWritten < 1)
				{
					float currentValue = starts_with(newLine,"nan") ? 0.0F : atof(newLine);
					if (currentValue != currentValue)
						currentValue = 0.0F;

					ipcValueBuffer += currentValue;
				}
				ipcLinesWritten++;
				ipcLinesWritten = 0;
			}
			else if (starts_with(newLine,"----Row Hit/Miss Counts"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL) break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL) break;
				*secondBracket = NULL;
				unsigned hitCount = max(atoi(firstBracket + 1),1);

				firstBracket = strchr(secondBracket + 1,'{');
				if (firstBracket == NULL) break;

				secondBracket = strchr(secondBracket + 1,'}');
				if (secondBracket == NULL) break;
				*secondBracket = NULL;
				unsigned missCount = max(atoi(firstBracket + 1),1);

				hitMissValueBuffer += hitCount / ((double)missCount + hitCount);
				hitMissTotalBuffer += (hitCount + missCount);
			}
			else if (starts_with(newLine,"----Utilization"))
			{
				char *position = newLine;
				while (position != NULL)
				{
					char *leftParen = strchr(position,'(');
					if (leftParen == NULL) break;
					char *firstComma = strchr(position,',');
					if (firstComma == NULL) break;
					char *secondComma = strchr(firstComma + 1,',');
					if (secondComma == NULL) break;
					char *rightParen = strchr(position,')');
					if (rightParen== NULL) break;

					*secondComma = NULL;
					*firstComma = NULL;
					*rightParen = NULL;

					unsigned channel = atoi(leftParen + 1);
					unsigned rank = atoi(firstComma + 1);
					unsigned bank = atoi(secondComma + 1);

					char *leftBracket = strchr(rightParen + 1,'{');
					if (leftBracket == NULL) break;
					char *rightBracket = strchr(rightParen + 1,'}');
					if (rightBracket == NULL) break;
					*rightBracket = NULL;
					unsigned value = atoi(leftBracket + 1);

					channelDistributionBuffer[channel][rank][bank] += value;
					channelDistributionBuffer[channel][rank].back() += value;

					position = rightBracket + 1;
				}
			}
			else if (starts_with(newLine,"----Latency Breakdown"))
			{
				char *position = newLine;
				while (position != NULL)
				{
					char *leftParen = strchr(position,'(') ;
					if (leftParen == NULL) break;
					char *firstComma = strchr(position,',');
					if (firstComma == NULL) break;
					char *secondComma = strchr(firstComma + 1,',');
					if (secondComma == NULL) break;
					char *rightParen = strchr(position,')');
					if (rightParen== NULL) break;

					*rightParen = NULL;
					*firstComma = NULL;
					*secondComma = NULL;

					unsigned channel = atoi(leftParen + 1);
					unsigned rank = atoi(firstComma + 1);
					unsigned bank = atoi(secondComma + 1);

					char *leftBracket = strchr(rightParen + 1,'{');
					if (leftBracket == NULL) break;
					char *rightBracket = strchr(rightParen + 1,'}');
					if (rightBracket == NULL) break;
					*rightBracket = NULL;
					unsigned value = atoi(leftBracket + 1);

					channelLatencyDistributionBuffer[channel][rank][bank] += value;
					channelLatencyDistributionBuffer[channel][rank].back() += value;

					position = rightBracket + 1;
				}
			}
			else if (starts_with(newLine,"----Cache Hit/Miss Counts"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL) break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL) break;
				*secondBracket = NULL;

				cacheHitMissBuffer.first = atoi(firstBracket + 1);

				firstBracket = strchr(secondBracket + 1,'{');
				if (firstBracket == NULL) break;

				secondBracket = strchr(secondBracket + 1,'}');
				if (secondBracket == NULL) break;
				*secondBracket = NULL;

				cacheHitMissBuffer.second = atol(firstBracket + 1);

			}
			else
			{
				// 				newLine[32] = NULL;
				// 				cerr << "Not matched: " << newLine << endl;
			}
		}


		inputStream.getline(newLine,NEWLINE_LENGTH);
	} // end going through lines

	userStop = false;

	if (channelLatencyDistribution.size() < 1)
		return;

	opstream p0("gnuplot");	
	p0 << terminal << basicSetup;
	opstream p1("gnuplot");
	p1 << terminal << basicSetup;
	opstream p2("gnuplot");
	p2 << terminal << basicSetup;
	opstream p3("gnuplot");
	p3 << terminal << basicSetup;

	bf::path outFilename;
	
	list<pair<string,string> > graphs;

	if (!cypressResults)
	{
		// make the address latency distribution per channel graphs
		for (unsigned channelID = 0; channelID < channelLatencyDistribution.size(); channelID++)
		{
			outFilename = outputDir / ("addressLatencyDistribution" + lexical_cast<string>(channelID) + "." + extension);
			graphs.push_back(pair<string,string>("addressLatencyDistribution","Address Latency Distribution, Channel " + lexical_cast<string>(channelID)));
			filesGenerated.push_back(outFilename.native_directory_string());
			p2 << "set output '" << outFilename.native_directory_string() << "'" << endl << subAddrDistroA;
			p2 << "set multiplot layout " << channelLatencyDistribution[channelID].size() << ", 1 title \"{/=18" << commandLine << "\"" << endl;

			for (unsigned rankID = 0; rankID < channelLatencyDistribution[channelID].size(); rankID++)
			{
				p2 << "set title \"Rank " << rankID << " Distribution Rate\" offset character 0, 0, 0 font \"\" norotate" << endl;

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
						p2 << max(1E-5F, channelLatencyDistribution[channelID][rankID][bankID][epoch] / ((float)channelLatencyDistribution[channelID][rankID].back()[epoch])) << endl;
					}
					p2 << "e" << endl;
				}
				p2 << channelLatencyDistribution[channelID][rankID][0].size() * epochTime << " " << "0.2" << endl << "e" << endl;
			}
			p2 << "unset multiplot" << endl << "unset output" << endl;
		}
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the address distribution per channel graphs
		for (unsigned channelID = 0; channelID < channelDistribution.size(); channelID++)
		{
			outFilename = outputDir / ("addressDistribution" + lexical_cast<string>(channelID) + "." + extension);
			graphs.push_back(pair<string,string>("addressDistribution","Address Distribution, Channel " + lexical_cast<string>(channelID)));
			filesGenerated.push_back(outFilename.native_directory_string());
			p1 << "set output '" << outFilename.native_directory_string() << "'" << endl << subAddrDistroA;
			p1 << "set multiplot layout " << channelLatencyDistribution[channelID].size() << ", 1 title \"" << commandLine << "\"" << endl;

			for (unsigned rankID = 0; rankID < channelDistribution[channelID].size(); rankID++)
			{
				p1 << "set title \"Rank " << rankID << " Distribution Rate\" offset character 0, 0, 0 font \"\" norotate" << endl;

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
						p1 << max(1E-5F, channelDistribution[channelID][rankID][bankID][epoch] / ((float)channelDistribution[channelID][rankID].back()[epoch])) << endl;
					}
					p1 << "e" << endl;
				}
				p1 << channelDistribution[channelID][rankID][0].size() * epochTime << " " << "0.2" << endl << "e" << endl;
			}
			p1 << "unset multiplot" << endl << "unset output" << endl;
		}
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the overall address distribution graphs
		outFilename = outputDir / ("addressDistribution." + extension);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string,string>("addressDistribution","Overall Address Distribution"));
			
		p0 << "set output '" << outFilename.native_directory_string() << "'" << endl;
		p0 << "set title \"" << commandLine << "\\nChannel Distribution Rate\"" << endl << addressDistroA;
		p0 << "plot ";
		for (unsigned i = 0; i < channelCount; ++i)
			p0 << "'-' using 1 axes x2y1 t 'ch[" << i << "]',";
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

		p0 << channelDistribution[0][0][0].size() * epochTime << " " << "0.2" << endl << "e" << endl;
		p0 << addressDistroB << endl << "plot ";

		for (unsigned i = 0; i < rankCount; ++i)
			p0 << "'-' using 1 axes x2y1 t 'rk[" << i << "]',";
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
		p0 << channelDistribution[0][0][0].size() * epochTime << " " << "0.2" << endl << "e" << endl;
		p0 << addressDistroC << endl << "plot ";
		for (unsigned i = 0; i < bankCount; ++i)
			p0 << "'-' using 1 axes x2y1 t 'bk[" << i << "]',";
		p0 << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01" << endl;

		for (unsigned bankID = 0; bankID < bankCount; bankID++)
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
		p0 << channelDistribution[0][0][0].size() * epochTime << " " << "0.2" << endl << "e" << endl;
		p0 << "unset multiplot" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the PC vs latency graph
		outFilename = outputDir / ("latencyVsPc." + extension);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string,string>("latencyVsPc","PC vs. Latency"));
			
		p1 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
		p1 << pcVsLatencyGraph << endl;
		p1 << "set multiplot layout 1, 2 title \"" << commandLine << "\\nTotal Latency Due to Reads vs. PC Value\"" << endl;
		p1 << "plot '-' using 1:2 t 'Total Latency' with boxes" << endl;

		if (latencyVsPcLow.size() > 0)
			for (std::tr1::unordered_map<uint64_t,pair<uint64_t,uint64_t> >::const_iterator i = latencyVsPcLow.begin(); i != latencyVsPcLow.end(); ++i)
				p1 << i->first << " " << period * i->second.first << endl;
		else
			p1 << "4294967296 1.01" << endl;

		p1 << endl << "e" << endl << "set format x '0x1%x'" << endl << "plot '-' using 1:2 t 'Total Latency' with boxes" << endl;
		if (latencyVsPcHigh.size() > 0)
			for (std::tr1::unordered_map<uint64_t,pair<uint64_t,uint64_t> >::const_iterator i = latencyVsPcHigh.begin(); i != latencyVsPcHigh.end(); ++i)
				p1 << (i->first - 0x100000000) << " " << period * i->second.first << endl;
		else
			p1 << "4294967296 1.01" << endl;
		p1 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the PC vs average latency graph
		outFilename = outputDir / ("avgLatencyVsPc." + extension);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string,string>("avgLatencyVsPc","PC vs. Average Latency"));
			
		p1 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
		p1 << avgPcVsLatencyGraph << endl;
		p1 << "set multiplot layout 1, 2 title \"" << commandLine << "\\nAverage Latency Due to Reads vs. PC Value\"" << endl;
		p1 << "plot '-' using 1:2 t 'Average Latency' with boxes" << endl;

		if (latencyVsPcLow.size() > 0)
			for (std::tr1::unordered_map<uint64_t,pair<uint64_t,uint64_t> >::const_iterator i = latencyVsPcLow.begin(); i != latencyVsPcLow.end(); ++i)
				p1 << i->first << " " << period * (i->second.first / i->second.second) << endl;
		else
			p1 << "4294967296 1.01" << endl;

		p1 << endl << "e" << endl;
		p1 << "set format x '0x1%x'" << endl << "plot '-' using 1:2 t 'Average Latency' with boxes" << endl;
		if (latencyVsPcHigh.size() > 0)
			for (std::tr1::unordered_map<uint64_t,pair<uint64_t,uint64_t> >::const_iterator i = latencyVsPcHigh.begin(); i != latencyVsPcHigh.end(); ++i)
				p1 << (i->first - 0x100000000) << " " << period * (i->second.first / i->second.second) << endl;
		else
			p1 << "4294967296 1.01" << endl;
		p1 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the transaction latency distribution graph
	outFilename = outputDir / ("transactionLatencyDistribution." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("transactionLatencyDistribution","Transaction Latency Distribution"));
		
	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title \"" << commandLine << "\\nRead Transaction Latency\""<< endl << transactionGraph << endl;
	StdDev<float> latencyDeviation;
	for (std::tr1::unordered_map<unsigned,unsigned>::const_iterator i = distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
	{
		latencyDeviation.add(i->first * period, i->second);
		p2 << i->first * period << " " << i->second << endl;
	}
	p2 << "e" << endl << "unset output" << endl;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the zoomed transaction latency distribution graph
	outFilename = outputDir / ("zoomedTransactionLatencyDistribution." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("zoomedTransactionLatencyDistribution","Zoomed Transaction Latency"));
		
	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title \"" << commandLine << "\\nRead Transaction Latency\""<< endl << 
		"set xrange [0:" << latencyDeviation.getStdDev().get<1>() + 8 * latencyDeviation.getStdDev().get<2>() << "]" << endl << transactionGraph << endl;
	for (std::tr1::unordered_map<unsigned,unsigned>::const_iterator i = distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
	{
		p2 << i->first * period << " " << i->second << endl;
	}
	p2 << "e" << endl << "unset output" << endl;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the adjusted transaction latency distribution graph
	outFilename = outputDir / ("adjustedTransactionLatencyDistribution." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("adjustedTransactionLatencyDistribution","Adjusted Transaction Latency"));
		
	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title \"" << commandLine << "\\nAdjusted Read Transaction Latency\""<< endl << transactionGraph << endl;
	latencyDeviation.clear();
	for (std::tr1::unordered_map<unsigned,unsigned>::const_iterator i = distAdjustedTransactionLatency.begin(), end = distAdjustedTransactionLatency.end();
		i != end; ++i)
	{
		latencyDeviation.add(i->first * period, i->second);
		p2 << i->first * period << " " << i->second << endl;
	}
	p2 << "e" << endl << "unset output" << endl;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the zoomed adjusted transaction latency distribution graph
	outFilename = outputDir / ("zoomedAdjustedTransactionLatencyDistribution." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("zoomedAdjustedTransactionLatencyDistribution","Zoomed Adjusted Transaction Latency"));
		
	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title \"" << commandLine << "\\nZoomed Adjusted Read Transaction Latency\""<< endl << 
		"set xrange [0:" << latencyDeviation.getStdDev().get<1>() + 8 * latencyDeviation.getStdDev().get<2>() << "]" << endl << transactionGraph << endl;
	for (std::tr1::unordered_map<unsigned,unsigned>::const_iterator i = distAdjustedTransactionLatency.begin(), end = distAdjustedTransactionLatency.end();
		i != end; ++i)
	{
		p2 << i->first * period << " " << i->second << endl;
	}
#ifndef NDEBUG
	cerr << "range is " << latencyDeviation.getStdDev().get<1>() + 3 * latencyDeviation.getStdDev().get<2>() << endl;
#endif
	p2 << "e" << endl << "unset output" << endl;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// make the bandwidth graph
	outFilename = outputDir / ("bandwidth." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("bandwidth","Bandwidth"));
		
	p3 << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p3 << "set multiplot title \"" << commandLine << "\""<< endl;
	p3 << bandwidthGraph << endl;

	for (vector<pair<uint64_t,uint64_t> >::const_iterator i = bandwidthValues.begin(); i != bandwidthValues.end(); ++i)
		p3 << 1.0 * i->first << endl;
	p3 << "e" << endl;

	for (vector<pair<uint64_t,uint64_t> >::const_iterator i = bandwidthValues.begin(); i != bandwidthValues.end(); ++i)
		p3 << 1.0 * i->second << endl;
	p3 << "e" << endl;

	for (vector<pair<uint64_t,uint64_t> >::const_iterator i = cacheBandwidthValues.begin(); i != cacheBandwidthValues.end(); ++i)
		p3 << 1.0 * i->first << endl;
	p3 << "e" << endl;

	for (vector<pair<uint64_t,uint64_t> >::const_iterator i = cacheBandwidthValues.begin(); i != cacheBandwidthValues.end(); ++i)
		p3 << 1.0 * i->second << endl;
	p3 << "e" << endl;

	float time = 0.0F;
	PriorMovingAverage bandwidthTotal(WINDOW);
	for (vector<pair<uint64_t,uint64_t> >::const_iterator i = bandwidthValues.begin(), j = cacheBandwidthValues.begin();
		i != bandwidthValues.end() && j != cacheBandwidthValues.end(); ++i, ++j)
	{
		bandwidthTotal.append(1.0F * (float)(i->first + i->second + j->first + j->second));
		p3 << time << " " << bandwidthTotal.getAverage() << endl;
		time += epochTime;
	}
	p3 << "e" << endl;

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the small IPC graph
		p3 << smallIPCGraph << endl;

		time = 0.0F;
		for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
		{
			p3 << time << " " << *i << endl;
			time += epochTime;
		}
		p3 << "e" << endl;

		time = 0.0F;
		CumulativePriorMovingAverage ipcTotal;
		for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
		{
			ipcTotal.add(1,*i);
			p3 << time << " " << ipcTotal.getAverage() << endl;
			time += epochTime;
		}
		p3 << "e" << endl;

		time = 0.0F;
		PriorMovingAverage ipcBuffer(WINDOW);
		for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
		{
			ipcBuffer.append(*i);
			p3 << time << " " << ipcBuffer.getAverage() << endl;
			time += epochTime;
		}
		p3 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		// make the cache graph
		outFilename = outputDir / ("cacheData." + extension);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string,string>(outFilename.native_directory_string(),"Cache Data"));
			
		p2 << "reset" << endl << terminal << basicSetup << cacheGraph0;
		p2 << "set output '" << outFilename.native_directory_string() << "'" << endl;
		p2 << "set multiplot layout 3, 1 title \"" << commandLine << "\"" << endl;
		p2 << cacheGraph1 << endl;

		for (vector<unsigned>::size_type i = 0; i < iCacheMisses.size(); ++i)
			p2 << (float)i * epochTime << " " << iCacheMisses[i] + iCacheHits[i] << endl;
		p2 << "e" << endl;
		for (vector<unsigned>::size_type i = 0; i < iCacheMisses.size(); ++i)
			p2 << (float)i * epochTime << " " << iCacheMisses[i] / ((float)max(iCacheMisses[i] + iCacheHits[i],1U)) << endl;
		p2 << "e" << endl;

		p2 << cacheGraph2 << endl;
		for (vector<unsigned>::size_type i = 0; i < dCacheMisses.size(); ++i)
			p2 << (float)i * epochTime << " " << dCacheMisses[i] + dCacheHits[i] << endl;
		p2 << "e" << endl;
		for (vector<unsigned>::size_type i = 0; i < dCacheMisses.size(); ++i)
			p2 << (float)i * epochTime << " " << dCacheMisses[i] / ((float)max(dCacheMisses[i] + dCacheHits[i],1U)) << endl;
		p2 << "e" << endl;

		p2 << cacheGraph3 << endl;
		for (vector<unsigned>::size_type i = 0; i < l2CacheMisses.size(); ++i)
			p2 << (float)i * epochTime << " " << l2CacheMisses[i] + l2CacheHits[i] << endl;
		p2 << "e" << endl;
		for (vector<unsigned>::size_type i = 0; i < l2CacheMisses.size(); ++i)
			p2 << (float)i * epochTime << " " << l2CacheMisses[i] / ((float)max(l2CacheMisses[i] + l2CacheHits[i],1U)) << endl;
		p2 << "e" << endl << "unset multiplot" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the other IPC graph
	outFilename = outputDir / ("averageIPCandLatency." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("averageIPCandLatency","IPC and Latency"));
		
	p1 << "reset" << endl << terminal << endl << basicSetup << endl << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p1 << "set multiplot layout 2,1 title \"" << commandLine << "\"" << endl;

	// make the transaction latency graph
	p1 << "set title 'Transaction Latency'" << endl << averageTransactionLatencyScript << endl;

	time = 0.0F;
	// access count
	for (vector<unsigned>::const_iterator i = transactionCount.begin(); i != transactionCount.end(); ++i)
	{
		p1 << time << " " << *i << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	time = 0.0F;
	// minimum
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p1 << time << " " << period * i->get<0>() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	// mean
	time = 0.0F;
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p1 << time << " " << period * i->get<1>() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	time = 0.0F;
	// mean + 1 std dev
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p1 << time << " " << period * (i->get<1>() + 2 * i->get<2>()) << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

	// ipc part
	p1 << "set title 'Average IPC vs. Time'" << endl << otherIPCGraph << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
	{
		p1 << time << " " << *i << endl;
		time += epochTime;
	}
	p1 << "e" << endl;

#if 0
	ipcTotal.clear();
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
	{
		ipcTotal.add(1, *i);
		p1 << time << " " << ipcTotal.getAverage() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;
#endif

	PriorMovingAverage ipcBuffer(WINDOW);
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
	{
		ipcBuffer.append(*i);
		p1 << time << " " << ipcBuffer.getAverage() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;


	p1 << "unset multiplot" << endl << "unset output" << endl;
	//////////////////////////////////////////////////////////////////////////

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the hit-miss graph
		outFilename = outputDir / ("rowHitRate." + extension);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string,string>("rowHitRate","Row Reuse"));
			
		p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
		p2 << "set title \"" << "Reuse Rate of Open Rows vs. Time\\n" << commandLine << "\"" << endl << rowHitMissGraph << endl;

		time = 0.0F;
		for (vector<float>::const_iterator i = hitMissValues.begin(); i != hitMissValues.end(); ++i)
		{
			p2 << time << " " << *i << endl;
			time += epochTime;
		}
		p2 << "e" << endl;

		CumulativePriorMovingAverage hitMissTotal;
		time = 0.0F;
		for (vector<float>::const_iterator i = hitMissValues.begin(); i != hitMissValues.end(); ++i)
		{
			hitMissTotal.add(1.0,*i);
			p2 << time << " " << hitMissTotal.getAverage() << endl;
			time += epochTime;
		}
		p2 << "e" << endl;

		time = 0.0F;
		for (vector<unsigned>::const_iterator i = hitMissTotals.begin(); i != hitMissTotals.end(); ++i)
		{
			p2 << time << " " << max(*i, 1U) << endl;
			time += epochTime;
		}

		p2 << "e" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the working set
		p3 << "reset" << endl << basicSetup << terminal;
		p3 << "set title \"" << commandLine << "\\nWorking Set Size vs Time\" offset character 0, -1, 0 font '' norotate" << endl;
		outFilename = outputDir / ("workingSet." + extension);
		graphs.push_back(pair<string,string>("workingSet","Working Set"));
			
		p3 << "set output '" << outFilename.native_directory_string() << "'" << endl;
		filesGenerated.push_back(outFilename.native_directory_string());
		p3 << workingSetSetup << endl;
		time = 0.0F;
		for (vector<unsigned>::const_iterator i = workingSetSize.begin(); i != workingSetSize.end(); ++i)
		{
			p3 << time << " " << *i << endl;
			time += epochTime;
		}
		p3 << "e" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make special IPC graph
		p0 << "reset" << endl << basicSetup << terminal;
		outFilename = outputDir / ("bigIpcGraph." + extension);
		graphs.push_back(pair<string,string>("bigIpcGraph","Big IPC"));
			
		p0 << "set output '" << outFilename.native_directory_string() << "'" << endl;
		filesGenerated.push_back(outFilename.native_directory_string());
		p0 << bigIPCGraph << endl;
		time = 0.0F;
		for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
		{
			p0 << time << " " << *i << endl;
			time += epochTime;
		}
		p0 << "e" << endl;

		time = 0.0F;
		CumulativePriorMovingAverage ipcTotal;
		for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
		{
			ipcTotal.add(1,*i);
			p0 << time << " " << ipcTotal.getAverage() << endl;
			time += epochTime;
		}
		p0 << "e" << endl << "0 0" << endl << "3.31 1E-5" << endl << "e" << endl << "unset output" << endl;
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the cache hit-miss graph
	outFilename = outputDir / ("cacheHitRate." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>("cacheHitRate","DIMM Cache Hit/Miss"));
		
	p1 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p1 << "set title \"" << "Per-DIMM Cache Hit Rate\\n" << commandLine << "\"" << endl << hitMissScript << endl;

	time = 0.0F;
	for (vector<pair<unsigned,unsigned> >::const_iterator i = cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		p1 << time << " " << i->first + i->second << endl;
		//cerr << time << " " << i->first + i->second << endl;
		time += epochTime;
	}

	p1 << "e" << endl;

	time = 0.0F;
	for (vector<pair<unsigned,unsigned> >::const_iterator i = cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		p1 << time << " " << (double)i->first / (i->first + i->second) << endl;
		//cerr << time << " " << i->first << " " <<  i->second << endl;
		time += epochTime;
	}

	p1 << "e" << endl << "unset output" << endl;
	//////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
	cerr << "Period: " << period << endl;
	cerr << "tRC: " << tRC * period << "ns" << endl;
	cerr << "Epoch Time: " << epochTime;
#endif

	//////////////////////////////////////////////////////////////////////////
#if 0
	//////////////////////////////////////////////////////////////////////////
	// make the cache hit-miss graph
	outFilename = outputDir / ("cachePower." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>(outFilename.native_directory_string(),"Cache Power"));
		
	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title \"" << "Power Usage of SRAM vs. DRAM\\n" << commandLine << "\"" << endl << hitMissPowerScript << endl;

	time = 0.0F;
	float voltageScaleFactor = 1.8F / 1.9F;
	float frequencyScaleFactor = 800000000 / 800000000;
	float devicesPerRank = 8;
	float IDD0 = 90.0F;
	float IDD3N = 60.0F;
	float IDD2N = 60.0F;
	float vddMax = 1.9F;
	float vdd = 1.8F;
	float IDD2P = 7.0F;
	float IDD3P = 50.0F;
	double CKE_LO_PRE = 0.95F;
	double CKE_LO_ACT = 0.01F;
	float PdsACT = IDD0 - ((IDD3N * tRAS + IDD2N * (tRC - tRAS)/tRC)) * vdd;

	for (vector<pair<unsigned,unsigned> >::const_iterator i = cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		double percentActive = ((i->first + i->second) * tRC * period) / epochTime;
		cerr << percentActive << endl;


		// calculate PsysACT-STBY
		float PsysACT_STBY = devicesPerRank * voltageScaleFactor * frequencyScaleFactor *
			IDD3N * vddMax * percentActive * (1.0F - CKE_LO_ACT);

		// calculate PsysPRE-STBY
		float PsysPRE_STBY = devicesPerRank * voltageScaleFactor * frequencyScaleFactor *
			IDD2N * vddMax * (1 - percentActive) * (1 - CKE_LO_PRE);

		// calculate PsysPRE-PDN
		float PsysPRE_PDN = voltageScaleFactor * IDD2P * vddMax * (1 - percentActive) * CKE_LO_PRE;

		// calculate PsysACT-PDN
		/// @todo: account for CKE
		float PsysACT_PDN = voltageScaleFactor * IDD3P * vddMax * percentActive * (1 - CKE_LO_ACT);

		// calculate PsysACT
		double tRRDsch = (double)(epochTime) / (i->first + i->second);

		double PsysACT = devicesPerRank * ((double)tRC / (double)tRRDsch) * voltageScaleFactor * PdsACT;

		double PsysACTTotal = ((double)tRC / (double)tRRDsch) * voltageScaleFactor * PdsACT;

		// calculate PdsRD
		double RDschPct =  / (double)(epochTime);

		PsysRD += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsRD() * RDschPct;

		// calculate PdsWR
		double WRschPct = k->getWriteCycles() / (double)(time - powerModel.getLastCalculation());

		PsysWR += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsWR() * WRschPct;


		p2 << time << " " << i->first + i->second << endl;
		//cerr << time << " " << i->first + i->second << endl;
		time += epochTime;
	}

	p2 << "e" << endl;

	time = 0.0F;
	for (vector<pair<unsigned,unsigned> >::const_iterator i = cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		p2 << time << " " << (double)i->first / (i->first + i->second) << endl;
		//cerr << time << " " << (double)i->first / (i->first + i->second) << endl;
		time += epochTime;
	}

	p2 << "e" << endl << "unset output" << endl;

	//////////////////////////////////////////////////////////////////////////
#endif

	p0 << endl << "exit" << endl;
	p1 << endl << "exit" << endl;
	p2 << endl << "exit" << endl;
	p3 << endl << "exit" << endl;

	p0.close();
	p1.close();
	p2.close();
	p3.close();

	{
		boost::mutex::scoped_lock lock(fileListMutex);
		for (list<string>::const_iterator i = filesGenerated.begin(); i != filesGenerated.end(); ++i)
			fileList.push_back(*i);
	}

	bf::path givenfilename(filename);
	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, channelDistribution.size(), graphs);
	return;
}

void sigproc(int i)
{
	userStop = true;
}

namespace opt = boost::program_options;

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
		("cypress,c","Generate only select graphs for Cypress study");

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

	// 	for (opt::variables_map::iterator i = vm.begin(); i != vm.end(); ++i)
	// 		cerr << i->first << endl;
	// 	
	if (vm.count("help"))
	{
		cout << "Usage: " << argv[0] << "(--help | -f | -p)" << endl;
	}

	// 	if (vm.count("png") > 0)
	// 		cerr << "PNG" << endl;

	bool generateResultsOnly = vm.count("create") > 0;

	generatePngFiles = vm.count("png") > 0;

	cypressResults = vm.count("cypress") > 0;

	
	// 	cerr << generateResultsOnly << " " << generatePngFiles << endl;
	// 
	// 	exit(0);

	list<string> toBeProcessed;

	map<string,vector<string> > results;

	vector<string> files;

	for (int i = 0; i < argc; ++i)
	{
		if (ends_with(argv[i],"power.gz") || ends_with(argv[i],"power.bz2") || 
			ends_with(argv[i],"stats.gz") || ends_with(argv[i],"stats.bz2"))
		{
			files.push_back(string(argv[i]));
		}
	}
	for (vector<string>::const_iterator currentFile = files.begin(); currentFile != files.end(); ++currentFile)
	{
		filtering_istream inputStream;

		if (ends_with(*currentFile,"gz"))
			inputStream.push(boost::iostreams::gzip_decompressor());
		else if (ends_with(*currentFile,"bz2"))
			inputStream.push(boost::iostreams::bzip2_decompressor());
		else
			continue;

		inputStream.push(file_source(*currentFile));

		char newLine[NEWLINE_LENGTH];

		if (!inputStream.is_complete())
			continue;

		pair<unsigned,unsigned> readHitsMisses;
		pair<unsigned,unsigned> hitsMisses;
		unsigned epochCounter = 0;
		double epoch = 0.0;
		float averageLatency;
		float averageAdjustedLatency;
		vector<string> currentLine;
		string basefilename;

		bool foundCommandline = false;
		bool foundEpoch = false;

		inputStream.getline(newLine,NEWLINE_LENGTH); 

		unsigned lineCounter = 0;

		while ((newLine[0] != NULL) && (!userStop))
		{
			const string filename(*currentFile);

			if (starts_with(newLine, "----Cumulative DIMM Cache Read Hits/Misses"))
			{
				epochCounter++;

				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL) break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL) break;
				*secondBracket = (char)NULL;
				readHitsMisses.first = max(atoi(firstBracket + 1),1);

				firstBracket = strchr(secondBracket + 1,'{');
				if (firstBracket == NULL) break;

				secondBracket = strchr(secondBracket + 1,'}');
				if (secondBracket == NULL) break;
				*secondBracket = (char)NULL;
				readHitsMisses.second = max(atoi(firstBracket + 1),1);
			}
			else if (starts_with(newLine, "----Cumulative DIMM Cache Hits/Misses"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL) break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL) break;
				*secondBracket = (char)NULL;
				hitsMisses.first = max(atoi(firstBracket + 1),1);

				firstBracket = strchr(secondBracket + 1,'{');
				if (firstBracket == NULL) break;

				secondBracket = strchr(secondBracket + 1,'}');
				if (secondBracket == NULL) break;
				*secondBracket = (char)NULL;
				hitsMisses.second = max(atoi(firstBracket + 1),1);
			}
			else if (starts_with(newLine, "----Average Cumulative Transaction Latency"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL) break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL) break;
				*secondBracket = (char)NULL;
				averageLatency = atof(firstBracket + 1);
			}
			else if (starts_with(newLine, "----Average Cumulative Adjusted Transaction Latency"))
			{
				char *firstBracket = strchr(newLine,'{');
				if (firstBracket == NULL) break;

				char *secondBracket = strchr(newLine,'}');
				if (secondBracket == NULL) break;
				*secondBracket = (char)NULL;
				averageAdjustedLatency = atof(firstBracket + 1);
			}				
			else if (!foundEpoch && starts_with(newLine, "----Epoch"))
			{
				foundEpoch = true;

				char *position = strchr(newLine,' ');
				if (position == NULL) break;
				epoch = atof(position + 1);
			}
			else if (!foundCommandline && starts_with(newLine, "----Command Line:"))
			{
				foundCommandline = true;

				toBeProcessed.push_back(*currentFile);

				basefilename = filename.substr(0,filename.find_last_of('-'));
				//string currentUrlString = ireplace_all_copy(urlString,"%1",basefilename);
				const string commandline(newLine);
				string modUrlString = commandline.substr(commandline.find(':')+2,commandline.length());
				vector<string> splitLine;
				erase_all(modUrlString, "_");
				erase_all(modUrlString, "{");
				erase_all(modUrlString, "}");
				split(splitLine, modUrlString, is_any_of(" "), token_compress_on);


				for (vector<string>::const_iterator x = splitLine.begin(); x != splitLine.end(); ++x)
				{
					string::size_type start = x->find("[");
					string::size_type end = x->find("]");
					string benchmarkName;

					if (start == string::npos || end == string::npos)
						benchmarkName = *x;
					else
						benchmarkName = x->substr(start + 1,end - start - 1);

					if (decoder.find(benchmarkName) != decoder.end())
						benchmarkName = decoder[benchmarkName];

					currentLine.push_back(benchmarkName);
					//"<td>" + ireplace_all_copy(currentUrlString,"%2",benchmarkName) + "</td>";
				}

				// then calculate the runtime for the last column
				if (!ends_with(*currentFile,"stats.gz") && !ends_with(*currentFile,"stats.bz2"))
				{
					if (results.find(basefilename) == results.end())
						results[basefilename] = currentLine;

					// do not read power files all the way through
					break;
				}
			}

			if ((++lineCounter % 2500 )== 0)
			{
				char *position = strchr(newLine,'\n');
				if (position != NULL)
					*position = (char)NULL;
				newLine[50] = NULL;
				cerr << "\r" << setiosflags(ios::right) << std::setw(4) << currentFile - files.begin() + 1 << "/" << setiosflags(ios::left) << std::setw(4) << files.end() - files.begin() << " "<< std::setw(9) << lineCounter << " " << newLine;

				// 					if (lineCounter % 5000 == 0)
				// 						break;
			}

			inputStream.getline(newLine,NEWLINE_LENGTH);
		}
		boost::iostreams::close(inputStream);

		stringstream current;
		current << std::dec << std::fixed << std::setprecision(6) << ((double)epochCounter * epoch);
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(6) << ((float)readHitsMisses.first / ((float)readHitsMisses.first + readHitsMisses.second));
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(6) <<((float)hitsMisses.first / ((float)hitsMisses.first + hitsMisses.second));
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(6) << averageLatency;
		currentLine.push_back(current.str());
		current.str("");

		current << std::dec << std::fixed << std::setprecision(6) << averageAdjustedLatency;
		currentLine.push_back(current.str());

		results[basefilename] = currentLine;

	}

	// then generate result.html
	bf::path openfile = executableDirectory / "template2.html";

	if (!fileExists(openfile.native_directory_string()))
	{
		cerr << "cannot open template file: " << openfile.native_directory_string();
		return -1;
	}

	ifstream instream(openfile.directory_string().c_str());
	stringstream entirefile;
	entirefile << instream.rdbuf();

	string fileList;
	string csvOutput;
	for (map<string,vector<string> >::const_iterator x = results.begin(); x != results.end();++x)
	{
		fileList += "<tr>";

		for (vector<string>::const_iterator i = x->second.begin(), end = x->second.end();
			i != end; )
		{
			csvOutput += *i;


			fileList += "<td>" + ireplace_all_copy(ireplace_all_copy(urlString,"%2",*i),"%1",x->first) + "</td>";

			if (++i != end)
				csvOutput += ',';
			else
				csvOutput += '\n';

		}

		fileList += "</tr>";		
	}

	// write the result html file
	ofstream out("result.html");
	string outString = entirefile.str();
	replace_all(outString,"@@@", fileList);
	out.write(outString.c_str(), outString.length()); 
	out.close();

	// write the result csv file
	ofstream outCsv("result.csv");
	outCsv.write(csvHeader.c_str(),csvHeader.length());
	outCsv.write(csvOutput.c_str(), csvOutput.length());
	outCsv.close();

	// create the js directory 
	bf::path jsDirectory = "js";
	if (!exists(jsDirectory))
		create_directory(jsDirectory);		
	else if (!is_directory(jsDirectory))
	{
		cerr << "error: 'js' exists but is not a directory" << endl;
		return -1;
	}

	bf::path sourceJsDirectory = executableDirectory / "js";

	bf::directory_iterator endIter;
	for (bf::directory_iterator iter(sourceJsDirectory); iter != endIter; ++iter)
	{
		if (!is_directory(*iter))
		{
			bf::path result = jsDirectory / iter->leaf();
			if (!exists(result))
				copy_file(*iter,result);
		}
	}

	// finally, process the files to generate results
	if (!generateResultsOnly)
	{
		boost::thread threadA(thumbNailWorker);

		for (list<string>::const_iterator i = toBeProcessed.begin(); i != toBeProcessed.end(); ++i)
		{
			if (ends_with(*i,"power.gz")|| ends_with(*i,"power.bz2"))
			{
				processPower(*i);
			}
			else if (ends_with(*i,"stats.gz")|| ends_with(*i,"stats.bz2"))
			{
				processStats(*i);
			}
		}

		doneEntering = true;

		boost::thread threadB(thumbNailWorker);


		cerr << "Waiting for post-processing to finish." << endl;

		threadA.join();
		threadB.join();
	}

	return 0;
}

