#include <iostream>
#include <assert.h>

#include "dramSystem.h"

using namespace std;
using namespace DRAMSimII;

bool dramChannel::checkForAvailableCommandSlots(const transaction *trans) const
{
	if (trans == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (trans->getAddresses().chan_id == channelID || trans->getType() == AUTO_REFRESH_TRANSACTION);

	const queue<command> *bank_q = &(rank[trans->getAddresses().rank_id].bank[trans->getAddresses().bank_id].per_bank_q);
	int availableCommandSlots = (trans->getType() == AUTO_REFRESH_TRANSACTION) ? 0 : bank_q->freecount();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig->getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	
			// refresh transactions become only one command and are handled differently
			if (trans->getType() == AUTO_REFRESH_TRANSACTION)
			{
				// make sure that there is room in all the queues for one command
				// refresh commands refresh a row, but kill everything currently in the sense amps
				// therefore, we need to make sure that the refresh commands happen when all banks
				// are available
				for (vector<bank_c>::const_iterator i = rank[trans->getAddresses().rank_id].bank.begin();
					i != rank[trans->getAddresses().rank_id].bank.end();
					i++)
				{
					if (i->per_bank_q.freecount() < 1)
						return false;
				}
			}
			// every transaction translates into at least two commands
			else if (availableCommandSlots < 2)
			{
				return false;
			}
			// or three commands if the CAS+Precharge command is not available
			else if (!systemConfig->isAutoPrecharge() && (availableCommandSlots < 3))
			{
				return false;
			}
		break;
		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be delayed

	case OPEN_PAGE:
		{		
			// FIXME: handle REFRESH transactions
			// look in the bank_q and see if there's a precharge for this row
			bool bypass_allowed = true;
			// go from tail to head
			for (int tail_offset = bank_q->size() - 1 ;(tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
			{	
				command *temp_c = bank_q->read(tail_offset);

				// goes right before the PRE command to ensure that the original order is preserved
				if ((temp_c->getCommandType() == PRECHARGE_COMMAND) &&
					(temp_c->getAddress().row_id == trans->getAddresses().row_id))
				{
					// can piggyback on other R-C-C...C-P sequences
					if (availableCommandSlots < 1)
						return false;

					return true;			
				}

				// even STRICT ORDER allows you to look at the tail of the queue
				// to see if that's the precharge command that you need. If so,
				// insert CAS COMMAND in front of PRECHARGE COMMAND

				if ((systemConfig->getCommandOrderingAlgorithm() == STRICT_ORDER) 
					|| ((int)(time - temp_c->getEnqueueTime()) > systemConfig->getSeniorityAgeLimit()))
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
	assert (this_t->getAddresses().chan_id == channelID || this_t->getType() == AUTO_REFRESH_TRANSACTION);

	queue<command> *bank_q = &(rank[this_t->getAddresses().rank_id].bank[this_t->getAddresses().bank_id].per_bank_q);

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	if (systemConfig->getRowBufferManagementPolicy() == CLOSE_PAGE)
	{
		// refresh transactions become only one command and are handled differently
		if (this_t->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			rank_c &currentRank = rank[this_t->getAddresses().rank_id];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<bank_c>::const_iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				if (i->per_bank_q.freecount() < 1)
					return false;
			}
			// then add the command to all queues
			for (vector<bank_c>::iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				bool result = i->per_bank_q.push(new command(this_t->getAddresses(), REFRESH_ALL_COMMAND, time, this_t, systemConfig->isPostedCAS()));
				assert (result);
			}
		}
		// every transaction translates into at least two commands
		else if (bank_q->freecount() < 2)
		{
			return false;
		}
		// or three commands if the CAS+Precharge command is not available
		else if ((systemConfig->isAutoPrecharge() == false) && (bank_q->freecount() < 3))
		{
			return false;
		}
		// if there is enough space to insert the commands that this transaction becomes
		else
		{
			// command one, the RAS command to activate the row
			bank_q->push(new command(this_t->getAddresses(),RAS_COMMAND,time,NULL,systemConfig->isPostedCAS()));

			// command two, CAS or CAS+Precharge

			// if CAS+Precharge is unavailable
			if (systemConfig->isAutoPrecharge() == false)
			{
				switch (this_t->getType())
				{
				case WRITE_TRANSACTION:					
					bank_q->push(new command(this_t->getAddresses(),CAS_WRITE_COMMAND,time,this_t,systemConfig->isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q->push(new command(this_t->getAddresses(),CAS_COMMAND,time,this_t,systemConfig->isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->getType();
					exit(-8);
					break;
				}				

				// command three, the Precharge command
				// only one of these commands has a pointer to the original transaction, thus NULL
				bank_q->push(new command(this_t->getAddresses(),PRECHARGE_COMMAND,time,NULL,systemConfig->isPostedCAS()));
			}
			else // precharge is implied, only need two commands
			{
				switch(this_t->getType())
				{
				case WRITE_TRANSACTION:
					bank_q->push(new command(this_t->getAddresses(),CAS_WRITE_AND_PRECHARGE_COMMAND,time,this_t,systemConfig->isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q->push(new command(this_t->getAddresses(),CAS_AND_PRECHARGE_COMMAND,time,this_t,systemConfig->isPostedCAS()));
					break;
				case PER_BANK_REFRESH_TRANSACTION:
					bank_q->push(new command(this_t->getAddresses(),PRECHARGE_COMMAND,time,this_t,systemConfig->isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->getType();
					exit(-8);
					break;
				}
			}
		}
	}

	// open page systems may, in the best case, add a CAS command to an already open row
	// closing the row and precharging may be delayed
	else if (systemConfig->getRowBufferManagementPolicy() == OPEN_PAGE)
	{
		int queued_command_count = bank_q->size();
		int empty_command_slot_count = bank_q->freecount();
		// look in the bank_q and see if there's a precharge for this row
		bool bypass_allowed = true;
		// go from tail to head
		for(int tail_offset = queued_command_count -1 ;(tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
		{	
			command *temp_c = bank_q->read(tail_offset);

			if (temp_c->getCommandType() == PRECHARGE_COMMAND) // found a precharge command
			{
				if (temp_c->getAddress().row_id == this_t->getAddresses().row_id) // same row, insert here 
				{
					if (empty_command_slot_count < 1)
						return false;

					if (this_t->getType() == WRITE_TRANSACTION)
						bank_q->insert(new command(this_t->getAddresses(),CAS_WRITE_COMMAND,time,this_t,systemConfig->isPostedCAS(),this_t->getLength()), tail_offset);	/* insert at this location */						
					else if (this_t->getType() == READ_TRANSACTION)
						bank_q->insert(new command(this_t->getAddresses(),CAS_COMMAND,time,this_t,systemConfig->isPostedCAS(),this_t->getLength()), tail_offset);	/* insert at this location */

					return true;
				}
			}

			// even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge
			// command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND

			if ((systemConfig->getCommandOrderingAlgorithm() == STRICT_ORDER) 
				|| ((time - temp_c->getEnqueueTime()) > (int)systemConfig->getSeniorityAgeLimit()))
			{
				bypass_allowed = false;
			}
		}
		if (empty_command_slot_count < 3)
		{
			return false;
		}

		bank_q->push(new command(this_t->getAddresses(),RAS_COMMAND,time,NULL,systemConfig->isPostedCAS(),this_t->getLength()));

		if (this_t->getType() == WRITE_TRANSACTION)
		{
			bank_q->push(new command(this_t->getAddresses(),CAS_WRITE_COMMAND,time,this_t,systemConfig->isPostedCAS(),this_t->getLength()));
		}
		else if ((this_t->getType() == READ_TRANSACTION) || (this_t->getType() == IFETCH_TRANSACTION))
		{
			bank_q->push(new command(this_t->getAddresses(),CAS_COMMAND,time,this_t,systemConfig->isPostedCAS(),this_t->getLength()));
		}
		else
		{
			cerr << "Unhandled transaction type: " << this_t->getType();
			exit(-8);
		}

		// last, the precharge command
		bank_q->push(new command(this_t->getAddresses(),PRECHARGE_COMMAND,time,NULL,systemConfig->isPostedCAS(),this_t->getLength()));
	}
	else
	{
		cerr << "Unhandled row buffer management policy" << endl;
		return false;
	}

	this_t->setDecodeTime(time);

	return true;
}
