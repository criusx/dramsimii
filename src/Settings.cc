// Copyright (C) 2010 University of Maryland.
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

#include <boost/random.hpp>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "globals.hh"
#include "Settings.hh"

using std::map;
using std::string;
using std::cerr;
using std::endl;
using std::ostream;

using namespace DRAMsimII;

namespace DRAMsimII
{
	map<string, FileIOToken> Settings::tokenizeMap = Settings::createTokenizer();
	map<FileIOToken, string> Settings::lookupMap = Settings::lookupSetup();
}


//////////////////////////////////////////////////////////////////////////
/// @brief blank settings constructor
//////////////////////////////////////////////////////////////////////////
Settings::Settings():
epoch(UINT_MAX),
	refreshPolicy(NO_REFRESH),
	dramType(DDR),
	dataRate(800000000),
	commandOrderingAlgorithm(STRICT_ORDER),
	transactionOrderingAlgorithm((TransactionOrderingAlgorithm)STRICT),
	perBankQueueDepth(),
	columnSize(0),
	rowSize(0),
	channelWidth(0),
	columnCount(0),
	rowCount(0),
	cacheLineSize(0),
	historyQueueDepth(0),
	completionQueueDepth(0),
	transactionQueueDepth(0),
	eventQueueDepth(0),
	refreshQueueDepth(0),
	refreshTime(0),
	seniorityAgeLimit(0),
	decodeWindow(UINT_MAX),
	rowBufferManagementPolicy(OPEN_PAGE),
	addressMappingScheme(Address::BURGER_BASE_MAP),
	postedCAS(false),
	readWriteGrouping(true),
	autoPrecharge(false),
	clockGranularity(0),
	cachelinesPerRow(0),
	channelCount(0U),
	dimmCount(0U),
	rankCount(0U),
	bankCount(0U),
	tRTRS(UINT_MAX),
	tAL(UINT_MAX),
	tBurst(UINT_MAX),
	tCAS(UINT_MAX),
	tCWD(UINT_MAX),
	tFAW(UINT_MAX),
	tRAS(UINT_MAX),
	tRC(UINT_MAX),
	tRCD(UINT_MAX),
	tREFI(UINT_MAX),
	tRFC(UINT_MAX),
	tRP(UINT_MAX),
	tRRD(UINT_MAX),
	tRTP(UINT_MAX),
	tWR(UINT_MAX),
	tWTR(UINT_MAX),
	tCMD(UINT_MAX),
	tInternalBurst(UINT_MAX),
	tBufferDelay(UINT_MAX),
	cpuToMemoryClockRatio(-1.0F),
	PdqRD(-1.0F),
	PdqWR(-1.0F),
	PdqRDoth(-1.0F),
	PdqWRoth(-1.0F),
	DQperDRAM(UINT_MAX),
	DQSperDRAM(UINT_MAX),
	DMperDRAM(UINT_MAX),
	frequencySpec(UINT_MAX),
	maxVCC(-1.0F),
	VDD(-1.0F),
	IDD0(UINT_MAX),
	IDD1(UINT_MAX),
	IDD2P(UINT_MAX),
	IDD2N(UINT_MAX),
	IDD3P(UINT_MAX),
	IDD3N(UINT_MAX),
	IDD4W(UINT_MAX),
	IDD4R(UINT_MAX),
	IDD5(UINT_MAX)
{}


