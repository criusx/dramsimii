#include "Algorithm.h"

using namespace DRAMSimII;

Algorithm::Algorithm(const Settings& settings)
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


	WHCC.resize(settings.rankCount * settings.bankCount * 2);

	Command *temp_c;

	if (settings.rankCount == 2)
	{
		if(settings.bankCount == 8)
		{
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		}
		else if (settings.bankCount == 16)
		{
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
		}
	}
	else if (settings.rankCount == 4)
	{
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
	}
}

Algorithm::Algorithm(int rank_count,
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


	WHCC.resize(rank_count * bank_count * 2);

	Command *temp_c;

	if((config_type == BASELINE_CONFIG) && (rank_count == 2))
	{
		if(bank_count == 8)
		{
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		}
		else if (bank_count == 16)
		{
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
		}
	}
	else if((config_type == BASELINE_CONFIG) && (rank_count == 4))
	{
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
	}
}

Algorithm::Algorithm(const Algorithm &a)
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
void Algorithm::init(int rank_count,
						 int bank_count,
						 int config_type)
{
	WHCC.resize(rank_count * bank_count * 2);

	Command *temp_c;

	if((config_type == BASELINE_CONFIG) && (rank_count == 2))
	{
		if(bank_count == 8)
		{
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		}
		else if (bank_count == 16)
		{
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  8; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  9; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 10; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 11; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 12; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 13; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 14; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
			temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank = 15; WHCC.push(temp_c);
		}
	}
	else if((config_type == BASELINE_CONFIG) && (rank_count == 4))
	{
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  2; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  0; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  4; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 1; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  5; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 2; temp_c->getAddress().bank =  3; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  6; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(RAS_COMMAND); temp_c->getAddress().rank = 3; temp_c->getAddress().bank =  1; WHCC.push(temp_c);
		temp_c = new Command; temp_c->setCommandType(CAS_COMMAND); temp_c->getAddress().rank = 0; temp_c->getAddress().bank =  7; WHCC.push(temp_c);
	}
}
