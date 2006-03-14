#include "dramsim.h"

/* could be getting transactions from the BIU */

transaction_t *get_next_input_transaction(dram_system_t *this_ds, input_stream_t *this_i){
  bus_event_t 	this_e;
  transaction_t	*this_t;

  if(this_i->type == RANDOM){
    this_t = get_next_random_request(this_ds,this_i);
  } else if((this_i->type == K6_TRACE) || (this_i->type == MASE_TRACE)){
    if(get_next_bus_event(this_i->trace_file_ptr,this_i->type,&(this_e)) == FAILURE){
      /* EOF reached */
      return NULL; 
    } else {
      this_t = (transaction_t *)acquire_item(this_ds->free_transaction_pool,TRANSACTION);
      /* proceed normally */
      this_t->addr.phys_addr = this_e.address; 
      convert_address(this_ds->config, &(this_t->addr));
      (this_t->event_no)++;
      this_t->type    		= this_e.attributes;
      this_t->length   	= 8;			/* assume burst length of 8 */
      this_t->arrival_time   	= this_e.timestamp;
      /* need to adjust arrival time for K6 traces to cycles */
    }
  } 
  return this_t;
}

void run_simulations(dram_system_t *this_ds, input_stream_t *this_i, simulation_parameters_t *this_sp, dram_statistics_t *stats){
  int i,EOF_reached,min_gap;
  int chan_id,rank_id,bank_id,oldest_chan_id;
  int cas_count;
  tick_t		master_time;
  tick_t		channel_time;
  transaction_t 	*input_t;
  transaction_t 	*temp_t;
  transaction_t 	*completed_t;
  command_t	*temp_c;
  queue_t		*bank_q;
  dram_system_configuration_t 	*config;
  dram_timing_specification_t 	*timing;

  EOF_reached = FALSE;
  config = this_ds->config;
  timing = this_ds->timing;
  for(i=0;(i<this_sp->request_count) && (EOF_reached == FALSE);i++){
    input_t = get_next_input_transaction(this_ds,this_i);
    if(input_t != NULL){
      collect_transaction_stats(stats, input_t);
      chan_id = input_t->addr.chan_id;
      while(enqueue((this_ds->channel[chan_id]).channel_q, input_t) != SUCCESS){
        /* tried to stuff req in channel queue, stalled, so try to drain channel queue first */
        /* and drain it completely */
        /* unfortunately, we may have to drain another channel first. */
        oldest_chan_id = find_oldest_channel(this_ds);
        temp_t = (transaction_t *) dequeue((this_ds->channel[oldest_chan_id]).channel_q);
        if(this_sp->debug_flag == TRUE){
          fprintf(stderr,"[%8d] ",oldest_chan_id);
          print_transaction(temp_t);
        }
        if(temp_t != NULL){
          rank_id = temp_t->addr.rank_id;
          bank_id = temp_t->addr.bank_id;
          bank_q = (((this_ds->channel[oldest_chan_id]).rank[rank_id]).bank[bank_id]).per_bank_q;
          while(transaction2commands(this_ds, temp_t) != SUCCESS){
            temp_c = get_next_command(this_ds,oldest_chan_id);
            min_gap = min_protocol_gap(config->config_type, &(this_ds->channel[chan_id]), temp_c, config, timing);
            if(this_sp->debug_flag == TRUE){
              fprintf(stderr,"[%8d] [%2d] ",(int)this_ds->time,min_gap);
              print_command(temp_c);
            }
            execute_command(this_ds, temp_c, min_gap);
            update_system_time(this_ds);
            completed_t = (transaction_t *) dequeue((this_ds->channel[oldest_chan_id]).completion_q);
            if(completed_t != NULL){
              /*
              print_transaction(completed_t);
              */
              release_item(this_ds->free_transaction_pool, (void *)completed_t, TRANSACTION);
            }
          }
        }
        /* randomness check
        for(rank_id = 0; rank_id < this_ds->config->rank_count;rank_id++){
        for(bank_id = 0; bank_id < this_ds->config->bank_count; bank_id++){
        fprintf(stderr,"[%d|%d|%d] ",
        (((this_ds->channel[0]).rank[rank_id]).bank[bank_id]).cas_count,
        (((this_ds->channel[0]).rank[rank_id]).bank[bank_id]).casw_count,
        (((this_ds->channel[0]).rank[rank_id]).bank[bank_id]).cas_count +
        (((this_ds->channel[0]).rank[rank_id]).bank[bank_id]).casw_count);
        }
        fprintf(stderr,"X");
        }
        fprintf(stderr,"\n");
        */
      }
    } else {
      EOF_reached = TRUE;
    }
  }
  stats->end_time = this_ds->time;
  stats->valid_transaction_count = this_sp->request_count;
}

