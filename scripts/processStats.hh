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
					  const string &commandLine, list<pair<string,
					  string> > &graphs);

bool fileExists(const string&);



unordered_map<string, string> &setupDecoder();

void processPower(const bf::path &outputDir, const string &filename);
void processStats(const bf::path &outputDir, const string &filename);
void sigproc(int i);
bool ensureDirectoryExists(const bf::path &outputDir);


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
			  throw;

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
#endif