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

#include <map>
#include <string>
#include <sstream>
#include "SystemConfiguration.hh"

#ifdef WIN32
#include <io.h> 
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <locale>

using std::ostream;
using namespace DRAMsimII;
using std::stringstream;
using std::cout;
using std::ios;
using std::setw;
using std::string;
using std::cerr;
using std::ifstream;
using std::setfill;
using std::setprecision;
using std::endl;


SystemConfiguration::SystemConfiguration(const Settings& settings):
commandOrderingAlgorithm(settings.commandOrderingAlgorithm),
transactionOrderingAlgorithm(settings.transactionOrderingAlgorithm),
refreshTime(settings.dataRate * settings.refreshTime),
refreshPolicy(settings.refreshPolicy),
columnSize(settings.columnSize),
rowSize(settings.rowSize),
seniorityAgeLimit(settings.seniorityAgeLimit),
dramType(settings.dramType),
rowBufferManagementPolicy(settings.rowBufferManagementPolicy),
addressMappingScheme(settings.addressMappingScheme),
datarate(settings.dataRate),
postedCAS(settings.postedCAS),
readWriteGrouping(settings.readWriteGrouping),
autoPrecharge(settings.autoPrecharge),
clockGranularity(settings.clockGranularity),
channelCount(settings.channelCount),
dimmCount(settings.dimmCount),
rankCount(settings.rankCount),
bankCount(settings.bankCount),
rowCount(settings.rowCount),
columnCount(settings.columnCount),
decodeWindow(settings.decodeWindow),
epoch(settings.epoch)
{
	Address::initialize(settings);

	assert(decodeWindow >= 1);
}


SystemConfiguration::SystemConfiguration(const SystemConfiguration &rhs):
commandOrderingAlgorithm(rhs.commandOrderingAlgorithm),
transactionOrderingAlgorithm(rhs.transactionOrderingAlgorithm),
refreshTime(rhs.refreshTime),
refreshPolicy(rhs.refreshPolicy),
columnSize(rhs.columnSize),
rowSize(rhs.rowSize),
seniorityAgeLimit(rhs.seniorityAgeLimit),
dramType(rhs.dramType),
rowBufferManagementPolicy(rhs.rowBufferManagementPolicy),
addressMappingScheme(rhs.addressMappingScheme),
datarate(rhs.datarate),
postedCAS(rhs.postedCAS),
readWriteGrouping(rhs.readWriteGrouping),
autoPrecharge(rhs.autoPrecharge),
clockGranularity(rhs.clockGranularity),
channelCount(rhs.channelCount),
dimmCount(rhs.dimmCount),
rankCount(rhs.rankCount),
bankCount(rhs.bankCount),
rowCount(rhs.rowCount),
columnCount(rhs.columnCount),
decodeWindow(rhs.decodeWindow),
epoch(rhs.epoch)
{}

SystemConfiguration& SystemConfiguration::operator =(const DRAMsimII::SystemConfiguration &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	commandOrderingAlgorithm = rhs.commandOrderingAlgorithm;
	transactionOrderingAlgorithm = rhs.transactionOrderingAlgorithm;
	refreshTime = rhs.refreshTime;
	refreshPolicy = rhs.refreshPolicy;
	columnSize = rhs.columnSize;
	rowSize = rhs.rowSize;
	rowCount = rhs.rowCount;
	columnCount = rhs.columnCount;
	seniorityAgeLimit = rhs.seniorityAgeLimit;
	dramType = rhs.dramType;
	rowBufferManagementPolicy = rhs.rowBufferManagementPolicy;
	addressMappingScheme = rhs.addressMappingScheme;
	datarate = rhs.datarate;
	postedCAS = rhs.postedCAS;
	readWriteGrouping = rhs.readWriteGrouping;
	autoPrecharge = rhs.autoPrecharge;
	clockGranularity = rhs.clockGranularity;
	channelCount = rhs.channelCount;
	dimmCount = rhs.dimmCount;
	rankCount = rhs.rankCount;
	bankCount = rhs.bankCount;
	decodeWindow = rhs.decodeWindow;
	assert(decodeWindow >= 1);
	
	return *this;
}

bool SystemConfiguration::operator ==(const SystemConfiguration& rhs) const
{
	return (commandOrderingAlgorithm == rhs.commandOrderingAlgorithm &&
		transactionOrderingAlgorithm == rhs.transactionOrderingAlgorithm &&
		refreshTime == rhs.refreshTime &&
		refreshPolicy == rhs.refreshPolicy &&
		columnSize == rhs.columnSize &&
		rowSize == rhs.rowSize &&
		seniorityAgeLimit == rhs.seniorityAgeLimit &&
		dramType == rhs.dramType &&
		rowBufferManagementPolicy == rhs.rowBufferManagementPolicy &&
		addressMappingScheme == rhs.addressMappingScheme &&
		AlmostEqual<double>(datarate,rhs.datarate) &&
		postedCAS == rhs.postedCAS &&
		readWriteGrouping == rhs.readWriteGrouping &&
		autoPrecharge == rhs.autoPrecharge &&
		clockGranularity == rhs.clockGranularity &&
		channelCount == rhs.channelCount &&
		dimmCount == rhs.dimmCount &&
		rankCount == rhs.rankCount &&
		bankCount == rhs.bankCount &&
		rowCount == rhs.rowCount &&
		columnCount == rhs.columnCount &&
		decodeWindow == rhs.decodeWindow &&
		epoch == rhs.epoch);
}

ostream &DRAMsimII::operator<<(ostream &os, const SystemConfiguration &this_a)
{
	os << "CH[" << this_a.channelCount << "] ";
	os << "RK[" << this_a.rankCount << "] ";
	os << "BK[" << this_a.bankCount << "] ";

	return os;
}
