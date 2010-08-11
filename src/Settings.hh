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

#ifndef DRAMSETTINGS
#define DRAMSETTINGS

#include "InputStream.hh"
#include "Address.hh"
#include "enumTypes.hh"

#include <string>
#include <iostream>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <map>
#include <cctype>

namespace DRAMsimII
{
	/// @brief stores the settings to be used to initialize a dramSystem object
	class Settings
	{
	private:
		static std::map<std::string, FileIOToken> tokenizeMap;
		static std::map<FileIOToken, std::string> lookupMap;
	public:
		//////////////////////////////////////////////////////////////////////////	
		// command data
		unsigned epoch;
		RefreshPolicy refreshPolicy;
		DRAMType dramType;
		unsigned dataRate; // frequency
		CommandOrderingAlgorithm commandOrderingAlgorithm;
		TransactionOrderingAlgorithm transactionOrderingAlgorithm;
		unsigned perBankQueueDepth;
		unsigned columnSize;
		unsigned rowSize;
		unsigned channelWidth;
		unsigned columnCount;
		unsigned rowCount;
		unsigned historyQueueDepth;
		unsigned completionQueueDepth;
		unsigned transactionQueueDepth;
		unsigned eventQueueDepth;
		unsigned refreshQueueDepth;
		unsigned refreshTime;
		unsigned seniorityAgeLimit;
		unsigned decodeWindow;
		RowBufferPolicy rowBufferManagementPolicy;
		Address::AddressMappingScheme addressMappingScheme;
		bool postedCAS;
		bool readWriteGrouping;
		bool autoPrecharge;
		unsigned clockGranularity;
		unsigned channelCount;
		unsigned dimmCount;
		unsigned rankCount;
		unsigned bankCount;
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
		unsigned IDD1;
		unsigned IDD2P;
		unsigned IDD2N;
		unsigned IDD3P;
		unsigned IDD3N;
		unsigned IDD4W;
		unsigned IDD4R;
		unsigned IDD5;
	
		// converts a string to a file_io_token
		static FileIOToken dramTokenizer(const std::string & value)
		{
			std::string lowerValue;
			
			for (std::string::const_iterator i = value.begin(), end = value.end(); i < end; i++)
			{
				lowerValue += tolower(*i);
			}

			FileIOToken first = unknown_token;
			
			if (!tokenize(lowerValue,first))
				std::cerr << "warn: unknown token: " << std::endl;

			return first;
		}

		bool setKeyValue(const std::string &nodeName, const std::string &nodeValue);
		bool setKeyValue(const char* nodeName, const std::string &nodeValue) { std::string name(nodeName); return setKeyValue(name, nodeValue); }

