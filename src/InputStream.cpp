#define _CRT_RAND_S

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>

#include "InputStream.h"

using std::vector;
using std::dec;
using std::hex;
using std::cerr;
using std::endl;
using std::string;
using std::ostream;
using namespace DRAMSimII;

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
cpuToMemoryRatio(settings.cpuToMemoryClockRatio),
averageInterarrivalCycleCount(10),
interarrivalDistributionModel(settings.arrivalDistributionModel),
traceFilename(settings.inFile),
randomNumberGenerator(std::time(0)),
rngDistributionModel(0,1),
rngIntDistributionModel(0u,16383u),
rngGenerator(randomNumberGenerator, rngDistributionModel),
rngIntGenerator(randomNumberGenerator, rngIntDistributionModel)
{
	if (interarrivalDistributionModel == UNIFORM_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / (double)averageInterarrivalCycleCount);
	else if (interarrivalDistributionModel == GAUSSIAN_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / boxMuller((double)averageInterarrivalCycleCount, 10));
	if (arrivalThreshold > 1.0F)
		arrivalThreshold = 1.0F / arrivalThreshold;

	if (!settings.inFile.empty())
	{
		traceFile.open(settings.inFile.c_str());

		if (!traceFile.good())
		{
			string inFileWithPath = "./traceFiles/" + settings.inFile;
			traceFile.open(inFileWithPath.c_str());
			if (!traceFile.good())
			{
				cerr << "Unable to open trace file \"" << settings.inFile << "\"" << endl;
				exit(-9);
			}
		}
	}
}

InputStream::InputStream(const SystemConfiguration &systemConfig, const vector<Channel> &systemChannel, DistributionType arrivalDistributionModel, string filename):
systemConfig(systemConfig),
channel(systemChannel),
interarrivalDistributionModel(arrivalDistributionModel),
randomNumberGenerator(std::time(0)),
rngDistributionModel(0,1),
rngIntDistributionModel(0u,16383u),
rngGenerator(randomNumberGenerator, rngDistributionModel),
rngIntGenerator(randomNumberGenerator, rngIntDistributionModel)
{
	if (!filename.empty())
	{
		traceFile.open(filename.c_str());

		if (!traceFile.good())
		{
			string inFileWithPath = "./traceFiles/" + filename;
			traceFile.open(inFileWithPath.c_str());
			if (!traceFile.good())
			{
				cerr << "Unable to reopen trace file \"" << filename << "\"" << endl;
				exit(-9);
			}
		}
	}
}

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
		if (xm != oldm) 
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
		if (xm != oldm)
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
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = true;
	}
	
	return floor(m + y1 * s);
}


bool InputStream::getNextBusEvent(BusEvent &this_e)
{	
	enum FileIOToken control;	
	string input;	

	switch (type)
	{
	case K6_TRACE:	
		{
			//int base_control;
			enum TransactionType attributes;
			int burst_length = 4; // Socket 7 cachelines are 32 byte long, burst of 4
			int burst_count;
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

				control = Settings::dramTokenizer(input);

				if (control == unknown_token)
				{
					cerr << "Unknown Token Found" << input << endl;
					return false;
				}

				if (control == MEM_WR)
				{
					attributes = WRITE_TRANSACTION;
				}
				else
				{
					attributes = READ_TRANSACTION;
				}

				traceFile >> input;
				if(traceFile.eof())
				{
					cerr << "Unexpected EOF, Please fix input trace file" << endl;
					return false;
				}
				if((this_e.attributes != control) || 
					(((this_e.address.getPhysicalAddress() ^ address) & 0xFFFFFFE0) != 0) || (burst_count == burst_length))
				{
					bursting = false;
					timestamp = static_cast<tick>(static_cast<double>(timestamp) * ascii2multiplier(input));
					this_e.address.setPhysicalAddress(0x3FFFFFFF & address); // mask out top addr bit
					this_e.attributes = CONTROL_TRANSACTION;
					this_e.timestamp = timestamp;
					burst_count = 1;
				}
				else
				{
					burst_count++;
				}
			}
			this_e.address.setPhysicalAddress(address);
			this_e.timestamp = timestamp;
		} 
		break;
	case MASE_TRACE:
		{
			PHYSICAL_ADDRESS tempPA;
			traceFile >> hex >> tempPA >> input >> dec >> this_e.timestamp;
			this_e.address.setPhysicalAddress(tempPA);

			//this_e.timestamp /= 40000;
			this_e.timestamp /= cpuToMemoryRatio;
			if(!traceFile.good()) /// found starting Hex address 
			{
				cerr << "Unexpected EOF, Please fix input trace file" << endl;
				return false;
			}

			control = Settings::dramTokenizer(input);

			switch (control)
			{
			case unknown_token:
				cerr << "Unknown Token Found " << input << endl;
				return false;
				break;
			case FETCH:
				this_e.attributes = IFETCH_TRANSACTION;
				break;
			case MEM_RD:
				this_e.attributes = READ_TRANSACTION;
				break;
			case MEM_WR:
				this_e.attributes = WRITE_TRANSACTION;
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

			traceFile >> dec >> this_e.timestamp >> input >> dec >> channel >> rank >> bank >> row >> column;
			this_e.address.setChannel(channel);
			this_e.address.setRank(rank);
			this_e.address.setBank(bank);
			this_e.address.setRow(row);
			this_e.address.setColumn(column);

			if(!traceFile.good()) /// found starting Hex address 
			{
				cerr << "Unexpected EOF, Please fix input trace file" << endl;
				return false;
			}

			control = Settings::dramTokenizer(input);

			switch (control)
			{
			case unknown_token:
				cerr << "Unknown Token Found " << input << endl;
				return false;
				break;
			case FETCH:
				this_e.attributes = IFETCH_TRANSACTION;
				break;
			case MEM_RD:
				this_e.attributes = READ_TRANSACTION;
				break;
			case MEM_WR:
				this_e.attributes = WRITE_TRANSACTION;
				break;
			default:
				cerr << "Unexpected transaction type: " << input;
				exit(-7);
				break;
			}
		}
		break;
	default:
		break;
	}

	//this_e.attributes = CONTROL_TRANSACTION;

	return true;
}

