#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>

#include "dramSystem.h"
#include "rank_c.h"
#include "globals.h"
#include "dramTimingSpecification.h"
#include "dramStatistics.h"
#include "gzstream/gzstream.h"


using namespace std;

// modified, writes to cerr or a compressed output file
boost::iostreams::filtering_ostream outStream;

double ascii2multiplier(const string &input)
{
	switch(dramTokenizer(input))
	{
	case PICOSECOND:
		return 0.001;
		break;
	case NANOSECOND:
		return 1.0;
		break;
	case MICROSECOND:
		return 1000.0;
		break;
	case MILLISECOND:
		return 1000000.0;
		break;
	case SECOND:
		return 1000000000.0;
		break;
	default:
		cerr << "unknown multipler " << input << endl;
		return 0.0;
		break;
	}
}

void create_input_map(int argc,const char **argv,map<enum file_io_token_t, string> &parameters)
{
	int argc_index=1;
	string trace_filename;
	string temp = argv[min(argc_index,argc-1)];


	if ((argc < 2) || (temp == "-help"))
	{
		cout << "Usage: " << argv[0] << " -options optionswitch" << endl;
		cout << "-input_type [k6|mase|random]" << endl;
		cout << "-trace_file TRACE_FILENAME" << endl;
		cout << "-dram:spd_input SPD_FILENAME" << endl;
		cout << "-output_file OUTPUT_FILENAME" << endl;
		cout << "-debug" << endl;
		exit(0);
	}

	// go find the spd file first, read in all params if it was specified (and it should be)
	for (argc_index = 1; argc_index < argc; ++argc_index)
	{
		string argValue = argv[argc_index];
		if (argValue == "-dram:spd_input")
		{
			ifstream cfg_file(argv[argc_index + 1],ifstream::in);
			if(!cfg_file.is_open()) {
				cerr << "Error in opening CFG file " << argv[argc_index + 1] << endl;
				exit(3);
			}
			else
			{
				create_input_map_from_input_file(parameters, cfg_file);
				break;
			}
		}
	}

	// then go lookup other parameters and override the spd file inputs, this is why reading the spd file first is necessary
	argc_index = 1;
	while (argc_index < argc)
	{
		string temp = argv[argc_index];
		string temp2;
		if (argc_index < argc - 1)
			temp2 = argv[argc_index+1];

		if (temp == "-input_type")
		{
			parameters[input_type_token]=temp2;
			argc_index += 2;
		} else if (temp == "-trace_file") {
			parameters[trace_file_token] = temp2;
			argc_index += 2;
		} else if (temp == "-dram:spd_input") {
			// already been read, bypass it this time
			argc_index += 2;
		} else if(temp == "-config:channel") {
			parameters[chan_count_token] = temp2;
			argc_index += 2;
		} else if(temp == "-config:rank") {
			parameters[rank_count_token] = temp2;
			argc_index += 2;
		} else if(temp == "-config:bank") {
			parameters[bank_count_token] = temp2;
			argc_index += 2;
		} else if(temp == "-config:queue_depth") {
			parameters[per_bank_queue_depth_token] = temp2;
			argc_index += 2;
		} else if(temp == "-config:ordering_algorithm") {
			parameters[ordering_algorithm_token] = temp2;
			argc_index += 2;
		} else if(temp == "-config:system_config") { 
			parameters[system_configuration_type_token] = temp2;
			argc_index += 2;
		} else if(temp == "-config:read_percentage") {
			parameters[read_percentage_token] = temp2;
			argc_index += 2;
		} else if(temp == "-config:short_burst_ratio") {
			parameters[short_burst_ratio_token] = temp2;
			argc_index += 2;
		} else if(temp == "-debug") {
			parameters[debug_token] = "true";
			cerr << "debug is now a compile-time option" << endl;
			argc_index ++;
		} else if(temp == "-request_count") {
			parameters[request_count_token] = temp2;
			argc_index += 2;
		} else if(temp == "-output_file") {
			parameters[output_file_token] = temp2;
			argc_index += 2;
		} else if(temp == "-timing:rtrs") {
			parameters[t_rtrs_token] = temp2;
			argc_index += 2;
		} else if (temp == "--settings") {
			argc_index += 2;
		} else if (temp == "-no_refresh") {
			parameters[refresh_policy_token] = "none";
			argc_index += 1;
		} else {
			cerr << temp << " is not a recognized option" << endl;
			argc_index ++;
		}
	}
}