		static std::map<FileIOToken, std::string> lookupSetup()
		{
			std::map<FileIOToken, std::string> theMap;
					
			theMap[clock_granularity_token] = "clockGranularity";
			theMap[channel_count_token] = "channels";
			theMap[dimm_count_token] = "dimms";
			theMap[channel_width_token] = "channelWidth";
			theMap[addr_mapping_scheme_token] = "physicalAddressMappingPolicy";
			theMap[row_buffer_management_policy_token] = "rowBufferPolicy";
			theMap[rank_count_token] = "ranks";
			theMap[bank_count_token] = "banks";
			theMap[row_count_token] = "rows";
			theMap[row_size_token] = "rowSize";
			theMap[col_count_token] = "columns";
			theMap[col_size_token] = "columnSize";
			theMap[t_buffer_delay_token] = "tBufferDelay";
			theMap[t_burst_token] = "tBurst";
			theMap[t_cas_token] = "tCAS";
			theMap[t_cmd_token] = "tCMD";
			theMap[t_cwd_token] = "tCWD";
			theMap[t_faw_token] = "tFAW";
			theMap[t_ras_token] = "tRAS";
			theMap[t_rc_token] = "tRC";
			theMap[t_rcd_token] = "tRCD";
			theMap[riff_token] = "RIFF";
			theMap[t_rfc_token] = "tRFC";
			theMap[t_rrd_token] = "tRRD";
			theMap[t_rp_token] = "tRP";
			theMap[t_rtp_token] = "tRTP";
			theMap[t_rtrs_token] = "tRTRS";
			theMap[t_wr_token] = "tWR";
			theMap[t_wtr_token] = "tWTR";
			theMap[posted_cas_token] = "postedCAS";
			theMap[average_interarrival_cycle_count] = "averageInterarrivalCycleCount";
			theMap[t_al_token] = "tAL";
			theMap[refresh_policy_token] = "autoRefreshPolicy";
			theMap[refresh_time_token] = "refreshTime";
			theMap[t_refi_token] = "tREFI";
			theMap[command_ordering_algorithm_token] = "commandOrderingAlgorithm";
			theMap[transaction_ordering_policy_token] = "transactionOrderingAlgorithm";
			theMap[per_bank_queue_depth_token] = "perBankQueueDepth";
			theMap[system_configuration_type_token] = "systemConfigurationType";
			theMap[history_queue_depth_token] = "historyQueueDepth";
			theMap[completion_queue_depth_token] = "completionQueueDepth";
			theMap[transaction_queue_depth_token] = "transactionQueueDepth";
			theMap[event_queue_depth_token] = "eventQueueDepth";
			theMap[refresh_queue_depth_token] = "refreshQueueDepth";
			theMap[seniority_age_limit_token] = "seniorityAgeLimit";
			theMap[read_write_grouping_token] = "readWriteGrouping";
			theMap[auto_precharge_token] = "autoPrecharge";
			theMap[p_dq_rd_token] = "PdqRD";
			theMap[p_dq_wr_token] = "PdqWR";
			theMap[p_dq_rd_oth_token] = "PdqRDoth";
			theMap[p_dq_wr_oth_token] = "PdqWRoth";
			theMap[dq_per_dram_token] = "DQperDRAM";
			theMap[dqs_per_dram_token] = "DQSperDRAM";
			theMap[dm_per_dram_token] = "DMperDRAM";
			theMap[frequency_spec_token] = "frequencySpec";
			theMap[max_vcc_token] = "maxVCC";
			theMap[vdd_token] = "systemVDD";
			theMap[idd0_token] = "IDD0";
			theMap[idd1_token] = "IDD1";
			theMap[idd2p_token] = "IDD2P";
			theMap[idd2n_token] = "IDD2N";
			theMap[idd3p_token] = "IDD3P";
			theMap[idd3n_token] = "IDD3N";
			theMap[idd4r_token] = "IDD4R";
			theMap[idd4w_token] = "IDD4W";
			theMap[idd5_token] = "IDD5";
			theMap[short_burst_ratio_token] = "shortBurstRatio";
			theMap[read_percentage_token] = "readPercentage";
			theMap[output_file_token] = "outFile";
			theMap[output_file_dir_token] = "outFileDirectory";
			theMap[input_file_token] = "inputFile";
			theMap[request_count_token] = "requestCount";
			theMap[cpu_to_memory_clock_ratio] = "cpuToMemoryClockRatio";
			theMap[epoch_token] = "epoch";
			theMap[output_file_type_token] = "outFile/@type";
			theMap[dbreporting_token] = "outFile/@dbreporting";
			theMap[dram_type_token] = "dramspec/@type";
			theMap[input_type_token] = "inputFile/@type";
			theMap[random_distribution_token] = "inputFile";
			theMap[datarate_token] = "datarate";
			theMap[decode_window_token] = "decodeWindow";


			return theMap;
		}

