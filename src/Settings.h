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
#include <functional>
#include <algorithm>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

namespace DRAMsimII
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
		bool dbReporting;
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
#ifdef USEXML
		xmlDocPtr doc;
#endif // USEXML


		bool setKeyValue(const std::string nodeName, const std::string nodeValue);

		// converts a string to a file_io_token
		static FileIOToken dramTokenizer(const std::string &value)
		{
			std::string input = value;
			std::transform(input.begin(),input.end(),input.begin(),std::ptr_fun((int (*)( int ))std::tolower));

			if (input.length() == 0)
				return unknown_token;
			else if(input.at(0) == '/' && input.at(1) == '/')
				return comment_token;
			else if (input == "type")
				return dram_type_token;
			else if (input == "dbreporting")
				return dbreporting_token;
			else if (input == "datarate")
				return datarate_token;
			else if (input == "dramspec")
				return dram_type_token;
			else if (input == "tbufferdelay")
				return t_buffer_delay_token;
			else if (input == "inputtype")
				return input_type_token;
			else if (input == "epoch")
				return epoch_token;
			else if (input == "clock_granularity" || input == "clockgranularity")
				return clock_granularity_token;
			else if (input == "row_buffer_policy" || input == "rowbufferpolicy")
				return row_buffer_management_policy_token;
			else if (input == "auto_precharge" || input == "autoprecharge")
				return auto_precharge_token;
			else if (input == "pa_mapping_policy" || input == "physicaladdressmappingpolicy")
				return addr_mapping_scheme_token;
			else if (input == "addr_mapping_scheme" || input == "addressmappingscheme")
				return addr_mapping_scheme_token;
			else if (input == "transaction_ordering_policy" || input == "transactionorderingpolicy")
				return transaction_ordering_policy_token;
			else if (input == "command_ordering_algorithm" || input == "commandorderingalgorithm")
				return command_ordering_algorithm_token;
			else if (input == "transactionorderingalgorithm")
				return transaction_ordering_policy_token;
			else if (input == "readpercentage")
				return read_percentage_token;
			else if (input == "shortburstratio")
				return short_burst_ratio_token;
			else if (input == "command_ordering_policy" || input == "commandorderingpolicy")
				return command_ordering_algorithm_token;
			else if (input == "requestcount")
				return request_count_token;
			else if (input == "inputfile")
				return input_file_token;
			else if (input == "outfile")
				return output_file_token;
			else if (input == "outfiledirectory")
				return output_file_dir_token;
			else if (input == "outfiletype")
				return output_file_type_token;
			else if (input == "random")
				return random_distribution_token;
			else if (input == "auto_refresh_policy" || input == "autorefreshpolicy")
				return refresh_policy_token;
			else if (input == "cputomemoryclockratio")
				return cpu_to_memory_clock_ratio;
			else if (input == "refresh_policy" || input == "refreshpolicy")
				return refresh_policy_token;
			else if (input == "refresh_time" || input == "refreshtime")
				return refresh_time_token;
			else if (input == "posted_cas" || input == "postedcas")
				return posted_cas_token;
			else if (input == "channel_count" || input == "channelcount" || input == "channels")
				return chan_count_token;
			else if (input == "channel_width" || input == "channelwidth")
				return channel_width_token;
			else if (input == "rank_count" || input == "rankcount" || input == "ranks")
				return rank_count_token;
			else if (input == "bank_count" || input == "bankcount" || input == "banks")
				return bank_count_token;
			else if (input == "row_count" || input == "rowcount" || input == "rows")
				return row_count_token;
			else if (input == "col_count" || input == "columncount" || input == "columns")
				return col_count_token;
			else if (input == "col_size" || input == "columnsize")
				return col_size_token;
			else if (input == "row_size" || input == "rowsize")
				return row_size_token;
			else if (input == "cacheline_size" || input == "cachelinesize")
				return cacheline_size_token;
			else if (input == "per_bank_queue_depth" || input == "perbankqueuedepth")
				return per_bank_queue_depth_token;
			else if (input == "t_cmd" || input == "tcmd")
				return t_cmd_token;
			else if (input == "t_faw" || input == "tfaw")
				return t_faw_token;
			else if (input == "t_rfc" || input == "trfc")
				return t_rfc_token;
			else if (input == "t_rrd" || input == "trrd")
				return t_rrd_token;
			else if (input == "t_rtp" || input == "trtp")
				return t_rtp_token;
			else if (input == "t_wr" || input == "twr")
				return t_wr_token;
			else if (input == "t_wtr" || input == "twtr")
				return t_wtr_token;
			else if (input == "t_ras" || input == "tras")
				return t_ras_token;
			else if (input == "t_rcd" || input == "trcd")
				return t_rcd_token;
			else if (input == "t_cas" || input == "tcas")
				return t_cas_token;
			else if (input == "t_cac" || input == "tcac")
				return deprecated_ignore_token;
			else if (input == "t_rp" || input == "trp")
				return t_rp_token;
			else if (input == "t_rcd" || input == "trcd")
				return t_rcd_token;
			else if (input == "t_rc" || input == "trc")
				return t_rc_token;
			else if (input == "t_cwd" || input == "tcwd")
				return t_cwd_token;
			else if (input == "t_al" || input == "tal")
				return t_al_token;
			else if (input == "t_rl" || input == "trl")
				return t_rl_token;
			else if (input == "t_dqs" || input == "tdqs")
				return t_rtrs_token;
			else if (input == "t_rtrs" || input == "trtrs")
				return t_rtrs_token;
			else if (input == "t_burst" || input == "tburst")
				return t_burst_token;
			else if (input == "trefi")
				return t_refi_token;
			else if (input == "read_write_grouping" || input == "readwritegrouping")
				return read_write_grouping_token;
			else if (input == "seniority_age_limit" || input == "seniorityagelimit")
				return seniority_age_limit_token;
			else if (input == "cachelines_per_row" || input == "cachelinesperrow")
				return cachelines_per_row_token;
			else if (input == "history_queue_depth" || input == "historyqueuedepth")
				return history_queue_depth_token;
			else if (input == "completion_queue_depth" || input == "completionqueuedepth")
				return completion_queue_depth_token;
			else if (input == "transaction_queue_depth" || input == "transactionqueuedepth")
				return transaction_queue_depth_token;
			else if (input == "refresh_queue_depth" || input == "refreshqueuedepth")
				return refresh_queue_depth_token;
			else if (input == "event_queue_depth" || input == "eventqueuedepth")
				return event_queue_depth_token;
			else if (input == "systemconfigurationtype")
				return system_configuration_type_token;
			else if (input == "idd0")
				return idd0_token;
			else if (input == "idd2p")
				return idd2p_token;
			else if (input == "idd2n")
				return idd2n_token;
			else if (input == "idd3p")
				return idd3p_token;
			else if (input == "idd3n")
				return idd3n_token;
			else if (input == "idd4r")
				return idd4r_token;
			else if (input == "idd4w")
				return idd4w_token;
			else if (input == "idd5" || input == "idd5a")
				return idd5_token;
			else if (input == "maxvcc")
				return max_vcc_token;
			else if (input == "systemvdd")
				return vdd_token;
			else if (input == "frequencyspec")
				return frequency_spec_token;
			else if (input == "pdqrd")
				return p_dq_rd_token;
			else if (input == "pdqwr")
				return p_dq_wr_token;
			else if (input == "pdqrdoth")
				return p_dq_rd_oth_token;
			else if (input == "pdqwroth")
				return p_dq_wr_oth_token;
			else if (input == "dqperdram")
				return dq_per_dram_token;
			else if (input == "dqsperdram")
				return dqs_per_dram_token;
			else if (input == "dmperdram")
				return dm_per_dram_token;
			else if(input == "fetch")
				return FETCH;
			else if (input == "ifetch")
				return FETCH;
			else if (input == "p_fetch")
				return FETCH;
			else if (input == "p_lock_rd")
				return LOCK_RD;
			else if (input == "p_lock_wr")
				return LOCK_WR;
			else if (input == "lock_rd")
				return LOCK_RD;
			else if (input == "lock_wr")
				return LOCK_WR;
			else if (input == "mem_rd")
				return MEM_RD;
			else if (input == "write")
				return MEM_WR;
			else if (input == "mem_wr")
				return MEM_WR;
			else if (input == "read")
				return MEM_RD;
			else if (input == "p_mem_rd")
				return MEM_RD;
			else if (input == "p_mem_wr")
				return MEM_WR;
			else if (input == "p_i/o_rd")
				return IO_RD;
			else if (input == "p_i/o_wr")
				return IO_WR;
			else if (input == "io_rd")
				return IO_RD;
			else if (input == "i/o_rd")
				return IO_RD;
			else if (input == "io_wr")
				return IO_WR;
			else if (input == "i/o_wr")
				return IO_WR;
			else if (input == "p_int_ack")
				return INT_ACK;
			else if (input == "int_ack")
				return INT_ACK;
			else if (input == "boff")
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
			{
				std::cerr << "warn: Unknown value" << input << std::endl;
				return unknown_token;
			}
		}


		static FileIOToken fileIOToken(const unsigned char *input)
		{
			const std::string inputS((const char *)input);
			return dramTokenizer(inputS);
		}

		~Settings()
		{
#ifdef USEXML
			//xmlFreeDoc(doc);
#endif // USEXML
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
