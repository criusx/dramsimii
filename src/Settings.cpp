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

#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "globals.h"
#include "Settings.h"


using std::map;
using std::string;
using std::cerr;
using std::endl;
using std::ostream;
using boost::lexical_cast;

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
settingsOutputFile(""),
epoch(UINT_MAX),
inFile(""),
sessionID(""),
arrivalDistributionModel(InputStream::NORMAL_DISTRIBUTION),
inFileType(InputStream::MASE_TRACE),
outFile(""),
outFileDir(""),
commandLine(""),
outFileType(BZ),
requestCount(UINT_MAX),
refreshPolicy(NO_REFRESH),
dramType(DDR),
commandOrderingAlgorithm(STRICT_ORDER),
transactionOrderingAlgorithm(STRICT),
systemType(FBD_CONFIG),
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
rowBufferManagementPolicy(OPEN_PAGE),
addressMappingScheme(Address::BURGER_BASE_MAP),
postedCAS(false),
readWriteGrouping(true),
autoPrecharge(false),
clockGranularity(0),
cachelinesPerRow(0),
channelCount(0),
rankCount(0U),
bankCount(0U),
shortBurstRatio(2.0F),
readPercentage(2.0F),
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
	bool result = true;
	boost::algorithm::to_lower(nodeValue);

	const FileIOToken token = dramTokenizer(nodeName);

	try
	{
		switch (token)
		{
		case unknown_token:
			cerr << nodeName << " failed" << endl;
			return false;
			break;
		case cpu_to_memory_clock_ratio:
			cpuToMemoryClockRatio = lexical_cast<float>(nodeValue);
			break;
		case input_file_token:
			inFile = value;
			break;
		case output_file_token:
			outFile = value;
			break;
		case output_file_dir_token:
			outFileDir = value;
			break;	
		case dbreporting_token:
			dbReporting = nodeValue == "true";
			break;
		case epoch_token:
			epoch = lexical_cast<unsigned>(nodeValue);
			break;
		case frequency_spec_token:
			frequencySpec = lexical_cast<unsigned>(nodeValue);
			break;
		case p_dq_rd_token:
			PdqRD = lexical_cast<float>(nodeValue);
			break;
		case p_dq_wr_token:
			PdqWR = lexical_cast<float>(nodeValue);
			break;
		case p_dq_rd_oth_token:
			PdqRDoth = lexical_cast<float>(nodeValue);
			break;
		case p_dq_wr_oth_token:
			PdqWRoth = lexical_cast<float>(nodeValue);
			break;
		case dq_per_dram_token:
			DQperDRAM = lexical_cast<unsigned>(nodeValue);
			break;
		case dqs_per_dram_token:
			DQSperDRAM = lexical_cast<unsigned>(nodeValue);
			break;
		case dm_per_dram_token:
			DMperDRAM = lexical_cast<unsigned>(nodeValue);
			break;
		case request_count_token:
			requestCount = lexical_cast<tick>(nodeValue);
			break;
		case idd5_token:
			IDD5 = lexical_cast<unsigned>(nodeValue);
			break;
		case idd4r_token:
			IDD4R = lexical_cast<unsigned>(nodeValue);
			break;
		case idd4w_token:
			IDD4W = lexical_cast<unsigned>(nodeValue);
			break;
		case idd3n_token:
			IDD3N = lexical_cast<unsigned>(nodeValue);
			break;
		case idd3p_token:
			IDD3P = lexical_cast<unsigned>(nodeValue);
			break;
		case idd2n_token:
			IDD2N = lexical_cast<unsigned>(nodeValue);
			break;
		case idd2p_token:
			IDD2P = lexical_cast<unsigned>(nodeValue);
			break;
		case average_interarrival_cycle_count:
			averageInterarrivalCycleCount = lexical_cast<unsigned>(nodeValue);
			break;
		case idd0_token:
			IDD0 = lexical_cast<unsigned>(nodeValue);
			break;
		case vdd_token:
			VDD = lexical_cast<float>(nodeValue);
			break;
		case t_cwd_token:
			tCWD = lexical_cast<unsigned>(nodeValue);
			break;
		case t_buffer_delay_token:
			tBufferDelay = lexical_cast<unsigned>(nodeValue);
			break;
		case t_cmd_token:
			tCMD = lexical_cast<unsigned>(nodeValue);
			break;
		case t_wtr_token:
			tWTR = lexical_cast<unsigned>(nodeValue);
			break;
		case t_wr_token:
			tWR = lexical_cast<unsigned>(nodeValue);
			break;
		case t_refi_token:
			tREFI = lexical_cast<unsigned>(nodeValue);
			break;
		case t_rtrs_token:
			tRTRS = lexical_cast<unsigned>(nodeValue);
			break;
		case t_rtp_token:
			tRTP = lexical_cast<unsigned>(nodeValue);
			break;
		case t_rrd_token:
			tRRD = lexical_cast<unsigned>(nodeValue);
			break;
		case t_rfc_token:
			tRFC = lexical_cast<unsigned>(nodeValue);
			break;
		case t_rcd_token:
			tRCD = lexical_cast<unsigned>(nodeValue);
			break;
		case t_rc_token:
			tRC = lexical_cast<unsigned>(nodeValue);
			break;
		case t_rp_token:
			tRP = lexical_cast<unsigned>(nodeValue);
			break;
		case t_ras_token:
			tRAS = lexical_cast<unsigned>(nodeValue);
			break;
		case t_faw_token:
			tFAW = lexical_cast<unsigned>(nodeValue);
			break;
		case t_cas_token:
			tCAS = lexical_cast<unsigned>(nodeValue);
			break;
		case t_burst_token:
			tBurst = lexical_cast<unsigned>(nodeValue);
			break;
		case t_al_token:
			tAL = lexical_cast<unsigned>(nodeValue);
			break;
		case refresh_queue_depth_token:
			refreshQueueDepth = lexical_cast<unsigned>(nodeValue);
			break;
		case bank_count_token:
			bankCount = lexical_cast<unsigned>(nodeValue);
			break;
		case rank_count_token:
			rankCount = lexical_cast<unsigned>(nodeValue);
			break;
		case channel_count_token:
			channelCount = lexical_cast<unsigned>(nodeValue);
			break;
		case short_burst_ratio_token:
			shortBurstRatio = lexical_cast<float>(nodeValue);
			break;
		case read_percentage_token:
			readPercentage = lexical_cast<float>(nodeValue);
			break;					
		case per_bank_queue_depth_token:
			perBankQueueDepth = lexical_cast<unsigned>(nodeValue);
			break;
		case event_queue_depth_token:
			eventQueueDepth = lexical_cast<unsigned>(nodeValue);
			break;
		case transaction_queue_depth_token:
			transactionQueueDepth = lexical_cast<unsigned>(nodeValue);
			break;
		case decode_window_token:
			decodeWindow = lexical_cast<unsigned>(nodeValue);
			break;
		case completion_queue_depth_token:
			completionQueueDepth = lexical_cast<unsigned>(nodeValue);
			break;
		case history_queue_depth_token:
			historyQueueDepth = lexical_cast<unsigned>(nodeValue);
			break;
		case cacheline_size_token:
			cacheLineSize = lexical_cast<unsigned>(nodeValue);
			break;
		case row_size_token:
			rowSize = lexical_cast<unsigned>(nodeValue);
			break;
		case col_size_token:
			columnSize = lexical_cast<unsigned>(nodeValue);
			break;
		case channel_width_token:
			channelWidth = lexical_cast<unsigned>(nodeValue);
			break;
		case col_count_token:
			columnCount = lexical_cast<unsigned>(nodeValue);
			break;
		case row_count_token:
			rowCount = lexical_cast<unsigned>(nodeValue);
			break;
		case cachelines_per_row_token:
			cachelinesPerRow = lexical_cast<unsigned>(nodeValue);
			break;
		case posted_cas_token:
			postedCAS = nodeValue == "true";
			break;
		case seniority_age_limit_token:
			seniorityAgeLimit = lexical_cast<unsigned>(nodeValue);
			break;
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
				refreshPolicy = NO_REFRESH;
			break;
		case read_write_grouping_token:
			readWriteGrouping = nodeValue == "true";
			break;
		case refresh_time_token:
			refreshTime = lexical_cast<unsigned>(nodeValue);
			break;
		case system_configuration_type_token:
			// TODO: if baseline, then normal system, if FBD, then make a FBD system

			break;
		case transaction_ordering_policy_token:
			if (nodeValue == "strict")
				transactionOrderingAlgorithm = STRICT;
			else if (nodeValue == "riff")
				transactionOrderingAlgorithm = RIFF;
			else
				transactionOrderingAlgorithm = STRICT;						
			break;
		case command_ordering_algorithm_token:
			if (nodeValue == "strict")
				commandOrderingAlgorithm = STRICT_ORDER;
			else if (nodeValue == "bankroundrobin")
				commandOrderingAlgorithm = BANK_ROUND_ROBIN;
			else if (nodeValue == "rankroundrobin")
				commandOrderingAlgorithm = RANK_ROUND_ROBIN;
			else if (nodeValue == "wanghop")
				commandOrderingAlgorithm = WANG_RANK_HOP;
			else if (nodeValue == "firstavailableage" || nodeValue == "firstavailable")
				commandOrderingAlgorithm = FIRST_AVAILABLE_AGE;
			else if (nodeValue == "firstavailableriff")
				commandOrderingAlgorithm = FIRST_AVAILABLE_RIFF;
			else if (nodeValue == "firstavailablequeue")
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
				addressMappingScheme = Address::INTEL845G_MAP;
			else if (nodeValue == "sdrambase")
				addressMappingScheme = Address::SDRAM_BASE_MAP;
			else if (nodeValue == "sdramhiperf")
				addressMappingScheme = Address::SDRAM_HIPERF_MAP;
			else if (nodeValue == "closepagehighlocality" || nodeValue == "highlocality")
				addressMappingScheme = Address::CLOSE_PAGE_HIGH_LOCALITY;
			else if (nodeValue == "closepagelowlocality" || nodeValue == "lowlocality")
				addressMappingScheme = Address::CLOSE_PAGE_LOW_LOCALITY;
			else if (nodeValue == "closepagebaselineopt")
				addressMappingScheme = Address::CLOSE_PAGE_BASELINE_OPT;
			else
				addressMappingScheme = Address::SDRAM_HIPERF_MAP;
			break;
		case auto_precharge_token:
			autoPrecharge = nodeValue == "true";
			break;
		case row_buffer_management_policy_token:
			if (nodeValue == "openpage")
				rowBufferManagementPolicy = OPEN_PAGE;
			else if (nodeValue == "closepage")
				rowBufferManagementPolicy = CLOSE_PAGE;
			else if (nodeValue == "closepageaggressive" || nodeValue == "closepageoptimized")
				rowBufferManagementPolicy = CLOSE_PAGE_AGGRESSIVE;
			else if (nodeValue == "openpageaggressive")
				rowBufferManagementPolicy = OPEN_PAGE_AGGRESSIVE;
			else
				rowBufferManagementPolicy = CLOSE_PAGE_AGGRESSIVE;
			break;
		case clock_granularity_token:
			clockGranularity = lexical_cast<unsigned>(nodeValue);
			break;
		case datarate_token:
			dataRate = lexical_cast<unsigned>(nodeValue);
			break;
		case max_vcc_token:
			maxVCC = lexical_cast<float>(nodeValue);
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
		case input_type_token:
			if (nodeValue == "k6" || nodeValue == "K6")
				inFileType = InputStream::K6_TRACE;
			else if (nodeValue == "mase" || nodeValue == "MASE" || nodeValue == "Mase")
				inFileType = InputStream::MASE_TRACE;
			else if (nodeValue == "random" || nodeValue == "RANDOM" || nodeValue == "Random")
				inFileType = InputStream::RANDOM;
			else if (nodeValue == "mapped" || nodeValue == "Mapped" || nodeValue == "MAPPED")
				inFileType = InputStream::MAPPED;
			else if (nodeValue == "ds" || nodeValue == "ds2" || nodeValue == "dramsim" || nodeValue == "dramsim2")
				inFileType = InputStream::DRAMSIM;
			else
				inFileType = InputStream::MAPPED;
			break;
		case random_distribution_token:
			if (nodeValue == "uniform")
				arrivalDistributionModel = InputStream::UNIFORM_DISTRIBUTION;
			else if (nodeValue == "poisson")
				arrivalDistributionModel = InputStream::POISSON_DISTRIBUTION;
			else if (nodeValue == "gaussian")
				arrivalDistributionModel = InputStream::GAUSSIAN_DISTRIBUTION;
			else if (nodeValue == "normal")
				arrivalDistributionModel = InputStream::NORMAL_DISTRIBUTION;
			else
			{
				cerr << "warn: Unrecognized distribution model: \"" << nodeValue << "\", defaulting to uniform";
				arrivalDistributionModel = InputStream::NORMAL_DISTRIBUTION;
			}
			break;
		case output_file_type_token:
			if (nodeValue == "gz")
				outFileType = GZ;
			else if (nodeValue == "bz" || nodeValue == "bzip" || nodeValue == "bzip2")
				outFileType = BZ;
			else if (nodeValue == "cout" || nodeValue == "stdout")
				outFileType = COUT;
			else if (nodeValue == "uncompressed" || nodeValue == "plain")
				outFileType = UNCOMPRESSED;
			else
				outFileType = NONE;
			break;
		default:
			result = false;
			break;
		}

	}
	catch (boost::bad_lexical_cast exception)
	{
		cerr << "Invalid numeric conversion '" << nodeValue << "'" << endl;
		return false;
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
	case WANG_RANK_HOP:
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
		os << "CPAG";
		break;
	default:
		os << "UNKN";
		break;
	}

	return os;
}