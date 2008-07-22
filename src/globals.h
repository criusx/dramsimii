/*
To do list:

2. add true open page support
3. Look at power up/down models.
4. improve power models.
6. attach BIU/MCH, port to alpha-sim
7. convert passed pointers/values to references
8. standardize time variables
9. organize classes better, divide work more intelligently
10. move away from srand48, drand48 functions, they are deprecated
11. switch from machine dependent vars to definite types. e.g. unsigned int -> UINT32, __int64, etc
12. make vars private again and declare various functions as friends
*/

#ifndef GLOBALS_H
#define GLOBALS_H
#pragma once

#include <limits>
#include <boost/integer_traits.hpp>
#include <boost/cstdint.hpp>

using boost::int64_t;
using boost::uint64_t;
using boost::integer_traits;

#define tick int64_t
#define TICK_MAX static_cast<tick>(integer_traits<tick>::const_max)
#define PHYSICAL_ADDRESS uint64_t
#define PHYSICAL_ADDRESS_MAX static_cast<PHYSICAL_ADDRESS>(integer_traits<PHYSICAL_ADDRESS>::const_max)
//#define TICK_MAX std::numeric_limits<boost::uint64_t>::max()

#include <boost/iostreams/filtering_stream.hpp>
#include <sstream>

#include "enumTypes.h"

#define ABS(a) ((a) < 0 ? (-a) : (a))

//#define INVALID -1

#define PI 3.1415926535897932384626433832795

#define COMMAND_QUEUE_SIZE 32

//#define DEBUG_FLAG

#define DEBUG_COMMAND

#define DEBUG_TRANSACTION

//#define DEBUG_RAND

#define DEBUGDRAMSIM

//#define DEBUG_MIN_PROTOCOL_GAP

//#define DEBUG_FLAG_2

// global vars and functions
namespace DRAMSimII
{
	// class forward declarations
	class Command;
	class Address;
	class Transaction;
	class System;
	class Channel;
	class fbdFrame;
	class SystemConfiguration;
	class TimingSpecification;

	// overloaded insertion operator functions for printing various aspects of the dram system
	std::ostream &operator<<(std::ostream &, const DRAMSimII::CommandType &);
	std::ostream &operator<<(std::ostream &, const DRAMSimII::Command &);
	std::ostream &operator<<(std::ostream &, const DRAMSimII::Address &);
	std::ostream &operator<<(std::ostream &, const DRAMSimII::Transaction *);
	std::ostream &operator<<(std::ostream &, const DRAMSimII::AddressMappingScheme &);
	std::ostream &operator<<(std::ostream &, const DRAMSimII::TransactionType );
	std::ostream &operator<<(std::ostream &, const DRAMSimII::fbdFrame &);	

	// will compute log2(n)=x for any n, where n=2**x
	unsigned inline log2(unsigned input)
	{
		unsigned l2 = 0;
		for (input >>= 1; input > 0; input >>= 1)
		{
			l2++;
		}
		return l2;
	}
	

	// converts a string to a number using stringstreams
	template <class T>
	bool toNumeric(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
	{
		std::istringstream iss(s);
		return !(iss >> f >> t).fail();
	}	

	template <class T>
	inline std::string toString(const T& convertThis)
	{
		std::stringstream ss;
		ss << convertThis;
		return ss.str();
	}

	// global var forward
	extern boost::iostreams::filtering_ostream timingOutStream;
	extern boost::iostreams::filtering_ostream powerOutStream;
	extern boost::iostreams::filtering_ostream statsOutStream;

	// converts a string to its corresponding magnitude representation
	double ascii2multiplier(const std::string &);

	// debug macros
#if defined(DEBUG) && defined(M5DEBUG) // compiler should declare this
#define M5_TIMING_LOG(X) timingOutStream << X << endl;
#define M5_DEBUG(X) X;
#else
#define M5_TIMING_LOG(X)
#define M5_DEBUG(X)
#endif

#if defined(DEBUG) && defined(DEBUG_TRANSACTION)// compiler should declare this
#define DEBUG_TRANSACTION_LOG(X) timingOutStream << X << endl;
#else
#define DEBUG_TRANSACTION_LOG(X)
#endif

#if defined(DEBUG) && defined(DEBUG_COMMAND)
#define DEBUG_COMMAND_LOG(X) timingOutStream << X << endl;
#else
#define DEBUG_COMMAND_LOG(X)
#endif

#if defined(DEBUG) && defined(DEBUGDRAMSIM)
#define DEBUG_TIMING_LOG(X) timingOutStream << X << endl;
#define DEBUG_LOG(X) cerr << X << endl;
#else
#define DEBUG_TIMING_LOG(X)
#define DEBUG_LOG(X)
#endif



}

#endif
