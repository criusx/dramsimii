#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H
#pragma once

#include <fstream>
#include <map>
#include <vector>
#include <boost/random.hpp>
#include <boost/random/variate_generator.hpp>

#include "enumTypes.h"
#include "busEvent.h"
#include "globals.h"
#include "Settings.h"
#include "SystemConfiguration.h"
#include "Channel.h"

namespace DRAMSimII
{
	/// @brief creates transactions in standalone mode, whether random or from a trace file
	class InputStream
	{
	protected:
		
		InputType type;								///< trace type or random number generator
		const SystemConfiguration& systemConfig;	///< reference to the system config to obtain specs
		const std::vector<Channel> &channel;
		float channelLocality;						///< channel locality for random generator
		float rankLocality;							///< rank locality for random generator
		float bankLocality;							///< bank locality for random generator
		tick time;									///< random number generator's simulated time
		float rowLocality;							///< row locality for random generator
		float readPercentage;						///< the percentage of accesses that are reads. should replace with access_distribution[]
		float shortBurstRatio;						///< long burst or short burst?
		float arrivalThreshold;
		float cpuToMemoryRatio;						///< the ratio of the cpu clock to the memory clock
		unsigned averageInterarrivalCycleCount;		///< used by random number generator
		DistributionType interarrivalDistributionModel;	///< the model used to predict and determine when the next arrival will happen

		//enum input_status_t get_next_input_transaction(transaction*);
		std::ifstream traceFile;
		boost::mt19937 randomNumberGenerator;
		boost::uniform_real<> rngDistributionModel;	///< generates a random distribution from the set of [0:1)
		boost::uniform_int<> rngIntDistributionModel; ///< generates a distribution of integers
		mutable boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rngGenerator;
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rngIntGenerator;

	public: 

		// constructors	
		InputStream(const Settings& settings, const SystemConfiguration &systemConfigVal, const std::vector<Channel> &systemChannel);

		// functions
		InputType toInputToken(const std::string&) const;
		float boxMuller(float, float) const;
		float Poisson (float) const;
		float gammaLn(float) const;
		bool getNextBusEvent(BusEvent &);
		Transaction *getNextIncomingTransaction();
		Transaction *getNextRandomRequest();
	protected:
		double ascii2multiplier(const std::string &input) const;
	public:

		// accessors
		InputType getType() const { return type; }
		float getChannelLocality() const { return channelLocality; }
		float getRankLocality() const { return rankLocality; }
		float getBankLocality() const { return bankLocality; }
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
