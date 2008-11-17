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

#ifndef DRAMSETTINGS
#define DRAMSETTINGS

#include "globals.h"

#include <string>
#include <iostream>
#include <iomanip>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

namespace DRAMSimII
{
	/// @brief stores the settings to be used to initialize a dramSystem object
	class Settings
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		// output file
		std::string settingsOutputFile;
		//////////////////////////////////////////////////////////////////////////	
		// command data
		unsigned epoch;
		std::string inFile;
		std::string sessionID;
		DistributionType arrivalDistributionModel;
		InputType inFileType;
		std::string outFile;
		std::string outFileDir;
		std::string commandLine;
		OutputFileType outFileType;
		unsigned requestCount;
		RefreshPolicy refreshPolicy;
		DRAMType dramType;
		unsigned dataRate; // frequency
		CommandOrderingAlgorithm commandOrderingAlgorithm;
		TransactionOrderingAlgorithm transactionOrderingAlgorithm;
		SystemConfigurationType systemType;
		unsigned perBankQueueDepth;
		unsigned columnSize;
		unsigned rowSize;
		unsigned channelWidth;
		unsigned columnCount;
		unsigned rowCount;
		unsigned cacheLineSize;
		unsigned historyQueueDepth;
		unsigned completionQueueDepth;
		unsigned transactionQueueDepth;
		unsigned eventQueueDepth;
		unsigned refreshQueueDepth;
		unsigned refreshTime;
		unsigned seniorityAgeLimit;
		RowBufferPolicy rowBufferManagementPolicy;
		AddressMappingScheme addressMappingScheme;
		bool postedCAS;
		bool readWriteGrouping;
		bool autoPrecharge;
		unsigned clockGranularity;
		unsigned cachelinesPerRow;
		unsigned channelCount;
		unsigned rankCount;
		unsigned bankCount;
		float shortBurstRatio;
		float readPercentage;
		//////////////////////////////////////////////////////////////////////////	
		// timing data
		unsigned tRTRS;
		unsigned tAL;
		unsigned tBurst;
		unsigned tCAS;
		unsigned tCWD;
		unsigned tFAW;
		unsigned tRAS;
		unsigned tRC;
		unsigned tRCD;
		unsigned tREFI;
		unsigned tRFC;
		unsigned tRP;
		unsigned tRRD;
		unsigned tRTP;
		unsigned tWR;
		unsigned tWTR;
		unsigned tCMD;
		unsigned tInternalBurst;
		unsigned tBufferDelay;
		float cpuToMemoryClockRatio;
		//////////////////////////////////////////////////////////////////////////
		// power data
		float PdqRD;
		float PdqWR;
		float PdqRDoth;
		float PdqWRoth;
		unsigned DQperDRAM;
		unsigned DQSperDRAM;
		unsigned DMperDRAM;
		unsigned frequencySpec;
		float maxVCC;
		float VDD;
		unsigned IDD0;
		unsigned IDD2P;
		unsigned IDD2N;
		unsigned IDD3P;
		unsigned IDD3N;
		unsigned IDD4W;
		unsigned IDD4R;
		unsigned IDD5;

		void setKeyValue(const std::string nodeName, const std::string nodeValue);

