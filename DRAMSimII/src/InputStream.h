#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H
#pragma once

#include <fstream>
#include <map>

#include "enumTypes.h"
#include "busEvent.h"
#include "globals.h"
#include "dramSettings.h"

namespace DRAMSimII
{
	class inputStream
	{
	private:
		
		input_type_t type; // trace type or random number generator
		float chan_locality;
		float rank_locality;
		float bank_locality;
		tick_t time; // time reported by trace or recorded by random number generator
		float row_locality; 
		float read_percentage; // the percentage of accesses that are reads. should replace with access_distribution[]
		float short_burst_ratio; // long burst or short burst?
		float arrival_thresh_hold;
		int average_interarrival_cycle_count; // used by random number generator
		distribution_type_t interarrival_distribution_model;

		//enum input_status_t get_next_input_transaction(transaction*);
		std::ifstream trace_file;

	public: 
		// constructors	
		inputStream(const dramSettings& settings);
		

		// functions
		input_type_t toInputToken(const std::string&) const;
		float box_muller(float, float) const;
		float poisson_rng (float) const;
		float gammaln(float) const;
		input_status_t getNextBusEvent(busEvent &);

		// accessors
		input_type_t getType() const { return type; }
		float getChannelLocality() const { return chan_locality; }
		float getRankLocality() const { return rank_locality; }
		float getBankLocality() const { return bank_locality; }
		float getRowLocality() const { return row_locality; }
		float getReadPercentage() const { return read_percentage; }
		float getShortBurstRatio() const { return short_burst_ratio; }
		float getArrivalThreshhold() const { return arrival_thresh_hold; }
		distribution_type_t getInterarrivalDistributionModel() const { return interarrival_distribution_model; }
		float getAverageInterarrivalCycleCount() const { return average_interarrival_cycle_count; }
		tick_t getTime() const { return time; }

		// mutators
		void setTime(tick_t _time) { time = _time; }
		void setArrivalThreshhold(float _arrivalThreshhold) { arrival_thresh_hold = _arrivalThreshhold; }
	};
}
#endif
