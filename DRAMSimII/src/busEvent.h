#ifndef BUSEVENT_H
#define BUSEVENT_H
#pragma once

#include "globals.h"
#include "Address.h"

namespace DRAMSimII
{
	// bus_event_t is used with the file I/O interface to get bus events from input trace files

	/// @brief represents an event that has happened on the memory bus
	class BusEvent /* 6 DWord per event */
	{
	public:
		TransactionType attributes;
		// read/write/Fetch type stuff. Not overloaded with other stuff
		Address address; // assume to be <= 32 bits for now
		tick timestamp; // time stamp will now have a large dynamic range, but only 53 bit precision
		BusEvent();
	};
}

#endif
