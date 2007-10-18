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

inputStream::inputStream(const dramSettings *settings):
type(settings->inFileType),
time(0),
row_locality(0.2f),
average_interarrival_cycle_count(10),
interarrival_distribution_model(UNIFORM_DISTRIBUTION),
chan_locality(1 / static_cast<double>(settings->channelCount)),
read_percentage(settings->readPercentage),
short_burst_ratio(settings->shortBurstRatio),
rank_locality(1 / settings->rankCount),
bank_locality(1 / settings->bankCount)
{
	if (interarrival_distribution_model == UNIFORM_DISTRIBUTION)
		arrival_thresh_hold = 1.0 - (1.0 / (double)average_interarrival_cycle_count);
	else if (interarrival_distribution_model == GAUSSIAN_DISTRIBUTION)
		arrival_thresh_hold = 1.0 - (1.0 / box_muller((double)average_interarrival_cycle_count, 10));

	if (settings->inFile.length() > 2)
	{
		string inFileWithPath = "./traceFiles/" + settings->inFile;
		trace_file.open(inFileWithPath.c_str());
		if (!trace_file.good())
		{
			cerr << "Unable to open trace file \"" << settings->inFile << "\"" << endl;
			exit(-9);
		}
	}
	srand48((long) (1010321 + 9763099));
}

///
/// from the book "Numerical Recipes in C: The Art of Scientific Computing"///

float inputStream::poisson_rng (float xm) const
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
			g = xm * alxm-gammaln(xm + 1.0);
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
			t = 0.9 * (1.0 + y*y) * exp(em * alxm - gammaln(em+1.0) -g);
			rand_s(&j);
		} while ((float)j/(float)UINT_MAX > t);
	}
	return em;
}

float inputStream::gammaln(const float xx) const
{
	static float cof[6] = 
	{76.18009172947146F, -86.50532032941677F,
	24.01409824083091F, -1.231739572450155F,
	0.1208650973866179e-2F, -0.5395239384953e-5F};

	float y = xx;
	float x = xx;
	float tmp = x + 5.5;
	tmp -= (x+0.5) * log(tmp);
	float ser = 1.000000000190015F;
	for (int j=0; j<=5; j++)
		ser += cof[j]/++y;
	return -tmp + log(2.5066282746310005F * ser/x);

}

float inputStream::box_muller(const float m, const float s) const
{
	float w, y1;
	static float y2;
	static bool use_last = false;

	if (use_last)		        /// use value from previous call
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


enum input_status_t inputStream::getNextBusEvent(busEvent &this_e)
{	
	enum file_io_token_t control;	
	string input;	

	if(type == K6_TRACE)
	{
		//int base_control;
		enum transaction_type_t attributes;
		int burst_length = 4; // Socket 7 cachelines are 32 byte long, burst of 4
		int burst_count;
		bool bursting = true;
		//double multiplier;
		tick_t timestamp = TICK_T_MAX;
		unsigned address = UINT_MAX;

		while((bursting == true) && trace_file.good())
		{
			trace_file >> std::hex >> address >> input >> timestamp;

			if(trace_file.good())
			{
				cout << "Unexpected EOF, Please fix input trace file" << endl;
				return FAILURE;
			}

			control = dramSettings::dramTokenizer(input);

			if(control == unknown_token)
			{
				cout << "Unknown Token Found" << input << endl;
				return FAILURE;
			}

			if(control == MEM_WR)
			{
				attributes = WRITE_TRANSACTION;
			}
			else
			{
				attributes = READ_TRANSACTION;
			}

			trace_file >> input;
			if(trace_file.eof())
			{
				cout << "Unexpected EOF, Please fix input trace file" << endl;
				return FAILURE;
			}
			if((this_e.attributes != control) || 
				(((this_e.address.phys_addr ^ address) & 0xFFFFFFE0) != 0) || (burst_count == burst_length))
			{
				bursting = false;
				timestamp = static_cast<tick_t>(static_cast<double>(timestamp) * ascii2multiplier(input));
				this_e.address.phys_addr = 0x3FFFFFFF & address; // mask out top addr bit
				this_e.attributes = CONTROL_TRANSACTION;
				this_e.timestamp = timestamp;
				burst_count = 1;
			}
			else
			{
				burst_count++;
			}
		}
		this_e.address.phys_addr = address;
		this_e.timestamp = timestamp;
	} 
	else if (type == MASE_TRACE)
	{
		trace_file >> std::hex >> this_e.address.phys_addr >> input >> std::dec >> this_e.timestamp;

		if(!trace_file.good()) /// found starting Hex address 
		{
			cerr << "Unexpected EOF, Please fix input trace file" << endl;
			return FAILURE;
		}

		control = dramSettings::dramTokenizer(input);

		switch (control)
		{
		case unknown_token:
			cerr << "Unknown Token Found " << input << endl;
			return FAILURE;
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
	else if (type == MAPPED)
	{
		trace_file >> std::dec >> this_e.timestamp >> input >> std::dec >> this_e.address.chan_id >> this_e.address.rank_id >> this_e.address.bank_id >> this_e.address.row_id >> this_e.address.col_id;

		if(!trace_file.good()) /// found starting Hex address 
		{
			cerr << "Unexpected EOF, Please fix input trace file" << endl;
			return FAILURE;
		}

		control = dramSettings::dramTokenizer(input);

		switch (control)
		{
		case unknown_token:
			cerr << "Unknown Token Found " << input << endl;
			return FAILURE;
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

	//this_e.attributes = CONTROL_TRANSACTION;

	return SUCCESS;
}

enum input_type_t inputStream::input_token(const string &input) const
{
	if (input == "k6" || input == "K6")
		return K6_TRACE;
	else if (input == "mase" || input == "MASE" || input == "Mase")
		return MASE_TRACE;
	else if (input == "random" || input == "RANDOM" || input == "Random")
		return RANDOM;
	else if (input == "mapped" || input == "Mapped" || input == "MAPPED")
		return MAPPED;
	else
		return UNKNOWN;
}
