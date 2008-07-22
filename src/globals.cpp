#include <boost/iostreams/filtering_stream.hpp>

#include "globals.h"

using boost::iostreams::filtering_ostream;

// modified, writes to cerr or a compressed output file
filtering_ostream DRAMSimII::timingOutStream;
filtering_ostream DRAMSimII::powerOutStream;
filtering_ostream DRAMSimII::statsOutStream;


