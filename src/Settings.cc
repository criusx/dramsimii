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
	systemType(BASELINE_CONFIG),
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