void create_input_map_from_input_file(map<enum file_io_token_t,string> &parameters,ifstream &cfg_file)
{

	while (!cfg_file.eof())
	{ 
		string input_string;
		cfg_file >> input_string;
		file_io_token_t temp;
		switch(temp = dramTokenizer(input_string))
		{
		case dram_type_token:
		case clock_granularity_token:
		case datarate_token: /* aka memory_frequency: units is MBPS */ 
		case row_buffer_management_policy_token:
		case auto_precharge_token: 
		case addr_mapping_scheme_token:
		case transaction_ordering_policy_token: 
		case refresh_time_token: 
		case refresh_policy_token: 
		case posted_cas_token: 
		case chan_count_token: 
		case channel_width_token: 
		case rank_count_token:
		case bank_count_token:
		case row_count_token:
		case col_count_token: 
		case col_size_token: 
		case row_size_token: 
		case cacheline_size_token:
		case per_bank_queue_depth_token:
		case t_al_token:
		case t_burst_token:
		case t_cas_token:
		case t_cmd_token:
		case t_cwd_token:
		case t_faw_token:
		case t_ras_token:
		case t_rc_token:
		case t_rcd_token:
		case t_rfc_token:
		case t_rp_token:
		case t_rrd_token:
		case t_rtp_token:
		case t_rtrs_token:
		case t_wr_token:
		case t_wtr_token:
			cfg_file >> input_string;
			parameters[temp] = input_string;
			break;
		case comment_token:
			cfg_file.ignore(65536,'\n');
			break;
		default:
		case unknown_token:
			cerr << "Unknown Token \"" << input_string << "\"" << endl;
			break;
		case deprecated_ignore_token:
			cerr << input_string << "is no longer supported" << endl;
			break;
		}
	}
}

ostream &operator<<(ostream &os, const dramStatistics &this_a)
{
	os << "RR[" << setw(6) << setprecision(6) << (double)this_a.end_time/max(1,this_a.bo4_count + this_a.bo8_count);
	os << "] BWE[" << setw(6) << setprecision(6) << ((double)this_a.bo8_count * 8.0 + this_a.bo4_count * 4.0) * 100.0 / max(this_a.end_time,(tick_t)1);
	os << "]";
	return os;

}

file_io_token_t fileIOToken(const unsigned char *input)
{
	const string inputS((const char *)input);
	return dramTokenizer(inputS);
}

file_io_token_t dramTokenizer(const string &input)
{
	if (input.length() == 0)
		return unknown_token;
	else if(input.at(0) == '/' && input.at(1) == '/')
		return comment_token;
	else if (input == "type")
		return dram_type_token;
	else if (input == "datarate")
		return datarate_token;
	else if (input == "tBufferDelay")
		return t_buffer_delay_token;
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
	else if (input == "auto_refresh_policy" || input == "autoRefreshPolicy")
		return refresh_policy_token;
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
		cerr << "Unknown input: " << input << endl;
	return unknown_token;
}

//std::ostream &operator<<(outputStream& os, const char *str)
//{
//	if (os.good())
//		os << str;
//		//os.write(str, strlen(str));
//	else
//		std::cerr << str;
//	return os;
//}

//std::ostream &operator<<(outputStream& os, std::ostream


ostream &operator<<(ostream &os, const addresses &this_a)
{
	os << "addr[0x" << setbase(16) << this_a.phys_addr <<
		"] chan[" << setbase(16) << this_a.chan_id << "] rank[" <<
		this_a.rank_id << "] bank[" << setw(2) << setbase(16) << this_a.bank_id <<
		"] row[" << setw(4) << setbase(16) << this_a.row_id << "] col[" <<
		setbase(16) << this_a.col_id << "]";
	return os;
}

ostream &operator<<(ostream &os, const address_mapping_scheme_t &this_ams)
{
	switch (this_ams)
	{
	case BURGER_BASE_MAP:
		os << "Burger Baseline Map";
		break;
	case CLOSE_PAGE_BASELINE:
		os << "Close Page Baseline Map";
		break;
	case INTEL845G_MAP:
		os << "Intel 845G Map";
		break;
	case OPEN_PAGE_BASELINE:
		os << "Open Page Baseline Map";
		break;
	case SDRAM_BASE_MAP:
		os << "SDRAM Baseline Map";
		break;
	case SDRAM_CLOSE_PAGE_MAP:
		os << "SDRAM Close Page Map";
		break;
	case SDRAM_HIPERF_MAP:
		os << "SDRAM High Performance Map";
		break;
	}
	return os;
}
