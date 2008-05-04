#ifndef DRAMALGORITHM_H
#define DRAMALGORITHM_H
#pragma once

#include "command.h"
#include "globals.h"
#include "queue.h"
#include "dramSettings.h"
#include "transaction.h"
#include "command.h"

namespace DRAMSimII
{
	/// @brief Algorithm specific data structures should go in here.
	class Algorithm
	{

	protected:
		Queue<Command> WHCC; /// Wang Hop Command Chain 
		int WHCC_offset[2];
		int transaction_type[4];
		int rank_id[2];
		int ras_bank_id[4];
		int cas_count[4];

	public:
		// constructors
		Algorithm(int, int, int);
		Algorithm(const Algorithm&);
		Algorithm(const Settings& settings);

		// functions
		void init(int, int, int); 
		Queue<Command> &getWHCC() { return WHCC; }
		int *WHCCOffset() { return WHCC_offset; }
		int *getTransactionType() { return transaction_type; }
	};
}
#endif
