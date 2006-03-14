#include "dramsim.h"

command_t *get_next_command(dram_system_t *dram_system, int chan_id){
	dram_controller_t	*channel;
	dram_system_configuration_t     *config;
	dram_command_algorithm_t        *algorithm;
	queue_t 	*free_command_pool;
	queue_t 	*history_q;
	queue_t 	*candidate_bank_q;
	queue_t 	*temp_q;
	command_t	*last_c;
	command_t	*temp_c;
	command_t	*candidate_c;
	command_t	*next_c;
	command_t	*ptr_c;
	int		offset;
	int		tail_offset;
	int		oldest_rank_id;
	int		oldest_bank_id;
	int		rank_id;
	int		bank_id;
	int		section_id;
	int		transaction_type;
	int		candidate_found;
	int		start_bank_id;
	int		debug;
	int		count;
	int		i,j,k;
	int		candidate_count;
	tick_t		oldest_command_time;

	debug 		= FALSE;

	channel		= &(dram_system->channel[chan_id]);
	config		= dram_system->config;
	algorithm	= dram_system->algorithm;
	free_command_pool = dram_system->free_command_pool;

	history_q = channel->history_q;
	tail_offset = q_count(history_q) - 1;
	if((last_c = q_read(history_q, tail_offset)) == NULL){		/* nothing in history q, start from rank 0, bank 0 */
		last_c = acquire_item(free_command_pool, COMMAND);
		last_c->addr.rank_id  	= config->rank_count - 1;
		last_c->addr.bank_id  	= config->bank_count - 1;
		last_c->command 	= CAS_WRITE_AND_PRECHARGE_COMMAND;
	}
	candidate_found = FALSE;

	if(config->command_ordering_algorithm == STRICT_ORDER){ /* look for oldest command, execute that */
		oldest_command_time 	= -1;
		oldest_rank_id 		= -1;
		oldest_bank_id 		= -1;
		for(rank_id = 0; rank_id< config->rank_count; rank_id++){
			for(bank_id = 0; bank_id< config->rank_count; bank_id++){
				candidate_bank_q = ((channel->rank[rank_id]).bank[bank_id]).per_bank_q;
				temp_c = (command_t *) q_read(candidate_bank_q,0);
				if(temp_c != NULL) {
					if((oldest_command_time < 0) || (oldest_command_time > temp_c->enqueue_time)){
						oldest_command_time = temp_c->enqueue_time;
						oldest_rank_id 	= rank_id;
						oldest_bank_id 	= bank_id;
					}
				}
			}
		}
		candidate_bank_q = ((channel->rank[oldest_rank_id]).bank[oldest_bank_id]).per_bank_q;
		return (command_t *) dequeue(candidate_bank_q);
	} else if(config->command_ordering_algorithm == RANK_ROUND_ROBIN){ /* alternate ranks as we go down banks */
		debug = FALSE;
		bank_id 	= last_c->addr.bank_id;
		rank_id 	= last_c->addr.rank_id;
		if(last_c->command == RAS_COMMAND){
			candidate_bank_q = ((channel->rank[last_c->addr.rank_id]).bank[last_c->addr.bank_id]).per_bank_q;
			temp_c = q_read(candidate_bank_q,0);
			if((temp_c != NULL) && 
				((temp_c->command == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->command == CAS_AND_PRECHARGE_COMMAND))){
				temp_c = dequeue(candidate_bank_q);
				return temp_c;
			} else {
				fprintf(stderr,"Serious problems. RAS not followed by CAS\n");
				_exit(2);
			}
		} else if (last_c->command == CAS_AND_PRECHARGE_COMMAND){
			transaction_type = READ_TRANSACTION;
		} else if (last_c->command == CAS_WRITE_AND_PRECHARGE_COMMAND){
			transaction_type = WRITE_TRANSACTION;
		} 
		while(candidate_found == FALSE){
			rank_id = (rank_id + 1) % config->rank_count;
			if(rank_id == 0){
				bank_id = (bank_id + 1) % config->bank_count;
				if(bank_id == 0){
					if(transaction_type == WRITE_TRANSACTION){
						transaction_type = READ_TRANSACTION;
					} else {
						transaction_type = WRITE_TRANSACTION;
					}
				}
			}
			candidate_bank_q = ((channel->rank[rank_id]).bank[bank_id]).per_bank_q;
			temp_c = (command_t *)q_read(candidate_bank_q, 0);
			if(temp_c != NULL){	
				if(config->read_write_grouping == FALSE){
					temp_c = (command_t *)dequeue(candidate_bank_q);
					return temp_c;
				} else {					/* have to follow read_write grouping considerations */
					next_c = (command_t *)q_read(candidate_bank_q, 1);	/* look at the second command */
					if (((next_c->command == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
					 	((next_c->command == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION))){
						temp_c = (command_t *)dequeue(candidate_bank_q);
						return temp_c;
					}
				}
				if(debug == TRUE) {
					fprintf(stderr,"Looked in [%d] [%d] but wrong type, We want [%d]. Candidate command type ",
							temp_c->addr.rank_id,
							temp_c->addr.bank_id,
							transaction_type);
					print_command_type(temp_c->command);
					fprintf(stderr," followed by ");
					print_command_type(next_c->command);
					fprintf(stderr,"q_count[%d]\n",q_count(candidate_bank_q));
				}
			} else if (debug == TRUE){
				fprintf(stderr,"Looked in [%d] [%d] but Q empty\n",
						rank_id,
						bank_id);
			}	
		}
	} else if(config->command_ordering_algorithm == BANK_ROUND_ROBIN){	/* keep rank id as long as possible, go round robin down a given rank */
		bank_id 	= last_c->addr.bank_id;
		rank_id 	= last_c->addr.rank_id;
		if(last_c->command == RAS_COMMAND){
			candidate_bank_q = ((channel->rank[last_c->addr.rank_id]).bank[last_c->addr.bank_id]).per_bank_q;
			temp_c = q_read(candidate_bank_q,0);
			if((temp_c != NULL) && 
				((temp_c->command == CAS_WRITE_AND_PRECHARGE_COMMAND) || (temp_c->command == CAS_AND_PRECHARGE_COMMAND))){
				temp_c = dequeue(candidate_bank_q);
				return temp_c;
			} else {
				fprintf(stderr,"Serious problems. RAS not followed by CAS\n");
				_exit(2);
			}
		} else if (last_c->command == CAS_AND_PRECHARGE_COMMAND){
			transaction_type = READ_TRANSACTION;
		} else if (last_c->command == CAS_WRITE_AND_PRECHARGE_COMMAND){
			transaction_type = WRITE_TRANSACTION;
		}
		while(candidate_found == FALSE){
			bank_id = (bank_id + 1) % config->bank_count;
			if(bank_id == 0){
				rank_id = (rank_id + 1) % config->rank_count;
				if(rank_id == 0){
					if(transaction_type == WRITE_TRANSACTION){
						transaction_type = READ_TRANSACTION;
					} else {
						transaction_type = WRITE_TRANSACTION;
					}
				}
			}
			candidate_bank_q = ((channel->rank[rank_id]).bank[bank_id]).per_bank_q;
			temp_c = (command_t *)q_read(candidate_bank_q, 0);
			if(temp_c != NULL){	
				if(config->read_write_grouping == FALSE){
					return temp_c;
				} else {					/* have to follow read_write grouping considerations */
					next_c = (command_t *)q_read(candidate_bank_q, 1);	/* look at the second command */
					if (((next_c->command == CAS_AND_PRECHARGE_COMMAND) && (transaction_type == READ_TRANSACTION)) ||
					 	((next_c->command == CAS_WRITE_AND_PRECHARGE_COMMAND) && (transaction_type == WRITE_TRANSACTION))){
						temp_c = (command_t *)dequeue(candidate_bank_q);
						return temp_c;
					}
				}
				if(debug == TRUE) {
					fprintf(stderr,"Looked in [%d] [%d] but wrong type, We want [%d] Candidate command type [%d]\n",
							temp_c->addr.rank_id,
							temp_c->addr.bank_id,
							transaction_type,
							next_c->command);
				}
			} else if (debug == TRUE){
				fprintf(stderr,"Looked in [%d] [%d] but Q empty\n",
						rank_id,
						bank_id);
			}	
		}
	} else if((config->command_ordering_algorithm == WANG_RANK_HOP) && (config->config_type == BASELINE_CONFIG)){	/* baseline*/
		while(candidate_found == FALSE){
			ptr_c = q_read(algorithm->WHCC,algorithm->WHCC_offset[0]);
			algorithm->WHCC_offset[0] = (algorithm->WHCC_offset[0] + 1) % q_count(algorithm->WHCC);
			rank_id 	= ptr_c->addr.rank_id;
			bank_id 	= ptr_c->addr.bank_id;
			candidate_bank_q = ((channel->rank[rank_id]).bank[bank_id]).per_bank_q;
			candidate_c = q_read(candidate_bank_q, 0);
	
			if((candidate_c != NULL) && (candidate_c->command == RAS_COMMAND)) {
				if(ptr_c->command == RAS_COMMAND){
					if(bank_id == 0){	/* see if this rank needs a R/W switch around */
						algorithm->transaction_type[rank_id] = set_read_write_type(channel,rank_id,config->bank_count);
					}
					next_c = q_read(candidate_bank_q, 1);
					if(((algorithm->transaction_type[rank_id] == READ_TRANSACTION) && (next_c->command = CAS_AND_PRECHARGE_COMMAND)) ||
						((algorithm->transaction_type[rank_id] == WRITE_TRANSACTION) && (next_c->command = CAS_WRITE_AND_PRECHARGE_COMMAND))){
						candidate_found = TRUE;
						candidate_c = dequeue(candidate_bank_q);
						return candidate_c;
					}
				}
			} else if((candidate_c != NULL) && (candidate_c->command == CAS_AND_PRECHARGE_COMMAND)) {
				if((ptr_c->command == CAS_COMMAND) && (algorithm->transaction_type[rank_id] == READ_TRANSACTION)){
					candidate_found = TRUE;
					candidate_c = dequeue(candidate_bank_q);
					return candidate_c;
				}
			} else if((candidate_c != NULL) && (candidate_c->command == CAS_WRITE_AND_PRECHARGE_COMMAND)) {
				if((ptr_c->command == CAS_COMMAND) && (algorithm->transaction_type[rank_id] == WRITE_TRANSACTION)){
					candidate_found = TRUE;
					candidate_c = dequeue(candidate_bank_q);
					return candidate_c;
				}
			} else {
				/*
				fprintf(stderr,"some debug message\n");
				*/
			}
		}
	} else {
		fprintf(stderr,"This configuration and algorithm combination is not supported\n");
		_exit(0);
	}
}

int	set_read_write_type(
		dram_controller_t *channel,
		int rank_id,
		int bank_count){
	int read_count,write_count,empty_count,i;
	queue_t *temp_q;
	command_t	*temp_c;

	read_count = 0;
	write_count = 0;
	empty_count = 0;
	for(i=0;i<bank_count;i++){
		temp_q = ((channel->rank[rank_id]).bank[i]).per_bank_q;
		temp_c = q_read(temp_q,1);
		if(temp_c != NULL){
			if(temp_c->command == CAS_AND_PRECHARGE_COMMAND){
				read_count++;
			} else {
				write_count++;
			}
		} else {
			empty_count++;
		}
	}
	/*
	fprintf(stderr,"Rank [%d] Read[%d] Write[%d]\n",rank_id, read_count,write_count);
	*/
	if(read_count >= write_count){
		return READ_TRANSACTION;
	} else {
		return WRITE_TRANSACTION;
	}
}

int pending_count(
		dram_controller_t *channel,
		int rank_id,
		int bank_count){
	int count,i;
	queue_t *temp_q;
	count = 0;
	for(i=0;i<bank_count;i++){
		temp_q = ((channel->rank[rank_id]).bank[i]).per_bank_q;
		count += q_count(temp_q);
	}
	return count;
}
