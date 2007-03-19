#include <iostream>
#include <assert.h>

#include "dramSystem.h"

using namespace std;

bool dramSystem::checkForAvailableCommandSlots(const transaction *trans) const
{
	const queue<command> *bank_q = &(channel[trans->addr.chan_id].getRank(trans->addr.rank_id).bank[trans->addr.bank_id].per_bank_q);

	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	if (system_config.row_buffer_management_policy == CLOSE_PAGE)
	{
		int empty_command_slot_count = bank_q->freecount();

		// refresh transactions become only one command and are handled differently
		if (trans->type == AUTO_REFRESH_TRANSACTION)
		{
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<bank_c>::const_iterator i = channel[trans->addr.chan_id].getRank(trans->addr.rank_id).bank.begin();
				i != channel[trans->addr.chan_id].getRank(trans->addr.rank_id).bank.end();
				i++)
			{
				if (i->per_bank_q.freecount() < 1)
					return false;
			}
		}
		// every transaction translates into at least two commands
		else if (empty_command_slot_count < 2)
		{
			return false;
		}
		// or three commands if the CAS+Precharge command is not available
		else if ((system_config.auto_precharge == false) && (empty_command_slot_count < 3))
		{
			return false;
		}
	}
	// open page systems may, in the best case, add a CAS command to an already open row
	// closing the row and precharging may be delayed
	else if (system_config.row_buffer_management_policy == OPEN_PAGE)
	{
		int queued_command_count = bank_q->get_count();
		int empty_command_slot_count = bank_q->freecount();
		
		// look in the bank_q and see if there's a precharge for this row
		bool bypass_allowed = true;
		// go from tail to head
		for (int tail_offset = queued_command_count -1 ;(tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
		{	
			command *temp_c = bank_q->read(tail_offset);

			// goes right before the PRE command to ensure that the original order is preserved
			if ((temp_c->getCommandType() == PRECHARGE_COMMAND) &&
				(temp_c->getAddress().row_id == trans->addr.row_id))
			{
				// can piggyback on other R-C-C...C-P sequences
				if (empty_command_slot_count < 1)
					return false;

				return true;			
			}

			// even STRICT ORDER allows you to look at the tail of the queue
			// to see if that's the precharge command that you need. If so,
			// insert CAS COMMAND in front of PRECHARGE COMMAND

			if ((system_config.command_ordering_algorithm == STRICT_ORDER) 
				|| ((int)(channel[trans->addr.chan_id].get_time() - temp_c->getEnqueueTime()) > system_config.seniority_age_limit))
			{
				bypass_allowed = false;
			}
		}
		if (empty_command_slot_count < 3)
		{
			return false;
		}
	}
	else
	{
		cerr << "Unhandled row buffer management policy" << endl;
		return FAILURE;
	}
	return true;
}

/// <summary>
/// Converts a transaction in a particular channel into commands,
/// then inserts them in the bank queues.
/// Functionality similar to that of the memory controller moving between the
/// transaction queue and the per bank command queues 
/// </summary>
enum input_status_t dramSystem::transaction2commands(transaction *this_t) 
{
	if (this_t == NULL)
	{
		assert(this_t != NULL);
		return FAILURE;
	}
	queue<command> *bank_q = &(channel[this_t->addr.chan_id].getRank(this_t->addr.rank_id).bank[this_t->addr.bank_id].per_bank_q);
	
	// with closed page, all transactions convert into one of the following:
	// RAS, CAS, Precharge
	// RAS, CAS+Precharge
	if (system_config.row_buffer_management_policy == CLOSE_PAGE)
	{
		int empty_command_slot_count = bank_q->freecount();

		// refresh transactions become only one command and are handled differently
		if (this_t->type == AUTO_REFRESH_TRANSACTION)
		{
			// check to see if every per bank command queue has room for one command
			rank_c &rank = channel[this_t->addr.chan_id].getRank(this_t->addr.rank_id);
			// make sure that there is room in all the queues for one command
			// refresh commands refresh a row, but kill everything currently in the sense amps
			// therefore, we need to make sure that the refresh commands happen when all banks
			// are available
			for (vector<bank_c>::iterator i = rank.bank.begin(); i != rank.bank.end(); i++)
			{
				if (i->per_bank_q.freecount() < 1)
					return FAILURE;
			}
			// then add the command to all queues
			for (vector<bank_c>::iterator i = rank.bank.begin(); i != rank.bank.end(); i++)
			{
				i->per_bank_q.enqueue(new command(this_t->addr, REFRESH_ALL_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas));
			}
		}
		// every transaction translates into at least two commands
		else if (empty_command_slot_count < 2)
		{
			return FAILURE;
		}
		// or three commands if the CAS+Precharge command is not available
		else if ((system_config.auto_precharge == false) && (empty_command_slot_count < 3))
		{
			return FAILURE;
		}
		// if there is enough space to insert the commands that this transaction becomes
		else
		{
			// command one, the RAS command to activate the row
			bank_q->enqueue(new command(this_t->addr,RAS_COMMAND,channel[this_t->addr.chan_id].get_time(),NULL,system_config.posted_cas));

			// command two, CAS or CAS+Precharge

			// if CAS+Precharge is unavailable
			if (system_config.auto_precharge == false)
			{
				switch (this_t->type)
				{
				case WRITE_TRANSACTION:					
					bank_q->enqueue(new command(this_t->addr,CAS_WRITE_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q->enqueue(new command(this_t->addr,CAS_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas));
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->type;
					exit(-8);
					break;
				}				

				// command three, the Precharge command
				// only one of these commands has a pointer to the original transaction, thus NULL
				bank_q->enqueue(new command(this_t->addr,PRECHARGE_COMMAND,channel[this_t->addr.chan_id].get_time(),NULL,system_config.posted_cas));
			}
			else // precharge is implied, only need two commands
			{
				switch(this_t->type)
				{
				case WRITE_TRANSACTION:
					bank_q->enqueue(new command(this_t->addr,CAS_WRITE_AND_PRECHARGE_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas));
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					bank_q->enqueue(new command(this_t->addr,CAS_AND_PRECHARGE_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas));
					break;
				case PER_BANK_REFRESH_TRANSACTION:
					bank_q->enqueue(new command(this_t->addr,PRECHARGE_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas));
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->type;
					exit(-8);
					break;
				}
			}
		}
	}

	// open page systems may, in the best case, add a CAS command to an already open row
	// closing the row and precharging may be delayed
	else if (system_config.row_buffer_management_policy == OPEN_PAGE)
	{
		int queued_command_count = bank_q->get_count();
		int empty_command_slot_count = bank_q->freecount();
		// look in the bank_q and see if there's a precharge for this row
		bool bypass_allowed = true;
		// go from tail to head
		for(int tail_offset = queued_command_count -1 ;(tail_offset >= 0) && (bypass_allowed == true); --tail_offset)
		{	
			command *temp_c = bank_q->read(tail_offset);

			if (temp_c->getCommandType() == PRECHARGE_COMMAND) // found a precharge command
			{
				if (temp_c->getAddress().row_id == this_t->addr.row_id) // same row, insert here 
				{
					if (empty_command_slot_count < 1)
						return FAILURE;

					if (this_t->type == WRITE_TRANSACTION)
						bank_q->insert(new command(this_t->addr,CAS_WRITE_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas,this_t->length), tail_offset);	/* insert at this location */						
					else if (this_t->type == READ_TRANSACTION)
						bank_q->insert(new command(this_t->addr,CAS_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas,this_t->length), tail_offset);	/* insert at this location */
					
					return SUCCESS;
				}
			}

			// even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge
			// command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND

			if ((system_config.command_ordering_algorithm == STRICT_ORDER) 
				|| ((int)(channel[this_t->addr.chan_id].get_time() - temp_c->getEnqueueTime()) > system_config.seniority_age_limit))
			{
				bypass_allowed = false;
			}
		}
		if (empty_command_slot_count < 3)
		{
			return FAILURE;
		}

		bank_q->enqueue(new command(this_t->addr,RAS_COMMAND,channel[this_t->addr.chan_id].get_time(),NULL,system_config.posted_cas,this_t->length));

		if (this_t->type == WRITE_TRANSACTION)
		{
			bank_q->enqueue(new command(this_t->addr,CAS_WRITE_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas,this_t->length));
		}
		else if ((this_t->type == READ_TRANSACTION) || (this_t->type == IFETCH_TRANSACTION))
		{
			bank_q->enqueue(new command(this_t->addr,CAS_COMMAND,channel[this_t->addr.chan_id].get_time(),this_t,system_config.posted_cas,this_t->length));
		}
		else
		{
			cerr << "Unhandled transaction type: " << this_t->type;
			exit(-8);
		}

		// last, the precharge command
		bank_q->enqueue(new command(this_t->addr,PRECHARGE_COMMAND,channel[this_t->addr.chan_id].get_time(),NULL,system_config.posted_cas,this_t->length));
	}
	else
	{
		cerr << "Unhandled row buffer management policy" << endl;
		return FAILURE;
	}
	return SUCCESS;
}
