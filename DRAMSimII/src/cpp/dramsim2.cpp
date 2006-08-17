#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <sstream>

#include "globals.h"
#include "dramSystemConfiguration.h"
#include "dramTimingSpecification.h"
#include "dramAlgorithm.h"
#include "simulationParameters.h"
#include "dramStatistics.h"
#include "busevent.h"
#include "event.h"

using namespace std;

///
/// Contains all of the configuration information.
/// Constructor for the dram_system_configuration class
dramSystemConfiguration::dramSystemConfiguration(map<file_io_token_t,string> &parameter)
{
	/// set standards for typical memory config
	map<file_io_token_t, string>::iterator temp;

	if ((temp=parameter.find(dram_type_token))!=parameter.end())
	{
		if (temp->second == "sdram")
			dram_type = SDRAM;
		else if (temp->second == "ddr" || temp->second == "ddrsdram")
			dram_type = DDR;
		else if (temp->second == "ddr2")
			dram_type = DDR2;
		else if (temp->second == "ddr3")
			dram_type = DDR3;
		else if (temp->second == "drdram")
			dram_type = DRDRAM;
		else
			dram_type = DDR2;
	}
	else
		dram_type = DDR2;

	if ((temp=parameter.find(no_refresh_token)) != parameter.end())
		noRefresh = true;
	else
		noRefresh = false;

	if ((temp=parameter.find(row_buffer_management_policy_token))!=parameter.end())
	{
		if (temp->second == "open_page")
			row_buffer_management_policy = OPEN_PAGE;
		else if (temp->second == "close_page")
			row_buffer_management_policy = CLOSE_PAGE;
		else
			row_buffer_management_policy = AUTO_PAGE;
	}
	else
		row_buffer_management_policy = CLOSE_PAGE;

	if ((temp=parameter.find(auto_precharge_token))!=parameter.end())
	{
		if (temp->second == "true" || temp->second == "TRUE")
			auto_precharge = true;
		else
			auto_precharge = false;
	}
	else
		auto_precharge = true;

	if ((temp=parameter.find(addr_mapping_scheme_token))!=parameter.end())
	{
		if (temp->second == "burger_base_map")
			addr_mapping_scheme = BURGER_BASE_MAP;
		else if (temp->second == "close_page_baseline")
			addr_mapping_scheme = CLOSE_PAGE_BASELINE;
		else if (temp->second == "INTEL845G_MAP")
			addr_mapping_scheme = INTEL845G_MAP;
		else if (temp->second == "sdram_base_map")
			addr_mapping_scheme = SDRAM_BASE_MAP;
		else if (temp->second == "sdram_close_page_map")
			addr_mapping_scheme = SDRAM_CLOSE_PAGE_MAP;
		else
			addr_mapping_scheme = SDRAM_HIPERF_MAP;
	}
	else
		addr_mapping_scheme = CLOSE_PAGE_BASELINE;

	if ((temp=parameter.find(datarate_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> datarate;
	}
	else
		datarate = 1000; 		/* 1 Gbit per sec */

	if ((temp=parameter.find(refresh_time_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> refresh_time;
		refresh_time *= datarate;
	}
	else
		refresh_time = 64000 * datarate;

	if ((temp=parameter.find(read_write_grouping_token))!=parameter.end())
	{
		if (temp->second == "TRUE" || temp->second == "true")
			read_write_grouping = true;
		else
			read_write_grouping = false;
	}
	else
		read_write_grouping = true;

	if ((temp=parameter.find(refresh_policy_token))!=parameter.end())
	{
		if (temp->second == "none" || temp->second == "no refresh")
			refresh_policy = NO_REFRESH;
		else if (temp->second == "bank_concurrent" || temp->second == "bank concurrent")
			refresh_policy = BANK_CONCURRENT;
		else
			refresh_policy = BANK_STAGGERED_HIDDEN;
	}
	else
		refresh_policy = NO_REFRESH;

	if ((temp=parameter.find(seniority_age_limit_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> seniority_age_limit;
	}
	else
		seniority_age_limit = 200;// prevent against starvation

	if ((temp=parameter.find(posted_cas_token))!=parameter.end())
	{
		if (temp->second == "TRUE" || temp->second == "true")
			posted_cas = true;
		else
			posted_cas = false;
	}
	else
		posted_cas = false;

	if ((temp=parameter.find(clock_granularity_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> clock_granularity;
	}
	else
		clock_granularity = 2;

	if ((temp=parameter.find(row_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> row_count;
	}
	else
		row_count = 16384;

	if ((temp=parameter.find(cachelines_per_row_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> cachelines_per_row;
	}
	else
		cachelines_per_row = INVALID;/* not used now, set to INVALID */

	if ((temp=parameter.find(col_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> col_count;
	}
	else
		col_count = 512;

	if ((temp=parameter.find(col_size_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> col_size;/* width of data bus */
	}
	else
		col_size = 8;

	if ((temp=parameter.find(row_size_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> row_size;
	}
	else
		row_size = INVALID;

	if ((temp=parameter.find(cacheline_size_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> cacheline_size;
	}
	else
		cacheline_size = 64;

	if ((temp=parameter.find(history_queue_depth_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> history_queue_depth;
	}
	else
		history_queue_depth = 8;

	if ((temp=parameter.find(completion_queue_depth_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> completion_queue_depth;
	}
	else
		completion_queue_depth = 8;

	if ((temp=parameter.find(transaction_queue_depth_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> transaction_queue_depth;
	}
	else
		transaction_queue_depth = 32;

	if ((temp=parameter.find(event_queue_depth_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> event_queue_depth;
	}
	else
		event_queue_depth = 32;

	if ((temp=parameter.find(per_bank_queue_depth_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> per_bank_queue_depth;
	}
	else
		per_bank_queue_depth = 8;

	if ((temp=parameter.find(ordering_algorithm_token))!=parameter.end())
	{
		if (temp->second == "strict_order" || temp->second == "strict")
			command_ordering_algorithm = STRICT_ORDER;
		else if (temp->second == "bank_round_robin")
			command_ordering_algorithm = BANK_ROUND_ROBIN;
		else if (temp->second == "rank_round_robin")
			command_ordering_algorithm = RANK_ROUND_ROBIN;
		else if (temp->second == "wang_hop")
			command_ordering_algorithm = WANG_RANK_HOP;
		else if (temp->second == "greedy")
			command_ordering_algorithm = GREEDY;
		else {
			cerr << "Unrecognized ordering algorithm: " << temp->second << endl;
			command_ordering_algorithm = BANK_ROUND_ROBIN;
		}
	}
	else
		command_ordering_algorithm = BANK_ROUND_ROBIN;

	if ((temp=parameter.find(system_configuration_type_token))!=parameter.end())
		config_type = BASELINE_CONFIG;
	else
		config_type = BASELINE_CONFIG;

	if ((temp=parameter.find(read_percentage_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> read_percentage;
	}
	else
		read_percentage = 0.5;

	if ((temp=parameter.find(short_burst_ratio_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> short_burst_ratio;
	}
	else
		short_burst_ratio = 0.0;

	if ((temp=parameter.find(chan_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> chan_count;
	}
	else
		chan_count = 1;

	if ((temp=parameter.find(rank_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> rank_count;
	}
	else
		rank_count = 2;

	if ((temp=parameter.find(bank_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> bank_count;
	}
	else
		bank_count = 16;

	if ((temp=parameter.find(refresh_queue_depth_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> refresh_queue_depth;
	}
	else
		refresh_queue_depth = row_count * rank_count;
}



dramTimingSpecification::dramTimingSpecification(map<file_io_token_t,string> &parameter)
{
	map<file_io_token_t,string>::iterator temp;
	if ((temp=parameter.find(dram_type_token))==parameter.end())
		_exit(9);

	if(temp->second == "sdram"){						/* @ 100 MHz */
		t_al = 0;		/* no such thing as posted CAS in SDRAM */
		t_burst = 8;		/* depending on system config! can be 1, 2, 4, or 8 */
		t_cas = 2;
		t_cmd = 1;		/* protocol specific, cannot be changed */
		t_cwd = 0;		/* no such thing in SDRAM */
		t_int_burst = 1;		/* prefetch length is 1 */
		t_faw = 0;		/* no such thing in SDRAM */
		t_ras = 5;		/* */
		t_rc = 7;		
		t_rcd = 2;
		t_rfc = 7;		/* same as t_rc */
		t_rp = 2;		/* 12 ns @ 1.25ns per cycle = 9.6 cycles */
		t_rrd = 0;		/* no such thing in SDRAM */
		t_rtp = 1;
		t_rtrs = 0;		/* no such thing in SDRAM */
		t_wr = 2;		

	} 
	// @ 200 MHz (400 Mbps)
	else if (temp->second == "ddr" || temp->second == "ddrsdram")					
	{
		t_al = 0;		// no such thing in DDR 
		t_burst = 8;	// depending on system config! can be 2, 4, or 8
		t_cas = 6;
		t_cmd = 2;		// protocol specific, cannot be changed
		t_cwd = 2;		// protocol specific, cannot be changed
		t_int_burst = 2;// protocol specific, cannot be changed
		t_faw = 0;		// no such thing in DDR
		t_ras = 16;		// 40 ns @ 2.5 ns per beat == 16 beats
		t_rc = 22;		// 55 ns t_rc
		t_rcd = 6;
		t_rfc = 28;		// 70 ns @ 2.5 ns per beat == 28 beats
		t_rp = 6;		// 15 ns @ 2.5ns per beat = 6 beats
		t_rrd = 0;
		t_rtp = 2;
		t_rtrs = 2;
		t_wr = 6;		// 15 ns @ 2.5 ns per beat = 6 beats
		t_wtr = 4;

	}
	else if (temp->second == "ddr2")					/* @ 800 Mbps */
	{
		if ((temp=parameter.find(t_al_token))==parameter.end())
			t_al = 0;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_al;
		}
		if ((temp=parameter.find(t_burst_token))==parameter.end())
			t_burst = 8;/* depending on system config! can be 4, or 8 */
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_burst;
		}
		if ((temp=parameter.find(t_cas_token))==parameter.end())
			t_cas = 10;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_cas;
		}

		t_cmd = 2;		/* protocol specific, cannot be changed */
		t_cwd = t_cas - 2;	/* protocol specific, cannot be changed */
		t_int_burst = 4;		/* protocol specific, cannot be changed */

		if ((temp=parameter.find(t_faw_token))==parameter.end())
			t_faw = 30;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_faw;
		}
		if ((temp=parameter.find(t_ras_token))==parameter.end())
			t_ras = 36;/* 45 ns @ 1.25ns per beat = 36 beats */
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_ras;
		}
		if ((temp=parameter.find(t_rc_token))==parameter.end())
			t_rc = 46;/* 57 ns @ 1.25ns per beat = 45.6 beats */
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_rc;
		}
		if ((temp=parameter.find(t_rcd_token))==parameter.end())
			t_rcd = 10;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_rcd;
		}
		if ((temp=parameter.find(t_rfc_token))==parameter.end())
			t_rfc = 102;/* 128 ns @ 1.25ns per beat ~= 102 beats */
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_rfc;
		}
		if ((temp=parameter.find(t_rp_token))==parameter.end())
			t_rp = 10;/* 12 ns @ 1.25ns per beat = 9.6 beats */
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_rp;
		}
		if ((temp=parameter.find(t_rrd_token))==parameter.end())
			t_rrd = 6;/* 7.5 ns */
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_rrd;
		}
		if ((temp=parameter.find(t_rtp_token))==parameter.end())
			t_rtp = 6;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_rtp;
		}
		if ((temp=parameter.find(t_rtrs_token))==parameter.end())
			t_rtrs = 2;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_rtrs;
		}
		if ((temp=parameter.find(t_wr_token))==parameter.end())
			t_wr = 12;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_wr;
		}
		if ((temp=parameter.find(t_wtr_token))==parameter.end())
			t_wtr = 6;
		else
		{
			stringstream temp2(temp->second);
			temp2 >> t_wtr;
		}
	}
	else if (temp->second == "ddr3")					/* @ 1.33 Gbps = 0.75 ns per beat */
	{
		t_al = 0;
		t_burst = 8;		/* protocol specific, cannot be changed */
		t_cas = 10;
		t_cmd = 2;		/* protocol specific, cannot be changed */
		t_cwd = t_cas - 2;
		t_int_burst = 8;		/* protocol specific, cannot be changed */
		t_faw = 30;
		t_ras = 36;		/* 27 ns @ 0.75ns per beat = 36 beats */
		t_rc = 48;		/* 36 ns @ 0.75ns per beat = 48 beats */
		t_rcd = 12;
		t_rfc = 280;
		t_rp = 12;		/* 9 ns @ 0.75ns per beat = 12 beats */
		t_rrd = 8;
		t_rtrs = 2;
		t_rtp = 8;
		t_wr = 12;
		t_wtr = 8;

	}
	else if (temp->second == "drdram")					/* FIXME */
	{
		;/* not currently supported */
	}

	if ((temp=parameter.find(t_rtrs_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> t_rtrs;
	}

}





dramAlgorithm::dramAlgorithm(queue<command> &free_command_pool,
							 int rank_count,
							 int bank_count,
							 int config_type)
{
	rank_id[0] = 0;
	rank_id[1] = 3;
	ras_bank_id[0] = 0;
	ras_bank_id[1] = 0;
	ras_bank_id[2] = 0;
	ras_bank_id[3] = 0;
	cas_count[0] = 0;
	cas_count[1] = 0;
	cas_count[2] = 0;
	cas_count[3] = 0;
	WHCC_offset[0] = 0;
	WHCC_offset[1] = 0;
	transaction_type[0] = READ_TRANSACTION;
	transaction_type[1] = READ_TRANSACTION;
	transaction_type[2] = READ_TRANSACTION;
	transaction_type[3] = READ_TRANSACTION;


	WHCC.init(rank_count * bank_count * 2);

	command *temp_c;

	if((config_type == BASELINE_CONFIG) && (rank_count == 2))
	{
		if(bank_count == 8)
		{
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		}
		else if (bank_count == 16)
		{
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
		}
	}
	else if((config_type == BASELINE_CONFIG) && (rank_count == 4))
	{
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
	}
}

dramAlgorithm::dramAlgorithm(const dramAlgorithm &a)
{
	rank_id[0] = a.rank_id[0];
	rank_id[1] = a.rank_id[1];
	ras_bank_id[0] = a.ras_bank_id[0];
	ras_bank_id[1] = a.ras_bank_id[1];
	ras_bank_id[2] = a.ras_bank_id[2];
	ras_bank_id[3] = a.ras_bank_id[3];
	cas_count[0] = a.cas_count[0];
	cas_count[1] = a.cas_count[1];
	cas_count[2] = a.cas_count[2];
	cas_count[3] = a.cas_count[3];
	WHCC_offset[0] = a.WHCC_offset[0];
	WHCC_offset[1] = a.WHCC_offset[1];
	transaction_type[0] = a.transaction_type[0];
	transaction_type[1] = a.transaction_type[1];
	transaction_type[2] = a.transaction_type[2];
	transaction_type[3] = a.transaction_type[3];
}




void dramAlgorithm::init(queue<command> &free_command_pool,
						  int rank_count,
						  int bank_count,
						  int config_type)
{
	WHCC.init(rank_count * bank_count * 2);

	command *temp_c;

	if((config_type == BASELINE_CONFIG) && (rank_count == 2))
	{
		if(bank_count == 8)
		{
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		}
		else if (bank_count == 16)
		{
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  8; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  9; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 10; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 11; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 12; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 13; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 14; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
			temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id = 15; WHCC.enqueue(temp_c);
		}
	}
	else if((config_type == BASELINE_CONFIG) && (rank_count == 4))
	{
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  2; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  0; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  4; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 1; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  5; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 2; temp_c->addr.bank_id =  3; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  6; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = RAS_COMMAND; temp_c->addr.rank_id = 3; temp_c->addr.bank_id =  1; WHCC.enqueue(temp_c);
		temp_c = free_command_pool.acquire_item(); temp_c->this_command = CAS_COMMAND; temp_c->addr.rank_id = 0; temp_c->addr.bank_id =  7; WHCC.enqueue(temp_c);
	}
}



simulationParameters::simulationParameters(map<file_io_token_t,string> &parameter)
{
	request_count = 0;
	input_type = RANDOM;
	map<file_io_token_t,string>::iterator temp;
	if ((temp=parameter.find(request_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> request_count;
	}
}

dramStatistics::dramStatistics()
{
	//fout.o    = std::cout;
	start_no = 0;
	end_no   = 0;
	start_time = 0;
	end_time = 0;
	valid_transaction_count = 0;
	bo8_count = 0;
	bo4_count = 0;
}

void dramStatistics::collect_transaction_stats(transaction *this_t)
{
	if(this_t->length == 8)
	{
		++bo8_count;
	}
	else
	{
		++bo4_count;
	}
}


/* convert transation to commands, and insert into bank queues in proper sequence if there is room */
/* need to be cleaned up to handle refresh requests */
/* convert transactions to commands */



transaction::transaction()
{
	arrival_time = 0;
	completion_time = 0;
	status = 0;
	event_no = 0;

	type = CONTROL_TRANSACTION;
}

busEvent::busEvent()
{
	timestamp = 0;
	attributes = CONTROL_TRANSACTION;
}

addresses::addresses()
{
	virt_addr = phys_addr = chan_id = rank_id = bank_id = row_id = col_id = 0;
}

command::command()
{
	this_command = RETIRE_COMMAND;                        /* which command is this? */
	start_time = 0;                     /* when did the transaction start? */
	enqueue_time = 0;			/* when did it make it into the queues? */
	completion_time = 0;
	host_t = NULL;	/* backward pointer to the original transaction */
	/** Added list of completion times in order to clean up code */
	link_comm_tran_comp_time = 0;
	amb_proc_comp_time = 0;
	dimm_comm_tran_comp_time = 0;
	dram_proc_comp_time = 0;
	dimm_data_tran_comp_time = 0;
	amb_down_proc_comp_time = 0;
	link_data_tran_comp_time = 0;

	/* Variables added for the FB-DIMM */
	bundle_id = 0;              /* Bundle into which command is being sent - Do we need this ?? */
	tran_id = 0;                /*      The transaction id number */
	data_word = 0;              /* Which portion of data is returned i.e. entire cacheline or fragment thereof which portions are being sent*/
	data_word_position = 0;     /** Which part of the data transmission are we doing : postions include FIRST , MIDDLE, LAST **/
	refresh = 0;                /** This is used to determine if the ras/prec are part of refresh **/
	posted_cas = false;             /** This is used to determine if the ras + cas were in the same bundle **/
	length = 0;
}

command::command(const command &rhs)
{
	this_command = rhs.this_command;
	start_time = rhs.start_time;
	enqueue_time = rhs.enqueue_time;
	completion_time = rhs.completion_time;
	addr = rhs.addr;
	host_t = rhs.host_t;		
	
	link_comm_tran_comp_time = rhs.link_comm_tran_comp_time;
	amb_proc_comp_time = rhs.amb_down_proc_comp_time;
	dimm_comm_tran_comp_time = rhs.dimm_comm_tran_comp_time;
	dram_proc_comp_time = rhs.dram_proc_comp_time;
	dimm_data_tran_comp_time = rhs.dimm_data_tran_comp_time;
	amb_down_proc_comp_time = rhs.amb_down_proc_comp_time;
	link_data_tran_comp_time = rhs.link_data_tran_comp_time;

	bundle_id = rhs.bundle_id;
	tran_id = rhs.tran_id;
	data_word = rhs.data_word;
	data_word_position = rhs.data_word_position;
	refresh = rhs.refresh;
	posted_cas = rhs.posted_cas;
	length = rhs.length;
}

event::event()
{
	event_type = 0;
	time = 0;
	event_ptr = NULL;
}
