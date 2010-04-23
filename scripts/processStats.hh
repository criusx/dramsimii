#ifndef PROCESSSTATS_HH
#define PROCESSSTATS_HH

#include <parallel/algorithm>
#include <cstdio>
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
#include <boost/circular_buffer.hpp>
#include <boost/regex.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/algorithm/string/regex.hpp>
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

#include "pstream.h"

namespace bf = boost::filesystem;
namespace opt = boost::program_options;

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
using boost::replace_first;
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

// incomplete declarations
void prepareOutputDir(const bf::path &outputDir, const string &filename,
					  const vector<string> &commandLine, list<pair<string,string> > &graphs);

bool fileExists(const string&);

unordered_map<string, string> &setupDecoder();
vector<string> getCommandLine(const string &inputLine);
void processPower(const bf::path &outputDir, const string &filename, const list<pair<string,string> > &powerParams);
void processStats(const bf::path &outputDir, const string &filename);
void sigproc(int i);
bool ensureDirectoryExists(const bf::path &outputDir);
void printTitle(const char *title, const vector<string> &commandLine, std::ostream &p, const unsigned numPlots = 0);
template <typename T>
T regexMatch(const string &input, const char *regex)
{
	return regexMatch<T>(input.c_str(),regex);
}

template <typename T>
T regexMatch(const char *input, const char *regex)
{
	boost::regex currentRegex(regex);
	boost::cmatch match;
	if (!boost::regex_search(input, match, currentRegex))
	{
		cerr << "did not find " << regex << " in " << input << endl;
		throw std::exception();
	}
	else
	{
		//cerr << regex << " " << match[1] << endl;
		return boost::lexical_cast<T>(match[1]);
	}

	return (T)0;
}



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

template<typename T>
class WeightedAverage
{
	unsigned count;
	T total;

public:
	WeightedAverage() :
	  count(0), total(0)
	  {
	  }

	  void add(T value, unsigned count)
	  {
		  this->total += value * (T) count;
		  this->count += count;
	  }

	  void clear()
	  {
		  total = 0;
		  count = 0;
	  }

	  T average()
	  {
		  return total / ((count > 0) ? (T) count : (T) 1);
	  }
};

// using the method of Mendenhall and Sincich (1995)
template<typename T>
class BoxPlot
{
protected:
	map<T, unsigned> values;
	unsigned totalCount;
public:
	BoxPlot() :
	  values(), totalCount(0)
	  {
	  }

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
			  unsigned thirdQuartile = max(round((3.0 * totalCount + 3.0) / 4.0),
				  1.0);
			  if (thirdQuartile > totalCount)
				  thirdQuartile = totalCount;
			  assert(median <= thirdQuartile);

			  T firstQuartileValue;
			  T thirdQuartileValue;
			  T medianValue;

			  unsigned cumulativeSum = 0;

			  typename map<T, unsigned>::const_iterator end = values.end();
			  WeightedAverage<double> wa;

			  // go determine which bin the quartile elements are in
			  for (typename map<T, unsigned>::const_iterator i = values.begin(); i
				  != end; ++i)
			  {
				  wa.add(i->first, i->second);

				  if (firstQuartile > cumulativeSum && firstQuartile
					  <= cumulativeSum + i->second)
					  firstQuartileValue = i->first;
				  if (thirdQuartile > cumulativeSum && thirdQuartile
					  <= cumulativeSum + i->second)
					  thirdQuartileValue = i->first;
				  if (median > cumulativeSum && median <= cumulativeSum
					  + i->second)
				  {
					  // only if it's the last element
					  if (totalCount % 2 == 1 && (cumulativeSum + i->second
						  == median))
					  {
						  typename map<T, unsigned>::const_iterator j(i);
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

			  return tuple<T, T, T, T, T, T> (values.begin()->first,
				  firstQuartileValue, medianValue, wa.average(),
				  thirdQuartileValue, values.rbegin()->first);
		  }
		  else
			  return tuple<T, T, T, T, T, T> ((T) 0, (T) 0, (T) 0, (T) 0, (T) 0,
			  (T) 0);
	  }
};

template<typename T>
class StdDev
{
protected:
	map<T, unsigned> values;
	unsigned totalCount;
public:
	StdDev() :
	  values(), totalCount(0)
	  {
	  }

	  void clear()
	  {
		  values.clear();
		  totalCount = 0;
	  }

	  void add(const T value, const unsigned count)
	  {
		  // shouldn't be any duplicates
		  if (values.find(value) != values.end())
			  cerr << "error: " << value << " exists more than once" << endl;
		  //throw;

		  values[value] = count;

		  totalCount += count;
	  }

