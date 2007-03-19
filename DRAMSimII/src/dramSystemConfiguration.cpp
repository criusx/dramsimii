#include <map>
#include <string>
#include <sstream>
#include "globals.h"
#include "enumTypes.h"
#include "dramSystemConfiguration.h"

using namespace std;

dramSystemConfiguration::dramSystemConfiguration(dramSettings *settings):
dram_type(settings->dramType),
row_buffer_management_policy(settings->rowBufferManagementPolicy),
auto_precharge(settings->autoPrecharge),
addr_mapping_scheme(settings->addressMappingScheme),
datarate(settings->dataRate),
refresh_time(settings->dataRate * settings->refreshTime),
read_write_grouping(settings->readWriteGrouping),
refresh_policy(settings->refreshPolicy),
seniority_age_limit(settings->seniorityAgeLimit),
posted_cas(settings->postedCAS),
clock_granularity(settings->clockGranularity),
row_count(settings->rowCount),
cachelines_per_row(settings->cachelinesPerRow),
col_count(settings->columnCount),
col_size(settings->columnSize),
row_size(settings->rowSize),
cacheline_size(settings->cacheLineSize),
// any of these really needed?
history_queue_depth(settings->historyQueueDepth),
completion_queue_depth(settings->completionQueueDepth),
transaction_queue_depth(settings->transactionQueueDepth),
event_queue_depth(settings->eventQueueDepth),
per_bank_queue_depth(settings->perBankQueueDepth),
chan_count(settings->channelCount),
rank_count(settings->rankCount),
bank_count(settings->bankCount),
refresh_queue_depth(settings->refreshQueueDepth),
//
command_ordering_algorithm(settings->commandOrderingAlgorithm),
config_type(BASELINE_CONFIG),
read_percentage(settings->readPercentage),
short_burst_ratio(settings->shortBurstRatio)
{}

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
		else if (temp->second == "sdram_hiperf_map")
			addr_mapping_scheme = SDRAM_HIPERF_MAP;
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
		refresh_time *= static_cast<unsigned>(datarate);
	}
	else
		refresh_time = static_cast<unsigned>(64000 * datarate);

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

	if ((temp=parameter.find(command_ordering_algorithm_token))!=parameter.end())
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
			command_ordering_algorithm = STRICT_ORDER;
		}
	}
	else
		command_ordering_algorithm = STRICT_ORDER;

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

#ifdef M5
dramSystemConfiguration::dramSystemConfiguration(dramSystem::Params *parameter)
{
	if (parameter->dramType == "sdram")
		dram_type = SDRAM;
	else if (parameter->dramType == "ddr" || parameter->dramType == "ddrsdram")
		dram_type = DDR;
	else if (parameter->dramType == "ddr2")
		dram_type = DDR2;
	else if (parameter->dramType == "ddr3")
		dram_type = DDR3;
	else if (parameter->dramType == "drdram")
		dram_type = DRDRAM;
	else
		dram_type = DDR2;

	noRefresh = parameter->refresh;

	if (parameter->rowBufferManagmentPolicy == "open_page")
		row_buffer_management_policy = OPEN_PAGE;
	else if (parameter->rowBufferManagmentPolicy == "close_page")
		row_buffer_management_policy = CLOSE_PAGE;
	else
		row_buffer_management_policy = AUTO_PAGE;

	auto_precharge = parameter->autoPrecharge;

	if (parameter->addrMappingScheme == "burger_base_map")
		addr_mapping_scheme = BURGER_BASE_MAP;
	else if (parameter->addrMappingScheme == "close_page_baseline")
		addr_mapping_scheme = CLOSE_PAGE_BASELINE;
	else if (parameter->addrMappingScheme == "INTEL845G_MAP")
		addr_mapping_scheme = INTEL845G_MAP;
	else if (parameter->addrMappingScheme == "sdram_base_map")
		addr_mapping_scheme = SDRAM_BASE_MAP;
	else if (parameter->addrMappingScheme == "sdram_close_page_map")
		addr_mapping_scheme = SDRAM_CLOSE_PAGE_MAP;
	else
		addr_mapping_scheme = SDRAM_HIPERF_MAP;

	datarate = parameter->datarate;

	refresh_time = parameter->refreshTime * datarate;

	read_write_grouping = parameter->readWriteGrouping;

	if (parameter->refreshPolicy == "none" || parameter->refreshPolicy == "no refresh")
		refresh_policy = NO_REFRESH;
	else if (parameter->refreshPolicy == "bank_concurrent" || parameter->refreshPolicy == "bank concurrent")
		refresh_policy = BANK_CONCURRENT;
	else
		refresh_policy = BANK_STAGGERED_HIDDEN;

	seniority_age_limit = parameter->seniorityAgeLimit;

	posted_cas = parameter->postedCas;

	clock_granularity = parameter->clockGranularity;

	cachelines_per_row = parameter->cachelinesPerRow;

	row_count = parameter->rowCount;

	col_count = parameter->colCount;

	col_size = parameter->colSize;

	row_size = parameter->rowSize;

	cacheline_size = parameter->cachelineSize;

	history_queue_depth = parameter->historyQueueDepth;

}
#endif
