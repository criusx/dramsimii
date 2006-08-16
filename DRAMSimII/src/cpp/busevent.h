#ifndef BUSEVENT_H
#define BUSEVENT_H
#pragma once

#include "globals.h"

// bus_event_t is used with the file I/O interface to get bus events from input trace files

class busEvent /* 6 DWord per event */
{
public:
	enum transaction_type_t attributes;
	// read/write/Fetch type stuff. Not overloaded with other stuff
	addresses address; // assume to be <= 32 bits for now
	tick_t timestamp; // time stamp will now have a large dynamic range, but only 53 bit precision
	busEvent();
};

#endif