	  tuple<T, T, T, T> getStdDev() const
	  {
		  if (totalCount > 0)
		  {
			  typename map<T, unsigned>::const_iterator end = values.end();
			  WeightedAverage<double> wa;

			  // go determine which bin the quartile elements are in
			  for (typename map<T, unsigned>::const_iterator i = values.begin(); i
				  != end; ++i)
			  {
				  wa.add(i->first, i->second);
			  }

			  double mean = wa.average();

			  uint64_t sum = 0;

			  for (typename map<T, unsigned>::const_iterator i = values.begin(); i
				  != end; ++i)
			  {
				  sum += (uint64_t)((double) i->second * ((double) i->first
					  - mean) * ((double) i->first - mean));
			  }

			  double stdDev = sqrt((double) sum / (double) totalCount);

			  return tuple<T, T, T, T> (values.begin()->first, mean, stdDev,
				  values.rbegin()->first);
		  }
		  else
			  return tuple<T, T, T, T> ((T) 0, (T) 0, (T) 0, (T) 0);
	  }
};

class CumulativePriorMovingAverage
{
	double average;
	double totalCount;

public:
	CumulativePriorMovingAverage() :
	  average(0.0), totalCount(0.0)
	  {
	  }

	  void add(double count, double value)
	  {
		  totalCount += count;
		  average += ((count * value - average) / totalCount);
	  }

	  double getAverage() const
	  {
		  return average;
	  }

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
	PriorMovingAverage(unsigned size) :
	  data(size), average(0)
	  {
	  }

	  double getAverage() const
	  {
		  return average;
	  }

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
			  float size = (float) data.size();
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

class PowerCalculations
{
public:
	double PsysACT_STBY, PsysPRE_STBY, PsysPRE_PDN, PsysACT_PDN, PsysACT, PsysRD, PsysWR;
	double PsysACTAdjusted, PsysRdAdjusted, PsysACT_STBYAdjusted, PsysACT_PDNAdjusted;
	double sramActivePower, sramIdlePower;
	double energy, reducedEnergy;

	PowerCalculations():
	PsysACT_STBY(0.0),
		PsysPRE_STBY(0.0),
		PsysPRE_PDN(0.0),
		PsysACT_PDN(0.0),
		PsysACT(0.0),
		PsysRD(0.0),
		PsysWR(0.0), 
		PsysACTAdjusted(0.0),
		PsysRdAdjusted(0.0),
		PsysACT_STBYAdjusted(0.0),
		PsysACT_PDNAdjusted(0.0),
		sramActivePower(0.0),
		sramIdlePower(0.0),
		energy(0.0),
		reducedEnergy(0.0)
	{};
};

template <typename T>
class PowerParameters
{
public:
	// power values
	T pDsAct;
	T pDsActStby;
	T pDsActPdn;
	T pDsPreStby;
	T pDsPrePdn;
	T pDsRd;
	T pDsWr;
	T voltageScaleFactor;
	T frequencyScaleFactor;
	T tRc;
	T tBurst;
	T CKE_LO_PRE;
	T CKE_LO_ACT;
	T freq;
	T idd1;
	T vdd;
	T devicesPerRank;
	T hitLatency;
	T idd;
	T isb1;

	// system information
	unsigned ranksPerDimm;

	PowerParameters():
	CKE_LO_ACT(0.01),
		CKE_LO_PRE(0.95),
		devicesPerRank(8.0)
	{}

