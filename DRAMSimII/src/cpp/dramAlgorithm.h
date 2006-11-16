#ifndef DRAMALGORITHM_H
#define DRAMALGORITHM_H
#pragma once

#include "command.h"
#include "globals.h"
#include "queue.h"

// Algorithm specific data structures should go in here.
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
	dramAlgorithm(int, int, int);
	dramAlgorithm(const dramAlgorithm &);
	void init(int, int, int); 
	//friend class dramSystem;
	queue<command> &getWHCC() { return WHCC; }
	int *WHCCOffset() { return WHCC_offset; }
	int *getTransactionType() { return transaction_type; }
};

#endif