int find_oldest_channel(dram_system_t *this_ds){
  int oldest_chan_id, chan_id;
  tick_t oldest_time;

  oldest_chan_id = 0;
  oldest_time = (this_ds->channel[0]).time;
  for(chan_id = 1; chan_id < (this_ds->config)->chan_count ; chan_id++){
    if((this_ds->channel[chan_id]).time < oldest_time){
      oldest_chan_id = chan_id;
      oldest_time = (this_ds->channel[chan_id]).time;
    }
  }
  return oldest_chan_id;
}

void update_system_time(dram_system_t *this_ds){
  int oldest_chan_id, chan_id;
  tick_t oldest_time;

  oldest_chan_id = 0;
  oldest_time = (this_ds->channel[0]).time;
  for(chan_id = 1; chan_id < (this_ds->config)->chan_count ; chan_id++){
    if((this_ds->channel[chan_id]).time < oldest_time){
      oldest_chan_id = chan_id;
      oldest_time = (this_ds->channel[chan_id]).time;
    }
  }
  this_ds->time = oldest_time;
}

/* convert transation to commands, and insert into bank queues in proper sequence if there is room */
/* need to be cleaned up to handle refresh requests */

int transaction2commands(dram_system_t *this_ds, transaction_t *this_t){		/* convert transactions to commands */  
  dram_system_configuration_t 	*config;
  tick_t		now;
  int		queued_command_count;
  int		empty_command_slot_count;
  int		chan_id;
  int		rank_id;
  int		bank_id;
  int		row_id;
  int		tail_offset;
  int		bypass_allowed;
  queue_t		*bank_q;
  queue_t		*free_command_pool;
  command_t	*free_c;
  command_t	*temp_c;

  config 		= this_ds->config;

  now 		= this_ds->time;
  chan_id 	= this_t->addr.chan_id;
  rank_id 	= this_t->addr.rank_id;
  bank_id 	= this_t->addr.bank_id;
  row_id 		= this_t->addr.row_id;
  bank_q 		= (((this_ds->channel[chan_id]).rank[rank_id]).bank[bank_id]).per_bank_q;
  free_command_pool	= this_ds->free_command_pool;
  queued_command_count 	= q_count(bank_q);
  empty_command_slot_count = q_freecount(bank_q);

  if(config->row_buffer_management_policy == CLOSE_PAGE){
    if(empty_command_slot_count < 2){
      return FAILURE;
    } else if((config->auto_precharge == FALSE) && (empty_command_slot_count < 3)){
      return FAILURE;
    } else {
      free_c = acquire_item(free_command_pool, COMMAND);
      free_c->command 	= RAS_COMMAND;
      free_c->start_time 	= this_t->arrival_time;
      free_c->enqueue_time 	= now;
      free_c->addr		= this_t->addr;		/* copy the addr stuff over */
      free_c->host_t		= NULL;
      enqueue(bank_q,free_c);

      if(config->auto_precharge == FALSE){
        free_c = acquire_item(free_command_pool, COMMAND);
        if(this_t->type == WRITE_TRANSACTION){
          free_c->command 	= CAS_WRITE_COMMAND;
        } else if (this_t->type == READ_TRANSACTION){
          free_c->command 	= CAS_COMMAND;
        }
        free_c->start_time 	= this_t->arrival_time;
        free_c->enqueue_time 	= now;
        free_c->addr		= this_t->addr;		/* copy the addr stuff over */
        free_c->posted_cas	= config->posted_cas;
        free_c->host_t		= this_t;
        free_c->length		= this_t->length;
        enqueue(bank_q,free_c);

        free_c = acquire_item(free_command_pool, COMMAND);
        free_c->command 	= PRECHARGE_COMMAND;
        free_c->start_time 	= this_t->arrival_time;
        free_c->enqueue_time 	= now;
        free_c->addr		= this_t->addr;		/* copy the addr stuff over */
        free_c->host_t		= NULL;
        enqueue(bank_q,free_c);
      } else {
        free_c = acquire_item(free_command_pool, COMMAND);
        if(this_t->type == WRITE_TRANSACTION){
          free_c->command 	= CAS_WRITE_AND_PRECHARGE_COMMAND;
        } else if (this_t->type == READ_TRANSACTION){
          free_c->command 	= CAS_AND_PRECHARGE_COMMAND;
        } else if (this_t->type == PER_BANK_REFRESH_TRANSACTION){
          free_c->command 	= PRECHARGE_COMMAND;
        }
        free_c->start_time 	= this_t->arrival_time;
        free_c->enqueue_time 	= now;
        free_c->addr		= this_t->addr;		/* copy the addr stuff over */
        free_c->posted_cas	= config->posted_cas;
        free_c->host_t		= this_t;
        free_c->length		= this_t->length;
        enqueue(bank_q,free_c);
      }
    }
  } else if(config->row_buffer_management_policy == OPEN_PAGE){
    /* look in the bank_q and see if there's a precharge for this row */
    bypass_allowed 	= TRUE;
    for(tail_offset = queued_command_count -1 ;(tail_offset >= 0) && (bypass_allowed == TRUE);tail_offset--){	/* go from tail to head */
      temp_c = (command_t *)q_read(bank_q, tail_offset);
      if(temp_c->command == PRECHARGE_COMMAND){	/* found a precharge command */
        if(temp_c->addr.row_id == this_t->addr.row_id){	/* same row, insert here */
          if(empty_command_slot_count < 1){
            return FAILURE;
          }
          free_c = acquire_item(free_command_pool, COMMAND);
          if(this_t->type == WRITE_TRANSACTION){
            free_c->command 	= CAS_WRITE_COMMAND;
          } else if (this_t->type == READ_TRANSACTION){
            free_c->command 	= CAS_COMMAND;
          }
          free_c->start_time 	= this_t->arrival_time;
          free_c->enqueue_time 	= now;
          free_c->addr		= this_t->addr;		/* copy the addr stuff over */
          free_c->host_t		= this_t;
          free_c->length		= this_t->length;
          q_insert(bank_q, (void *)free_c, tail_offset);	/* insert at this location */
          return SUCCESS;
        }
      }
      /* even STRICT ORDER allows you to look at the tail of the queue to see if that's the precharge 
      * command that you need. If so, insert CAS COMMAND in front of PRECHARGE COMMAND
      */
      if((config->command_ordering_algorithm == STRICT_ORDER) || 
        ((int)(now-temp_c->enqueue_time) > config->seniority_age_limit)){ 
          bypass_allowed = FALSE;
      }
    }
    if(empty_command_slot_count < 3){
      return FAILURE;
    }
    free_c = acquire_item(free_command_pool, COMMAND);
    free_c->command 	= RAS_COMMAND;
    free_c->start_time 	= this_t->arrival_time;
    free_c->enqueue_time 	= now;
    free_c->addr		= this_t->addr;		/* copy the addr stuff over */
    free_c->host_t		= NULL;
    enqueue(bank_q,free_c);

    free_c = acquire_item(free_command_pool, COMMAND);
    if(this_t->type == WRITE_TRANSACTION){
      free_c->command 	= CAS_WRITE_COMMAND;
    } else if (this_t->type == READ_TRANSACTION){
      free_c->command 	= CAS_COMMAND;
    }
    free_c->start_time 	= this_t->arrival_time;
    free_c->enqueue_time 	= now;
    free_c->posted_cas	= config->posted_cas;
    free_c->addr		= this_t->addr;		/* copy the addr stuff over */
    free_c->length		= this_t->length;
    enqueue(bank_q,free_c);

    free_c = acquire_item(free_command_pool, COMMAND);
    free_c->command 	= PRECHARGE_COMMAND;
    free_c->start_time 	= this_t->arrival_time;
    free_c->enqueue_time 	= now;
    free_c->addr		= this_t->addr;		/* copy the addr stuff over */
    free_c->host_t		= NULL;
    enqueue(bank_q,free_c);
  }
  return SUCCESS;
}


