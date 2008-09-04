#include <boost/iostreams/filtering_stream.hpp>

#include "globals.h"
#include "transaction.h"
#include "Address.h"
#include "queue.h"
#include "System.h"
#include "Settings.h"
#include "SystemConfiguration.h"

#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>

using boost::iostreams::filtering_ostream;

// modified, writes to cerr or a compressed output file
filtering_ostream DRAMSimII::timingOutStream;
filtering_ostream DRAMSimII::powerOutStream;
filtering_ostream DRAMSimII::statsOutStream;
