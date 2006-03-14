/** gaussian number generator ganked from the internet ->
 * http://www.taygeta.com/random/gaussian.html
 */

#include "dramsim.h"
#define			PI		3.1415926535

double box_muller (double m, double s) {
	double x1, x2, w, y1;
	static double y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * drand48() - 1.0;
			x2 = 2.0 * drand48() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}
	return floor(m+y1*s);
}

double gammaln(double xx) {
	double x, y, tmp, ser;
	static double cof[6] = {76.18009172947146, -86.50532032941677,
				24.01409824083091, -1.231739572450155,
				0.1208650973866179e-2, -0.5395239384953e-5};
	int j;

	y = x = xx;
	tmp = x + 5.5;
	tmp -= (x+0.5) * log(tmp);
	ser = 1.000000000190015;
	for (j=0; j<=5; j++) ser += cof[j]/++y;
	return -tmp + log(2.5066282746310005 * ser/x);
						
}
/**
 * from the book "Numerical Recipes in C: The Art of Scientific Computing"
 **/

double poisson_rng (double xm) {	
	static double sq, alxm, g, oldm = (-1.0);
    double em, t, y;

	if (xm < 12.0) {
		if (xm != oldm) {
			oldm = xm;
			g = exp(-xm);
		}
		em = -1;
		t = 1.0;
		do {
			++em;
			t *= drand48();
		} while (t > g);		
	}
	else {
		if (xm != oldm) {
			oldm = xm;
			sq = sqrt(2.0 * xm);
			alxm = log(xm);
			g = xm * alxm-gammaln(xm + 1.0);
		}
		do {
			do {
				y = tan(PI * drand48());
				em = sq * y + xm;
			} while (em < 0.0);
			em = floor(em);
			t = 0.9 * (1.0 + y*y) * exp(em * alxm - gammaln(em+1.0) -g);
		} while (drand48() > t);
	}
	return em;
}

transaction_t *get_next_random_request(dram_system_t *this_ds, input_stream_t *this_i){
	int chan_id;
	int rank_id;
	int bank_id;
	int row_id;
	int arrival;
	int time_gap;
	dram_system_configuration_t *config;
	transaction_t *this_t;

	config = this_ds->config;
	this_t = (transaction_t *)acquire_item(this_ds->free_transaction_pool,TRANSACTION);

	if(this_i->type == RANDOM){
		/* check against last transaction to see what the chan_id was, and whether we need to change channels or not */
		if (this_i->chan_locality < drand48()){
			this_t->addr.chan_id = 
				(this_t->addr.chan_id + 1 + (int) (drand48() * (config->chan_count - 1))) % config->chan_count;
		}
		/* check against the rank_id of the last transaction to the newly selected channel to see if we need to change the rank_id
		   or keep to this rank_id */
		chan_id = this_t->addr.chan_id;
		rank_id = (this_ds->channel[chan_id]).last_rank_id;
		if (this_i->rank_locality < drand48()){
			this_t->addr.rank_id = 
				(rank_id + 1 + (int) (drand48() * (double)(config->rank_count - 1))) % config->rank_count;
			rank_id = this_t->addr.rank_id ;
		} else {
			this_t->addr.rank_id = rank_id;
		}
		bank_id = ((this_ds->channel[chan_id]).rank[rank_id]).last_bank_id;
		if (this_i->bank_locality < drand48()){
			this_t->addr.bank_id = 
				(bank_id + 1 + (int) (drand48() * (double)(config->bank_count - 1))) % config->bank_count;
			bank_id = this_t->addr.bank_id ;
		} else {
			this_t->addr.bank_id = bank_id;
		}
		row_id = (((this_ds->channel[chan_id]).rank[rank_id]).bank[bank_id]).row_id;
		if (this_i->row_locality < drand48()){
			this_t->addr.row_id = 
				(row_id + 1 + (int) (drand48() * (double)(config->row_count - 1))) % config->row_count;
			row_id = this_t->addr.row_id ;
		} else {
			this_t->addr.row_id = row_id;
		}
		if(this_i->read_percentage < drand48()){
			this_t->type = READ_TRANSACTION;
		} else {
			this_t->type = WRITE_TRANSACTION;
		}
		if(this_i->short_burst_ratio > drand48()){
			this_t->length = 4;
		} else {
			this_t->length = 8;
		}
		
		arrival = FALSE;
		time_gap = 0;
		while(arrival == FALSE){
			if( drand48() > this_i->arrival_thresh_hold){                   /* interarrival probability function */
				//gaussian distribution function
				if (this_i->interarrival_distribution_model == GAUSSIAN_DISTRIBUTION) {
					this_i->arrival_thresh_hold = 1.0 - (1.0 / box_muller((double)this_i->average_interarrival_cycle_count, 10));
				}
				//poisson distribution function
				else if (this_i->interarrival_distribution_model == POISSON_DISTRIBUTION) {
					this_i->arrival_thresh_hold = 1.0 - (1.0 / poisson_rng((double)this_i->average_interarrival_cycle_count));
				}
				arrival = TRUE;
			} else {
				time_gap++;
			}
		}

		this_i->time += time_gap;
		this_t->arrival_time = this_i->time;
		this_t->addr.col_id = 0;
	}
	return this_t;
}
