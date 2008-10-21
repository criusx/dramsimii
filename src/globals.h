// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
// 
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.


/*
To do list:

2. add true open page support
3. Look at power up/down models.
4. improve power models.
6. attach BIU/MCH, port to alpha-sim
*/

#ifndef GLOBALS_H
#define GLOBALS_H
#pragma once

#include <boost/iostreams/filtering_stream.hpp>
#include <sstream>

#include <limits>
#include <boost/integer_traits.hpp>
#include <boost/cstdint.hpp>

#include "enumTypes.h"


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
	class InputStream;
	class Settings;
	class PowerConfig;
	class Bank;
	class Rank;

	void unitTests(const Settings &settings);

	// overloaded insertion operator functions for printing various aspects of the dram system
	std::ostream& operator<<(std::ostream&, const DRAMSimII::CommandType&);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::Command&);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::Address&);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::Transaction&);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::AddressMappingScheme&);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::TransactionType );
	std::ostream& operator<<(std::ostream&, const DRAMSimII::fbdFrame&);	
	std::ostream& operator<<(std::ostream&, const DRAMSimII::PowerConfig&);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::Address &);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::AddressMappingScheme&);
	std::ostream& operator<<(std::ostream&, const DRAMSimII::InputStream&);

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

	//#define DEBUG_FLAG

#define DEBUG_COMMAND

#define DEBUG_TRANSACTION

	//#define DEBUG_RAND

#define DEBUGDRAMSIM

	//#define DEBUG_MIN_PROTOCOL_GAP

	//#define DEBUG_FLAG_2


#if defined(DEBUG) && defined(M5DEBUG) // compiler should declare this
#define M5_TIMING_LOG(X) timingOutStream << X << endl;
#define M5_DEBUG(X) X;
#else
#define M5_TIMING_LOG(X)
#define M5_DEBUG(X)
#endif

#if defined(DEBUG) && defined(SSTDEBUG) // compiler should declare this
#define SST_TIMING_LOG(X) timingOutStream << X << endl;
#define SST_DEBUG(X) X;
#else
#define SST_TIMING_LOG(X)
#define SST_DEBUG(X)
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



	using boost::int64_t;
	using boost::uint64_t;
	using boost::integer_traits;

#define tick int64_t
#define TICK_MAX static_cast<tick>(integer_traits<tick>::const_max)
#define PHYSICAL_ADDRESS uint64_t
#define PHYSICAL_ADDRESS_MAX static_cast<PHYSICAL_ADDRESS>(integer_traits<PHYSICAL_ADDRESS>::const_max)
	//#define TICK_MAX std::numeric_limits<boost::uint64_t>::max()

#define ABS(a) ((a) < 0 ? (-a) : (a))

	//#define INVALID -1

#define PI 3.1415926535897932384626433832795

#define COMMAND_QUEUE_SIZE 32


}
#endif
