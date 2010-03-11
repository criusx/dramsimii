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
#ifndef GLOBALS_H
#define GLOBALS_H
#pragma once
//#define __STDC_LIMIT_MACROS

#include <boost/iostreams/filtering_stream.hpp>
#include <sstream>

#include <limits>
#include <cmath>

#include "enumTypes.hh"
#ifdef _MSC_VER
typedef __int64 int64_t;
#include <boost/cstdint.hpp>
//typedef boost::uint64_t uint64_t;
#include <limits.h>
#define TICK_MAX _I64_MAX
#define TICK_MIN _I64_MIN
#define PHYSICAL_ADDRESS_MAX _UI64_MAX
#else
#include <cstdint>
#define TICK_MAX INT64_MAX
#define TICK_MIN INT64_MIN
#define PHYSICAL_ADDRESS_MAX UINT64_MAX
#endif


// global vars and functions
namespace DRAMsimII
{
	// class forward declarations	
	class Address;
	class Event;
	class Command;
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
	class BusEvent;
	class Cache;

	void unitTests(const Settings &settings);
	
	// overloaded insertion operator functions for printing various aspects of the dram system
	//std::ostream& operator<<(std::ostream&, const DRAMsimII::Command::CommandType&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::Bank&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::Channel&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::Command&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::Event&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::Address&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::Transaction&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::fbdFrame&);	
	std::ostream& operator<<(std::ostream&, const DRAMsimII::PowerConfig&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::InputStream&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::CommandOrderingAlgorithm);
	std::ostream& operator<<(std::ostream&, const RowBufferPolicy);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::SystemConfigurationType);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::RefreshPolicy);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::TransactionOrderingAlgorithm);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::OutputFileType);
	//std::ostream& operator<<(std::ostream&, const DRAMsimII::Cache::ReplacementPolicy);

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

	template <class T>
	inline T leastSigBit(T n)
	{
		return n & ~(n - 1);
	}

	template <class T>
	inline bool isPowerOf2(T n)
	{
		return n != 0 && leastSigBit(n) == n;
	}

	inline int floorLog2(unsigned x)
	{
		assert(x > 0);

		int y = 0;

		if (x & 0xffff0000) { y += 16; x >>= 16; }
		if (x & 0x0000ff00) { y +=  8; x >>=  8; }
		if (x & 0x000000f0) { y +=  4; x >>=  4; }
		if (x & 0x0000000c) { y +=  2; x >>=  2; }
		if (x & 0x00000002) { y +=  1; }

		return y;
	}

#define EPSILON 1E-5

	template <class T>
	inline bool AlmostEqual(T nVal1, T nVal2)
	{
		return std::abs(nVal1 - nVal2) <= EPSILON * std::fabs(nVal1);
		// see Knuth section 4.2.2 pages 217-218
	}
	
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


#if defined(DEBUG) && defined(M5DEBUG) && !defined(NDEBUG) // compiler should declare this
#define M5_TIMING_LOG(X) systemConfig.timingOutStream << X << std::endl;
#define M5_DEBUG(X) X;
#else
#define M5_TIMING_LOG(X)
#define M5_DEBUG(X)
#endif

#if defined(DEBUG) && defined(SSTDEBUG) && !defined(NDEBUG) // compiler should declare this
#define SST_TIMING_LOG(X) systemConfig.timingOutStream << X << std::endl;
#define SST_DEBUG(X) X;
#else
#define SST_TIMING_LOG(X)
#define SST_DEBUG(X)
#endif

#if defined(DEBUG) && defined(DEBUG_TRANSACTION) && !defined(NDEBUG) // compiler should declare this
#define DEBUG_TRANSACTION_LOG(X) systemConfig.timingOutStream << X << std::endl;
#else
#define DEBUG_TRANSACTION_LOG(X)
#endif

	
#if defined(DEBUG) && defined(DEBUG_COMMAND) && !defined(NDEBUG)
#define DEBUG_COMMAND_LOG(X) systemConfig.timingOutStream << X << std::endl;
#else
#define DEBUG_COMMAND_LOG(X)
#endif

#if defined(DEBUG) && defined(DEBUGDRAMSIM) && !defined(NDEBUG)
#define DEBUG_TIMING_LOG(X) systemConfig.timingOutStream << X << std::endl;
#define DEBUG_LOG(X) cerr << X << endl;
#else
#define DEBUG_TIMING_LOG(X)
#define DEBUG_LOG(X)
#endif

#ifdef _MSC_VER
	typedef boost::uint64_t PhysicalAddress;
typedef __int64 tick;
#else
	typedef uint64_t PhysicalAddress;
	typedef int64_t tick;
#endif



//	typedef boost::int64_t tick;
//#define TICK_MAX static_cast<tick>(boost::integer_traits<tick>::const_max)
//#define TICK_MIN static_cast<tick>(boost::integer_traits<tick>::const_min)
// x86-64 defines long mode as having a physical address space of 64-bits, although most current implementations use only 48
//#define PHYSICAL_ADDRESS_MAX static_cast<PhysicalAddress>(boost::integer_traits<PhysicalAddress>::const_max)

#define PI 3.1415926535897932384626433832795

#define POOL_SIZE 64
}
#endif