		// converts a string to a file_io_token
		static FileIOToken dramTokenizer(const std::string &input)
		{
			if (input.length() == 0)
				return unknown_token;
			else if(input.at(0) == '/' && input.at(1) == '/')
				return comment_token;
			else if (input == "type")
				return dram_type_token;
			else if (input == "datarate")
				return datarate_token;
			else if (input == "dramspec")
				return dram_type_token;
			else if (input == "tBufferDelay")
				return t_buffer_delay_token;
			else if (input == "inputType")
				return input_type_token;
			else if (input == "epoch")
				return epoch_token;
			else if (input == "clock_granularity" || input == "clockGranularity")
				return clock_granularity_token;
			else if (input == "row_buffer_policy" || input == "rowBufferPolicy")
				return row_buffer_management_policy_token;
			else if (input == "auto_precharge" || input == "autoPrecharge")
				return auto_precharge_token;
			else if (input == "PA_mapping_policy" || input == "physicalAddressMappingPolicy")
				return addr_mapping_scheme_token;
			else if (input == "addr_mapping_scheme" || input == "addressMappingScheme")
				return addr_mapping_scheme_token;
			else if (input == "transaction_ordering_policy" || input == "transactionOrderingPolicy")
				return transaction_ordering_policy_token;
			else if (input == "command_ordering_algorithm" || input == "commandOrderingAlgorithm")
				return command_ordering_algorithm_token;
			else if (input == "transactionOrderingAlgorithm")
				return transaction_ordering_policy_token;
			else if (input == "readPercentage")
				return read_percentage_token;
			else if (input == "shortBurstRatio")
				return short_burst_ratio_token;
			else if (input == "command_ordering_policy" || input == "commandOrderingPolicy")
				return command_ordering_algorithm_token;
			else if (input == "requestCount")
				return request_count_token;
			else if (input == "inputFile")
				return input_file_token;
			else if (input == "outFile")
				return output_file_token;
			else if (input == "outFileDirectory")
				return output_file_dir_token;
			else if (input == "outFileType")
				return output_file_type_token;
			else if (input == "random")
				return random_distribution_token;
			else if (input == "auto_refresh_policy" || input == "autoRefreshPolicy")
				return refresh_policy_token;
			else if (input == "cpuToMemoryClockRatio")
				return cpu_to_memory_clock_ratio;
			else if (input == "refresh_policy" || input == "refreshPolicy")
				return refresh_policy_token;
			else if (input == "refresh_time" || input == "refreshTime")
				return refresh_time_token;
			else if (input == "posted_cas" || input == "postedCAS")
				return posted_cas_token;
			else if (input == "channel_count" || input == "channelCount" || input == "channels")
				return chan_count_token;
			else if (input == "channel_width" || input == "channelWidth")
				return channel_width_token;
			else if (input == "rank_count" || input == "rankCount" || input == "ranks")
				return rank_count_token;
			else if (input == "bank_count" || input == "bankCount" || input == "banks")
				return bank_count_token;
			else if (input == "row_count" || input == "rowCount" || input == "rows")
				return row_count_token;
			else if (input == "col_count" || input == "columnCount" || input == "columns")
				return col_count_token;
			else if (input == "col_size" || input == "columnSize")
				return col_size_token;
			else if (input == "row_size" || input == "rowSize")
				return row_size_token;
			else if (input == "cacheline_size" || input == "cacheLineSize")
				return cacheline_size_token;
			else if (input == "per_bank_queue_depth" || input == "perBankQueueDepth")
				return per_bank_queue_depth_token;
			else if (input == "t_cmd" || input == "tCMD")
				return t_cmd_token;
			else if (input == "t_faw" || input == "tFAW")
				return t_faw_token;
			else if (input == "t_rfc" || input == "tRFC")
				return t_rfc_token;
			else if (input == "t_rrd" || input == "tRRD")
				return t_rrd_token;
			else if (input == "t_rtp" || input == "tRTP")
				return t_rtp_token;
			else if (input == "t_wr" || input == "tWR")
				return t_wr_token;
			else if (input == "t_wtr" || input == "tWTR")
				return t_wtr_token;
			else if (input == "t_ras" || input == "tRAS")
				return t_ras_token;
			else if (input == "t_rcd" || input == "tRCD")
				return t_rcd_token;
			else if (input == "t_cas" || input == "tCAS")
				return t_cas_token;
			else if (input == "t_cac" || input == "tCAC")
				return deprecated_ignore_token;
			else if (input == "t_rp" || input == "tRP")
				return t_rp_token;
			else if (input == "t_rcd" || input == "tRCD")
				return t_rcd_token;
			else if (input == "t_rc" || input == "tRC")
				return t_rc_token;
			else if (input == "t_cwd" || input == "tCWD")
				return t_cwd_token;
			else if (input == "t_al" || input == "tAL")
				return t_al_token;
			else if (input == "t_rl" || input == "tRL")
				return t_rl_token;
			else if (input == "t_dqs" || input == "tDQS")
				return t_rtrs_token;
			else if (input == "t_rtrs" || input == "tRTRS")
				return t_rtrs_token;
			else if (input == "t_burst" || input == "tBurst")
				return t_burst_token;
			else if (input == "tREFI")
				return t_refi_token;
			else if (input == "read_write_grouping" || input == "readWriteGrouping")
				return read_write_grouping_token;
			else if (input == "seniority_age_limit" || input == "seniorityAgeLimit")
				return seniority_age_limit_token;
			else if (input == "cachelines_per_row" || input == "cachelinesPerRow")
				return cachelines_per_row_token;
			else if (input == "history_queue_depth" || input == "historyQueueDepth")
				return history_queue_depth_token;
			else if (input == "completion_queue_depth" || input == "completionQueueDepth")
				return completion_queue_depth_token;
			else if (input == "transaction_queue_depth" || input == "transactionQueueDepth")
				return transaction_queue_depth_token;
			else if (input == "refresh_queue_depth" || input == "refreshQueueDepth")
				return refresh_queue_depth_token;
			else if (input == "event_queue_depth" || input == "eventQueueDepth")
				return event_queue_depth_token;
			else if (input == "systemConfigurationType")
				return system_configuration_type_token;
			else if (input == "IDD0")
				return idd0_token;
			else if (input == "IDD2P")
				return idd2p_token;
			else if (input == "IDD2N")
				return idd2n_token;
			else if (input == "IDD3P")
				return idd3p_token;
			else if (input == "IDD3N")
				return idd3n_token;
			else if (input == "IDD4R")
				return idd4r_token;
			else if (input == "IDD4W")
				return idd4w_token;
			else if (input == "IDD5" || input == "IDD5A")
				return idd5_token;
			else if (input == "maxVCC")
				return max_vcc_token;
			else if (input == "systemVDD")
				return vdd_token;
			else if (input == "frequencySpec")
				return frequency_spec_token;
			else if (input == "PdqRD")
				return p_dq_rd_token;
			else if (input == "PdqWR")
				return p_dq_wr_token;
			else if (input == "PdqRDoth")
				return p_dq_rd_oth_token;
			else if (input == "PdqWRoth")
				return p_dq_wr_oth_token;
			else if (input == "DQperDRAM")
				return dq_per_dram_token;
			else if (input == "DQSperDRAM")
				return dqs_per_dram_token;
			else if (input == "DMperDRAM")
				return dm_per_dram_token;
			else if(input == "FETCH")
				return FETCH;
			else if (input == "IFETCH")
				return FETCH;
			else if (input == "P_FETCH")
				return FETCH;
			else if (input == "P_LOCK_RD")
				return LOCK_RD;
			else if (input == "P_LOCK_WR")
				return LOCK_WR;
			else if (input == "LOCK_RD")
				return LOCK_RD;
			else if (input == "LOCK_WR")
				return LOCK_WR;
			else if (input == "MEM_RD")
				return MEM_RD;
			else if (input == "WRITE")
				return MEM_WR;
			else if (input == "MEM_WR")
				return MEM_WR;
			else if (input == "READ")
				return MEM_RD;
			else if (input == "P_MEM_RD")
				return MEM_RD;
			else if (input == "P_MEM_WR")
				return MEM_WR;
			else if (input == "P_I/O_RD")
				return IO_RD;
			else if (input == "P_I/O_WR")
				return IO_WR;
			else if (input == "IO_RD")
				return IO_RD;
			else if (input == "I/O_RD")
				return IO_RD;
			else if (input == "IO_WR")
				return IO_WR;
			else if (input == "I/O_WR")
				return IO_WR;
			else if (input == "P_INT_ACK")
				return INT_ACK;
			else if (input == "INT_ACK")
				return INT_ACK;
			else if (input == "BOFF")
				return BOFF;
			else if (input == "ps")
				return PICOSECOND;
			else if (input == "ns")
				return NANOSECOND;
			else if (input == "us")
				return MICROSECOND;
			else if (input == "ms")
				return MILLISECOND;
			else if (input == "s")
				return SECOND;
			else 
				return unknown_token;
		}

		
		static FileIOToken fileIOToken(const unsigned char *input)
		{
			const std::string inputS((const char *)input);
			return dramTokenizer(inputS);
		}


