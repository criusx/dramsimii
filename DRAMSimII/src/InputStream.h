#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H
#pragma once

#include <fstream>
#include <map>

#include "enumTypes.h"
#include "busevent.h"
#include "globals.h"
#include "dramSettings.h"

class inputStream
{
private:
	input_type_t input_token(const std::string&) const;
	input_type_t type; // trace type or random number generator
	double chan_locality;
	double rank_locality;
	double bank_locality;
	tick_t time; // time reported by trace or recorded by random number generator
	double row_locality; 
	double read_percentage; // the percentage of accesses that are reads. should replace with access_distribution[]
	double short_burst_ratio; // long burst or short burst?
	double arrival_thresh_hold;
	int average_interarrival_cycle_count; // used by random number generator
	distribution_type_t interarrival_distribution_model;
	double box_muller(double, double) const;
	double poisson_rng (double) const;
	double gammaln(double) const;
	enum input_status_t get_next_bus_event(busEvent &);
	//enum input_status_t get_next_input_transaction(transaction*);
	std::ifstream trace_file;

public: 
	// constructors	
	inputStream(const dramSettings *settings);
	inline input_type_t getType() { return type; }

	//friends
	friend class dramSystem;
};

#endif