enum InputType InputStream::toInputToken(const string &input) const
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
/// @brief get a random request
/// @details create a random transaction according to the parameters in systemConfig
/// use the probabilities specified to generate a mapped request
/// @author Joe Gross
/// @return a pointer to the transaction that was generated
//////////////////////////////////////////////////////////////////////
#if 0
Transaction *InputStream::getNextRandomRequest()
{
	if (type == RANDOM)
	{
		unsigned int randVar;

		static Address nextAddress;

		//Transaction *thisTransaction = new Transaction();

		rand_s(&randVar);

		static unsigned nextChannel;

		// check against last transaction to see what the chan_id was, and whether we need to change channels or not
		// choose a random channel that's not this one
		if (channelLocality * UINT_MAX < randVar)
		{
			nextChannel = (nextChannel + (randVar % (systemConfig.getChannelCount() - 1))) % systemConfig.getChannelCount();
		}
		// choose a random channel from any of them
		else
		{
			nextChannel = randVar % systemConfig.getChannelCount();
		}

		// check against the rank_id of the last transaction to the newly selected channel to see if we need to change the rank_id
		// or keep to this rank_id 
		unsigned nextRank = channel[nextChannel].getLastRankID();

		rand_s(&randVar);

		if (rankLocality * UINT_MAX < randVar)
		{
			// choose a rank that's not this one
			nextRank = (nextRank + 1 + (randVar % (systemConfig.getRankCount() - 1))) % systemConfig.getRankCount();
		}
		// else choose the same rank again
		
		unsigned nextBank = channel[nextChannel].getRank(nextRank).getLastBankID();

		rand_s(&randVar);

		if (bankLocality * UINT_MAX < randVar)
		{
			// choose a new bank that's not this one
			nextBank =  (nextBank + 1 + (randVar % (systemConfig.getBankCount() - 1))) % systemConfig.getBankCount();
		}
		// else leave it as is

		unsigned nextRow = channel[nextChannel].getRank(nextRank).bank[nextBank].getOpenRowID();

		rand_s(&randVar);

		if (rowLocality * UINT_MAX < randVar)
		{
			// choose a new row that's not this one
			nextRow = (nextRow + 1 + (randVar % (systemConfig.getRowCount() - 1))) % systemConfig.getRowCount();
		}
		// else leave it as is

		TransactionType type;

		rand_s(&randVar);

		if (readPercentage * UINT_MAX > randVar)
		{
			type = READ_TRANSACTION;
		}
		else
		{
			type = WRITE_TRANSACTION;
		}

		rand_s(&randVar);

		unsigned burstLength = shortBurstRatio * UINT_MAX > randVar ? 4 : 8;

		unsigned nextColumn = randVar & systemConfig.getColumnCount();

		while (true)
		{
			rand_s(&randVar);

			if (arrivalThreshold * UINT_MAX < randVar) // interarrival probability function
			{

				// Gaussian distribution function
				if (interarrivalDistributionModel == GAUSSIAN_DISTRIBUTION)
				{
					arrivalThreshold = 1.0F - (1.0F / abs(boxMuller(averageInterarrivalCycleCount, 10)));
				}
				// Poisson distribution function
				else if (interarrivalDistributionModel == POISSON_DISTRIBUTION)
				{
					arrivalThreshold = 1.0F - (1.0F / Poisson(averageInterarrivalCycleCount));
				}
				break;
			}
			else
			{
				time = time + 1;
			}
		}

		// set arrival time

		static tick oldTime;
		cerr << time - oldTime << endl;
		oldTime = time;

		nextAddress.channel = nextChannel;
		nextAddress.rank = nextRank;
		nextAddress.bank = nextBank;
		nextAddress.row = nextRow;
		nextAddress.column = nextColumn;
		return new Transaction(type,time, burstLength, nextAddress, NULL);
	}
	else
		return NULL;
}
#endif

