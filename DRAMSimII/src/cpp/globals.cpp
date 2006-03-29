#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>

#include "dramsim2.h"

ostream &operator<<(ostream &os, const command_type_t &command)
{
	switch(command)
	{
	case	RAS_COMMAND:
		os << "RAS    ";
		break;
	case 	CAS_COMMAND:
		os << "CAS    ";
		break;
	case 	CAS_AND_PRECHARGE_COMMAND:
		os << "CAS+P  ";
		break;
	case 	CAS_WRITE_COMMAND:
		os << "CASW   ";
		break;
	case 	CAS_WRITE_AND_PRECHARGE_COMMAND:
		os << "CASW+P ";
		break;
	case 	RETIRE_COMMAND:
		os << "RETIRE ";
		break;
	case 	PRECHARGE_COMMAND:
		os << "PREC   ";
		break;
	case 	PRECHARGE_ALL_COMMAND:
		os << "PREC_A ";
		break;
	case 	RAS_ALL_COMMAND:
		os << "RAS_A  ";
		break;
	case 	DRIVE_COMMAND:
		os << "DRIVE  ";
		break;
	case 	DATA_COMMAND:
		os << "DATA   ";
		break;
	case 	CAS_WITH_DRIVE_COMMAND:
		os << "CAS+D  ";
		break;
	case 	REFRESH_ALL_COMMAND:
		os << "REF_A  ";
		break;
	}
	return os;
}
ostream &operator<<(ostream &os, const command &this_c)
//void print_command(command *this_c)
{
	os << this_c.this_command << this_c.addr;
	//print_command_type(this_c->this_command);
	//print_addresses(this_c->addr);
	return os;
}


double ascii2multiplier(const string input)
{
	switch(file_io_token(input))
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

void create_input_map(int argc,char *argv[],map<enum file_io_token_t, string> &parameters)
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
		_exit(0);
	}

	// go find the spd file first, read in all params if it was specified (and it should be)
	for (argc_index = 1; argc_index < argc; ++argc_index)
	{
		string temp = argv[argc_index];
		if (temp == "-dram:spd_input")
		{
			string temp2 = argv[argc_index + 1];
			ifstream cfg_file(temp2.c_str(),ifstream::in);
			if(!cfg_file.is_open()) {
				cerr << "Error in opening CFG file " << temp2 << endl;
				_exit(3);
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
		} else if(temp ==  "-config:bank") {
			parameters[bank_count_token] = temp2;
			argc_index += 2;
		} else if(temp ==  "-config:queue_depth") {
			parameters[per_bank_queue_depth_token] = temp2;
			argc_index += 2;
		} else if(temp ==  "-config:ordering_algorithm") {
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
		} else {
			cerr << temp <<  " is not a recognized option" << endl;
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
		switch(temp = file_io_token(input_string))
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
			cerr << "Unknown Token " << input_string << endl;
			break;
		case deprecated_ignore_token:
			cerr << input_string << "is no longer supported" << endl;
			break;
		}
	}
}

ostream &operator<<(ostream &os, const dram_timing_specification &this_a)
{
	os << "rtrs[" << this_a.t_rtrs << "] ";
	return os;
}

ostream &operator<<(ostream &os, const dram_statistics &this_a)
{
	os << "RR[" << setw(6) << setprecision(6) << (double)this_a.end_time/max(1,this_a.bo4_count + this_a.bo8_count);
	os << "] BWE[" << setw(6) << setprecision(6) << ((double)this_a.bo8_count * 8.0 + this_a.bo4_count * 4.0) * 100.0 / max(this_a.end_time,(tick_t)1);
	os << "]";
	return os;

}

int log2(int input)
{
	int count=1;
	while ((input & 0x01) == 0)
	{
		input>>=1;
		++count;
	}
	return count;
}


