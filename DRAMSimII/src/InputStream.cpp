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

inputStream::inputStream(const dramSettings *settings):
type(settings->inFileType),
time(0),
row_locality(0.2),
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

inputStream::inputStream(map<file_io_token_t,string> &parameter):
type(RANDOM),
time(0),
row_locality(0.2),
average_interarrival_cycle_count(10),
interarrival_distribution_model(UNIFORM_DISTRIBUTION)
{
	if (interarrival_distribution_model == UNIFORM_DISTRIBUTION)
		arrival_thresh_hold = 1.0 - (1.0 / (double)average_interarrival_cycle_count);
	else if (interarrival_distribution_model == GAUSSIAN_DISTRIBUTION)
		arrival_thresh_hold = 1.0 - (1.0 / box_muller((double)average_interarrival_cycle_count, 10));


	map<file_io_token_t, string>::iterator temp;
	stringstream temp2;

	if ((temp=parameter.find(input_type_token))!=parameter.end())
	{
		type=input_token(temp->second);
		if ((type != RANDOM) && (type != K6_TRACE) && (type != MASE_TRACE) && (type != MAPPED))
			cerr << "Unknown input type" << temp2;
	}

	if ((temp=parameter.find(trace_file_token))!=parameter.end())
	{
		if (temp->second.length() > 0)
		{
			trace_file.open(temp->second.c_str(), ifstream::in);
			if(!trace_file.is_open()) 
			{
				cerr << "Error in opening trace file " << temp->second.c_str() << endl;
				exit(3);
			}
		}
		else
		{
			cerr << "Unable to open trace file";
			exit(6);
		}
	}
	else if (type != RANDOM)
		exit(7);

	if ((temp=parameter.find(chan_count_token))!=parameter.end())
	{
		toNumeric<double>(chan_locality,temp->second,std::dec);
		chan_locality = 1 / chan_locality;
	}
	else
		exit(5);

	if ((temp=parameter.find(read_percentage_token))!=parameter.end())
	{
		toNumeric<double>(read_percentage,temp->second,std::dec);
	}
	else
		read_percentage = 0.5; /// set this to a fixed value for now, FIXME

	if ((temp=parameter.find(short_burst_ratio_token))!=parameter.end())
	{
		toNumeric<double>(short_burst_ratio,temp->second,std::dec);
	}
	else
		short_burst_ratio = 0.0;


	if ((temp=parameter.find(rank_count_token))!=parameter.end())
	{
		toNumeric<double>(rank_locality,temp->second,std::dec);
		rank_locality = 1 / rank_locality;
	}
	else
		exit(8);

	if ((temp=parameter.find(bank_count_token))!=parameter.end())
	{
		toNumeric<double>(bank_locality,temp->second,std::dec);
		bank_locality = 1 / bank_locality;
	}
	else
		exit(9);

	srand48((long) (1010321 + 9763099));
}


///
/// from the book "Numerical Recipes in C: The Art of Scientific Computing"///

double inputStream::poisson_rng (double xm) const
{	
	static double sq, alxm, g, oldm = (-1.0);
	double em, t, y;

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
			t *= (double)j/(double)UINT_MAX;
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
				y = tan(PI * (double)j/(double)UINT_MAX);
				em = sq * y + xm;

			} while (em < 0.0);
			em = floor(em);
			t = 0.9 * (1.0 + y*y) * exp(em * alxm - gammaln(em+1.0) -g);
			rand_s(&j);
		} while ((double)j/(double)UINT_MAX > t);
	}
	return em;
}

double inputStream::gammaln(const double xx) const
{
	static double cof[6] = 
	{76.18009172947146, -86.50532032941677,
	24.01409824083091, -1.231739572450155,
	0.1208650973866179e-2, -0.5395239384953e-5};

	double y = xx;
	double const x = xx;
	double tmp = x + 5.5;
	tmp -= (x+0.5) * log(tmp);
	double ser = 1.000000000190015;
	for (int j=0; j<=5; j++)
		ser += cof[j]/++y;
	return -tmp + log(2.5066282746310005 * ser/x);

}

double inputStream::box_muller(double m, double s) const
{
	double w, y1;
	static double y2;
	static int use_last = 0;

	if (use_last)		        /// use value from previous call
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		double x1, x2;
		do 
		{
			unsigned int j;
			rand_s(&j);
			x1 = 2.0 * (double)j/(double)UINT_MAX - 1.0;
			rand_s(&j);
			x2 = 2.0 * (double)j/(double)UINT_MAX - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}
	return floor(m + y1 * s);
}


enum input_status_t inputStream::get_next_bus_event(busEvent &this_e)
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
		tick_t timestamp;
		unsigned address;

		while((bursting == true) && trace_file.good())
		{
			trace_file >> std::hex >> address >> input >> timestamp;

			if(trace_file.good())
			{
				cout << "Unexpected EOF, Please fix input trace file" << endl;
				return FAILURE;
			}

			control = dramTokenizer(input);

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

		control = dramTokenizer(input);

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

		control = dramTokenizer(input);

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