Transaction *InputStream::getNextRandomRequest()
{
	if (type == RANDOM)
	{
		float randVar;

		static Address nextAddress;

		//Transaction *thisTransaction = new Transaction();

		randVar = rngGenerator();

		static unsigned nextChannel;

		// check against last transaction to see what the chan_id was, and whether we need to change channels or not
		// choose a random channel that's not this one
		if (channelLocality < randVar)
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

		randVar = rngGenerator();

		if (rankLocality < randVar)
		{
			// choose a rank that's not this one
			nextRank = (nextRank + 1 + (rngIntGenerator() % (systemConfig.getRankCount() - 1))) % systemConfig.getRankCount();
		}
		// else choose the same rank again

		unsigned nextBank = channel[nextChannel].getRank(nextRank).getLastBankID();

		randVar = rngGenerator();

		if (bankLocality < randVar)
		{
			// choose a new bank that's not this one
			nextBank =  (nextBank + 1 + (rngIntGenerator() % (systemConfig.getBankCount() - 1))) % systemConfig.getBankCount();
		}
		// else leave it as is

		unsigned nextRow = channel[nextChannel].getRank(nextRank).bank[nextBank].getOpenRowID();

		randVar = rngGenerator();

		if (rowLocality < randVar)
		{
			// choose a new row that's not this one
			nextRow = (nextRow + 1 + (rngIntGenerator() % (systemConfig.getRowCount() - 1))) % systemConfig.getRowCount();
		}
		// else leave it as is

		TransactionType type;

		randVar = rngGenerator();

		if (readPercentage > randVar)
		{
			type = READ_TRANSACTION;
		}
		else
		{
			type = WRITE_TRANSACTION;
		}

		randVar = rngGenerator();

		unsigned burstLength = shortBurstRatio > randVar ? 4 : 8;

		unsigned nextColumn = rngIntGenerator() & systemConfig.getColumnCount();

		assert(arrivalThreshold <= 1.0F);

		while (true)
		{
			randVar = rngGenerator();

			if (arrivalThreshold < randVar) // interarrival probability function
			{

				// Gaussian distribution function
				if (interarrivalDistributionModel == GAUSSIAN_DISTRIBUTION)
				{
					arrivalThreshold = 1.0F - (1.0F / abs(boxMuller(averageInterarrivalCycleCount, 10)));
					assert(arrivalThreshold <= 1.0F);
				}
				// Poisson distribution function
				else if (interarrivalDistributionModel == POISSON_DISTRIBUTION)
				{
					arrivalThreshold = 1.0F - (1.0F / Poisson(averageInterarrivalCycleCount));
					assert(arrivalThreshold <= 1.0F);
				}
				break;
			}
			else
			{
				time = time + 1;
			}
		}

		// set arrival time

		static tick oldTime;
		//cerr << time - oldTime << endl;
		oldTime = time;

		nextAddress.setChannel(nextChannel);
		nextAddress.setRank(nextRank);
		nextAddress.setBank(nextBank);
		nextAddress.setRow(nextRow);
		nextAddress.setColumn(nextColumn);

		return new Transaction(type,time, burstLength, nextAddress, NULL);
	}
	else
		return NULL;
}




#define COMPRESS_INCOMING_TRANSACTIONS 8

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
				Transaction *tempTransaction = new Transaction(newEvent.attributes,newEvent.timestamp >> COMPRESS_INCOMING_TRANSACTIONS,0,newEvent.address,NULL) ;
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

ostream& DRAMSimII::operator<<(ostream& os, const InputStream& is)
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

bool InputStream::operator==(const InputStream& rhs) const
{
	return type == rhs.type && systemConfig == rhs.systemConfig && channel == rhs.channel &&
		channelLocality == rhs.channelLocality && rankLocality == rhs.rankLocality && bankLocality == rhs.bankLocality &&
		time == rhs.time && rowLocality == rhs.rowLocality && readPercentage == rhs.readPercentage &&
		shortBurstRatio == rhs.shortBurstRatio && arrivalThreshold == rhs.arrivalThreshold &&
		cpuToMemoryRatio == rhs.cpuToMemoryRatio && averageInterarrivalCycleCount == rhs.averageInterarrivalCycleCount &&
		interarrivalDistributionModel == rhs.interarrivalDistributionModel;
}

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
rngGenerator(rhs.rngGenerator),
rngIntGenerator(rhs.rngIntGenerator)
{}

InputStream::InputStream(const InputStream& rhs, const SystemConfiguration &systemConfigVal, const vector<Channel> &systemChannel):
type(rhs.type),
systemConfig(systemConfig),
channel(channel),
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
rngGenerator(rhs.rngGenerator),
rngIntGenerator(rhs.rngIntGenerator)
{}
