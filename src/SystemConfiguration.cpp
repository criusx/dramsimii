#include <map>
#include <string>
#include <sstream>
#include "SystemConfiguration.h"

using std::ostream;
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
dramType(settings.dramType),
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
epoch(settings.epoch),
shortBurstRatio(settings.shortBurstRatio),
readPercentage(settings.readPercentage),
sessionID(settings.sessionID)
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
	dramType = rs.dramType;
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
	sessionID = rs.sessionID;

	return *this;
}

bool SystemConfiguration::operator ==(const SystemConfiguration& right) const 
{
	return (commandOrderingAlgorithm==right.commandOrderingAlgorithm &&
		transactionOrderingAlgorithm==right.transactionOrderingAlgorithm &&
		configType==right.configType &&
		refreshTime==right.refreshTime &&
		refreshPolicy==right.refreshPolicy &&
		columnSize==right.columnSize &&
		rowSize==right.rowSize &&
		cachelineSize==right.cachelineSize &&
		seniorityAgeLimit==right.seniorityAgeLimit &&
		dramType==right.dramType &&
		rowBufferManagementPolicy==right.rowBufferManagementPolicy &&
		addressMappingScheme==right.addressMappingScheme &&
		datarate==right.datarate &&
		postedCAS==right.postedCAS &&
		readWriteGrouping==right.readWriteGrouping &&
		autoPrecharge==right.autoPrecharge &&
		clockGranularity==right.clockGranularity &&
		cachelinesPerRow==right.cachelinesPerRow &&
		channelCount==right.channelCount &&
		rankCount==right.rankCount &&
		bankCount==right.bankCount &&
		rowCount==right.rowCount &&
		columnCount==right.columnCount &&
		epoch==right.epoch &&
		shortBurstRatio==right.shortBurstRatio &&
		readPercentage==right.readPercentage &&
		sessionID==right.sessionID);
}