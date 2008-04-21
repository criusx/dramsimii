#include <iostream>
#include <assert.h>

#include "dramSystem.h"

using namespace std;
using namespace DRAMSimII;

//////////////////////////////////////////////////////////////////////
/// @brief determines if there are enough command slots for the transaction to be decoded
/// @details this will ensure that a given transaction can be broken into and inserted as commands in this channel
/// if there is not enough room according to a given algorithm, then it will indicate that this is not possible
/// @author Joe Gross
/// @param trans the transaction to be considered
/// @return true if there is enough room, false otherwise
//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
/// @brief converts a transaction into corresponding commands
/// @details takes a transaction and divides it into a number of commands, depending which row buffer management policy is chosen\n
/// currently will divide it into RAS, CAS, Pre or RAS, CAS+P and insert these commands into the per bank queues\n
/// the CAS command will contain a pointer to the host transaction to indicate that a read transaction has available data
/// @author Joe Gross
/// @param newTransaction the transaction which is divided up
/// @return true if the transaction was able to be divided up and put into per bank queues
//////////////////////////////////////////////////////////////////////
bool dramChannel::transaction2commands(transaction *newTransaction) 
{
	if (newTransaction == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (newTransaction->getAddresses().channel == channelID || newTransaction->getType() == AUTO_REFRESH_TRANSACTION);

	queue<command> &bank_q = rank[newTransaction->getAddresses().rank].bank[newTransaction->getAddresses().bank].getPerBankQueue();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	

		// refresh transactions become only one command and are handled differently
		if (newTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			rank_c &currentRank = rank[newTransaction->getAddresses().rank];
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
				bool result = i->getPerBankQueue().push(new command(newTransaction->getAddresses(), REFRESH_ALL_COMMAND, time, newTransaction, systemConfig.isPostedCAS()));
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
			bank_q.push(new command(newTransaction->getAddresses(),RAS_COMMAND,time,NULL,systemConfig.isPostedCAS()));

			// command two, CAS or CAS+Precharge

			// if CAS+Precharge is unavailable
			if (systemConfig.isAutoPrecharge() == false)
			{
				switch (newTransaction->getType())
				{
				case WRITE_TRANSACTION:					
					bank_q.push(new command(newTransaction->getAddresses(),CAS_WRITE_COMMAND,time,newTransaction,systemConfig.isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q.push(new command(newTransaction->getAddresses(),CAS_COMMAND,time,newTransaction,systemConfig.isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << newTransaction->getType();
					exit(-8);
					break;
				}				

				// command three, the Precharge command
				// only one of these commands has a pointer to the original transaction, thus NULL
				bank_q.push(new command(newTransaction->getAddresses(),PRECHARGE_COMMAND,time,NULL,systemConfig.isPostedCAS()));
			}
			else // precharge is implied, only need two commands
			{
				switch(newTransaction->getType())
				{
				case WRITE_TRANSACTION:
					bank_q.push(new command(newTransaction->getAddresses(),CAS_WRITE_AND_PRECHARGE_COMMAND,time,newTransaction,systemConfig.isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q.push(new command(newTransaction->getAddresses(),CAS_AND_PRECHARGE_COMMAND,time,newTransaction,systemConfig.isPostedCAS()));
					break;
				case PER_BANK_REFRESH_TRANSACTION:
					bank_q.push(new command(newTransaction->getAddresses(),PRECHARGE_COMMAND,time,newTransaction,systemConfig.isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << newTransaction->getType();
					exit(-8);
					
				}
			}
		}
		break;

		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be delayed
	case OPEN_PAGE:

		// refresh transactions become only one command and are handled differently
		if (newTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			rank_c &currentRank = rank[newTransaction->getAddresses().rank];
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
				bool result = i->getPerBankQueue().push(new command(newTransaction->getAddresses(), REFRESH_ALL_COMMAND, time, newTransaction, false));
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
					if (temp_c->getAddress().row == newTransaction->getAddresses().row) // same row, insert here 
					{
						switch (newTransaction->getType())
						{
						case WRITE_TRANSACTION:
							bank_q.insert(new command(newTransaction->getAddresses(),CAS_WRITE_COMMAND,time,newTransaction,false,newTransaction->getLength()), tail_offset);	/* insert at this location */						
							break;
						case IFETCH_TRANSACTION:
						case READ_TRANSACTION:
							bank_q.insert(new command(newTransaction->getAddresses(),CAS_COMMAND,time,newTransaction,false,newTransaction->getLength()), tail_offset);	/* insert at this location */
							break;
						default:
							cerr << "Unrecognized transaction type." << endl;
							break;
						}
						
						newTransaction->setDecodeTime(time);

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

			bank_q.push(new command(newTransaction->getAddresses(),RAS_COMMAND,time,NULL,false,newTransaction->getLength()));

			switch (newTransaction->getType())
			{
			case WRITE_TRANSACTION:
				bank_q.push(new command(newTransaction->getAddresses(),CAS_WRITE_COMMAND,time,newTransaction,false,newTransaction->getLength()));
				break;
			case READ_TRANSACTION:
			case IFETCH_TRANSACTION:
				bank_q.push(new command(newTransaction->getAddresses(),CAS_COMMAND,time,newTransaction,false,newTransaction->getLength()));
				break;
			default:
				cerr << "Unhandled transaction type: " << newTransaction->getType();
				exit(-8);
				break;
			}

			// last, the precharge command
			bank_q.push(new command(newTransaction->getAddresses(),PRECHARGE_COMMAND,time,NULL,false,newTransaction->getLength()));
		}
		break;


	default:

		cerr << "Unhandled row buffer management policy" << endl;
		return false;

	}

	newTransaction->setDecodeTime(time);

	return true;
}
