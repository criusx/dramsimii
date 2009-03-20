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

#ifndef BUSEVENT_H
#define BUSEVENT_H
#pragma once

#include "globals.h"
#include "Address.h"
#include "transaction.h"

namespace DRAMsimII
{
	// bus_event_t is used with the file I/O interface to get bus events from input trace files

	/// @brief represents an event that has happened on the memory bus
	class BusEvent /* 6 DWord per event */
	{
	public:
		Transaction::TransactionType attributes;
		// read/write/Fetch type stuff. Not overloaded with other stuff
		Address address; // assume to be <= 32 bits for now
		tick timestamp; // time stamp will now have a large dynamic range, but only 53 bit precision
		BusEvent();
	};
}

#endif
