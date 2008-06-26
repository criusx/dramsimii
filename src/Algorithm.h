#ifndef DRAMALGORITHM_H
#define DRAMALGORITHM_H
#pragma once

#include "command.h"
#include "globals.h"
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
		Algorithm(int, int, SystemConfigurationType);
		Algorithm(const Algorithm&);
		Algorithm(const Settings& settings);

		// functions
		void init(int, int, int); 
		Queue<Command> &getWHCC() { return WHCC; }
		unsigned getWHCCOffset(const unsigned value) const { return WHCCOffset[value]; }
		int *getTransactionType() { return transactionType; }
	};
}
#endif
