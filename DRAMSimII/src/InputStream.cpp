#define _CRT_RAND_S

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>

#include "InputStream.h"

using namespace std;
using namespace DRAMSimII;

InputStream::InputStream(const Settings& settings):
type(settings.inFileType),
channelLocality(1 / static_cast<double>(settings.channelCount)),
rankLocality(1 / settings.rankCount),
BankLocality(1 / settings.bankCount),
time(0),
rowLocality(0.2f),
readPercentage(settings.readPercentage),
shortBurstRatio(settings.shortBurstRatio),
arrivalThreshold(0.0F),
cpuToMemoryRatio(settings.cpuToMemoryClockRatio),
averageInterarrivalCycleCount(10),
interarrivalDistributionModel(UNIFORM_DISTRIBUTION)
{
	if (interarrivalDistributionModel == UNIFORM_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / (double)averageInterarrivalCycleCount);
	else if (interarrivalDistributionModel == GAUSSIAN_DISTRIBUTION)
		arrivalThreshold = 1.0 - (1.0 / boxMuller((double)averageInterarrivalCycleCount, 10));

	if (settings.inFile.length() > 2)
	{
		string inFileWithPath = "./traceFiles/" + settings.inFile;
		traceFile.open(inFileWithPath.c_str());
		if (!traceFile.good())
		{
			cerr << "Unable to open trace file \"" << settings.inFile << "\"" << endl;
			exit(-9);
		}
	}
	srand48((long) (1010321 + 9763099));
}

//////////////////////////////////////////////////////////////////////
/// @brief generates a number using a Poisson random variable
/// @details from the book "Numerical Recipes in C: The Art of Scientific Computing"
/// @param xm the average inter arrival cycle count
/// @return the time until the next transaction arrives
//////////////////////////////////////////////////////////////////////
float InputStream::Poisson (float xm) const
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
			unsigned int j;
			rand_s(&j);
			t *= (float)j/(float)UINT_MAX;
		} while (t > g);		
	}
	else
	{
		if (xm != oldm)
		{
			oldm = xm;
			sq = sqrt(2.0 * xm);
			alxm = log(xm);
			g = xm * alxm-gammaLn(xm + 1.0);
		}
		unsigned int j;
		do
		{
			do
			{
				rand_s(&j);
				y = tan(PI * (float)j/(float)UINT_MAX);
				em = sq * y + xm;

			} while (em < 0.0);
			em = floor(em);
			t = 0.9 * (1.0 + y*y) * exp(em * alxm - gammaLn(em+1.0) -g);
			rand_s(&j);
		} while ((float)j/(float)UINT_MAX > t);
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
			unsigned int j;
			rand_s(&j);
			x1 = 2.0 * (float)j/(float)UINT_MAX - 1.0;
			rand_s(&j);
			x2 = 2.0 * (float)j/(float)UINT_MAX - 1.0;
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

			while((bursting == true) && traceFile.good())
			{
				traceFile >> std::hex >> address >> input >> timestamp;

				if(traceFile.good())
				{
					cout << "Unexpected EOF, Please fix input trace file" << endl;
					return false;
				}

				control = Settings::dramTokenizer(input);

				if(control == unknown_token)
				{
					cout << "Unknown Token Found" << input << endl;
					return false;
				}

				if(control == MEM_WR)
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
					cout << "Unexpected EOF, Please fix input trace file" << endl;
					return false;
				}
				if((this_e.attributes != control) || 
					(((this_e.address.physicalAddress ^ address) & 0xFFFFFFE0) != 0) || (burst_count == burst_length))
				{
					bursting = false;
					timestamp = static_cast<tick>(static_cast<double>(timestamp) * ascii2multiplier(input));
					this_e.address.physicalAddress = 0x3FFFFFFF & address; // mask out top addr bit
					this_e.attributes = CONTROL_TRANSACTION;
					this_e.timestamp = timestamp;
					burst_count = 1;
				}
				else
				{
					burst_count++;
				}
			}
			this_e.address.physicalAddress = address;
			this_e.timestamp = timestamp;
		} 
		break;
	case MASE_TRACE:
		{
			traceFile >> std::hex >> this_e.address.physicalAddress >> input >> std::dec >> this_e.timestamp;

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
			traceFile >> std::dec >> this_e.timestamp >> input >> std::dec >> this_e.address.channel >> this_e.address.rank >> this_e.address.bank >> this_e.address.row >> this_e.address.column;

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