//////////////////////////////////////////////////////////////////////////
/// @brief sets a parameter based on a name and value
/// @details looks for and attempts to parse a parameter name and value
/// using the name and value passed in
/// @return true if the parameter was recognized and the value converted
/// false otherwise
//////////////////////////////////////////////////////////////////////////
bool Settings::setKeyValue(const string &nodeName, const string &value)
{
	string nodeValue = value;
	//bool result = true;
	boost::algorithm::to_lower(nodeValue);

	const FileIOToken token = dramTokenizer(nodeName);

	switch (token)
	{

	case refresh_policy_token:
		if (nodeValue == "none" || nodeValue == "no refresh")
			refreshPolicy = NO_REFRESH;
		else if (nodeValue == "bankconcurrent")
			refreshPolicy = BANK_CONCURRENT;
		else if (nodeValue == "bankstaggeredhidden")
			refreshPolicy = BANK_STAGGERED_HIDDEN;
		else if (nodeValue == "refreshonechanallrankallbank")
			refreshPolicy = ONE_CHANNEL_ALL_RANK_ALL_BANK;
		else
			return false;
		break;
	case system_configuration_type_token:
		// TODO: if baseline, then normal system, if FBD, then make a FBD system

		break;
	case transaction_ordering_policy_token:
		if (nodeValue == "strict")
			transactionOrderingAlgorithm = (TransactionOrderingAlgorithm)STRICT;
		else if (nodeValue == "riff")
			transactionOrderingAlgorithm = (TransactionOrderingAlgorithm)RIFF;
		else
			transactionOrderingAlgorithm = (TransactionOrderingAlgorithm)STRICT;						
		break;
	case command_ordering_algorithm_token:
		if (nodeValue == "strict")
			commandOrderingAlgorithm = STRICT_ORDER;
		else if (nodeValue == "bankroundrobin" || nodeValue == "brr")
			commandOrderingAlgorithm = BANK_ROUND_ROBIN;
		else if (nodeValue == "rankroundrobin" || nodeValue == "rrr")
			commandOrderingAlgorithm = RANK_ROUND_ROBIN;
		else if (nodeValue == "cprh" || nodeValue == "commandpairrankhop" || nodeValue == "command_pair_rank_hop" || nodeValue == "commandpairrankhopping")
			commandOrderingAlgorithm = COMMAND_PAIR_RANK_HOPPING;
		else if (nodeValue == "firstavailableage" || nodeValue == "firstavailable" || nodeValue == "frsta")
			commandOrderingAlgorithm = FIRST_AVAILABLE_AGE;
		else if (nodeValue == "firstavailableriff" || nodeValue == "frstr")
			commandOrderingAlgorithm = FIRST_AVAILABLE_RIFF;
		else if (nodeValue == "firstavailablequeue" || nodeValue == "frstq")
			commandOrderingAlgorithm = FIRST_AVAILABLE_QUEUE;
		else 
		{
			cerr << "Unrecognized ordering algorithm: " << nodeValue << endl;
			commandOrderingAlgorithm = FIRST_AVAILABLE_AGE;
		}
		break;
	case addr_mapping_scheme_token:
		if (nodeValue == "burgerbase")
			addressMappingScheme = Address::BURGER_BASE_MAP;
		else if (nodeValue == "closepagebaseline")
			addressMappingScheme = Address::CLOSE_PAGE_BASELINE;
		else if (nodeValue == "intel845g")
		{
			if (channelCount > 1)
			{
				cerr << "error: Intel 845G doesn't support multiple channels, resetting to SDRAM Base Map" << endl;
				addressMappingScheme = Address::SDRAM_BASE_MAP;
			}
			else
			{
				addressMappingScheme = Address::INTEL845G_MAP;
			}				
		}
		else if (nodeValue == "sdrambase" || nodeValue == "sdbas")
			addressMappingScheme = Address::SDRAM_BASE_MAP;
		else if (nodeValue == "sdramhiperf" || nodeValue == "sdhipf")
			addressMappingScheme = Address::SDRAM_HIPERF_MAP;
		else if (nodeValue == "closepagehighlocality" || nodeValue == "highlocality" || nodeValue == "hiloc")
			addressMappingScheme = Address::CLOSE_PAGE_HIGH_LOCALITY;
		else if (nodeValue == "closepagelowlocality" || nodeValue == "lowlocality"  || nodeValue == "loloc")
			addressMappingScheme = Address::CLOSE_PAGE_LOW_LOCALITY;
		else if (nodeValue == "closepagebaselineopt"  || nodeValue == "cpbopt")
			addressMappingScheme = Address::CLOSE_PAGE_BASELINE_OPT;
		else
			addressMappingScheme = Address::SDRAM_HIPERF_MAP;
		break;
	case row_buffer_management_policy_token:
		if (nodeValue == "openpage" || nodeValue == "open")
			rowBufferManagementPolicy = OPEN_PAGE;
		else if (nodeValue == "closepage" || nodeValue == "clos")
			rowBufferManagementPolicy = CLOSE_PAGE;
		else if (nodeValue == "closepageaggressive" || nodeValue == "closepageoptimized" || nodeValue == "cpa")
			rowBufferManagementPolicy = CLOSE_PAGE_AGGRESSIVE;
		else if (nodeValue == "openpageaggressive" || nodeValue == "opa")
			rowBufferManagementPolicy = OPEN_PAGE_AGGRESSIVE;
		else
			return false;
		break;
	case dram_type_token:
		if (nodeValue == "ddr2")
			dramType = DDR2;
		else if (nodeValue == "ddr")
			dramType = DDR;
		else if (nodeValue == "ddr3")
			dramType = DDR3;
		else if (nodeValue == "drdram")
			dramType = DRDRAM;
		else
			dramType = DDR2;
		break;
	default:
		cerr << nodeName << " was not recognized" << endl;
		return false;
		break;
		//result = false;
		break;
	}


	return true;
}

ostream &DRAMsimII::operator<<(ostream &os, const CommandOrderingAlgorithm coa)
{
	switch (coa)
	{
	case STRICT_ORDER:
		os << "STR";
		break;
	case RANK_ROUND_ROBIN:
		os << "RRR";
		break;
	case BANK_ROUND_ROBIN:
		os << "BRR";
		break;
	case FIRST_AVAILABLE_AGE:
		os << "FRSTA";
		break;
	case FIRST_AVAILABLE_RIFF:
		os << "FRSTR";
		break;
	case FIRST_AVAILABLE_QUEUE:
		os << "FRSTQ";
		break;
	case COMMAND_PAIR_RANK_HOPPING:
		os << "CPRH";
		break;
	}

	return os;
}

ostream &DRAMsimII::operator<<(ostream &os, const RowBufferPolicy rbp)
{
	switch (rbp)
	{
	case AUTO_PAGE:
		os << "AUTO";
		break;
	case OPEN_PAGE:
		os << "OPEN";
		break;
	case OPEN_PAGE_AGGRESSIVE:
		os << "OPA";
		break;
	case CLOSE_PAGE:
		os << "CLOS";
		break;
	case CLOSE_PAGE_AGGRESSIVE:
		os << "CPA";
		break;
	default:
		os << "UNKN";
		break;
	}

	return os;
}