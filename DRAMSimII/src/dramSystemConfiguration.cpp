#include <map>
#include <string>
#include <sstream>
#include "dramSystemConfiguration.h"

using namespace std;
using namespace DRAMSimII;

dramSystemConfiguration::dramSystemConfiguration(const dramSettings *settings):
command_ordering_algorithm(settings->commandOrderingAlgorithm),
transactionOrderingAlgorithm(settings->transactionOrderingAlgorithm),
config_type(BASELINE_CONFIG), // FIXME: add FBD
refresh_time(settings->dataRate * settings->refreshTime),
refresh_policy(settings->refreshPolicy),
col_size(settings->columnSize),
row_size(settings->rowSize),
cacheline_size(settings->cacheLineSize),
seniority_age_limit(settings->seniorityAgeLimit),
dram_type(settings->dramType),
row_buffer_management_policy(settings->rowBufferManagementPolicy),
addr_mapping_scheme(settings->addressMappingScheme),
datarate(settings->dataRate),
posted_cas(settings->postedCAS),
read_write_grouping(settings->readWriteGrouping),
auto_precharge(settings->autoPrecharge),
clock_granularity(settings->clockGranularity),
cachelines_per_row(settings->cachelinesPerRow),
channelCount(settings->channelCount),
rankCount(settings->rankCount),
bankCount(settings->bankCount),
rowCount(settings->rowCount),
columnCount(settings->columnCount),
short_burst_ratio(settings->shortBurstRatio),
read_percentage(settings->readPercentage)
{}


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
cachelines_per_row(rhs->cachelines_per_row),
col_size(rhs->col_size),
row_size(rhs->row_size),
cacheline_size(rhs->cacheline_size),
channelCount(rhs->channelCount),
rankCount(rhs->rankCount),
bankCount(rhs->bankCount),
rowCount(rhs->rowCount),
columnCount(rhs->columnCount),
command_ordering_algorithm(rhs->command_ordering_algorithm),
config_type(rhs->config_type),
read_percentage(rhs->read_percentage),
short_burst_ratio(rhs->short_burst_ratio)
{}



ostream &DRAMSimII::operator<<(ostream &os, const dramSystemConfiguration &this_a)
{
	//os << "PerBankQ[" << this_a.perBankQueueDepth << "] ";
	os << "CH[" << this_a.channelCount << "] ";
	os << "RK[" << this_a.rankCount << "] ";
	os << "BK[" << this_a.bankCount << "] ";

	return os;
}

dramSystemConfiguration& dramSystemConfiguration::operator =(const DRAMSimII::dramSystemConfiguration &rs)
{
	if (this == &rs)
	{
		return *this;
	}
	command_ordering_algorithm = rs.command_ordering_algorithm;
	config_type = rs.config_type;
	refresh_time = rs.refresh_time;
	refresh_policy = rs.refresh_policy;
	col_size = rs.col_size;
	row_size = rs.row_size;
	rowCount = rs.rowCount;
	columnCount = rs.columnCount;
	cacheline_size = rs.cacheline_size;
	seniority_age_limit = rs.seniority_age_limit;
	dram_type = rs.dram_type;
	row_buffer_management_policy = rs.row_buffer_management_policy;
	addr_mapping_scheme = rs.addr_mapping_scheme;
	datarate = rs.datarate;
	posted_cas = rs.posted_cas;
	read_write_grouping = rs.read_write_grouping;
	auto_precharge = rs.auto_precharge;
	clock_granularity = rs.clock_granularity;
	cachelines_per_row = rs.cachelines_per_row;
	channelCount = rs.channelCount;
	rankCount = rs.rankCount;
	bankCount = rs.bankCount;
	short_burst_ratio = rs.short_burst_ratio;
	read_percentage = rs.read_percentage;

	return *this;
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

	rowCount = parameter->rowCount;

	columnCount = parameter->colCount;

	col_size = parameter->colSize;

	row_size = parameter->rowSize;

	cacheline_size = parameter->cachelineSize;
}
#endif
