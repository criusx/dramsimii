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
