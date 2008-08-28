#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H
#pragma once

#include "globals.h"
#include "busEvent.h"
#include "Settings.h"
#include "SystemConfiguration.h"
#include "Channel.h"

#include <fstream>
#include <map>
#include <vector>
#include <boost/random.hpp>
#include <boost/random/variate_generator.hpp>

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
		std::string traceFilename;

		std::ifstream traceFile;
		boost::mt19937 randomNumberGenerator;
		boost::uniform_real<> rngDistributionModel;	///< generates a random distribution from the set of [0:1)
		boost::uniform_int<> rngIntDistributionModel; ///< generates a distribution of integers
		mutable boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rngGenerator;
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rngIntGenerator;

	public: 

		// constructors	
		InputStream(const Settings& settings, const SystemConfiguration &systemConfigVal, const std::vector<Channel> &systemChannel);
		explicit InputStream(const InputStream& rhs, const SystemConfiguration &systemConfigVal, const std::vector<Channel> &systemChannel);
		InputStream(const InputStream& rhs);

		// functions
		InputType toInputToken(const std::string&) const;
		float boxMuller(float, float) const;
		float Poisson (float) const;
		float gammaLn(float) const;
		bool getNextBusEvent(BusEvent &);
		Transaction *getNextIncomingTransaction();
		Transaction *getNextRandomRequest();

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
		void setTime(tick value) { time = value; }
		void setArrivalThreshhold(float value) { arrivalThreshold = value; }

		// operator overloads
		bool operator==(const InputStream& rhs) const;
		friend std::ostream& operator<<(std::ostream&, const DRAMSimII::InputStream&);

	private:
		double ascii2multiplier(const std::string &input) const;


		// serialization
		friend class boost::serialization::access;

		InputStream(const SystemConfiguration &systemConfig, const std::vector<Channel> &systemChannel, DistributionType arrivalDistributionModel, std::string filename);

		template<class Archive>
		void serialize(Archive& ar, const unsigned version)
		{
			ar & type & channelLocality & rankLocality & bankLocality & time & rowLocality & readPercentage;
			ar & shortBurstRatio & arrivalThreshold & cpuToMemoryRatio & averageInterarrivalCycleCount;
			//ar & traceFile;
			//ar & serialization::make_nvp("rng",randomNumberGenerator & rngDistributionModel & rngIntDistributionModel & rngGenerator & rngIntGenerator;
		}

		template <class Archive>
		friend inline void save_construct_data(Archive& ar, const InputStream* t, const unsigned version)
		{
			const SystemConfiguration* const sysConfig = &(t->systemConfig);
			ar << sysConfig;
			const std::vector<Channel>* const channel = &(t->channel);
			ar << channel;
			ar << t->interarrivalDistributionModel;

			//std::basic_istream<char,std::char_traits<char>>::pos_type location = t->traceFile.tellg();
			//ar << location;

			ar << t->traceFilename;			
		}

		template <class Archive>
		friend inline void load_construct_data(Archive& ar, InputStream* t, const unsigned version)
		{
			SystemConfiguration *sysConfig;
			ar >> sysConfig;
			std::vector<Channel> *channel;
			ar >> channel;
			DistributionType interarrivalDistributionModel;
			ar >> interarrivalDistributionModel;
			//std::basic_istream<char,std::char_traits<char>>::pos_type location;
			//ar >> location;
			std::string filename;
			ar >> filename;

			new(t)InputStream(*sysConfig,*channel, interarrivalDistributionModel, filename);

			//t->traceFile.seekg(location);
		}
	};
}
#endif
