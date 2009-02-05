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
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "globals.h"
#include "Settings.h"

using namespace std;
using namespace DRAMsimII;

namespace DRAMsimII
{
	std::map<std::string, FileIOToken> Settings::tokenizeMap = Settings::createTokenizer();
	std::map<FileIOToken, std::string> Settings::lookupMap = Settings::lookupSetup();
}


// for unit tests, should not be instantiated
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
bool Settings::setKeyValue(const string nodeName, const string value)
{
	string nodeValue = value;
	bool result = true;
	//std::transform(nodeValue.begin(),nodeValue.end(),nodeValue.begin(),std::ptr_fun((int (*)( int ))std::tolower));
	boost::algorithm::to_lower(nodeValue);

	const FileIOToken token = dramTokenizer(nodeName);
	
	switch (token)
	{
	case unknown_token:
		cerr << nodeName << " failed" << endl;
		return false;
		break;
	case cpu_to_memory_clock_ratio:
		result = toNumeric<float>(cpuToMemoryClockRatio,nodeValue,std::dec);
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
		dbReporting = nodeValue == "true" ? true : false;
		break;
	case epoch_token:
		result = toNumeric<unsigned>(epoch,nodeValue,std::dec);
		break;
	case frequency_spec_token:
		result =  toNumeric<unsigned>(frequencySpec,nodeValue,std::dec);
		break;
	case p_dq_rd_token:
		result =  toNumeric<float>(PdqRD,nodeValue,std::dec);
		break;
	case p_dq_wr_token:
		result =  toNumeric<float>(PdqWR,nodeValue,std::dec);
		break;
	case p_dq_rd_oth_token:
		result =  toNumeric<float>(PdqRDoth,nodeValue,std::dec);
		break;
	case p_dq_wr_oth_token:
		result =  toNumeric<float>(PdqWRoth,nodeValue,std::dec);
		break;
	case dq_per_dram_token:
		result =  toNumeric<unsigned>(DQperDRAM,nodeValue,std::dec);
		break;
	case dqs_per_dram_token:
		result =  toNumeric<unsigned>(DQSperDRAM,nodeValue,std::dec);
		break;
	case dm_per_dram_token:
		result =  toNumeric<unsigned>(DMperDRAM,nodeValue,std::dec);
		break;
	case request_count_token:
		result =  toNumeric<tick>(requestCount,nodeValue,std::dec);
		break;
	case idd5_token:
		result =  toNumeric<unsigned>(IDD5,nodeValue,std::dec);
		break;
	case idd4r_token:
		result = toNumeric<unsigned>(IDD4R,nodeValue,std::dec);
		break;
	case idd4w_token:
		result = toNumeric<unsigned>(IDD4W,nodeValue,std::dec);
		break;
	case idd3n_token:
		result = toNumeric<unsigned>(IDD3N,nodeValue,std::dec);
		break;
	case idd3p_token:
		result = toNumeric<unsigned>(IDD3P,nodeValue,std::dec);
		break;
	case idd2n_token:
		result = toNumeric<unsigned>(IDD2N,nodeValue,std::dec);
		break;
	case idd2p_token:
		result = toNumeric<unsigned>(IDD2P,nodeValue,std::dec);
		break;
	case average_interarrival_cycle_count:
		result = toNumeric<unsigned>(averageInterarrivalCycleCount,nodeValue,std::dec);
		break;
	case idd0_token:
		result = toNumeric<unsigned>(IDD0,nodeValue,std::dec);
		break;
	case vdd_token:
		result = toNumeric<float>(VDD,nodeValue,std::dec);
		break;
	case t_cwd_token:
		result = toNumeric<unsigned>(tCWD,nodeValue,std::dec);
		break;
	case t_buffer_delay_token:
		result = toNumeric<unsigned>(tBufferDelay,nodeValue,std::dec);
		break;
	case t_cmd_token:
		result = toNumeric<unsigned>(tCMD,nodeValue,std::dec);
		break;
	case t_wtr_token:
		result = toNumeric<unsigned>(tWTR,nodeValue,std::dec);
		break;
	case t_wr_token:
		result = toNumeric<unsigned>(tWR,nodeValue,std::dec);
		break;
	case t_refi_token:
		result = toNumeric<unsigned>(tREFI,nodeValue,std::dec);
		break;
	case t_rtrs_token:
		result = toNumeric<unsigned>(tRTRS,nodeValue,std::dec);
		break;
	case t_rtp_token:
		result = toNumeric<unsigned>(tRTP,nodeValue,std::dec);
		break;
	case t_rrd_token:
		result = toNumeric<unsigned>(tRRD,nodeValue,std::dec);
		break;
	case t_rfc_token:
		result = toNumeric<unsigned>(tRFC,nodeValue,std::dec);
		break;
	case t_rcd_token:
		result = toNumeric<unsigned>(tRCD,nodeValue,std::dec);
		break;
	case t_rc_token:
		result = toNumeric<unsigned>(tRC,nodeValue,std::dec);
		break;
	case t_rp_token:
		result = toNumeric<unsigned>(tRP,nodeValue,std::dec);
		break;
	case t_ras_token:
		result = toNumeric<unsigned>(tRAS,nodeValue,std::dec);
		break;
	case t_faw_token:
		result = toNumeric<unsigned>(tFAW,nodeValue,std::dec);
		break;
	case t_cas_token:
		result = toNumeric<unsigned>(tCAS,nodeValue,std::dec);
		break;
	case t_burst_token:
		result = toNumeric<unsigned>(tBurst,nodeValue,std::dec);
		break;
	case t_al_token:
		result = toNumeric<unsigned>(tAL,nodeValue,std::dec);
		break;
	case refresh_queue_depth_token:
		result = toNumeric<unsigned>(refreshQueueDepth,nodeValue,std::dec);
		break;
	case bank_count_token:
		result = toNumeric<unsigned>(bankCount,nodeValue,std::dec);
		break;
	case rank_count_token:
		result = toNumeric<unsigned>(rankCount,nodeValue,std::dec);
		break;
	case chan_count_token:
		result = toNumeric<unsigned>(channelCount,nodeValue,std::dec);
		break;
	case short_burst_ratio_token:
		result = toNumeric<float>(shortBurstRatio,nodeValue,std::dec);
		break;
	case read_percentage_token:
		result = toNumeric<float>(readPercentage,nodeValue,std::dec);
		break;					
	case per_bank_queue_depth_token:
		result = toNumeric<unsigned>(perBankQueueDepth,nodeValue,std::dec);
		break;
	case event_queue_depth_token:
		result = toNumeric<unsigned>(eventQueueDepth,nodeValue,std::dec);
		break;
	case transaction_queue_depth_token:
		result = toNumeric<unsigned>(transactionQueueDepth,nodeValue,std::dec);
		break;
	case completion_queue_depth_token:
		result = toNumeric<unsigned>(completionQueueDepth,nodeValue,std::dec);
		break;
	case history_queue_depth_token:
		result = toNumeric<unsigned>(historyQueueDepth,nodeValue,std::dec);
		break;
	case cacheline_size_token:
		result = toNumeric<unsigned>(cacheLineSize,nodeValue,std::dec);
		break;
	case row_size_token:
		result = toNumeric<unsigned>(rowSize,nodeValue,std::dec);
		break;
	case col_size_token:
		result = toNumeric<unsigned>(columnSize,nodeValue,std::dec);
		break;
	case channel_width_token:
		result = toNumeric<unsigned>(channelWidth,nodeValue,std::dec);
		break;
	case col_count_token:
		result = toNumeric<unsigned>(columnCount,nodeValue,std::dec);
		break;
	case row_count_token:
		result = toNumeric<unsigned>(rowCount,nodeValue,std::dec);
		break;
	case cachelines_per_row_token:
		result = toNumeric<unsigned>(cachelinesPerRow,nodeValue,std::dec);
		break;
	case posted_cas_token:
		postedCAS = (nodeValue == "true") ? true : false;
		break;
	case seniority_age_limit_token:
		result = toNumeric<unsigned>(seniorityAgeLimit,nodeValue,std::dec);
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
		readWriteGrouping = (nodeValue == "true") ? true : false;
		break;
	case refresh_time_token:
		result = toNumeric<unsigned>(refreshTime,nodeValue,std::dec);
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
		else if (nodeValue == "greedy")
			commandOrderingAlgorithm = GREEDY;
		else {
			cerr << "Unrecognized ordering algorithm: " << nodeValue << endl;
			commandOrderingAlgorithm = BANK_ROUND_ROBIN;
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
		else if (nodeValue == "sdramclosepage")
			addressMappingScheme = Address::SDRAM_CLOSE_PAGE_MAP;
		else if (nodeValue == "sdramhiperf")
			addressMappingScheme = Address::SDRAM_HIPERF_MAP;
		else if (nodeValue == "closepagehighlocality")
			addressMappingScheme = Address::CLOSE_PAGE_HIGH_LOCALITY;
		else if (nodeValue == "closepagelowlocality")
			addressMappingScheme = Address::CLOSE_PAGE_LOW_LOCALITY;
		else
			addressMappingScheme = Address::SDRAM_HIPERF_MAP;
		break;
	case auto_precharge_token:
		autoPrecharge = (nodeValue == "true") ? true : false;
		break;
	case row_buffer_management_policy_token:
		if (nodeValue == "openpage")
			rowBufferManagementPolicy = OPEN_PAGE;
		else if (nodeValue == "closepage")
			rowBufferManagementPolicy = CLOSE_PAGE;
		else if (nodeValue == "closepageaggressive" || nodeValue == "closepageoptimized")
			rowBufferManagementPolicy = CLOSE_PAGE_AGGRESSIVE;
		else
			rowBufferManagementPolicy = AUTO_PAGE;
		break;
	case clock_granularity_token:
		result = toNumeric<unsigned>(clockGranularity,nodeValue,std::dec);
		break;
	case datarate_token:
		result = toNumeric<unsigned>(dataRate,nodeValue,std::dec);
		break;
	case max_vcc_token:
		result = toNumeric<float>(maxVCC,nodeValue,std::dec);
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
		if (nodeValue == "mase")
			inFileType = InputStream::MASE_TRACE;
		else if (nodeValue == "k6")
			inFileType = InputStream::K6_TRACE;
		else if (nodeValue == "mapped")
			inFileType = InputStream::MAPPED;
		else if (nodeValue == "random")
			inFileType = InputStream::RANDOM;
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
	return result;
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
	case GREEDY:
		os << "GRD";
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