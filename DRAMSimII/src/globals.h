/*
To do list:

1. add refresh
2. add open page support
3. Look at power up/down models.
4. re-add power models.
6. attach BIU, port to alpha-sim
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
#include <sstream>

#define tick_t long long
//#define TICK_T_MAX LLONG_MAX

// class forward declarations
class dramSystemConfiguration;
class dramTimingSpecification;
class dramAlgorithm;
class rank_c;
class bank_c;
class transaction;
class command;
class dramSystem;
class ogzstream;

#include <iostream>
#include <map>
#include "addresses.h"
#include "command.h"
#include "enumTypes.h"
#include "transaction.h"

// this is to take care of some nonstandard differences that arise due to having different platforms
// check ISO24731 for details on what rand_s() will be
#ifdef WIN32
#ifdef DEBUG_RAND
#define rand_s(a) (*a)=rand()
#define srand48 srand
#undef UINT_MAX
#define UINT_MAX RAND_MAX
#endif
#define srand48 srand
#else
#define rand_s(a) *a=lrand48()*2
#endif


#define ABS(a) ((a) < 0 ? (-a) : (a))

#define INVALID -1

#define PI 3.1415926535897932384626433832795

#define COMMAND_QUEUE_SIZE 32

//#define DEBUG_FLAG

#define DEBUG_COMMAND

#define DEBUG_TRANSACTION

//#define DEBUG_RAND

//#define DEBUG_MIN_PROTOCOL_GAP

//#define DEBUG_FLAG_2

unsigned inline log2(unsigned input)
{
	int l2 = 0;
	for (input >>= 1; input > 0; input >>= 1)
	{
		l2++;
	}
	return l2;
}


template <class T>
bool toNumeric(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}

// overloaded insertion operator functions for printing various aspects of the dram system
std::ostream &operator<<(std::ostream &, const command_type_t &);
std::ostream &operator<<(std::ostream &, const command &);
std::ostream &operator<<(std::ostream &, const addresses &);
std::ostream &operator<<(std::ostream &, const transaction *&);
std::ostream &operator<<(std::ostream &, const address_mapping_scheme_t &);
std::ostream &operator<<(std::ostream &, const transaction_type_t );

// global var forward
extern ogzstream outStream;

// converts a string to a file_io_token
file_io_token_t file_io_token(const std::string &);
file_io_token_t fileIOToken(const unsigned char *input);

// converts a string to its corresponding magnitude representation
double ascii2multiplier(const std::string &);

// maps the inputs to file_io_tokens and corresponding strings to simplify initialization
void create_input_map(int ,const char **,std::map<file_io_token_t, std::string> &);
void create_input_map_from_input_file(std::map<file_io_token_t, std::string> &,std::ifstream &);

#endif
