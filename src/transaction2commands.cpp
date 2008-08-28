#include <iostream>
#include <assert.h>

#include "System.h"

using std::cerr;
using std::endl;
using std::vector;
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
	assert (incomingTransaction->getAddresses().getChannel() == channelID || incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION);

	const Bank &destinationBank = rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()];

	unsigned availableCommandSlots = (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION) ? 0 : rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()].freeCommandSlots();

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	switch (systemConfig.getRowBufferManagementPolicy())
	{
	case CLOSE_PAGE:	
		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			const Rank &destinationRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = destinationRank.bank.begin(); currentBank != destinationRank.bank.end(); currentBank++)
			{
				if (currentBank->isFull())
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
	case CLOSE_PAGE_OPTIMIZED:
		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			const Rank &destinationRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator currentBank = destinationRank.bank.begin(); currentBank != destinationRank.bank.end(); currentBank++)
			{
				if (currentBank->isFull())
					return false;
			}
		}
		// need at least one free command slot
		else if (destinationBank.isFull())			
		{
			return false;
		}
		else if (destinationBank.back() 
			&& destinationBank.back()->getAddress().getRow() == incomingTransaction->getAddresses().getRow() // rows match
			&& (time - destinationBank.back()->getEnqueueTime() < systemConfig.getSeniorityAgeLimit()) // not starving the last command
			&& destinationBank.back()->getCommandType() != REFRESH_ALL_COMMAND) // ends with CAS+P or PRE
		{
			return true;
		}		
		else if ((!systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 3) ||
			(systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 2))
		{
			return false;
		}
		else
		{
			return true;
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
				const Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
				// make sure that there is room in all the queues for one command
				// refresh commands refresh a row, but kill everything currently in the sense amps
				// therefore, we need to make sure that the refresh commands happen when all banks
				// are available
				for (vector<Bank>::const_iterator currentBank = currentRank.bank.begin();
					currentBank != currentRank.bank.end();
					currentBank++)
				{					
					if (currentBank->isFull())
						return false;
				}
				return true;
			}
			// look in the bank_q and see if there's a precharge for this row
			bool bypass_allowed = true;

			// go from tail to head
			for (int tail_offset = rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()].size() - 1; (tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
			{	
				const Command *currentCommand = destinationBank.read(tail_offset);

				if (time - currentCommand->getEnqueueTime() > systemConfig.getSeniorityAgeLimit())
				{
					break;
				}
				// goes right before the PRE command to ensure that the original order is preserved
				else if ((currentCommand->getCommandType() == PRECHARGE_COMMAND) &&
					(currentCommand->getAddress().getRow() == incomingTransaction->getAddresses().getRow()))
				{
					// can piggyback on other R-C-C...C-P sequences
					if (availableCommandSlots < 1)
						return false;

					return true;			
				}

				// even STRICT ORDER allows you to look at the tail of the queue
				// to see if that's the precharge command that you need. If so,
				// insert CAS COMMAND in front of PRECHARGE COMMAND

				else if (systemConfig.getCommandOrderingAlgorithm() == STRICT_ORDER) 
				{
					//bypass_allowed = false;
					break;
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
	assert (incomingTransaction->getAddresses().getChannel() == channelID || incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION);

	Bank &destinationBank = rank[incomingTransaction->getAddresses().getRank()].bank[incomingTransaction->getAddresses().getBank()];

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
			Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
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
				bool result = currentBank->push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge()));
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
			destinationBank.push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), RAS_COMMAND));

			// command two, CAS or CAS+Precharge
			destinationBank.push(new Command(*incomingTransaction, time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge()));

			// if CAS+Precharge is unavailable
			if (!systemConfig.isAutoPrecharge())
			{
				// command three, the Precharge command
				// only one of these commands has a pointer to the original transaction, thus NULL
				destinationBank.push(new Command(*incomingTransaction, time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), PRECHARGE_COMMAND));
			}
		}
		break;

		// will either convert a CAS+P into CAS, CAS+P by appending a new command or will add a CAS before a PRE (when autoprecharge is not available)
	case CLOSE_PAGE_OPTIMIZED:

		// refresh transactions become only one command and are handled differently
		if (incomingTransaction->getType() == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
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
				bool result = currentBank->push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge()));
				assert (result);
			}
		}
		// need at least one free command slot
		// or three commands if the CAS+Precharge command is not available
		else if (destinationBank.isFull())			
		{
			return false;
		}
		// then it can be piggybacked on an existing R, C, P or R, C+P chain
		// also make sure not to starve		
		// R C1 P => R C1 C2 P
		// R C1+P => R C1 C2+P 
		// TODO: look for the last non-refresh command in the per-bank queue
		else if (destinationBank.back()
			&& destinationBank.back()->getAddress().getRow() == incomingTransaction->getAddresses().getRow() // rows match
			&& (time - destinationBank.back()->getEnqueueTime() < systemConfig.getSeniorityAgeLimit()) // not starving the last command
			&& destinationBank.back()->getCommandType() != REFRESH_ALL_COMMAND) // ends with CAS+P or PRE
		{
			// ignore other command types
			if (systemConfig.isAutoPrecharge())
			{
				// adjust existing commands
				destinationBank.back()->setAutoPrecharge(false);

				// insert new command
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge()));				
			}
			else // no CAS+P available
			{
				assert(destinationBank.back()->getCommandType() == PRECHARGE_COMMAND);

				// insert new command
				destinationBank.insert(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge()), destinationBank.size() - 1);

			}
		}
		else if ((!systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 3) ||
			(systemConfig.isAutoPrecharge() && destinationBank.freeCommandSlots() < 2))
		{
			return false;
		}
		// if there is enough space to insert the commands that this transaction becomes
		else
		{
			// command one, the RAS command to activate the row
			destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), RAS_COMMAND));

			// command two, CAS or CAS+Precharge			
			destinationBank.push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge()));

			// possible command three, Precharge
			if (!systemConfig.isAutoPrecharge())
			{				
				destinationBank.push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(), systemConfig.isAutoPrecharge(), PRECHARGE_COMMAND));
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
			Rank &currentRank = rank[incomingTransaction->getAddresses().getRank()];
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<Bank>::const_iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				if (i->isFull())
					return false;
			}
			// then add the command to all queues
			for (vector<Bank>::iterator i = currentRank.bank.begin(); i != currentRank.bank.end(); i++)
			{
				bool result = i->push(new Command(*incomingTransaction, time, systemConfig.isPostedCAS(),systemConfig.isAutoPrecharge()));
				//bool result = i->push(new Command(incomingTransaction->getAddresses(), REFRESH_ALL_COMMAND, time, incomingTransaction, false));
				assert (result);
			}
		}
		else 
		{	
			// try to do a normal open page insert on this transaction
			if (destinationBank.openPageInsert(incomingTransaction, time))
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

				// RAS command
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(), false, RAS_COMMAND));

				// CAS command
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(),false));

				// last, the precharge command
				destinationBank.push(new Command(*incomingTransaction,time,systemConfig.isPostedCAS(),false,PRECHARGE_COMMAND));
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
