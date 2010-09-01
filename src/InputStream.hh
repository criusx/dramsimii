// Copyright (C) 2010 University of Maryland.
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

#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H
#pragma once

#include "globals.hh"

#include <fstream>
#include <map>
#include <vector>
#include <boost/random.hpp>
#include <boost/random/variate_generator.hpp>

namespace DRAMsimII
{
	/// @brief creates transactions in standalone mode, whether random or from a trace file
	class InputStream
	{
	public:
		enum InputType
		{
			K6_TRACE,
			MASE_TRACE,
			RANDOM,
			MAPPED,
			ALPHASIM,
			GEMS,
			DRAMSIM
		};

		enum DistributionType
		{
			UNIFORM_DISTRIBUTION,
			GAUSSIAN_DISTRIBUTION,
			POISSON_DISTRIBUTION,
			NORMAL_DISTRIBUTION,
			LOGNORMAL_DISTRIBUTION
		};

		enum InputStatus
		{
			FAILURE,
			SUCCESS
		};

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
		float arrivalThreshold;						///< the minimum threshold at which to decide that a transaction has arrived
		float cpuToMemoryRatio;						///< the ratio of the cpu clock to the memory clock
		unsigned averageInterarrivalCycleCount;		///< used by random number generator
		DistributionType interarrivalDistributionModel;	///< the model used to predict and determine when the next arrival will happen
		std::string traceFilename;

		boost::iostreams::filtering_istream traceFile;
		boost::mt19937 randomNumberGenerator;
		boost::uniform_real<> rngDistributionModel;	///< generates a random distribution from the set of [0:1)
		boost::uniform_int<> rngIntDistributionModel; ///< generates a distribution of integers
		boost::normal_distribution<> gaussianDistribution; ///< creates a Gaussian distribution
		mutable boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rngGenerator;
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rngIntGenerator;
		boost::variate_generator<boost::mt19937, boost::normal_distribution<> > arrivalGenerator;

	public:

		// constructors	
		InputStream(const Settings& settings, const SystemConfiguration &systemConfigVal, const std::vector<Channel> &systemChannel);
		explicit InputStream(const InputStream& rhs, const SystemConfiguration &systemConfigVal, const std::vector<Channel> &systemChannel);
		InputStream(const InputStream& rhs);
		~InputStream();

		// functions
		InputType toInputToken(const std::string&) const;
		float boxMuller(float, float) const;
		float Poisson (float) const;
		float gammaLn(float) const;
		Transaction *getNextIncomingTransaction(const unsigned transactionID);
		Transaction *getNextRandomRequest(const unsigned transactionID);

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
		unsigned getAverageInterarrivalCycleCount() const { return averageInterarrivalCycleCount; }
		tick getTime() const { return time; }

		// mutators
		void setTime(tick value) { time = value; }
		void setArrivalThreshhold(float value) { arrivalThreshold = value; }

		// operator overloads
		bool operator==(const InputStream& rhs) const;
		friend std::ostream& operator<<(std::ostream&, const DRAMsimII::InputStream&);

	private:
		double ascii2multiplier(const std::string &input) const;
	};

	std::ostream& operator<<(std::ostream&, const DRAMsimII::InputStream&);
	std::ostream& operator<<(std::ostream&, const DRAMsimII::InputStream::InputType);

}
#endif