file_io_token_t file_io_token(const string &input)
{
	if (input.length() == 0)
		return unknown_token;
	else if(input.at(0) == '/' && input.at(1) == '/')
		return comment_token;
	else if (input == "type")
		return dram_type_token;
	else if (input == "datarate")
		return datarate_token;
	else if (input == "clock_granularity")
		return clock_granularity_token;
	else if (input == "row_buffer_policy")
		return row_buffer_management_policy_token;
	else if (input == "auto_precharge")
		return auto_precharge_token;
	else if (input == "PA_mapping_policy")
		return addr_mapping_scheme_token;
	else if (input == "addr_mapping_scheme")
		return addr_mapping_scheme_token;
	else if (input == "transaction_ordering_policy")
		return transaction_ordering_policy_token;
	else if (input == "command_ordering_algorithm")
		return command_ordering_algorithm_token;
	else if (input == "command_ordering_policy")
		return command_ordering_algorithm_token;
	else if (input == "auto_refresh_policy")
		return refresh_policy_token;
	else if (input == "refresh_policy")
		return refresh_policy_token;
	else if (input == "refresh_time")
		return refresh_time_token;
	else if (input == "posted_cas")
		return posted_cas_token;
	else if (input == "channel_count")
		return chan_count_token;
	else if (input == "channel_width")
		return channel_width_token;
	else if (input == "rank_count")
		return rank_count_token;
	else if (input == "bank_count")
		return bank_count_token;
	else if (input == "row_count")
		return row_count_token;
	else if (input == "col_count")
		return col_count_token;
	else if (input == "col_size")
		return col_size_token;
	else if (input == "row_size")
		return row_size_token;
	else if (input == "cacheline_size")
		return cacheline_size_token;
	else if (input == "per_bank_queue_depth")
		return per_bank_queue_depth_token;
	else if (input == "t_cmd")
		return t_cmd_token;
	else if (input == "t_faw")
		return t_faw_token;
	else if (input == "t_rfc")
		return t_rfc_token;
	else if (input == "t_rrd")
		return t_rrd_token;
	else if (input == "t_rtp")
		return t_rtp_token;
	else if (input == "t_wr")
		return t_wr_token;
	else if (input == "t_wtr")
		return t_wtr_token;
	else if (input == "t_ras")
		return t_ras_token;
	else if (input == "t_rcd")
		return t_rcd_token;
	else if (input == "t_cas")
		return t_cas_token;
	else if (input == "t_cac")
		return deprecated_ignore_token;
	else if (input == "t_rp")
		return t_rp_token;
	else if (input == "t_rcd")
		return t_rcd_token;
	else if (input == "t_rc")
		return t_rc_token;
	else if (input == "t_cwd")
		return t_cwd_token;
	else if (input == "t_al")
		return t_al_token;
	else if (input == "t_rl")
		return t_rl_token;
	else if (input == "t_dqs")
		return t_rtrs_token;
	else if (input == "t_rtrs")
		return t_rtrs_token;
	else if (input == "t_burst")
		return t_burst_token;
	else if (input == "read_write_grouping")
		return read_write_grouping_token;
	else if (input == "seniority_age_limit")
		return seniority_age_limit_token;
	else if (input == "cachelines_per_row")
		return cachelines_per_row_token;
	else if (input == "history_queue_depth")
		return history_queue_depth_token;
	else if (input == "completion_queue_depth")
		return completion_queue_depth_token;
	else if (input == "transaction_queue_depth")
		return transaction_queue_depth_token;
	else if (input == "refresh_queue_depth")
		return refresh_queue_depth_token;
	else if (input == "event_queue_depth")
		return event_queue_depth_token;
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


ostream &operator<<(ostream &os, const transaction *this_t)
{
	os << "[" << setw(8) << this_t->arrival_time << "] ";
	switch(this_t->type)
	{
	case IFETCH_TRANSACTION:
		os << "IFETCH ";
		break;
	case WRITE_TRANSACTION:
		os << "WRITE ";
		break;
	case READ_TRANSACTION:
		os << "READ  ";
		break;
	case PREFETCH_TRANSACTION:
		os << "PREF  ";
		break;
	case AUTO_REFRESH_TRANSACTION:
		os << "AUTOR ";
		break;
	case PER_BANK_REFRESH_TRANSACTION:
		os << "BANKR ";
		break;
	case AUTO_PRECHARGE_TRANSACTION:
		os << "AUTOP ";
		break;
	}
	os << "[" << setw(8) << setbase(10) << this_t->completion_time << "]" << endl;
	return os;
}


ostream &operator<<(ostream &os, const dram_system &this_a)
{
	os << "SYS[";
	switch(this_a.system_config.config_type)
	{
	case BASELINE_CONFIG:
		os << "BASE] ";
		break;
	default:
		os << "UNKN] ";
		break;
	}
	os << "RC[" << this_a.system_config.rank_count << "] ";
	os << "BC[" << this_a.system_config.bank_count << "] ";
	os << "ALG[";
	switch(this_a.system_config.command_ordering_algorithm)
	{
	case STRICT_ORDER:
		os << "STRO] ";
		break;
	case RANK_ROUND_ROBIN:
		os << "RRR ] ";
		break;
	case BANK_ROUND_ROBIN:
		os << "BRR ] ";
		break;
	case WANG_RANK_HOP:
		os << "WANG] ";
		break;
	case GREEDY:
		os << "GRDY] ";
		break;
	default:
		os << "UNKN] ";
		break;
	}
	os << "BQD[" << this_a.system_config.per_bank_queue_depth << "] ";
	os << "BLR[" << setprecision(0) << floor(100*(this_a.system_config.short_burst_ratio+0.0001)+.5) << "] ";
	os << "RP[" << (int)(100*this_a.system_config.read_percentage) << "] ";

	os << this_a.timing_specification;
	os << this_a.statistics;

	return os;
}

