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

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "InputStream.h"
#include "Settings.h"
#include "busEvent.h"
#include "SystemConfiguration.h"
#include "Channel.h"

using std::vector;
using std::dec;
using std::hex;
using std::cerr;
using std::endl;
using std::string;
using std::ostream;
using namespace DRAMsimII;

#ifndef WIN32
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
using boost::iostreams::bzip2_decompressor;
using boost::iostreams::gzip_decompressor;
using boost::iostreams::gzip_params;
#endif

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>


#define COMPRESS_INCOMING_TRANSACTIONS 0

//////////////////////////////////////////////////////////////////////////
/// @brief standard constructor
//////////////////////////////////////////////////////////////////////////
InputStream::InputStream(const Settings& settings,const SystemConfiguration &systemConfigVal, const vector<Channel> &systemChannel):
type(settings.inFileType),
systemConfig(systemConfigVal),
channel(systemChannel),
channelLocality(1.0F / settings.channelCount),
rankLocality(1.0F / settings.rankCount),
bankLocality(1.0F / settings.bankCount),
time(0),
rowLocality(1.0F / settings.rowCount),
readPercentage(settings.readPercentage),
shortBurstRatio(settings.shortBurstRatio),
arrivalThreshold(0.0F),
cpuToMemoryRatio(settings.dataRate * 1E-9),
averageInterarrivalCycleCount(settings.averageInterarrivalCycleCount),
interarrivalDistributionModel(settings.arrivalDistributionModel),
traceFilename(settings.inFile),
randomNumberGenerator(std::time(0)),
rngDistributionModel(0,1),
rngIntDistributionModel(0u,16383u),
gaussianDistribution(settings.averageInterarrivalCycleCount,8),
rngGenerator(randomNumberGenerator, rngDistributionModel),
rngIntGenerator(randomNumberGenerator, rngIntDistributionModel),
arrivalGenerator(randomNumberGenerator, gaussianDistribution)
{
	using boost::iostreams::file_source;
	using namespace boost::filesystem;
	using boost::algorithm::ends_with;

	if (interarrivalDistributionModel == UNIFORM_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / (double)averageInterarrivalCycleCount);
	else if (interarrivalDistributionModel == GAUSSIAN_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / boxMuller((double)averageInterarrivalCycleCount, 10));	
	if (arrivalThreshold > 1.0F)
		arrivalThreshold = 1.0F / arrivalThreshold;

	if (!traceFilename.empty() && (type != RANDOM))
	{
		if (!traceFile.is_complete() || !traceFile.good())
		{			
#ifndef WIN32
			if (ends_with(traceFilename,"gz"))
				traceFile.push(gzip_decompressor());
			else if (ends_with(traceFilename,"bz2") || ends_with(traceFilename,"bz"))
				traceFile.push(bzip2_decompressor());
#endif
			path inFileWithPath(traceFilename);
			// first check the absolute path
			if (!exists(inFileWithPath) || !is_regular(inFileWithPath))
			{
				inFileWithPath = current_path() / traceFilename;

				// then check the traceFiles dir
				if (!exists(inFileWithPath) || !is_regular(inFileWithPath))
				{
					inFileWithPath = current_path() / "traceFiles" / settings.inFile;

					if (!exists(inFileWithPath) || !is_regular(inFileWithPath))
					{
						cerr << "Unable to open trace file \"" << settings.inFile << "\"" << endl;
						exit(-9);
					}
					else
					{
						traceFile.push(file_source(inFileWithPath.string().c_str()));
					}
				}
				else
				{
					traceFile.push(file_source(inFileWithPath.string().c_str()));
				}
			}
			else
			{
				traceFile.push(file_source(inFileWithPath.string().c_str()));
			}

			if (!traceFile.is_complete())
			{
				cerr << "Unable to open trace file \"" << settings.inFile << "\"" << endl;
				exit(-9);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief deserialization constructor
//////////////////////////////////////////////////////////////////////////
InputStream::InputStream(const SystemConfiguration &systemConfig, const vector<Channel> &systemChannel, DistributionType arrivalDistributionModel, string filename):
systemConfig(systemConfig),
channel(systemChannel),
interarrivalDistributionModel(arrivalDistributionModel),
randomNumberGenerator(std::time(0)),
rngDistributionModel(0,1),
rngIntDistributionModel(0u,16383u),
gaussianDistribution(averageInterarrivalCycleCount, 8),
rngGenerator(randomNumberGenerator, rngDistributionModel),
rngIntGenerator(randomNumberGenerator, rngIntDistributionModel),
arrivalGenerator(randomNumberGenerator, gaussianDistribution)
{
	using namespace boost::algorithm;
	using boost::iostreams::file_source;
	if (interarrivalDistributionModel == UNIFORM_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / (double)averageInterarrivalCycleCount);
	else if (interarrivalDistributionModel == GAUSSIAN_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / boxMuller((double)averageInterarrivalCycleCount, 10));
	if (arrivalThreshold > 1.0F)
		arrivalThreshold = 1.0F / arrivalThreshold;

	if (!traceFilename.empty() && (type != RANDOM))
	{
		//traceFile.open(settings.inFile.c_str());

		if (!traceFile.good())
		{
			boost::filesystem::path cwd(boost::filesystem::current_path());
			cwd = cwd / "traceFiles" / traceFilename;
			string inFileWithPath = cwd.string();
#ifndef WIN32
			if (ends_with(traceFilename,"gz"))
				traceFile.push(gzip_decompressor());
			else if (ends_with(traceFilename,"bz2") || ends_with(traceFilename,"bz"))
				traceFile.push(bzip2_decompressor());
#endif

			traceFile.push(file_source(inFileWithPath.c_str()));

			if (!traceFile.is_complete())
			{
				cerr << "Unable to open trace file \"" << traceFilename << "\"" << endl;
				exit(-9);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief copy constructor
//////////////////////////////////////////////////////////////////////////
InputStream::InputStream(const InputStream& rhs):
type(rhs.type),
systemConfig(rhs.systemConfig),
channel(rhs.channel),
channelLocality(rhs.channelLocality),
rankLocality(rhs.rankLocality),
bankLocality(rhs.bankLocality),
time(rhs.time),
rowLocality(rhs.rowLocality),
readPercentage(rhs.readPercentage),
shortBurstRatio(rhs.shortBurstRatio),
arrivalThreshold(rhs.arrivalThreshold),
cpuToMemoryRatio(rhs.cpuToMemoryRatio),
averageInterarrivalCycleCount(rhs.averageInterarrivalCycleCount),
interarrivalDistributionModel(rhs.interarrivalDistributionModel),
// TODO: serialize tracefile position
randomNumberGenerator(rhs.randomNumberGenerator),
rngDistributionModel(rhs.rngDistributionModel),
rngIntDistributionModel(rhs.rngIntDistributionModel),
gaussianDistribution(rhs.gaussianDistribution),
rngGenerator(rhs.rngGenerator),
rngIntGenerator(rhs.rngIntGenerator),
arrivalGenerator(rhs.arrivalGenerator)
{}

InputStream::InputStream(const InputStream& rhs, const SystemConfiguration &systemConfigVal, const vector<Channel> &systemChannel):
type(rhs.type),
systemConfig(systemConfigVal),
channel(systemChannel),
channelLocality(rhs.channelLocality),
rankLocality(rhs.rankLocality),
bankLocality(rhs.bankLocality),
time(rhs.time),
rowLocality(rhs.rowLocality),
readPercentage(rhs.readPercentage),
shortBurstRatio(rhs.shortBurstRatio),
arrivalThreshold(rhs.arrivalThreshold),
cpuToMemoryRatio(rhs.cpuToMemoryRatio),
averageInterarrivalCycleCount(rhs.averageInterarrivalCycleCount),
interarrivalDistributionModel(rhs.interarrivalDistributionModel),
// TODO: serialize tracefile position
randomNumberGenerator(rhs.randomNumberGenerator),
rngDistributionModel(rhs.rngDistributionModel),
rngIntDistributionModel(rhs.rngIntDistributionModel),
gaussianDistribution(rhs.gaussianDistribution),
rngGenerator(rhs.rngGenerator),
rngIntGenerator(rhs.rngIntGenerator),
arrivalGenerator(rhs.arrivalGenerator)
{}

//////////////////////////////////////////////////////////////////////
/// @brief generates a number using a Poisson random variable
/// @details from the book "Numerical Recipes in C: The Art of Scientific Computing"
/// @param xm the average inter arrival cycle count
/// @return the time until the next transaction arrives
//////////////////////////////////////////////////////////////////////
float InputStream::Poisson(float xm) const
{	
	static float sq, alxm, g, oldm = -1.0F;
	float em, t, y;

	if (xm < 12.0)
	{
		if (!AlmostEqual<float>(xm,oldm))
		{
			oldm = xm;
			g = exp(-xm);
		}
		em = -1;
		t = 1.0;
		do
		{
			++em;
			t *= rngGenerator();
		} while (t > g);		
	}
	else
	{
		if (!AlmostEqual<float>(xm,oldm))
		{
			oldm = xm;
			sq = sqrt(2.0 * xm);
			alxm = log(xm);
			g = xm * alxm - gammaLn(xm + 1.0);
		}
		do
		{
			do
			{
				y = tan(PI * rngGenerator());
				em = sq * y + xm;

			} while (em < 0.0);

			em = floor(em);
			t = 0.9 * (1.0 + y*y) * exp(em * alxm - gammaLn(em+1.0) -g);

		} while (rngGenerator() > t);
	}
	return em;
}

float InputStream::gammaLn(const float xx) const
{
	static float coefficients[6] =
	{76.18009172947146F, -86.50532032941677F,
	24.01409824083091F, -1.231739572450155F,
	0.1208650973866179e-2F, -0.5395239384953e-5F};

	float y = xx;
	float x = xx;
	float tmp = x + 5.5F;
	tmp -= (x+0.5F) * log(tmp);
	float ser = 1.000000000190015F;
	for (unsigned j = 0; j <= 5; j++)
		ser += coefficients[j]/++y;
	return -tmp + log(2.5066282746310005F * ser/x);

}

float InputStream::boxMuller(const float m, const float s) const
{
	float w, y1;
	static float y2;
	static bool use_last = false;

	if (use_last) // use value from previous call
	{
		y1 = y2;
		use_last = false;
	}
	else
	{
		float x1, x2;
		do
		{
			x1 = 2.0F * rngGenerator() - 1.0F;
			x2 = 2.0F * rngGenerator() - 1.0F;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0);

		w = sqrt( (-2.0 * log( w)) / w);
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = true;
	}

	return floor(m + y1 * s);
}


bool InputStream::getNextBusEvent(BusEvent &thisEvent)
{	
	string input;	

	switch (type)
	{
	case K6_TRACE:	
		{
			//int base_control;
			Transaction::TransactionType attributes;
			int burst_length = 4; // Socket 7 cachelines are 32 byte long, burst of 4
			int burst_count = 0;
			bool bursting = true;
			//double multiplier;
			tick timestamp = TICK_MAX;
			unsigned address = UINT_MAX;

			while ((bursting == true) && traceFile.good())
			{
				traceFile >> hex >> address >> input >> timestamp;

				if(traceFile.good())
				{
					cerr << "Unexpected EOF, Please fix input trace file" << endl;
					return false;
				}

				FileIOToken control = Settings::dramTokenizer(input);

				if (control == unknown_token)
				{
					cerr << "Unknown Token Found" << input << endl;
					return false;
				}

				if (control == MEM_WR)
				{
					attributes = Transaction::WRITE_TRANSACTION;
				}
				else
				{
					attributes = Transaction::READ_TRANSACTION;
				}

				traceFile >> input;
				if(traceFile.eof())
				{
					cerr << "Unexpected EOF, Please fix input trace file" << endl;
					return false;
				}
				// FIXME: what was this for?
				if (/*(thisEvent.attributes != control) || */
					(((thisEvent.address.getPhysicalAddress() ^ address) & 0xFFFFFFE0) != 0) || (burst_count == burst_length))
				{
					bursting = false;
					timestamp = static_cast<tick>(static_cast<double>(timestamp) * ascii2multiplier(input));
					thisEvent.address.setPhysicalAddress(0x3FFFFFFF & address); // mask out top addr bit
					thisEvent.attributes = Transaction::CONTROL_TRANSACTION;
					thisEvent.timestamp = timestamp;
					burst_count = 1;
				}
				else
				{
					burst_count++;
				}
			}
			thisEvent.address.setPhysicalAddress(address);
			thisEvent.timestamp = timestamp;
		}
		break;
	case MASE_TRACE:
		{
			PhysicalAddress tempPA;
			traceFile >> hex >> tempPA >> input >> dec >> thisEvent.timestamp;
			thisEvent.address.setPhysicalAddress(tempPA);

			//thisEvent.timestamp /= 40000;

			//thisEvent.timestamp /= cpuToMemoryRatio;
			thisEvent.timestamp *= cpuToMemoryRatio;
			if(!traceFile.good()) /// found starting Hex address
			{
				cerr << "Unexpected EOF, Please fix input trace file" << endl;
				return false;
			}

			switch (Settings::dramTokenizer(input))
			{
			case FETCH:
				thisEvent.attributes = Transaction::IFETCH_TRANSACTION;
				break;
			case MEM_RD:
				thisEvent.attributes = Transaction::READ_TRANSACTION;
				break;
			case MEM_WR:
				thisEvent.attributes = Transaction::WRITE_TRANSACTION;
				break;
			case unknown_token:
				cerr << "Unknown Token Found " << input << endl;
				return false;
				break;			
			default:
				cerr << "Unexpected transaction type: " << input;
				exit(-7);
				break;
			}
		}
		break;
	case MAPPED:
		{
			unsigned channel, rank, bank, row, column;

			traceFile >> dec >> thisEvent.timestamp >> input >> dec >> channel >> rank >> bank >> row >> column;

			thisEvent.address.setAddress(channel,rank,bank,row,column);

			if(!traceFile.good()) /// found starting Hex address
			{
				cerr << "Unexpected EOF, Please fix input trace file" << endl;
				return false;
			}

			FileIOToken control = Settings::dramTokenizer(input);

			switch (control)
			{
			case unknown_token:
				cerr << "Unknown Token Found " << input << endl;
				return false;
				break;
			case FETCH:
				thisEvent.attributes = Transaction::IFETCH_TRANSACTION;
				break;
			case MEM_RD:
				thisEvent.attributes = Transaction::READ_TRANSACTION;
				break;
			case MEM_WR:
				thisEvent.attributes = Transaction::WRITE_TRANSACTION;
				break;
			default:
				cerr << "Unexpected transaction type: " << input;
				exit(-7);
				break;
			}
		}
		break;
	default:
		cerr << "Unknown address trace type" << endl;
		exit(-8);
		break;
	}

	//thisEvent.attributes = CONTROL_TRANSACTION;

	return true;
}

InputStream::InputType InputStream::toInputToken(const string &input) const
{
	if (input == "k6" || input == "K6")
		return K6_TRACE;
	else if (input == "mase" || input == "MASE" || input == "Mase")
		return MASE_TRACE;
	else if (input == "random" || input == "RANDOM" || input == "Random")
		return RANDOM;
	else if (input == "mapped" || input == "Mapped" || input == "MAPPED")
		return MAPPED;
	return MAPPED;
}

//////////////////////////////////////////////////////////////////////
/// @brief get the next logical transaction
/// @details get the transaction from whichever source is currently being used
/// whether a random input, mase, k6 or mapped trace file
/// @author Joe Gross
/// @return the next transaction, NULL if there are no more available transactions
//////////////////////////////////////////////////////////////////////
Transaction *InputStream::getNextIncomingTransaction()
{
	switch (type)
	{
	case RANDOM:
		return getNextRandomRequest();
		break;
	case K6_TRACE:
	case MASE_TRACE:
	case MAPPED:
		{
			static BusEvent newEvent;

			if (!getNextBusEvent(newEvent))
			{
				/* EOF reached */
				return NULL;
			}
			else
			{
				Transaction *tempTransaction = new Transaction(newEvent.attributes,newEvent.timestamp >> COMPRESS_INCOMING_TRANSACTIONS,0,newEvent.address, 0, 0);
				// FIXME: ignores return type
				//convertAddress(tempTransaction->getAddresses());

				// need to adjust arrival time for K6 traces to cycles
				return tempTransaction;
			}
		}
		break;
	case NONE:
	default:
		cerr << "Unknown input trace format" << endl;
		exit(-20);
		break;
	}
}

double InputStream::ascii2multiplier(const string &input) const
{
	switch(Settings::dramTokenizer(input))
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

bool InputStream::operator==(const InputStream& rhs) const
{
	return type == rhs.type && systemConfig == rhs.systemConfig && channel == rhs.channel &&
		AlmostEqual<float>(channelLocality,rhs.channelLocality) && AlmostEqual<float>(rankLocality,rhs.rankLocality) &&
		AlmostEqual<float>(bankLocality,rhs.bankLocality) && time == rhs.time && AlmostEqual<float>(rowLocality,rhs.rowLocality) &&
		AlmostEqual<float>(readPercentage,rhs.readPercentage) && interarrivalDistributionModel == rhs.interarrivalDistributionModel &&
		AlmostEqual<float>(shortBurstRatio, rhs.shortBurstRatio) && AlmostEqual<float>(arrivalThreshold, rhs.arrivalThreshold) &&
		AlmostEqual<float>(cpuToMemoryRatio, rhs.cpuToMemoryRatio) && averageInterarrivalCycleCount == rhs.averageInterarrivalCycleCount;
}


//////////////////////////////////////////////////////////////////////
/// @brief get a random request
/// @details create a random transaction according to the parameters in systemConfig
/// use the probabilities specified to generate a mapped request
/// @author Joe Gross
/// @return a pointer to the transaction that was generated
//////////////////////////////////////////////////////////////////////
Transaction *InputStream::getNextRandomRequest()
{
	if (type == RANDOM)
	{
		static unsigned nextChannel;

		// check against last transaction to see what the chan_id was, and whether we need to change channels or not
		// choose a random channel that's not this one
		if (channelLocality < rngGenerator())
		{
			nextChannel = (nextChannel + (rngIntGenerator() % (systemConfig.getChannelCount() - 1))) % systemConfig.getChannelCount();
		}
		// choose a random channel from any of them
		else
		{
			nextChannel = rngIntGenerator() % systemConfig.getChannelCount();
		}

		// check against the rank_id of the last transaction to the newly selected channel to see if we need to change the rank_id
		// or keep to this rank_id
		unsigned nextRank = channel[nextChannel].getLastRankID();

		if (rankLocality < rngGenerator())
		{
			// choose a rank that's not this one
			nextRank = (nextRank + 1 + (rngIntGenerator() % (systemConfig.getRankCount() - 1))) % systemConfig.getRankCount();
		}
		// else choose the same rank again

		unsigned nextBank = channel[nextChannel].getRank(nextRank).getLastBankID();

		if (bankLocality < rngGenerator())
		{
			// choose a new bank that's not this one
			nextBank =  (nextBank + 1 + (rngIntGenerator() % (systemConfig.getBankCount() - 1))) % systemConfig.getBankCount();
		}
		// else leave it as is

		unsigned nextRow = channel[nextChannel].getRank(nextRank).bank[nextBank].getOpenRowID();

		if (rowLocality < rngGenerator())
		{
			// choose a new row that's not this one
			nextRow = (nextRow + 1 + (rngIntGenerator() % (systemConfig.getRowCount() - 1))) % systemConfig.getRowCount();
		}
		// else leave it as is

		Transaction::TransactionType nextType = (readPercentage > rngGenerator()) ? Transaction::READ_TRANSACTION : Transaction::WRITE_TRANSACTION;

		unsigned burstLength = (shortBurstRatio > rngGenerator()) ? 4 : 8;

		unsigned nextColumn = rngIntGenerator() & (systemConfig.getColumnCount() - 1);

		assert(arrivalThreshold <= 1.0F);

		unsigned nextTime = 0;

		switch (interarrivalDistributionModel)
		{
		case GAUSSIAN_DISTRIBUTION:
			while (true)
			{
				if (arrivalThreshold < rngGenerator()) // interarrival probability function
				{
					arrivalThreshold = 1.0F - (1.0F / abs(boxMuller(averageInterarrivalCycleCount, 10)));
					assert(arrivalThreshold <= 1.0F);
					break;
				}
				else
				{
					nextTime++;
				}
			}
			break;
		case POISSON_DISTRIBUTION:
			while (true)
			{
				if (arrivalThreshold < rngGenerator()) // interarrival probability function
				{
					arrivalThreshold = 1.0F - (1.0F / Poisson(averageInterarrivalCycleCount));
					assert(arrivalThreshold <= 1.0F);
					break;
				}
				else
				{
					nextTime++;
				}
			}
			break;
		case UNIFORM_DISTRIBUTION:
		case LOGNORMAL_DISTRIBUTION:
		case NORMAL_DISTRIBUTION:
			nextTime = (int)std::max(0.0,arrivalGenerator());
			break;

		}


		// set arrival time

		static Address nextAddress;

		time += nextTime;

		nextAddress.setAddress(nextChannel,nextRank,nextBank,nextRow,nextColumn);

		return new Transaction(nextType, time, burstLength, nextAddress, 0, 0);
	}
	else
		return NULL;
}




ostream& DRAMsimII::operator<<(ostream& os, const InputStream& is)
{
	os << "type[" << is.type << "]" << endl;
	os << is.systemConfig << endl;
	for (vector<Channel>::const_iterator i = is.channel.begin(); i != is.channel.end(); i++)
	{
		os << *i << endl;
	}
	os << is.channelLocality << " " << is.rankLocality << " " << is.bankLocality << " " <<
		is.time << " " << is.rowLocality << " " << is.readPercentage << " " << is.shortBurstRatio <<
		is.arrivalThreshold << " " << is.arrivalThreshold << " " << is.cpuToMemoryRatio << " " <<
		is.averageInterarrivalCycleCount << " " << is.traceFilename << endl;

	return os;
}
