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
#include "SystemConfiguration.hh"


#ifndef WIN32
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
using boost::iostreams::bzip2_compressor;
using boost::iostreams::gzip_compressor;
using boost::iostreams::gzip_params;
#endif

#ifdef WIN32
#include <io.h> 
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <locale>

using std::ostream;
using namespace DRAMsimII;
using boost::iostreams::null_sink;
using boost::iostreams::file_sink;
using boost::iostreams::filtering_ostream;
using std::ifstream;
using std::ofstream;
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


namespace bf = boost::filesystem;

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
dimmCount(settings.dimmCount),
rankCount(settings.rankCount),
bankCount(settings.bankCount),
rowCount(settings.rowCount),
columnCount(settings.columnCount),
decodeWindow(settings.decodeWindow),
epoch(settings.epoch),
shortBurstRatio(settings.shortBurstRatio),
readPercentage(settings.readPercentage),
sessionID(settings.sessionID),
outType(settings.outFileType)
{
	Address::initialize(settings);

	assert(decodeWindow >= 1);

	// initialize the output streams
	if (!timingOutStream.is_complete())
	{
		string suffix;
		switch (outType)
		{			
		case COUT:
			timingOutStream.push(cout);
			powerOutStream.push(cout);
			statsOutStream.push(cout);
			verilogOutStream.push(cout);
			return;
			break;
		case NONE:
			timingOutStream.push(null_sink());
			powerOutStream.push(null_sink());
			statsOutStream.push(null_sink());
			verilogOutStream.push(null_sink());
			return;
			break;
		case BZ:
#ifndef WIN32
			suffix = ".bz2";
			break;
#endif
		case GZ:
#ifndef WIN32
			suffix = ".gz";
			break;
#endif
		default:
			break;
		}

		bf::path outDir = settings.outFileDir.length() > 0 ? bf::path(settings.outFileDir.c_str()) : bf::current_path();

		if (!bf::exists(outDir))
		{
			if (!bf::create_directory(outDir))
			{
				cerr << "Could not create dir " << outDir.leaf() << " and dir does not exist." << endl;
				exit(-1);
			}
		}
		else
		{
			if (!bf::is_directory(outDir))
			{
				cerr << "Directory " << outDir.leaf() << " exists, but is not a directory." << endl;
				exit(-1);
			}
		}
		if (settings.outFileType == GZ || settings.outFileType == BZ || settings.outFileType == UNCOMPRESSED)
		{
			string baseFilename = settings.outFile;

			boost::trim(baseFilename);

			// strip off the file suffix
			if (baseFilename.find("gz") != string::npos)
				baseFilename = baseFilename.substr(0,baseFilename.find(".gz"));
			if (baseFilename.find("bz2") != string::npos)
				baseFilename = baseFilename.substr(0,baseFilename.find(".bz2"));

			std::locale loc;
			char *jobID = getenv("PBS_JOBID");

			if (jobID != NULL)
			{
				char *dot = strchr(jobID,'.');
				if (dot != NULL)
					*dot = (char)NULL;
				string JobID(jobID);
				baseFilename += jobID;
			}
			bool endsWithNumber = std::isdigit(*(baseFilename.end() - 1), loc);

			unsigned fileCounter = 0;

			string counter = endsWithNumber ? "" : boost::lexical_cast<string>(fileCounter);

			stringstream settingsFilename;

			do
			{
				stringstream timingFilename;
				stringstream powerFilename;
				stringstream statsFilename;
				stringstream verilogFilename;

				timingFilename.str("");
				powerFilename.str("");
				statsFilename.str("");
				settingsFilename.str("");
				verilogFilename.str("");
				timingFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(endsWithNumber ? 0 : 3) << counter << "-timing" << suffix;
				powerFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(endsWithNumber ? 0 : 3) << counter << "-power" << suffix;
				statsFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(endsWithNumber ? 0 : 3) << counter << "-stats" << suffix;
				verilogFilename << outDir.string() << "/" << baseFilename << setfill('0') << setw(endsWithNumber ? 0 : 3) << counter << "-verilog" << suffix;
				settingsFilename << outDir.native_directory_string() << "/" << baseFilename << setfill('0') << setw(endsWithNumber ? 0 : 3) << counter << "-settings.xml";	
				timingFile = timingFilename.str();
				powerFile = powerFilename.str();
				statsFile = statsFilename.str();
				verilogFile = verilogFilename.str();

				if (!endsWithNumber)
				{
					fileCounter++;
				}
				else
				{
					endsWithNumber = false;
				}
				counter = boost::lexical_cast<string>(fileCounter);


			} while (!createNewFile(timingFile) || !createNewFile(powerFile) || !createNewFile(statsFile) || !createNewFile(settingsFilename.str()) || !createNewFile(verilogFile));

			if (setupStreams())
			{
				filtering_ostream settingsOutStream;
				settingsOutStream.push(file_sink(settingsFilename.str().c_str()));

				settingsOutStream << settings.settingsOutputFile;
			}
		}
	}
}