		// create a dramSettings from command line arguments
		explicit Settings(const int, const char **);
		explicit Settings();

		// serialization
		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar,const unsigned int version)
		{
			ar & settingsOutputFile & epoch & inFile & sessionID & arrivalDistributionModel & inFileType & outFile & outFileType & outFileDir & requestCount &
				refreshPolicy & dramType & dataRate & commandOrderingAlgorithm & transactionOrderingAlgorithm & systemType & perBankQueueDepth &
				columnSize & rowSize & channelWidth & columnCount & rowCount & cacheLineSize & historyQueueDepth & completionQueueDepth &
				transactionQueueDepth & eventQueueDepth & refreshQueueDepth & refreshTime & seniorityAgeLimit & rowBufferManagementPolicy &
				addressMappingScheme & postedCAS & readWriteGrouping & autoPrecharge & clockGranularity & cachelinesPerRow & channelCount & 
				rankCount & bankCount & shortBurstRatio & readPercentage & tRTRS & tAL & tBurst & tCAS & tCWD & tFAW & tRAS & tRC & tRCD & tREFI &
				tRFC & tRP & tRRD & tRTP & tWR & tCMD & tInternalBurst & tBufferDelay & cpuToMemoryClockRatio & PdqRD & PdqWR & PdqRDoth &
				PdqWRoth & DQperDRAM & DQSperDRAM & DMperDRAM & frequencySpec & maxVCC & VDD & IDD0 & IDD2P & IDD2N & IDD3P & IDD3N & IDD4W &
				IDD4R & IDD5;
		}
	};
}
#endif
