#include "Algorithm.h"

using namespace DRAMSimII;

Algorithm::Algorithm(const Settings& settings)
{
	rankID[0] = 0;
	rankID[1] = 3;
	rasBankID[0] = 0;
	rasBankID[1] = 0;
	rasBankID[2] = 0;
	rasBankID[3] = 0;
	casCount[0] = 0;
	casCount[1] = 0;
	casCount[2] = 0;
	casCount[3] = 0;
	WHCCOffset[0] = 0;
	WHCCOffset[1] = 0;
	transactionType[0] = READ_TRANSACTION;
	transactionType[1] = READ_TRANSACTION;
	transactionType[2] = READ_TRANSACTION;
	transactionType[3] = READ_TRANSACTION;


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
	rankID[0] = 0;
	rankID[1] = 3;
	rasBankID[0] = 0;
	rasBankID[1] = 0;
	rasBankID[2] = 0;
	rasBankID[3] = 0;
	casCount[0] = 0;
	casCount[1] = 0;
	casCount[2] = 0;
	casCount[3] = 0;
	WHCCOffset[0] = 0;
	WHCCOffset[1] = 0;
	transactionType[0] = READ_TRANSACTION;
	transactionType[1] = READ_TRANSACTION;
	transactionType[2] = READ_TRANSACTION;
	transactionType[3] = READ_TRANSACTION;


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
	rankID[0] = a.rankID[0];
	rankID[1] = a.rankID[1];
	rasBankID[0] = a.rasBankID[0];
	rasBankID[1] = a.rasBankID[1];
	rasBankID[2] = a.rasBankID[2];
	rasBankID[3] = a.rasBankID[3];
	casCount[0] = a.casCount[0];
	casCount[1] = a.casCount[1];
	casCount[2] = a.casCount[2];
	casCount[3] = a.casCount[3];
	WHCCOffset[0] = a.WHCCOffset[0];
	WHCCOffset[1] = a.WHCCOffset[1];
	transactionType[0] = a.transactionType[0];
	transactionType[1] = a.transactionType[1];
	transactionType[2] = a.transactionType[2];
	transactionType[3] = a.transactionType[3];
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
