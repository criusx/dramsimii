#ifndef DRAMALGORITHM_H
#define DRAMALGORITHM_H
#pragma once

#include "globals.h"
#include "command.h"
#include "queue.h"
#include "Settings.h"
#include "transaction.h"
#include "command.h"

namespace DRAMSimII
{
	/// @brief Algorithm specific data structures should go in here.
	class Algorithm
	{

	protected:
		Queue<Command> WHCC; /// Wang Hop Command Chain 
		unsigned WHCCOffset[2];
		int transactionType[4];
		int rankID[2];
		int rasBankID[4];
		int casCount[4];

	public:
		// constructors
		Algorithm(const Algorithm&);
		Algorithm(const Settings& settings);

		// functions
		Queue<Command> &getWHCC() { return WHCC; }
		unsigned getWHCCOffset(const unsigned value) const { return WHCCOffset[value]; }
		int *getTransactionType() { return transactionType; }

		// overloads
		bool operator==(const Algorithm& right) const;

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize( Archive & ar, const unsigned version)
		{
			ar & WHCC & transactionType & rankID & rasBankID & casCount;
		}

		template<class Archive>
		friend inline void save_construct_data(Archive &ar, const DRAMSimII::Algorithm *t, const unsigned version)
		{}

		template<class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMSimII::Algorithm * t, const unsigned version)
		{
			DRAMSimII::Settings settings;
			new(t)DRAMSimII::Algorithm(settings);
		}
	};
}
#endif
