#include "dramAlgorithm.h"

using namespace DRAMSimII;

dramAlgorithm::dramAlgorithm(const dramSettings *settings){
	rank_id[0] = 0;
	rank_id[1] = 3;
	ras_bank_id[0] = 0;
	ras_bank_id[1] = 0;
	ras_bank_id[2] = 0;
	ras_bank_id[3] = 0;
	cas_count[0] = 0;
	cas_count[1] = 0;
	cas_count[2] = 0;
	cas_count[3] = 0;
	WHCC_offset[0] = 0;
	WHCC_offset[1] = 0;
	transaction_type[0] = READ_TRANSACTION;
	transaction_type[1] = READ_TRANSACTION;
	transaction_type[2] = READ_TRANSACTION;
	transaction_type[3] = READ_TRANSACTION;


	WHCC.init(settings->rankCount * settings->bankCount * 2);

	command *temp_c;

	if (settings->rankCount == 2)
	{
		if(settings->bankCount == 8)
		{
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		}
		else if (settings->bankCount == 16)
		{
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
		}
	}
	else if (settings->rankCount == 4)
	{
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
	}
}

dramAlgorithm::dramAlgorithm(int rank_count,
							 int bank_count,
							 int config_type)
{
	rank_id[0] = 0;
	rank_id[1] = 3;
	ras_bank_id[0] = 0;
	ras_bank_id[1] = 0;
	ras_bank_id[2] = 0;
	ras_bank_id[3] = 0;
	cas_count[0] = 0;
	cas_count[1] = 0;
	cas_count[2] = 0;
	cas_count[3] = 0;
	WHCC_offset[0] = 0;
	WHCC_offset[1] = 0;
	transaction_type[0] = READ_TRANSACTION;
	transaction_type[1] = READ_TRANSACTION;
	transaction_type[2] = READ_TRANSACTION;
	transaction_type[3] = READ_TRANSACTION;


	WHCC.init(rank_count * bank_count * 2);

	command *temp_c;

	if((config_type == BASELINE_CONFIG) && (rank_count == 2))
	{
		if(bank_count == 8)
		{
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		}
		else if (bank_count == 16)
		{
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
		}
	}
	else if((config_type == BASELINE_CONFIG) && (rank_count == 4))
	{
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
	}
}

dramAlgorithm::dramAlgorithm(const dramAlgorithm &a)
{
	rank_id[0] = a.rank_id[0];
	rank_id[1] = a.rank_id[1];
	ras_bank_id[0] = a.ras_bank_id[0];
	ras_bank_id[1] = a.ras_bank_id[1];
	ras_bank_id[2] = a.ras_bank_id[2];
	ras_bank_id[3] = a.ras_bank_id[3];
	cas_count[0] = a.cas_count[0];
	cas_count[1] = a.cas_count[1];
	cas_count[2] = a.cas_count[2];
	cas_count[3] = a.cas_count[3];
	WHCC_offset[0] = a.WHCC_offset[0];
	WHCC_offset[1] = a.WHCC_offset[1];
	transaction_type[0] = a.transaction_type[0];
	transaction_type[1] = a.transaction_type[1];
	transaction_type[2] = a.transaction_type[2];
	transaction_type[3] = a.transaction_type[3];
}
void dramAlgorithm::init(int rank_count,
						 int bank_count,
						 int config_type)
{
	WHCC.init(rank_count * bank_count * 2);

	command *temp_c;

	if((config_type == BASELINE_CONFIG) && (rank_count == 2))
	{
		if(bank_count == 8)
		{
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		}
		else if (bank_count == 16)
		{
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  8; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  9; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 10; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 11; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 12; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 13; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 14; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
			temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id = 15; WHCC.push(temp_c);
		}
	}
	else if((config_type == BASELINE_CONFIG) && (rank_count == 4))
	{
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  2; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  0; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  4; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 1; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  5; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 2; temp_c->getAddress().bank_id =  3; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  6; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank_id = 3; temp_c->getAddress().bank_id =  1; WHCC.push(temp_c);
		temp_c = new command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank_id = 0; temp_c->getAddress().bank_id =  7; WHCC.push(temp_c);
	}
}
