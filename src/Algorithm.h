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

#ifndef DRAMALGORITHM_H
#define DRAMALGORITHM_H
#pragma once

#include "globals.h"
#include "command.h"
#include "queue.h"
#include "Settings.h"
#include "transaction.h"
#include "command.h"

namespace DRAMsimII
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
		friend inline void save_construct_data(Archive &ar, const DRAMsimII::Algorithm *t, const unsigned version)
		{}

		template<class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMsimII::Algorithm * t, const unsigned version)
		{
			DRAMsimII::Settings settings;
			new(t)DRAMsimII::Algorithm(settings);
		}
	};
}
#endif
