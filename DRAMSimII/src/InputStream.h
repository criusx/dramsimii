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
	/// @brief creates transactions in standalone mode, whether random or from a trace file
	/// @author Joe Gross
	class InputStream
	{
	protected:
		
		InputType type;			///< trace type or random number generator
		float channelLocality;		///< channel locality for random generator
		float rankLocality;		///< rank locality for random generator
		float BankLocality;		///< bank locality for random generator
		tick time;				///< random number generator's simulated time
		float rowLocality;			///< row locality for random generator
		float readPercentage;		///< the percentage of accesses that are reads. should replace with access_distribution[]
		float shortBurstRatio;	///< long burst or short burst?
		float arrivalThreshold;
		float cpuToMemoryRatio;		///< the ratio of the cpu clock to the memory clock
		int averageInterarrivalCycleCount;	///< used by random number generator
		DistributionType interarrivalDistributionModel;	///< the model used to predict and determine when the next arrival will happen

		//enum input_status_t get_next_input_transaction(transaction*);
		std::ifstream trace_file;	///< the trace file that contains transactions, addresses and times

	public: 
		// constructors	
		InputStream(const Settings& settings);

		// functions
		InputType toInputToken(const std::string&) const;
		float boxMuller(float, float) const;
		float Poisson (float) const;
		float gammaLn(float) const;
		bool getNextBusEvent(busEvent &);

		// accessors
		InputType getType() const { return type; }
		float getChannelLocality() const { return channelLocality; }
		float getRankLocality() const { return rankLocality; }
		float getBankLocality() const { return BankLocality; }
		float getRowLocality() const { return rowLocality; }
		float getReadPercentage() const { return readPercentage; }
		float getShortBurstRatio() const { return shortBurstRatio; }
		float getArrivalThreshhold() const { return arrivalThreshold; }
		DistributionType getInterarrivalDistributionModel() const { return interarrivalDistributionModel; }
		float getAverageInterarrivalCycleCount() const { return averageInterarrivalCycleCount; }
		tick getTime() const { return time; }

		// mutators
		void setTime(tick _time) { time = _time; }
		void setArrivalThreshhold(float _arrivalThreshhold) { arrivalThreshold = _arrivalThreshhold; }
	};
}
#endif