		static std::map<std::string, FileIOToken> createTokenizer()
		{
			std::map<std::string, FileIOToken> theMap;

			// should all be lower case
			theMap["type"]=dram_type_token;
			theMap["dbreporting"]=dbreporting_token;
			theMap["datarate"]=datarate_token;
			theMap["dramspec"]=dram_type_token;
			theMap["dramtype"]=dram_type_token;
			theMap["tbufferdelay"]=t_buffer_delay_token;
			theMap["inputtype"]=input_type_token;
			theMap["infileformat"] = input_type_token;
			theMap["infiletype"]=input_type_token;
			theMap["inputfiletype"]=input_type_token;
			theMap["epoch"]=epoch_token;
			theMap["clock_granularity"] = clock_granularity_token;
			theMap["clockgranularity"] = clock_granularity_token;
			theMap["row_buffer_policy"] = row_buffer_management_policy_token;
			theMap["rowbufferpolicy"] = row_buffer_management_policy_token;
			theMap["auto_precharge"] = auto_precharge_token;
			theMap["autoprecharge"] = auto_precharge_token;
			theMap["pa_mapping_policy"] = addr_mapping_scheme_token;
			theMap["physicaladdressmappingpolicy"] = addr_mapping_scheme_token;
			theMap["addr_mapping_scheme"] = addr_mapping_scheme_token;
			theMap["addressmappingscheme"] = addr_mapping_scheme_token;
			theMap["addressmappingpolicy"] = addr_mapping_scheme_token;
			theMap["physicaladdressmappingpolicy"] = addr_mapping_scheme_token;
			theMap["transaction_ordering_policy"] = transaction_ordering_policy_token;
			theMap["transactionorderingpolicy"] = transaction_ordering_policy_token;
			theMap["command_ordering_algorithm"] = command_ordering_algorithm_token;
			theMap["commandorderingalgorithm"] = command_ordering_algorithm_token;
			theMap["transactionorderingalgorithm"] = transaction_ordering_policy_token;
			theMap["readpercentage"] = read_percentage_token;
			theMap["shortburstratio"] = short_burst_ratio_token;
			theMap["command_ordering_policy"] = command_ordering_algorithm_token;
			theMap["commandorderingpolicy"] = command_ordering_algorithm_token;
			theMap["requestcount"] = request_count_token;
			theMap["inputfile"] = input_file_token;
			theMap["infile"] = input_file_token;
			theMap["averageinterarrivalcyclecount"] = average_interarrival_cycle_count;
			theMap["interarrivalcyclecount"] = average_interarrival_cycle_count;
			theMap["outfile"] = output_file_token;
			theMap["outfiledirectory"] = output_file_dir_token;
			theMap["outfiledir"] = output_file_dir_token;
			theMap["outputdir"] = output_file_dir_token;
			theMap["outfiletype"] = output_file_type_token;
			theMap["outputfiletype"] = output_file_type_token;
			theMap["outfileformat"] = output_file_type_token;
			theMap["outputfileformat"] = output_file_type_token;
			theMap["random"] = random_distribution_token;
			theMap["auto_refresh_policy"] = refresh_policy_token;
			theMap["autorefreshpolicy"] = refresh_policy_token;
			theMap["cputomemoryclockratio"] = cpu_to_memory_clock_ratio;
			theMap["refresh_policy"] = refresh_policy_token;
			theMap["refreshpolicy"] = refresh_policy_token;
			theMap["refresh_time"] = refresh_time_token;
			theMap["refreshtime"] = refresh_time_token;
			theMap["riff"] = riff_token;
			theMap["posted_cas"] = posted_cas_token;
			theMap["postedcas"] = posted_cas_token;
			theMap["channel_count"] = channel_count_token;
			theMap["channelcount"] = channel_count_token;
			theMap["channels"] = channel_count_token;
			theMap["dimms"] = dimm_count_token;
			theMap["channel_width" ] = channel_width_token;
			theMap["channelwidth"] = channel_width_token;
			theMap["rank_count"] = rank_count_token;
			theMap["rankcount"] = rank_count_token;
			theMap["ranks"] = rank_count_token;
			theMap["bank_count"] = bank_count_token;
			theMap["bankcount"] = bank_count_token;
			theMap["banks"] = bank_count_token;
			theMap["row_count" ] = row_count_token;
			theMap["rowcount" ] = row_count_token;
			theMap["rows"] = row_count_token;
			theMap["col_count" ] = col_count_token;
			theMap["columncount"] = col_count_token;
			theMap["columns"] = col_count_token;
			theMap["col_size" ] = col_size_token;
			theMap["columnsize"] = col_size_token;
			theMap["row_size" ] = row_size_token;
			theMap["rowsize"] = row_size_token;
			theMap["per_bank_queue_depth" ] = per_bank_queue_depth_token;
			theMap["perbankqueuedepth"] = per_bank_queue_depth_token;
			theMap["t_cmd" ] = t_cmd_token;
			theMap["tcmd"] = t_cmd_token;
			theMap["t_faw"] = t_faw_token;
			theMap["tfaw"] = t_faw_token;
			theMap["t_rfc" ] = t_rfc_token;
			theMap["trfc"] = t_rfc_token;
			theMap["t_rrd" ] = t_rrd_token;
			theMap["trrd"] = t_rrd_token;
			theMap["t_rtp" ] = t_rtp_token;
			theMap["trtp"] = t_rtp_token;
			theMap["t_wr" ] = t_wr_token;
			theMap["twr"] = t_wr_token;
			theMap["t_wtr" ] = t_wtr_token;
			theMap["twtr"] = t_wtr_token;
			theMap["t_ras" ] = t_ras_token;
			theMap["tras"] = t_ras_token;
			theMap["t_rcd" ] = t_rcd_token;
			theMap["trcd"] = t_rcd_token;
			theMap["t_cas" ] = t_cas_token;
			theMap["tcas"] = t_cas_token;
			theMap["t_cac"] = deprecated_ignore_token;
			theMap["tcac"] = deprecated_ignore_token;
			theMap["t_rp" ] = t_rp_token;
			theMap["trp"] = t_rp_token;
			theMap["t_rcd" ] = t_rcd_token;
			theMap["trcd"] = t_rcd_token;
			theMap["t_rc"] = t_rc_token;
			theMap["trc"] = t_rc_token;
			theMap["t_cwd" ] = t_cwd_token;
			theMap["tcwd"] = t_cwd_token;
			theMap["t_al" ] = t_al_token;
			theMap["tal"] = t_al_token;
			theMap["t_rl" ] = t_rl_token;
			theMap["trl"] = t_rl_token;
			theMap["t_dqs"] = t_dqs_token;
			theMap["tdqs"] = t_dqs_token;
			theMap["t_rtrs"] = t_rtrs_token;
			theMap["trtrs"] = t_rtrs_token;
			theMap["t_burst" ] = t_burst_token;
			theMap["tburst"] = t_burst_token;
			theMap["trefi"] = t_refi_token;
			theMap["read_write_grouping" ] = read_write_grouping_token;
			theMap["readwritegrouping"] = read_write_grouping_token;
			theMap["seniority_age_limit" ] = seniority_age_limit_token;
			theMap["seniorityagelimit"] = seniority_age_limit_token;
			theMap["history_queue_depth" ] = history_queue_depth_token;
			theMap["historyqueuedepth"] = history_queue_depth_token;
			theMap["decodewindow"] = decode_window_token;
			theMap["completion_queue_depth" ] = completion_queue_depth_token;
			theMap["completionqueuedepth"] = completion_queue_depth_token;
			theMap["transaction_queue_depth"] = transaction_queue_depth_token;
			theMap["transactionqueuedepth"] = transaction_queue_depth_token;
			theMap["refresh_queue_depth" ] = refresh_queue_depth_token;
			theMap["refreshqueuedepth"] = refresh_queue_depth_token;
			theMap["event_queue_depth"] = event_queue_depth_token;
			theMap["eventqueuedepth"] = event_queue_depth_token;
			theMap["systemconfigurationtype"] = system_configuration_type_token;
			theMap["idd0"] = idd0_token;
			theMap["idd1"] = idd1_token;
			theMap["idd2p"] = idd2p_token;
			theMap["idd2n"] = idd2n_token;
			theMap["idd3p"] = idd3p_token;
			theMap["idd3n"] = idd3n_token;
			theMap["idd4r"] = idd4r_token;
			theMap["idd4w"] = idd4w_token;
			theMap["idd5" ] = idd5_token;
			theMap["idd5a"] = idd5_token;
			theMap["maxvcc"] = max_vcc_token;
			theMap["systemvdd"] = vdd_token;
			theMap["frequencyspec"] = frequency_spec_token;
			theMap["pdqrd"] = p_dq_rd_token;
			theMap["pdqwr"] = p_dq_wr_token;
			theMap["pdqrdoth"] = p_dq_rd_oth_token;
			theMap["pdqwroth"] = p_dq_wr_oth_token;
			theMap["dqperdram"] = dq_per_dram_token;
			theMap["dqsperdram"] = dqs_per_dram_token;
			theMap["dmperdram"] = dm_per_dram_token;
			theMap["fetch"] = FETCH;
			theMap["ifetch"] = FETCH;
			theMap["p_fetch"] = FETCH;
			theMap["p_lock_rd"] = LOCK_RD;
			theMap["p_lock_wr"] = LOCK_WR;
			theMap["lock_rd"] = LOCK_RD;
			theMap["r"] = MEM_RD;
			theMap["w"] = MEM_WR;
			theMap["lock_wr"] = LOCK_WR;
			theMap["mem_rd"] = MEM_RD;
			theMap["write"] = MEM_WR;
			theMap["mem_wr"] = MEM_WR;
			theMap["read"] = MEM_RD;
			theMap["p_mem_rd"] = MEM_RD;
			theMap["p_mem_wr"] = MEM_WR;
			theMap["p_i/o_rd"] = IO_RD;
			theMap["p_i/o_wr"] = IO_WR;
			theMap["io_rd"] = IO_RD;
			theMap["i/o_rd"] = IO_RD;
			theMap["io_wr"] = IO_WR;
			theMap["i/o_wr"] = IO_WR;
			theMap["p_int_ack"] = INT_ACK;
			theMap["int_ack"] = INT_ACK;
			theMap["boff"] = BOFF;
			theMap["ps"] = PICOSECOND;
			theMap["ns"] = NANOSECOND;
			theMap["us"] = MICROSECOND;
			theMap["ms"] = MILLISECOND;
			theMap["s"] = SECOND;

			return theMap;
		}