	void setParameters(const char* commandLine, const list<pair<string,string> > &updatedPowerParams)
	{
		tRc = regexMatch<T>(commandLine, "\\{RC\\}\\[([0-9]+)\\]");

		tBurst = regexMatch<T>(commandLine, "tBurst\\[([0-9]+)\\]");

		idd1 = regexMatch<T>(commandLine,"IDD1\\[([0-9]+)\\]");

		T idd0 = regexMatch<T>(commandLine,"IDD0\\[([0-9]+)\\]");

		T idd3n = regexMatch<T>(commandLine,"IDD3N\\[([0-9]+)\\]");

		T tRas = regexMatch<T>(commandLine,"\\{RAS\\}\\[([0-9]+)\\]");

		T idd2n = regexMatch<T>(commandLine,"IDD2N\\[([0-9]+)\\]");

		vdd = regexMatch<T>(commandLine,"VDD\\[([0-9.]+)\\]");

		T vddMax = regexMatch<T>(commandLine,"VDDmax\\[([0-9.]+)\\]");

		T idd3p = regexMatch<T>(commandLine,"IDD3P\\[([0-9]+)\\]");

		T idd2p = regexMatch<T>(commandLine,"IDD2P\\[([0-9]+)\\]");

		T idd4r = regexMatch<T>(commandLine,"IDD4R\\[([0-9]+)\\]");

		T idd4w = regexMatch<T>(commandLine,"IDD4W\\[([0-9]+)\\]");

		T specFreq = regexMatch<T>(commandLine,"spedFreq\\[([0-9]+)\\]");

		freq = regexMatch<T>(commandLine,"DR\\[([0-9]+)M\\]") * 1E6;

		/// @TODO fix this to read the value from the simulation
		devicesPerRank = 8;

		int channelWidth = regexMatch<int>(commandLine,"ChannelWidth\\[([0-9]+)\\]");

		int dqPerDram = regexMatch<int>(commandLine,"DQPerDRAM\\[([0-9]+)\\]");

		ranksPerDimm = regexMatch<unsigned>(commandLine,"rk\\[([0-9]+)\\]");

		unsigned associativity = regexMatch<unsigned>(commandLine,"assoc\\[([0-9]+)\\]");

		/// @TODO get this value from the simulation
		idd = 500; // in mA

		/// @TODO get this from the simulation results
		isb1 = 340; // in mA

		if (associativity == 2)
			hitLatency = 2.0;
		else if (associativity == 4)
			hitLatency = 3.0;
		else if (associativity == 8)
			hitLatency = 4.0;
		else if (associativity == 16)
			hitLatency = 5.0;
		else if (associativity == 24)
			hitLatency = 6.0;
		else if (associativity == 32)
			hitLatency = 7.0;
		else
		{
			cerr << "error: unknown associativity, cannot set hit latency accordingly" << endl;
			exit(-1);
		}

		// read power params to see what the requested changes are
		for (list<pair<string,string> >::const_iterator currentPair = updatedPowerParams.begin(), end = updatedPowerParams.end();
			currentPair != end; ++currentPair)
		{
			string parameter(currentPair->first);
			string value(currentPair->second);
			boost::algorithm::to_lower(parameter);

			if (parameter == "idd0")
			{
#ifndef NDEBUG
				cerr << "note: idd0 updated to " << value << endl;
#endif
				idd0 = lexical_cast<T>(value);
			}
			else if (parameter == "devicesperrank")
			{
#ifndef NDEBUG
				cerr << "note: devicesPerRank updated to " << value << endl;
#endif // NDEBUG
				devicesPerRank = lexical_cast<T>(value);
			}
			else if (parameter == "idd")
			{
#ifndef NDEBUG
				cerr << "note: IDD (DIMM cache) updated to " << value << endl;
#endif
				idd = lexical_cast<T>(value);
			}
			else if (parameter == "isb1")
			{
#ifndef NDEBUG
				cerr << "note: ISB1 updated to " << value << endl;
#endif
				isb1 = lexical_cast<T>(value);
			}
			else if (parameter == "idd3n")
			{
#ifndef NDEBUG
				cerr << "note: idd3n updated to " << value << endl;
#endif
				idd3n = lexical_cast<T>(value);
			}
			else if (parameter == "tras")
			{
#ifndef NDEBUG
				cerr << "note: tras updated to " << value << endl;
#endif
				tRas = lexical_cast<T>(value);
			}
			else if (parameter == "idd2n")
			{
#ifndef NDEBUG
				cerr << "note: idd2n updated to " << value << endl;
#endif
				idd2n = lexical_cast<T>(value);
			}
			else if (parameter == "vdd")
			{
#ifndef NDEBUG
				cerr << "note: vdd updated to " << value << endl;
#endif
				vdd = lexical_cast<T>(value);
			}
			else if (parameter == "idd3n")
			{
#ifndef NDEBUG
				cerr << "note: idd3n updated to " << value << endl;
#endif
				idd3n = lexical_cast<T>(value);
			}
			else if (parameter == "vddmax")
			{
#ifndef NDEBUG
				cerr << "note: vddmax updated to " << value << endl;
#endif
				vddMax = lexical_cast<T>(value);
			}
			else if (parameter == "idd3p")
			{
#ifndef NDEBUG
				cerr << "note: idd3p updated to " << value << endl;
#endif
				idd3p = lexical_cast<T>(value);
			}
			else if (parameter == "idd2p")
			{
#ifndef NDEBUG
				cerr << "note: idd2p updated to " << value << endl;
#endif
				idd2p = lexical_cast<T>(value);
			}
			else if (parameter == "idd4r")
			{
#ifndef NDEBUG
				cerr << "note: idd4r updated to " << value << endl;
#endif
				idd4r = lexical_cast<T>(value);
			}
			else if (parameter == "idd4w")
			{
#ifndef NDEBUG
				cerr << "note: idd4w updated to " << value << endl;
#endif
				idd4w = lexical_cast<T>(value);
			}
			else if (parameter == "specfreq")
			{
#ifndef NDEBUG
				cerr << "note: specFreq updated to " << value << endl;
#endif
				specFreq = lexical_cast<T>(value);
			}
			else if (parameter == "freq")
			{
#ifndef NDEBUG
				cerr << "note: freq updated to " << value << endl;
#endif
				freq = lexical_cast<T>(value);
			}
			else if (parameter == "channelwidth")
			{
#ifndef NDEBUG
				cerr << "note: channelWidth updated to " << value << endl;
#endif
				channelWidth = lexical_cast<T>(value);
			}
			else if (parameter == "dqperdram")
			{
#ifndef NDEBUG
				cerr << "note: dqPerDram updated to " << value << endl;
#endif
				dqPerDram = lexical_cast<T>(value);
			}
			else if (parameter == "cke_lo_pre")
			{
#ifndef NDEBUG
				cerr << "note: CKE_LO_PRE updated to " << value << endl;
#endif
				CKE_LO_PRE = lexical_cast<T>(value);
			}
			else if (parameter == "cke_lo_act")
			{
#ifndef NDEBUG
				cerr << "note: CKE_LO_ACT updated to " << value << endl;
#endif
				CKE_LO_ACT = lexical_cast<T>(value);
			}
		}

		// update power values
		devicesPerRank = channelWidth / dqPerDram;
		pDsAct = (idd0 - ((idd3n * tRas + idd2n * (tRc - tRas))/tRc)) * vdd;
		pDsActStby = idd3n * vdd;
		pDsActPdn = idd3p * vdd;
		pDsPreStby = idd2n * vdd;
		pDsPrePdn = idd2p * vdd;
		frequencyScaleFactor= freq / specFreq;
		voltageScaleFactor = (vdd / vddMax) * (vdd / vddMax);
		pDsRd = (idd4r - idd3n) * vdd;
		pDsWr = (idd4w - idd3n) * vdd;
	}

