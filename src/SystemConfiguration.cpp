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

#include <map>
#include <string>
#include <sstream>
#include "SystemConfiguration.h"

using std::ostream;
using namespace DRAMsimII;

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

SystemConfiguration& SystemConfiguration::operator =(const DRAMsimII::SystemConfiguration &rs)
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
		AlmostEqual<double>(datarate,right.datarate,0.0005) &&
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
		AlmostEqual<double>(shortBurstRatio,right.shortBurstRatio,0.0005) &&
		AlmostEqual<double>(readPercentage,right.readPercentage,0.0005) &&
		sessionID==right.sessionID);
}

ostream &DRAMsimII::operator<<(ostream &os, const SystemConfiguration &this_a)
{
	os << "CH[" << this_a.channelCount << "] ";
	os << "RK[" << this_a.rankCount << "] ";
	os << "BK[" << this_a.bankCount << "] ";

	return os;
}
