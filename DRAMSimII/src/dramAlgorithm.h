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
	class dramAlgorithm
	{

	protected:
		queue<command> WHCC; /// Wang Hop Command Chain 
		int WHCC_offset[2];
		int transaction_type[4];
		int rank_id[2];
		int ras_bank_id[4];
		int cas_count[4];

	public:
		// constructors
		dramAlgorithm(int, int, int);
		dramAlgorithm(const dramAlgorithm &);
		dramAlgorithm(const dramSettings& settings);

		// functions
		void init(int, int, int); 
		queue<command> &getWHCC() { return WHCC; }
		int *WHCCOffset() { return WHCC_offset; }
		int *getTransactionType() { return transaction_type; }
	};
}
#endif
