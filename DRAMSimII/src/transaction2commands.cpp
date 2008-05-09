#include <iostream>
#include <assert.h>

#include "System.h"

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
bool Channel::checkForAvailableCommandSlots(const Transaction *incomingTransaction) const
{
	if (incomingTransaction == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (incomingTransaction->getAddresses().channel == channelID || incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION);

	const Bank &destinationBank = rank[incomingTransaction->getAddresses().rank].bank[incomingTransaction->getAddresses().bank];

	unsigned availableCommandSlots = (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION) ? 0 : rank[incomingTransaction->getAddresses().rank].bank[incomingTransaction->getAddresses().bank].freeCommandSlots();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	
		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = destinationBank.begin(); currentBank != destinationBank.end(); currentBank++)
			{
				if (currentBank->freeCommandSlots() < 1)
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
			if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
			{
				// make sure that there is room in all the queues for one command
				// refresh commands refresh a row, but kill everything currently in the sense amps
				// therefore, we need to make sure that the refresh commands happen when all banks
				// are available
				for (vector<Bank>::const_iterator currentBank = rank[incomingTransaction->getAddresses().rank].bank.begin();
					currentBank != rank[incomingTransaction->getAddresses().rank].bank.end();
					currentBank++)
				{					
					if (currentBank->isfull())
						return false;
				}
				return true;
			}
			// look in the bank_q and see if there's a precharge for this row
			bool bypass_allowed = true;

			// go from tail to head
			for (int tail_offset = rank[incomingTransaction->getAddresses().rank].bank[incomingTransaction->getAddresses().bank].size() - 1; (tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
			{	
				const Command *temp_c = rank[incomingTransaction->getAddresses().rank].bank[incomingTransaction->getAddresses().bank].read(tail_offset);

				// goes right before the PRE command to ensure that the original order is preserved
				if ((temp_c->getCommandType() == PRECHARGE_COMMAND) &&
					(temp_c->getAddress().row == incomingTransaction->getAddresses().row))
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
/// @param incomingTransaction the transaction which is divided up
/// @return true if the transaction was able to be divided up and put into per bank queues
//////////////////////////////////////////////////////////////////////
bool Channel::transaction2commands(Transaction *incomingTransaction) 
{
	if (incomingTransaction == NULL)
	{
		return false;
	}
	// ensure that this transaction belongs on this channel
	assert (incomingTransaction->getAddresses().channel == channelID || incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION);

	Bank &destinationBank = rank[incomingTransaction->getAddresses().rank].bank[incomingTransaction->getAddresses().bank];

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			Rank &currentRank = rank[incomingTransaction->getAddresses().rank];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = currentRank.bank.begin(); currentBank != currentRank.bank.end(); currentBank++)
			{
				if (currentBank->freeCommandSlots() < 1)
					return false;
			}
			// then add the command to all queues
			for (vector<Bank>::iterator currentBank = currentRank.bank.begin(); currentBank != currentRank.bank.end(); currentBank++)
			{
				bool result = currentBank->push(new Command(incomingTransaction->getAddresses(), REFRESH_ALL_COMMAND, time, incomingTransaction, systemConfig.isPostedCAS()));
				assert (result);
			}
		}
		// every transaction translates into at least two commands
		else if (destinationBank.freeCommandSlots() < 2)
		{
			return false;
		}
		// or three commands if the CAS+Precharge command is not available
		else if (!systemConfig.isAutoPrecharge() && (destinationBank.freeCommandSlots() < 3))
		{
			return false;
		}
		// if there is enough space to insert the commands that this transaction becomes
		else
		{
			// command one, the RAS command to activate the row
			destinationBank.push(new Command(incomingTransaction->getAddresses(),RAS_COMMAND,time,NULL,systemConfig.isPostedCAS()));

			// command two, CAS or CAS+Precharge

			// if CAS+Precharge is unavailable
			if (systemConfig.isAutoPrecharge() == false)
			{
				switch (incomingTransaction->getType())
				{
				case WRITE_TRANSACTION:					
					destinationBank.push(new Command(incomingTransaction->getAddresses(),CAS_WRITE_COMMAND,time,incomingTransaction,systemConfig.isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					destinationBank.push(new Command(incomingTransaction->getAddresses(),CAS_COMMAND,time,incomingTransaction,systemConfig.isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << incomingTransaction->getType();
					exit(-8);
					break;
				}				

				// command three, the Precharge command
				// only one of these commands has a pointer to the original transaction, thus NULL
				destinationBank.push(new Command(incomingTransaction->getAddresses(),PRECHARGE_COMMAND,time,NULL,systemConfig.isPostedCAS()));
			}
			else // precharge is implied, only need two commands
			{
				switch(incomingTransaction->getType())
				{
				case WRITE_TRANSACTION:
					destinationBank.push(new Command(incomingTransaction->getAddresses(),CAS_WRITE_AND_PRECHARGE_COMMAND,time,incomingTransaction,systemConfig.isPostedCAS()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					destinationBank.push(new Command(incomingTransaction->getAddresses(),CAS_AND_PRECHARGE_COMMAND,time,incomingTransaction,systemConfig.isPostedCAS()));
					break;
				case PER_BANK_REFRESH_TRANSACTION:
					destinationBank.push(new Command(incomingTransaction->getAddresses(),PRECHARGE_COMMAND,time,incomingTransaction,systemConfig.isPostedCAS()));
					break;
				default:
					cerr << "Unhandled transaction type: " << incomingTransaction->getType();
					exit(-8);
					
				}
			}
		}
		break;

		// open page systems may, in the best case, add a CAS command to an already open row
		// closing the row and precharging may be pushed back one slot
	case OPEN_PAGE:

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			Rank &currentRank = rank[incomingTransaction->getAddresses().rank];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				if (i->freeCommandSlots() == 0)
					return false;
			}
			// then add the command to all queues
			for (vector<Bank>::iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				bool result = i->push(new Command(incomingTransaction->getAddresses(), REFRESH_ALL_COMMAND, time, incomingTransaction, false));
				assert (result);
			}
		}
		else 
		{	
			// if there is no last command in the queue, the queue is empty to openPageInsert does not apply anyway
			if ((systemConfig.getCommandOrderingAlgorithm() != STRICT_ORDER) &&
				((destinationBank.back()) && (time - destinationBank.back()->getEnqueueTime() <= systemConfig.getSeniorityAgeLimit()))
			{
			}
			// try to do a normal open page insert on this transaction
			else if (destinationBank.openPageInsert(incomingTransaction))
			{
				incomingTransaction->setDecodeTime(time);
				return true;
			}
			else
			{
				// even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge
				// command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND
				// also prevent against starvation
#if 0
				if ((systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER) 
					|| ((time - temp_c->getEnqueueTime()) > (int)systemConfig.getSeniorityAgeLimit()))
				{
					break;
				}
#endif

				// if this command was not able to be combined with another, create a new series of commands
				if (destinationBank.freeCommandSlots() < 3)
				{
					return false;
				}

				destinationBank.push(new Command(incomingTransaction->getAddresses(),RAS_COMMAND,time,NULL,false,incomingTransaction->getLength()));

				switch (incomingTransaction->getType())
				{
				case WRITE_TRANSACTION:
					destinationBank.push(new Command(incomingTransaction->getAddresses(),CAS_WRITE_COMMAND,time,incomingTransaction,false,incomingTransaction->getLength()));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					destinationBank.push(new Command(incomingTransaction->getAddresses(),CAS_COMMAND,time,incomingTransaction,false,incomingTransaction->getLength()));
					break;
				default:
					cerr << "Unhandled transaction type: " << incomingTransaction->getType();
					exit(-8);
					break;
				}

				// last, the precharge command
				destinationBank.push(new Command(incomingTransaction->getAddresses(),PRECHARGE_COMMAND,time,NULL,false,incomingTransaction->getLength()));
			}
		}
		break;


	default:

		cerr << "Unhandled row buffer management policy" << endl;
		return false;

	}

	incomingTransaction->setDecodeTime(time);

	return true;
}