void execute_command(dram_system_t *this_ds, command_t *this_c, int gap){
  tick_t			now;
  tick_t			*this_ras_time;
  command_t		*temp_c;
  int 			chan_id, rank_id, bank_id,row_id;
  int			t_al;
  int			history_q_count;
  bank_t			*this_b;
  rank_t			*this_r;
  dram_controller_t	*channel;
  dram_timing_specification_t	*timing;
  dram_system_configuration_t	*config;
  transaction_t		*host_t;
  queue_t			*history_q;
  queue_t			*complete_q;

  host_t		= NULL;
  timing		= this_ds->timing;
  config		= this_ds->config;
  chan_id 	= this_c->addr.chan_id;
  rank_id 	= this_c->addr.rank_id;
  bank_id 	= this_c->addr.bank_id;
  row_id 		= this_c->addr.row_id;
  channel		= &(this_ds->channel[chan_id]);
  this_r		= &(channel->rank[rank_id]);
  this_b		= &(this_r->bank[bank_id]);
  this_r->last_bank_id = bank_id;
  if(this_c->posted_cas == TRUE){
    t_al = timing->t_al;
  } else {
    t_al = 0;
  }

  channel->time	+=	gap;
  now 		= channel->time;

  switch(this_c->command){
    case RAS_COMMAND:
      this_b->last_ras_time 	= now;
      this_b->row_id		= row_id;
      this_b->ras_count++;
      if(q_count(this_r->last_ras_times) == 4){	/* ras time history queue, per rank */ 
        this_ras_time	= (tick_t *) dequeue(this_r->last_ras_times);
      } else {
        this_ras_time 	= (tick_t *) calloc(1,sizeof(tick_t));
      }
      *this_ras_time	= now;
      enqueue(this_r->last_ras_times,(void *)this_ras_time);
      break;
    case CAS_AND_PRECHARGE_COMMAND:
      this_b->last_prec_time 	= MAX(now + t_al + timing->t_cas + timing->t_burst + timing->t_rtp,
        this_b->last_ras_time + timing->t_ras);
    case CAS_COMMAND:
      this_b->last_cas_time 	= now;
      this_r->last_cas_time	= now;
      this_b->last_cas_length	= this_c->length;
      this_r->last_cas_length	= this_c->length;
      this_b->cas_count++;
      host_t = this_c->host_t;
      host_t->completion_time	= now + timing->t_cas;
      break;
    case CAS_WRITE_AND_PRECHARGE_COMMAND:
      this_b->last_prec_time 	= MAX(now + t_al + timing->t_cwd + timing->t_burst + timing->t_wr,
        this_b->last_ras_time + timing->t_ras);
    case CAS_WRITE_COMMAND:
      this_b->last_casw_time 	= now;
      this_r->last_casw_time	= now;
      this_b->last_casw_length= this_c->length;
      this_r->last_casw_length= this_c->length;
      this_b->casw_count++;
      host_t = this_c->host_t;
      host_t->completion_time	= now;
      break;
    case RETIRE_COMMAND:
      break;
    case PRECHARGE_COMMAND:
      this_b->last_prec_time 	= now;
      break;
    case PRECHARGE_ALL_COMMAND:
      break;
    case RAS_ALL_COMMAND:
      break;
    case DRIVE_COMMAND:
      break;
    case DATA_COMMAND:
      break;
    case CAS_WITH_DRIVE_COMMAND:
      break;
    case REFRESH_ALL_COMMAND:
      this_b->last_refresh_all_time 	= now;
      break;
  }
  /* transaction complete? if so, put in completion queue */
  if(host_t != NULL) {
    if(enqueue((this_ds->channel[this_c->addr.chan_id]).completion_q, (void *) host_t) == FAILURE){
      fprintf(stderr,"Fatal error, cannot insert transaction into completion queue\n");
      fprintf(stderr,"increase execution q depth and resume. Should not occur. Check logic\n");
      _exit(2);
    }
  }

  /* record command history. Check to see if this can be removed */
  history_q	= channel->history_q;
  history_q_count = q_count(history_q);
  if(history_q_count == config->history_queue_depth){
    temp_c 	= (command_t *) dequeue(history_q);
    /*done with this command, release into pool */
    release_item(this_ds->free_command_pool, (void *)temp_c, COMMAND);
  }
  enqueue(history_q,this_c);
}