		static FileIOToken fileIOToken(const unsigned char *input)
		{
			const std::string inputS((const char *)input);
			return dramTokenizer(inputS);
		}

		static bool tokenize(const std::string &nodeName, FileIOToken &token)
		{
			if (nodeName.length() == 0)
			{
				token = unknown_token;
				return true;
			}
			else if (nodeName.length() > 1 && nodeName.at(0) == '/' && nodeName.at(1) == '/')
			{
				token = comment_token;
				return true;
			}				
			else
			{
				std::string lowerNodeName;
				
				for (std::string::const_iterator i = nodeName.begin(), end = nodeName.end(); i < end; i++)
				{
					lowerNodeName += tolower(*i);
				}

				std::map<std::string, FileIOToken>::iterator result = tokenizeMap.find(lowerNodeName);

				if (result != tokenizeMap.end())
				{
					token = result->second;
					return true;
				}
				else
					std::cerr << lowerNodeName << " failed" << std::endl;
					return false;
			}
		}

		static bool detokenize(const FileIOToken token, std::string &value)
		{
			std::map<FileIOToken,std::string>::iterator result = lookupMap.find(token);

			if (result != lookupMap.end())
			{
				value = result->second;
				return true;
			}
			else
				return false;
		}
	
		// create a dramSettings from command line arguments
		explicit Settings();
		
	};
}
#endif
