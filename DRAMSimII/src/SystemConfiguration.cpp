#include <map>
#include <string>
#include <sstream>
#include "SystemConfiguration.h"

using namespace std;
using namespace DRAMSimII;

SystemConfiguration::SystemConfiguration(const Settings& settings):
commandOrderingAlgorithm(settings.commandOrderingAlgorithm),
transactionOrderingAlgorithm(settings.transactionOrderingAlgorithm),
configType(settings.systemType),
refreshTime(settings.dataRate * settings.refreshTime),
refreshPolicy(settings.refreshPolicy),
columnSize(settings.columnSize),
rowSize(settings.rowSize),
cachelineSize(settings.cacheLineSize),
seniorityAgeLimit(settings.seniorityAgeLimit),
dram_type(settings.dramType),
rowBufferManagementPolicy(settings.rowBufferManagementPolicy),
addressMappingScheme(settings.addressMappingScheme),
datarate(settings.dataRate),
postedCAS(settings.postedCAS),
readWriteGrouping(settings.readWriteGrouping),
autoPrecharge(settings.autoPrecharge),
clockGranularity(settings.clockGranularity),
cachelinesPerRow(settings.cachelinesPerRow),
channelCount(settings.channelCount),
rankCount(settings.rankCount),
bankCount(settings.bankCount),
rowCount(settings.rowCount),
columnCount(settings.columnCount),
shortBurstRatio(settings.shortBurstRatio),
readPercentage(settings.readPercentage)
{}


SystemConfiguration::SystemConfiguration(const SystemConfiguration *rhs):
commandOrderingAlgorithm(rhs->commandOrderingAlgorithm),
transactionOrderingAlgorithm(rhs->transactionOrderingAlgorithm),
configType(rhs->configType),
refreshTime(rhs->refreshTime),
refreshPolicy(rhs->refreshPolicy),
columnSize(rhs->columnSize),
rowSize(rhs->rowSize),
cachelineSize(rhs->cachelineSize),
seniorityAgeLimit(rhs->seniorityAgeLimit),
dram_type(rhs->dram_type),
rowBufferManagementPolicy(rhs->rowBufferManagementPolicy),
addressMappingScheme(rhs->addressMappingScheme),
datarate(rhs->datarate),
postedCAS(rhs->postedCAS),
readWriteGrouping(rhs->readWriteGrouping),
autoPrecharge(rhs->autoPrecharge),
clockGranularity(rhs->clockGranularity),
cachelinesPerRow(rhs->cachelinesPerRow),
channelCount(rhs->channelCount),
rankCount(rhs->rankCount),
bankCount(rhs->bankCount),
rowCount(rhs->rowCount),
columnCount(rhs->columnCount),
shortBurstRatio(rhs->shortBurstRatio),
readPercentage(rhs->readPercentage)
{}



ostream &DRAMSimII::operator<<(ostream &os, const SystemConfiguration &this_a)
{
	//os << "PerBankQ[" << this_a.perBankQueueDepth << "] ";
	os << "CH[" << this_a.channelCount << "] ";
	os << "RK[" << this_a.rankCount << "] ";
	os << "BK[" << this_a.bankCount << "] ";

	return os;
}

SystemConfiguration& SystemConfiguration::operator =(const DRAMSimII::SystemConfiguration &rs)
{
	if (this == &rs)
	{
		return *this;
	}
	commandOrderingAlgorithm = rs.commandOrderingAlgorithm;
	configType = rs.configType;
	refreshTime = rs.refreshTime;
	refreshPolicy = rs.refreshPolicy;
	columnSize = rs.columnSize;
	rowSize = rs.rowSize;
	rowCount = rs.rowCount;
	columnCount = rs.columnCount;
	cachelineSize = rs.cachelineSize;
	seniorityAgeLimit = rs.seniorityAgeLimit;
	dram_type = rs.dram_type;
	rowBufferManagementPolicy = rs.rowBufferManagementPolicy;
	addressMappingScheme = rs.addressMappingScheme;
	datarate = rs.datarate;
	postedCAS = rs.postedCAS;
	readWriteGrouping = rs.readWriteGrouping;
	autoPrecharge = rs.autoPrecharge;
	clockGranularity = rs.clockGranularity;
	cachelinesPerRow = rs.cachelinesPerRow;
	channelCount = rs.channelCount;
	rankCount = rs.rankCount;
	bankCount = rs.bankCount;
	shortBurstRatio = rs.shortBurstRatio;
	readPercentage = rs.readPercentage;

	return *this;
}


#ifdef M5
SystemConfiguration::SystemConfiguration(System::Params *parameter)
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
		rowBufferManagementPolicy = OPEN_PAGE;
	else if (parameter->rowBufferManagmentPolicy == "close_page")
		rowBufferManagementPolicy = CLOSE_PAGE;
	else
		rowBufferManagementPolicy = AUTO_PAGE;

	autoPrecharge = parameter->autoPrecharge;

	if (parameter->addrMappingScheme == "burger_base_map")
		addressMappingScheme = BURGER_BASE_MAP;
	else if (parameter->addrMappingScheme == "close_page_baseline")
		addressMappingScheme = CLOSE_PAGE_BASELINE;
	else if (parameter->addrMappingScheme == "INTEL845G_MAP")
		addressMappingScheme = INTEL845G_MAP;
	else if (parameter->addrMappingScheme == "sdram_base_map")
		addressMappingScheme = SDRAM_BASE_MAP;
	else if (parameter->addrMappingScheme == "sdram_close_page_map")
		addressMappingScheme = SDRAM_CLOSE_PAGE_MAP;
	else
		addressMappingScheme = SDRAM_HIPERF_MAP;

	datarate = parameter->datarate;

	refreshTime = parameter->refreshTime * datarate;

	readWriteGrouping = parameter->readWriteGrouping;

	if (parameter->refreshPolicy == "none" || parameter->refreshPolicy == "no refresh")
		refreshPolicy = NO_REFRESH;
	else if (parameter->refreshPolicy == "bank_concurrent" || parameter->refreshPolicy == "bank concurrent")
		refreshPolicy = BANK_CONCURRENT;
	else
		refreshPolicy = BANK_STAGGERED_HIDDEN;

	seniorityAgeLimit = parameter->seniorityAgeLimit;

	postedCAS = parameter->postedCas;

	clockGranularity = parameter->clockGranularity;

	cachelinesPerRow = parameter->cachelinesPerRow;

	rowCount = parameter->rowCount;

	columnCount = parameter->colCount;

	columnSize = parameter->colSize;

	rowSize = parameter->rowSize;

	cachelineSize = parameter->cachelineSize;
}
#endif
