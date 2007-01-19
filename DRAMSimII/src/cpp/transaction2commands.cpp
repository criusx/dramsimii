#include <iostream>

#include "dramSystem.h"

using namespace std;

/// <summary>
/// Converts a transaction in a particular channel into commands,
/// then inserts them in the bank queues.
/// Functionality similar to that of the memory controller moving between the
/// transaction queue and the per bank command queues 
/// </summary>
enum input_status_t dramSystem::transaction2commands(transaction *this_t) 
{
	if (this_t == NULL)
	return FAILURE;
	//cerr << this_t << endl;
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
				command *temp_c = new command;
				temp_c->addr = this_t->addr;
				temp_c->enqueue_time = time;
				temp_c->start_time = channel[this_t->addr.chan_id].get_time();
				temp_c->this_command = REFRESH_ALL_COMMAND;
				temp_c->host_t = this_t;
				i->per_bank_q.enqueue(temp_c);
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
			command *free_c = new command;

			free_c->this_command = RAS_COMMAND;
			// start time of the command is the same as the arrival time of the transaction
			// because it is assumed that the movement from the transaction queue to the per
			// bank queue is instantaneous
			free_c->start_time = this_t->arrival_time;
			free_c->enqueue_time = time;
			free_c->addr = this_t->addr; // copy the addr stuff over
			free_c->host_t = NULL;

			bank_q->enqueue(free_c);

			// command two, CAS or CAS+Precharge
			free_c = new command;

			// if CAS+Precharge is available
			if(system_config.auto_precharge == false)
			{
				switch (this_t->type)
				{
				case WRITE_TRANSACTION:
					free_c->this_command = CAS_WRITE_COMMAND;
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					free_c->this_command = CAS_COMMAND;
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->type;
					exit(-8);
					break;
				}

				free_c->start_time = this_t->arrival_time;
				free_c->enqueue_time = time;
				free_c->addr = this_t->addr;
				free_c->posted_cas = system_config.posted_cas;
				free_c->host_t = this_t;				
				free_c->length = this_t->length;
				bank_q->enqueue(free_c);

				// command three, the Precharge command
				free_c = new command;
				free_c->this_command = PRECHARGE_COMMAND;
				free_c->start_time = this_t->arrival_time;
				free_c->enqueue_time = time;
				free_c->addr = this_t->addr;
				free_c->host_t = NULL; // only one of these commands has a pointer to the original transaction
				bank_q->enqueue(free_c);
			}
			else // precharge is implied, only need two commands
			{
				switch(this_t->type)
				{
				case WRITE_TRANSACTION:
					free_c->this_command = CAS_WRITE_AND_PRECHARGE_COMMAND;
					break;
				case READ_TRANSACTION:
				case IFETCH_TRANSACTION:
					free_c->this_command = CAS_AND_PRECHARGE_COMMAND;
					break;
				case PER_BANK_REFRESH_TRANSACTION:
					free_c->this_command = PRECHARGE_COMMAND;
					break;
				default:
					cerr << "Unhandled transaction type: " << this_t->type;
					exit(-8);
					break;
				}

				free_c->start_time = this_t->arrival_time;
				free_c->enqueue_time = time;
				free_c->addr = this_t->addr;		/* copy the addr stuff over */
				free_c->posted_cas = system_config.posted_cas;
				free_c->host_t = this_t;				
				free_c->length = this_t->length;

				bank_q->enqueue(free_c);
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

			if(temp_c->this_command == PRECHARGE_COMMAND) // found a precharge command
			{
				if(temp_c->addr.row_id == this_t->addr.row_id) // same row, insert here 
				{
					if(empty_command_slot_count < 1)
						return FAILURE;

					command *free_c = new command;

					if(this_t->type == WRITE_TRANSACTION)
						free_c->this_command = CAS_WRITE_COMMAND;
					else if (this_t->type == READ_TRANSACTION)
						free_c->this_command = CAS_COMMAND;
					free_c->start_time = this_t->arrival_time;
					free_c->enqueue_time = time;
					free_c->addr = this_t->addr; /// copy the addr stuff over
					free_c->host_t = this_t;

					free_c->length = this_t->length;

					bank_q->insert(free_c, tail_offset);	/* insert at this location */
					return SUCCESS;
				}
			}

			// even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge
			// command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND

			if ((system_config.command_ordering_algorithm == STRICT_ORDER) 
				|| ((int)(time-temp_c->enqueue_time) > system_config.seniority_age_limit))
			{
				bypass_allowed = false;
			}
		}
		if (empty_command_slot_count < 3)
		{
			return FAILURE;
		}
		command *free_c = new command;

		free_c->this_command = RAS_COMMAND;
		free_c->start_time = this_t->arrival_time;
		free_c->enqueue_time = time;
		free_c->addr = this_t->addr;		// copy the addr stuff over
		free_c->host_t = NULL;

		bank_q->enqueue(free_c);

		free_c = new command;
		if (this_t->type == WRITE_TRANSACTION)
		{
			free_c->this_command = CAS_WRITE_COMMAND;
		}
		else if (this_t->type == READ_TRANSACTION)
		{
			free_c->this_command = CAS_COMMAND;
		}
		else
		{
			cerr << "Unhandled transaction type: " << this_t->type;
			exit(-8);
		}

		free_c->start_time = this_t->arrival_time;
		free_c->enqueue_time = time;
		free_c->posted_cas = system_config.posted_cas;
		free_c->addr = this_t->addr;		// copy the addr stuff over
		free_c->length = this_t->length;
		// FIXME: not initializing the host_t value?
		bank_q->enqueue(free_c);

		free_c = new command;
		free_c->this_command = PRECHARGE_COMMAND;
		free_c->start_time = this_t->arrival_time;
		free_c->enqueue_time = time;
		free_c->addr = this_t->addr;		// copy the addr stuff over
		free_c->host_t = NULL;
		bank_q->enqueue(free_c);
	}
	else
	{
		cerr << "Unhandled row buffer management policy" << endl;
		return FAILURE;
	}
	return SUCCESS;
}
