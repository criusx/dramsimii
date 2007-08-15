#include <map>
#include <string>
#include <sstream>
#include "dramSystemConfiguration.h"

using namespace std;
using namespace DRAMSimII;

dramSystemConfiguration::dramSystemConfiguration(const dramSettings *settings):
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
{cerr << col_size << endl;}


dramSystemConfiguration::dramSystemConfiguration(const dramSystemConfiguration *rhs):
dram_type(rhs->dram_type),
row_buffer_management_policy(rhs->row_buffer_management_policy),
auto_precharge(rhs->auto_precharge),
addr_mapping_scheme(rhs->addr_mapping_scheme),
datarate(rhs->datarate),
refresh_time(rhs->refresh_time),
read_write_grouping(rhs->read_write_grouping),
refresh_policy(rhs->refresh_policy),
seniority_age_limit(rhs->seniority_age_limit),
posted_cas(rhs->posted_cas),
clock_granularity(rhs->clock_granularity),
row_count(rhs->row_count),
cachelines_per_row(rhs->cachelines_per_row),
col_count(rhs->col_count),
col_size(rhs->col_size),
row_size(rhs->row_size),
cacheline_size(rhs->cacheline_size),
// any of these really needed?
history_queue_depth(rhs->history_queue_depth),
completion_queue_depth(rhs->completion_queue_depth),
transaction_queue_depth(rhs->transaction_queue_depth),
event_queue_depth(rhs->event_queue_depth),
per_bank_queue_depth(rhs->per_bank_queue_depth),
chan_count(rhs->chan_count),
rank_count(rhs->rank_count),
bank_count(rhs->bank_count),
refresh_queue_depth(rhs->refresh_queue_depth),
//
command_ordering_algorithm(rhs->command_ordering_algorithm),
config_type(rhs->config_type),
read_percentage(rhs->read_percentage),
short_burst_ratio(rhs->short_burst_ratio)
{}



ostream &DRAMSimII::operator<<(ostream &os, const dramSystemConfiguration &this_a)
{
	os << "PerBankQ[" << this_a.per_bank_queue_depth << "] ";
	os << "CH[" << this_a.chan_count << "] ";
	os << "RK[" << this_a.rank_count << "] ";
	os << "BK[" << this_a.bank_count << "] ";

	return os;
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