SystemConfiguration::SystemConfiguration(const SystemConfiguration &rhs):
commandOrderingAlgorithm(rhs.commandOrderingAlgorithm),
transactionOrderingAlgorithm(rhs.transactionOrderingAlgorithm),
configType(rhs.configType),
refreshTime(rhs.refreshTime),
refreshPolicy(rhs.refreshPolicy),
columnSize(rhs.columnSize),
rowSize(rhs.rowSize),
cachelineSize(rhs.cachelineSize),
seniorityAgeLimit(rhs.seniorityAgeLimit),
dramType(rhs.dramType),
rowBufferManagementPolicy(rhs.rowBufferManagementPolicy),
addressMappingScheme(rhs.addressMappingScheme),
datarate(rhs.datarate),
postedCAS(rhs.postedCAS),
readWriteGrouping(rhs.readWriteGrouping),
autoPrecharge(rhs.autoPrecharge),
clockGranularity(rhs.clockGranularity),
cachelinesPerRow(rhs.cachelinesPerRow),
channelCount(rhs.channelCount),
dimmCount(rhs.dimmCount),
rankCount(rhs.rankCount),
bankCount(rhs.bankCount),
rowCount(rhs.rowCount),
columnCount(rhs.columnCount),
decodeWindow(rhs.decodeWindow),
epoch(rhs.epoch),
shortBurstRatio(rhs.shortBurstRatio),
readPercentage(rhs.readPercentage),
sessionID(rhs.sessionID),
timingFile(rhs.timingFile),
powerFile(rhs.powerFile),
statsFile(rhs.statsFile),
outType(rhs.outType)
{
	setupStreams();
}

SystemConfiguration::~SystemConfiguration()
{
	boost::iostreams::close(timingOutStream,std::ios_base::out);
	boost::iostreams::close(powerOutStream,std::ios_base::out);
	boost::iostreams::close(statsOutStream,std::ios_base::out);
	boost::iostreams::close(verilogOutStream, std::ios_base::out);
}