	PowerCalculations calculateSystemPower(const char* newLine, const T epochTime)
	{
		string line(newLine);
		vector<string> splitLine;
		boost::split_regex(splitLine, line, regex(" rk"));

		vector<string>::const_iterator currentRank = splitLine.begin(), end = splitLine.end();
		currentRank++;

		PowerCalculations pc;

		double totalReadHits = 0.0;

		const double duration = regexMatch<float>(currentRank->c_str(),"duration\\{([0-9]+)\\}");

		// calculate SRAM power from the DIMM caches
		string &lastRank = splitLine.back();
		vector<string> splitDimmLine;
		boost::split_regex(splitDimmLine, lastRank, regex(" dimm\\["));

		vector<string>::const_iterator currentDimm = splitDimmLine.begin(), dimmEnd = splitDimmLine.end();
		currentDimm++;

		for (;currentDimm != dimmEnd; ++currentDimm)
		{
			unsigned accesses;

			try
			{
				unsigned readHits = regexMatch<unsigned>(currentDimm->c_str(),"dimmReadHits\\{([0-9]+)\\}");
				unsigned readMisses = regexMatch<unsigned>(currentDimm->c_str(),"dimmReadMisses\\{([0-9]+)\\}");
				unsigned writeHits = regexMatch<unsigned>(currentDimm->c_str(),"dimmWriteHits\\{([0-9]+)\\}");
				unsigned writeMisses = regexMatch<unsigned>(currentDimm->c_str(),"dimmWriteMisses\\{([0-9]+)\\}");

				// number of accesses * duration of access gives the time that this DIMM cache spent in Idd
				accesses = readHits + readMisses + writeHits + writeMisses;
			}
			catch (std::exception& e)
			{
				unsigned reads = regexMatch<unsigned>(currentDimm->c_str(),"read\\{([0-9]+)\\}") / 8;
				unsigned writes = regexMatch<unsigned>(currentDimm->c_str(),"write\\{([0-9]+)\\}") / 8;

				accesses = reads + writes;
			}

			unsigned inUseTime = accesses * hitLatency;
			assert(inUseTime >= 0);

			// the remaining time was spent in idle mode
			double idleTime = duration - (double)inUseTime;
			assert(idleTime >= 0);

			pc.sramActivePower += (double)inUseTime / duration * idd;
			pc.sramIdlePower += (double)idleTime / duration * isb1;
		}

		for (;currentRank != end; ++currentRank)
		{
			//cerr << *currentRank << endl;
			unsigned currentRankID = regexMatch<unsigned>(currentRank->c_str(),"^\\[([0-9]+)\\]");


			// because each rank on any dimm will report the same hit and miss counts
			//if ((currentRankID % ranksPerDimm) == 0)


			//double duration = regexMatch<float>(currentRank->c_str(),"duration\\{([0-9]+)\\}");
			double thisRankRasCount = regexMatch<double>(currentRank->c_str(),"rasCount\\{([0-9]+)\\}");
			double thisRankAdjustedRasCount = regexMatch<double>(currentRank->c_str(),"adjRasCount\\{([0-9]+)\\}");
			double readCycles = regexMatch<double>(currentRank->c_str(),"read\\{([0-9]+)\\}");
			double writeCycles = regexMatch<double>(currentRank->c_str(),"write\\{([0-9]+)\\}");
			double readHits = regexMatch<double>(currentRank->c_str(),"readHits\\{([0-9]+)\\}");
			totalReadHits += readHits;
			double prechargeTime = regexMatch<float>(currentRank->c_str(),"prechargeTime\\{([0-9]+)\\}");
			double percentActive = 1.0 - (prechargeTime / max((double)(duration), 0.00000001));
			assert(percentActive >= 0.0F && percentActive <= 1.0F);

			// background power analysis
			// activate-standby
			double PschACT_STBY = pDsActStby * percentActive * (1 - CKE_LO_ACT);
			pc.PsysACT_STBY += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * PschACT_STBY;
			assert(readHits < readCycles / 8);
			pc.PsysACT_STBYAdjusted += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * PschACT_STBY * (readHits / (readCycles / 8));
			//cerr << pc.PsysACT_STBYAdjusted << " " << pc.PsysACT_STBY << endl;

			// precharge-standby
			double PschPRE_STBY = pDsPreStby * (1.0 - percentActive) * (1 - CKE_LO_PRE);
			pc.PsysPRE_STBY += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschPRE_STBY;
			
			// precharge-powerdown
			double PschPRE_PDN = pDsPrePdn * (1.0 - percentActive) * (CKE_LO_PRE);
			pc.PsysPRE_PDN += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschPRE_PDN;

			// activate-powerdown
			double PschACT_PDN = pDsActPdn * percentActive * CKE_LO_ACT;
			pc.PsysACT_PDN += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschACT_PDN;
			pc.PsysACT_PDNAdjusted += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschACT_PDN * (1 + readHits / (readCycles / 8));
			//cerr << pc.PsysACT_PDN << " " << pc.PsysACT_PDNAdjusted << endl;

			// activate power analysis
			double tRRDsch = ((double)duration) / (thisRankRasCount > 0 ? thisRankRasCount : 0.00000001);
			double PschACT = pDsAct * tRc / tRRDsch;
			pc.PsysACT += devicesPerRank * voltageScaleFactor * PschACT;
			double tRRDschAdj = ((double)duration) / (thisRankAdjustedRasCount > 0 ? thisRankAdjustedRasCount : 0.00000001);
			double PschACTAdjusted = pDsAct * tRc / tRRDsch;
			pc.PsysACTAdjusted += devicesPerRank * voltageScaleFactor * PschACTAdjusted;

			// read power analysis
			double RDschPct = readCycles / duration;
			pc.PsysRD += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsRd * RDschPct;
			// changed due to cache
			double RDschPctAdj = (readCycles - readHits * tBurst) / duration;
			assert(RDschPctAdj > 0.0);
			pc.PsysRdAdjusted += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsRd * RDschPctAdj;

			// write power analysis
			double WRschPct = writeCycles / duration;
			pc.PsysWR += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsWr * WRschPct;
		}	

		pc.energy = (pc.PsysACT_STBY + pc.PsysACT + pc.PsysPRE_STBY + pc.PsysRD + pc.PsysWR + pc.PsysACT_PDN + pc.PsysPRE_PDN) * epochTime;
		//cerr << epochTime << " " << freq << endl;
		//pc.reducedEnergy = idd1 * devicesPerRank * tRc / freq * vdd * totalReadHits;
		pc.reducedEnergy = (pc.PsysACT_STBYAdjusted + pc.PsysACTAdjusted + pc.PsysPRE_STBY + pc.PsysRdAdjusted + pc.PsysWR + pc.PsysACT_PDNAdjusted + pc.PsysPRE_PDN + pc.sramActivePower + pc.sramIdlePower) * epochTime;


		return pc;
	}
};
#endif