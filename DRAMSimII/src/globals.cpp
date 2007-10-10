#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>

#include "dramSystem.h"
#include "rank_c.h"
#include "globals.h"
#include "dramTimingSpecification.h"
#include "dramStatistics.h"


using namespace std;
using namespace DRAMSimII;

// modified, writes to cerr or a compressed output file
//boost::iostreams::filtering_ostream DRAMSimII::outStream;
boost::iostreams::filtering_ostream DRAMSimII::timingOutStream;
boost::iostreams::filtering_ostream DRAMSimII::powerOutStream;
boost::iostreams::filtering_ostream DRAMSimII::statsOutStream;

double DRAMSimII::ascii2multiplier(const string &input)
{
	switch(dramSettings::dramTokenizer(input))
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
