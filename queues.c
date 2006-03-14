/* 
 * queues.c
 *
 * queing and deueing function calls
 */

#include "dramsim.h"

queue_t *init_q(int depth){
	queue_t *this_q;
	this_q = (queue_t *)calloc(1,sizeof(queue_t));
	
	this_q->depth = depth;
	this_q->count = 0;
	this_q->head_ptr = 0;
	this_q->tail_ptr = 0;
	this_q->entry	= (void **)calloc(depth,sizeof(void *));
	return this_q;
}

/* queue: When you add stuff, tail pointer increments, if full, return FAILURE flag */
/* I'm adding to the tail and removing from the head */

int enqueue(queue_t *this_q, void *item){
	if(this_q->count == this_q->depth){
		return FAILURE;
	} else if (item == NULL) {
		fprintf(stderr,"input transaction pointer is NULL\n");
		_exit(2);
	} else {
		this_q->count++;
		this_q->entry[this_q->tail_ptr] = item;
		this_q->tail_ptr	= (this_q->tail_ptr+1) % this_q->depth; 	/*advance tail_ptr */
		return SUCCESS;
	}
}

void *dequeue(queue_t *this_q){
	void *item;
	if(this_q->count == 0){
		return NULL;
	} else {
		this_q->count--;
		item = this_q->entry[this_q->head_ptr]; 
		this_q->head_ptr	= (this_q->head_ptr+1) % this_q->depth; 	/*advance head_ptr */
		return item;
	}
}

/* event enqueue is the same as enqueue, except that the event insertion is placed into an ordered list
 * ordered from the oldest (smallest timestamp) to youngest (largest time), with the oldest at the head of the queue
 * The event queue is thus automatically ordered in terms of time.
 */

int event_enqueue(queue_t *this_q, event_t *input_e){
	int	inserted;
	int	offset;
	int	temp_offset;
	event_t *temp_ptr;
	if(this_q->count == this_q->depth){
		return FAILURE;
	} else if (input_e == NULL) {
		fprintf(stderr,"input event pointer is NULL\n");
		_exit(2);
	} else {
		inserted = FALSE;
		for(offset = 0; (offset < this_q->count) && (inserted == FALSE); offset++){
			temp_ptr = (event_t *)(this_q->entry[(this_q->head_ptr+offset) % this_q->depth]);
			if(temp_ptr->time > input_e->time){	/* this guy's timestamp is larger (younger), insert ahead of it */
				for(temp_offset = this_q->count ; temp_offset > offset ; temp_offset--){
					this_q->entry[(this_q->head_ptr + temp_offset) % this_q->depth] = 
						this_q->entry[(this_q->head_ptr + temp_offset - 1) % this_q->depth];
				}
				inserted = TRUE;
				this_q->count++;
				this_q->entry[(this_q->head_ptr + offset) % this_q->depth] = input_e;
				this_q->tail_ptr	= (this_q->tail_ptr+1) % this_q->depth; 	/*advance tail_ptr */
			}
		}
		if(inserted == FALSE){		/* went through list, nothing older (smaller), insert at tail */
			this_q->count++;
			this_q->entry[this_q->tail_ptr] = input_e;
			this_q->tail_ptr	= (this_q->tail_ptr+1) % this_q->depth; 	/*advance tail_ptr */
		}
		return SUCCESS;
	}
}

int q_count(queue_t *this_q){
	return this_q->count;
}

int q_freecount(queue_t *this_q){
	return this_q->depth - this_q->count;
}

/* read from head of queue + offset, if offset is 0, this returns head of queue */

void *q_read(queue_t *this_q, int offset){
	int index;
	if((offset >= this_q->count) || (offset < 0)){
		return NULL;
	} else {
		index	= (this_q->head_ptr+offset) % this_q->depth; 
		return (this_q->entry[index]);
	}
}

/* this function makes this queue a non-FIFO queue.  Allows insertion in middle */

int q_insert(queue_t *this_q, void *item, int offset){
	int i;
	if(this_q->count == this_q->depth){
		return FAILURE;
	} else if (item == NULL) {
		fprintf(stderr,"input transaction pointer is NULL\n");
		_exit(2);
	} else {
		for(int i=this_q->count-1; i>= offset;i++){		/* move everything back by one unit */
			this_q->entry[(this_q->head_ptr+i+1) % this_q->depth] = this_q->entry[(this_q->head_ptr+i) % this_q->depth];
		}
		this_q->count++;
		this_q->entry[(this_q->head_ptr+offset) % this_q->depth] = item;
		this_q->tail_ptr	= (this_q->tail_ptr+1) % this_q->depth; 	/*advance tail_ptr */
		return SUCCESS;
	}
}


void *acquire_item(queue_t *this_q, int type){
	void *item;

	if((item = dequeue(this_q)) == NULL){
		if(type == COMMAND){
			item = (void *)calloc(1,sizeof(command_t));
		} else if (type == TRANSACTION){
			item = (void *)calloc(1,sizeof(transaction_t));
		} else if (type == EVENT){
			item = (void *)calloc(1,sizeof(event_t));
		} else {
			fprintf(stderr,"unknown type requested, expecting command, transaction, or event\n");
			_exit(2);
		}
	} 
	return item;
}

/* release item into pool */

void release_item(queue_t *this_q, void *item, int type){
	if(enqueue(this_q, item) == FAILURE){
		if(type == COMMAND){
			free((command_t *)item);
		} else if(type == TRANSACTION){
			free((transaction_t *)item);
		} else if(type == EVENT){
			free((transaction_t *)item);
		} else {
			fprintf(stderr,"Unknown type, item not freed. Memory Leak here\n YO!\n");
		}
	}
}