//////////////////////////////////////////////////////////////////////////
/// @brief attempt to atomically create a file with this name
/// @return true if the file was created
//////////////////////////////////////////////////////////////////////////
bool SystemConfiguration::createNewFile(const string& fileName) const
{
	int fs = 
#ifdef WIN32
		_open(fileName.c_str(), O_CREAT | O_EXCL, _S_IREAD | _S_IWRITE);
#else
		open(fileName.c_str(), O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
	if (fs < 0)
		return false;
	else
	{
		close(fs);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief helper function to determine if a file already exists
/// @return true if the file already exists, false otherwise
//////////////////////////////////////////////////////////////////////////
bool SystemConfiguration::fileExists(stringstream& fileName) const
{
	bf::path newPath(fileName.str().c_str());
	return bf::exists(newPath);
}

SystemConfiguration& SystemConfiguration::operator =(const DRAMsimII::SystemConfiguration &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	commandOrderingAlgorithm = rhs.commandOrderingAlgorithm;
	outType = rhs.outType;
	statsFile = rhs.statsFile;
	powerFile = rhs.powerFile;
	timingFile = rhs.timingFile;
	transactionOrderingAlgorithm = rhs.transactionOrderingAlgorithm;
	configType = rhs.configType;
	refreshTime = rhs.refreshTime;
	refreshPolicy = rhs.refreshPolicy;
	columnSize = rhs.columnSize;
	rowSize = rhs.rowSize;
	rowCount = rhs.rowCount;
	columnCount = rhs.columnCount;
	cachelineSize = rhs.cachelineSize;
	seniorityAgeLimit = rhs.seniorityAgeLimit;
	dramType = rhs.dramType;
	rowBufferManagementPolicy = rhs.rowBufferManagementPolicy;
	addressMappingScheme = rhs.addressMappingScheme;
	datarate = rhs.datarate;
	postedCAS = rhs.postedCAS;
	readWriteGrouping = rhs.readWriteGrouping;
	autoPrecharge = rhs.autoPrecharge;
	clockGranularity = rhs.clockGranularity;
	cachelinesPerRow = rhs.cachelinesPerRow;
	channelCount = rhs.channelCount;
	dimmCount = rhs.dimmCount;
	rankCount = rhs.rankCount;
	bankCount = rhs.bankCount;
	decodeWindow = rhs.decodeWindow;
	assert(decodeWindow >= 1);
	shortBurstRatio = rhs.shortBurstRatio;
	readPercentage = rhs.readPercentage;
	sessionID = rhs.sessionID;
	verilogFile = rhs.verilogFile;

	return *this;
}

bool SystemConfiguration::operator ==(const SystemConfiguration& rhs) const
{
	return (commandOrderingAlgorithm == rhs.commandOrderingAlgorithm &&
		transactionOrderingAlgorithm == rhs.transactionOrderingAlgorithm &&
		configType == rhs.configType &&
		refreshTime == rhs.refreshTime &&
		refreshPolicy == rhs.refreshPolicy &&
		columnSize == rhs.columnSize &&
		rowSize == rhs.rowSize &&
		cachelineSize == rhs.cachelineSize &&
		seniorityAgeLimit == rhs.seniorityAgeLimit &&
		dramType == rhs.dramType &&
		rowBufferManagementPolicy == rhs.rowBufferManagementPolicy &&
		addressMappingScheme == rhs.addressMappingScheme &&
		AlmostEqual<double>(datarate,rhs.datarate) &&
		postedCAS == rhs.postedCAS &&
		readWriteGrouping == rhs.readWriteGrouping &&
		autoPrecharge == rhs.autoPrecharge &&
		clockGranularity == rhs.clockGranularity &&
		cachelinesPerRow == rhs.cachelinesPerRow &&
		channelCount == rhs.channelCount &&
		dimmCount == rhs.dimmCount &&
		rankCount == rhs.rankCount &&
		bankCount == rhs.bankCount &&
		rowCount == rhs.rowCount &&
		columnCount == rhs.columnCount &&
		decodeWindow == rhs.decodeWindow &&
		epoch == rhs.epoch &&
		AlmostEqual<double>(shortBurstRatio,rhs.shortBurstRatio) &&
		AlmostEqual<double>(readPercentage,rhs.readPercentage) &&
		sessionID == rhs.sessionID);
}

ostream &DRAMsimII::operator<<(ostream &os, const SystemConfiguration &this_a)
{
	os << "CH[" << this_a.channelCount << "] ";
	os << "RK[" << this_a.rankCount << "] ";
	os << "BK[" << this_a.bankCount << "] ";

	return os;
}

bool SystemConfiguration::setupStreams() const 
{
	if (!timingOutStream.is_complete())
	{
		string suffix;
		switch (outType)
		{	
		case COUT:
			timingOutStream.push(cout);
			return true;
			break;
		case NONE:
			timingOutStream.push(null_sink());
			return true;
			break;
		case BZ:
#ifndef WIN32
			timingOutStream.push(bzip2_compressor());
			suffix = ".bz2";
			break;
#endif
		case GZ:
#ifndef WIN32
			timingOutStream.push(gzip_compressor(gzip_params(9)));
			suffix = ".gz";
			break;
#endif
		default:
			break;
		}

		timingOutStream.push(file_sink(timingFile.c_str()));

		if (!timingOutStream.good() || !timingOutStream.is_complete())
		{
			cerr << "Error opening file \"" << timingFile << "\" for writing" << endl;
			exit(-12);
		}

		
	}

	if (!powerOutStream.is_complete())
	{
		string suffix;
		switch (outType)
		{	
		case COUT:
			powerOutStream.push(cout);
			return true;
			break;
		case NONE:
			powerOutStream.push(null_sink());
			return true;
			break;
		case BZ:
#ifndef WIN32
			powerOutStream.push(bzip2_compressor());
			suffix = ".bz2";
			break;
#endif
		case GZ:
#ifndef WIN32
			powerOutStream.push(gzip_compressor(gzip_params(9)));
			suffix = ".gz";
			break;
#endif
		default:
			break;
		}

		powerOutStream.push(file_sink(powerFile.c_str()));

		if (!powerOutStream.good() || !powerOutStream.is_complete())
		{
			cerr << "Error opening file \"" << powerFile << "\" for writing" << endl;
			exit(-12);
		}

	}

	if (!statsOutStream.is_complete())
	{
		string suffix;
		switch (outType)
		{	
		case COUT:
			statsOutStream.push(cout);
			return true;
			break;
		case NONE:
			statsOutStream.push(null_sink());
			return true;
			break;
		case BZ:
#ifndef WIN32
			statsOutStream.push(bzip2_compressor());
			suffix = ".bz2";
			break;
#endif
		case GZ:
#ifndef WIN32
			statsOutStream.push(gzip_compressor(gzip_params(9)));
			suffix = ".gz";
			break;
#endif
		default:
			break;
		}

		statsOutStream.push(file_sink(statsFile.c_str()));

		if (!statsOutStream.good() || !statsOutStream.is_complete())
		{
			cerr << "Error opening file \"" << statsFile << "\" for writing" << endl;
			exit(-12);
		}
	}

	if (!verilogOutStream.is_complete())
	{
		string suffix;
		switch (outType)
		{	
		case COUT:
			verilogOutStream.push(cout);
			return true;
			break;
		case NONE:
			verilogOutStream.push(null_sink());
			return true;
			break;
		case BZ:
#ifndef WIN32
			verilogOutStream.push(bzip2_compressor());
			suffix = ".bz2";
			break;
#endif
		case GZ:
#ifndef WIN32
			verilogOutStream.push(gzip_compressor(gzip_params(9)));
			suffix = ".gz";
			break;
#endif
		default:
			break;
		}

		verilogOutStream.push(file_sink(verilogFile.c_str()));

		if (!verilogOutStream.good() || !verilogOutStream.is_complete())
		{
			cerr << "Error opening file \"" << verilogFile << "\" for writing" << endl;
			exit(-12);
		}
	}
	return true;
}