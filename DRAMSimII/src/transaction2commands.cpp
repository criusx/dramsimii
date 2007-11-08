#include <iostream>
#include <assert.h>

#include "dramSystem.h"

using namespace std;
using namespace DRAMSimII;

/// this will ensure that a given transaction can be broken into and inserted as commands in this channel
/// if there is not room under a given scheme, then it will return false
bool dramChannel::checkForAvailableCommandSlots(const transaction *trans) const
{
	if (trans == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (trans->getAddresses().channel == channelID || trans->getType() == AUTO_REFRESH_TRANSACTION);

	const queue<command> &bank_q = rank[trans->getAddresses().rank].bank[trans->getAddresses().bank].getPerBankQueue();
	int availableCommandSlots = (trans->getType() == AUTO_REFRESH_TRANSACTION) ? 0 : bank_q.freecount();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	
		// refresh transactions become only one command and are handled differently
		if (trans->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<bank_c>::const_iterator i = rank[trans->getAddresses().rank].bank.begin();
				i != rank[trans->getAddresses().rank].bank.end();
				i++)
			{
				if (i->getPerBankQueue().freecount() < 1)
					return false;
			}
		}
		// every transaction translates into at least two commands
		else if (availableCommandSlots < 2)
		{
			return false;
		}
		// or three commands if the CAS+Precharge command is not available
		else if (!systemConfig.isAutoPrecharge() && (availableCommandSlots < 3))
		{
			return false;
		}
		break;
		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be delayed

		// all break down into PRE,RAS,CAS
		// or CAS
	case OPEN_PAGE:
		{		
			// refresh transactions become only one command and are handled differently
			if (trans->getType() == AUTO_REFRESH_TRANSACTION)
			{
				// make sure that there is room in all the queues for one command
				// refresh commands refresh a row, but kill everything currently in the sense amps
				// therefore, we need to make sure that the refresh commands happen when all banks
				// are available
				for (vector<bank_c>::const_iterator i = rank[trans->getAddresses().rank].bank.begin();
					i != rank[trans->getAddresses().rank].bank.end();
					i++)
				{					
					if (i->getPerBankQueue().freecount() < 1)
						return false;
				}
				return true;
			}
			// look in the bank_q and see if there's a precharge for this row
			bool bypass_allowed = true;
			// go from tail to head
			for (int tail_offset = bank_q.size() - 1 ;(tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
			{	
				command *temp_c = bank_q.read(tail_offset);

				// goes right before the PRE command to ensure that the original order is preserved
				if ((temp_c->getCommandType() == PRECHARGE_COMMAND) &&
					(temp_c->getAddress().row == trans->getAddresses().row))
				{
					// can piggyback on other R-C-C...C-P sequences
					if (availableCommandSlots < 1)
						return false;

					return true;			
				}

				// even STRICT ORDER allows you to look at the tail of the queue
				// to see if that's the precharge command that you need. If so,
				// insert CAS COMMAND in front of PRECHARGE COMMAND

				if ((systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER) 
					|| ((unsigned)(time - temp_c->getEnqueueTime()) > systemConfig.getSeniorityAgeLimit()))
				{
					bypass_allowed = false;
				}
			}
			if (availableCommandSlots < 3)
			{
				return false;
			}
		}
		break;

	default:

		cerr << "Unhandled row buffer management policy" << endl;
		return false;
		break;
	}
	return true;
}

/// <summary>
/// Converts a transaction in a particular channel into commands,
/// then inserts them in the bank queues.
/// Functionality similar to that of the memory controller moving between the
/// transaction queue and the per bank command queues 
/// </summary>
bool dramChannel::transaction2commands(transaction *this_t) 
{
	if (this_t == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (this_t->getAddresses().channel == channelID || this_t->getType() == AUTO_REFRESH_TRANSACTION);

	queue<command> &bank_q = rank[this_t->getAddresses().rank].bank[this_t->getAddresses().bank].getPerBankQueue();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	

		// refresh transactions become only one command and are handled differently
		if (this_t->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			rank_c &currentRank = rank[this_t->getAddresses().rank];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<bank_c>::const_iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				if (i->getPerBankQueue().freecount() < 1)
					return false;
			}
			// then add the command to all queues
			for (vector<bank_c>::iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				bool result = i->getPerBankQueue().push(new command(this_t->getAddresses(), REFRESH_ALL_COMMAND, time, this_t, systemConfig.isPostedCAS()));
				assert (result);
			}
		}
		// every transaction translates into at least two commands
		else if (bank_q.freecount() < 2)
		{
			return false;
		}
		// or three commands if the CAS+Precharge command is not available
		else if (!systemConfig.isAutoPrecharge() && (bank_q.freecount() < 3))
		{
			return false;
		}
		// if there is enough space to insert the commands that this transaction becomes
		else
		{
			// command one, the RAS command to activate the row
			bank_q.push(new command(this_t->getAddresses(),RAS_COMMAND,time,NULL,systemConfig.isPostedCAS()));

			// command two, CAS or CAS+Precharge

			// if CAS+Precharge is unavailable
			if (systemConfig.isAutoPrecharge() == false)
			{
				switch (this_t->getType())
				{
				case WRITE_TRANSACTION:					
					bank_q.push(new command(this_t->getAddresses(),CAS_WRITE_COMMAND,time,this_t,systemConfig.isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q.push(new command(this_t->getAddresses(),CAS_COMMAND,time,this_t,systemConfig.isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->getType();
					exit(-8);
					break;
				}				

				// command three, the Precharge command
				// only one of these commands has a pointer to the original transaction, thus NULL
				bank_q.push(new command(this_t->getAddresses(),PRECHARGE_COMMAND,time,NULL,systemConfig.isPostedCAS()));
			}
			else // precharge is implied, only need two commands
			{
				switch(this_t->getType())
				{
				case WRITE_TRANSACTION:
					bank_q.push(new command(this_t->getAddresses(),CAS_WRITE_AND_PRECHARGE_COMMAND,time,this_t,systemConfig.isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q.push(new command(this_t->getAddresses(),CAS_AND_PRECHARGE_COMMAND,time,this_t,systemConfig.isPostedCAS()));
					break;
				case PER_BANK_REFRESH_TRANSACTION:
					bank_q.push(new command(this_t->getAddresses(),PRECHARGE_COMMAND,time,this_t,systemConfig.isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->getType();
					exit(-8);
					
				}
			}
		}
		break;

		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be delayed
	case OPEN_PAGE:

		// refresh transactions become only one command and are handled differently
		if (this_t->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			rank_c &currentRank = rank[this_t->getAddresses().rank];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<bank_c>::const_iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				if (i->getPerBankQueue().freecount() < 1)
					return false;
			}
			// then add the command to all queues
			for (vector<bank_c>::iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				bool result = i->getPerBankQueue().push(new command(this_t->getAddresses(), REFRESH_ALL_COMMAND, time, this_t, false));
				assert (result);
			}
		}
		else 
		{	
			// if there's no room for any more commands
			if (bank_q.freecount() < 1)
				return false;

			// look in the bank_q and see if there's a precharge for this row to insert before		
			// go from tail to head
			for (int tail_offset = bank_q.size() - 1; tail_offset >= 0; --tail_offset)
			{	
				command *temp_c = bank_q.read(tail_offset);

				if (temp_c->getCommandType() == PRECHARGE_COMMAND) // found a precharge command
				{
					if (temp_c->getAddress().row == this_t->getAddresses().row) // same row, insert here 
					{
						switch (this_t->getType())
						{
						case WRITE_TRANSACTION:
							bank_q.insert(new command(this_t->getAddresses(),CAS_WRITE_COMMAND,time,this_t,false,this_t->getLength()), tail_offset);	/* insert at this location */						
							break;
						case IFETCH_TRANSACTION:
						case READ_TRANSACTION:
							bank_q.insert(new command(this_t->getAddresses(),CAS_COMMAND,time,this_t,false,this_t->getLength()), tail_offset);	/* insert at this location */
							break;
						default:
							cerr << "Unrecognized transaction type." << endl;
							break;
						}
						
						this_t->setDecodeTime(time);

						return true;
					}
				}

				// even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge
				// command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND
				// also prevent against starvation
				if ((systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER) 
					|| ((time - temp_c->getEnqueueTime()) > (int)systemConfig.getSeniorityAgeLimit()))
				{
					break;
				}
			}

			// if this command was not able to be combined with another, create a new series of commands
			if (bank_q.freecount() < 3)
			{
				return false;
			}

			bank_q.push(new command(this_t->getAddresses(),RAS_COMMAND,time,NULL,false,this_t->getLength()));

			switch (this_t->getType())
			{
			case WRITE_TRANSACTION:
				bank_q.push(new command(this_t->getAddresses(),CAS_WRITE_COMMAND,time,this_t,false,this_t->getLength()));
				break;
			case READ_TRANSACTION:
			case IFETCH_TRANSACTION:
				bank_q.push(new command(this_t->getAddresses(),CAS_COMMAND,time,this_t,false,this_t->getLength()));
				break;
			default:
				cerr << "Unhandled transaction type: " << this_t->getType();
				exit(-8);
				break;
			}

			// last, the precharge command
			bank_q.push(new command(this_t->getAddresses(),PRECHARGE_COMMAND,time,NULL,false,this_t->getLength()));
		}
		break;


	default:

		cerr << "Unhandled row buffer management policy" << endl;
		return false;

	}

	this_t->setDecodeTime(time);

	return true;
}
