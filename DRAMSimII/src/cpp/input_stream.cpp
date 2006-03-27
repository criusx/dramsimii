#define _CRT_RAND_S

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>

#include "dramsim2.h"

input_stream_c::input_stream_c(map<file_io_token_t,string> &parameter)
{
	type = RANDOM;
	time = 0;
	row_locality = 0.2;
	average_interarrival_cycle_count = 10;
	interarrival_distribution_model = UNIFORM_DISTRIBUTION;

	if (interarrival_distribution_model == UNIFORM_DISTRIBUTION)
		arrival_thresh_hold = 1.0 - (1.0 / (double)average_interarrival_cycle_count);
	else if (interarrival_distribution_model == GAUSSIAN_DISTRIBUTION)
		arrival_thresh_hold = 1.0 - (1.0 / box_muller((double)average_interarrival_cycle_count, 10));


	map<file_io_token_t, string>::iterator temp;
	stringstream temp2;

	if ((temp=parameter.find(input_type_token))!=parameter.end())
	{
		enum input_type_t type=input_token(temp->second);
		if ((type != RANDOM) && (type != K6_TRACE) && (type != MASE_TRACE))
			cerr << "Unknown input type" << temp2;
	}

	if ((temp=parameter.find(trace_file_token))!=parameter.end())
	{
		if (temp->second.length() > 0)
		{
			trace_file.open(temp->second.c_str(), ifstream::in);
			if(!trace_file.is_open()) {
				cerr << "Error in opening trace file " << temp2 << endl;
				_exit(3);
			}
		}
		else
			_exit(6);
	}
	else if (type != RANDOM)
		_exit(7);

	if ((temp=parameter.find(chan_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		double temp3;
		temp2 >> temp3;
		chan_locality = 1 / temp3;
	}
	else
		_exit(5);

	if ((temp=parameter.find(read_percentage_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> read_percentage;
	}
	else
		read_percentage = 0.5; /// set this to a fixed value for now, FIXME

	if ((temp=parameter.find(short_burst_ratio_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		temp2 >> short_burst_ratio;
	}
	else
		short_burst_ratio = 0.0;


	if ((temp=parameter.find(rank_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		double temp3;
		temp2 >> temp3;
		rank_locality = 1 / temp3;
	}
	else
		_exit(8);

	if ((temp=parameter.find(bank_count_token))!=parameter.end())
	{
		stringstream temp2(temp->second);
		double temp3;
		temp2 >> temp3;
		bank_locality = 1 / temp3;
	}
	else
		_exit(9);

	srand48((long) (1010321 + 9763099));
}


enum input_type_t input_stream_c::get_type()
{
	return type;
}


///
/// from the book "Numerical Recipes in C: The Art of Scientific Computing"///

double input_stream_c::poisson_rng (double xm)
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

double input_stream_c::gammaln(const double xx) const
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

double input_stream_c::box_muller(double m, double s) {
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


enum input_status_t input_stream_c::get_next_bus_event(bus_event &this_e)
{
	int		address;
	enum file_io_token_t control;
	int base_control;
	enum transaction_type_t attributes;
	int burst_length = 4; /* Socket 7 cachelines are 32 byte long, burst of 4 */
	int burst_count;
	bool bursting = true;
	string input;
	//char		input[1024];
	bool EOF_reached = false;
	tick_t timestamp;
	double multiplier;

	if(type == K6_TRACE)
	{
		while((bursting == true) && (EOF_reached == false))
		{
			trace_file >> address;
			if(trace_file.eof()) /// found starting hex address
			{ 
				cout << "EOF Reached" << endl;
				return FAILURE;
			}

			trace_file >> input;

			if(trace_file.eof()) 
			{
				cout << "Unexpected EOF, Please fix input trace file" << endl;
				return FAILURE;
			}

			control = file_io_token(input);
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

			trace_file >> timestamp;
			if(trace_file.eof()) 
			{
				cout << "Unexpected EOF, Please fix input trace file" << endl;
				return FAILURE;
			}
			trace_file >> input;
			if(trace_file.eof())
			{
				cout << "Unexpected EOF, Please fix input trace file" << endl;
				return FAILURE;
			}
			if((this_e.attributes != control) || 
				(((this_e.address ^ address) & 0xFFFFFFE0) != 0) ||
				(burst_count == burst_length))
			{
				bursting = false;
				timestamp = timestamp * ascii2multiplier(input);
				this_e.address 	= 0x3FFFFFFF & address;		/* mask out top addr bit */
				this_e.attributes 	= CONTROL_TRANSACTION;
				this_e.timestamp 	= timestamp;
				burst_count 		= 1;
			}
			else
			{
				burst_count++;
			}
		}
	} 
	else if (type == MASE_TRACE)
	{
		trace_file >> address >> input >> timestamp;
		control = file_io_token(input);
		if(trace_file.eof()) /// found starting Hex address 
		{
			cout << "Unexpected EOF, Please fix input trace file" << endl;
			return FAILURE;
		}
		if(control == unknown_token) 
		{
			cout << "Unknown Token Found " << input << endl;
			return FAILURE;
		}
	}
	this_e.address = address;
	this_e.attributes = CONTROL_TRANSACTION;
	this_e.timestamp = timestamp;
	return SUCCESS;
}

enum input_type_t input_stream_c::input_token(const string &input) const
{
	if (input == "k6" || input == "K6")
		return K6_TRACE;
	else if (input == "mase" || input == "MASE" || input == "Mase")
		return MASE_TRACE;
	else if (input == "random" || input == "RANDOM" || input == "Random")
		return RANDOM;
	else
		return UNKNOWN;
}
