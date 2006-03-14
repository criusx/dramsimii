/*
 * 
 *
 */
#include 	<stdio.h>

#ifndef WIN32

#include 	<unistd.h>
#include 	<sys/time.h>

#else
#define srand48(a) srand(a)
#define drand48 rand

#endif

#include 	<stdlib.h>
#include 	<string.h>

#include 	<math.h>
#define 	MIN(a,b) 		((a) < (b) ? (a) : (b))
#define 	MAX(a,b) 		((a) > (b) ? (a) : (b))
#define 	ABS(a) 			((a) < 0 ? (-a) : (a))

#define 	tick_t			double

#define		TRUE			1
#define		FALSE			0
#define		INVALID			-1

/* This section defines the refresh policy */

enum refresh_policy_t {
	NO_REFRESH,			
	BANK_CONCURRENT,			
	BANK_STAGGERED_HIDDEN		
};

/* This section defines the row buffer management policy */

enum row_buffer_policy_t {
	AUTO_PAGE,		/* same as OPEN PAGE, but close page after timer expires */
	OPEN_PAGE,		/* keep page open indefinitely */
	CLOSE_PAGE
};

/* This section defines the address mapping scheme */

enum address_mapping_scheme_t {
	OPEN_PAGE_BASELINE,
	CLOSE_PAGE_BASELINE,
	BURGER_BASE_MAP,
	SDRAM_HIPERF_MAP,
	SDRAM_BASE_MAP,
	INTEL845G_MAP,
	SDRAM_CLOSE_PAGE_MAP
};

enum system_configuration_type_t {
	BASELINE_CONFIG,		/* direct control. 1 or 2 ranks in DDR3 */
	FBD_CONFIG			/* fully buffered DIMMS */
};

/* we can define various algorithms previously explored by Rixner, McKee et al. here. */

enum ordering_algorithm_t {
	STRICT_ORDER,
	RANK_ROUND_ROBIN,	/* alternate ranks */
	BANK_ROUND_ROBIN,	/* keep same ranks as long as possible, go down banks */
	WANG_RANK_HOP		/* Patented stuff. davewang202@yahoo.com  ;) */
};

enum input_type_t {
	K6_TRACE,
	MASE_TRACE,
	RANDOM,
	MASE,
	ALPHASIM,
	GEMS
};

enum input_status_t {
	SUCCESS,
	FAILURE
};

enum dram_type_t {
	SDRAM,
	DDR,
	DDR2,
	DDR3,
	DRDRAM			
};

enum transaction_type_t {
	IFETCH_TRANSACTION,
	WRITE_TRANSACTION,   
	READ_TRANSACTION,    
	PREFETCH_TRANSACTION,        
	AUTO_REFRESH_TRANSACTION, 
	PER_BANK_REFRESH_TRANSACTION,
	AUTO_PRECHARGE_TRANSACTION 
};

enum queue_type_t {
	TRANSACTION,
	COMMAND,
	EVENT
};

enum command_type_t {
	RAS_COMMAND,
	CAS_COMMAND,
	CAS_AND_PRECHARGE_COMMAND,
	CAS_WRITE_COMMAND, 
	CAS_WRITE_AND_PRECHARGE_COMMAND,
	RETIRE_COMMAND,			/* ?? */
	PRECHARGE_COMMAND, 
	PRECHARGE_ALL_COMMAND, 		/* ?? */
	RAS_ALL_COMMAND,		/* ?? */
	DRIVE_COMMAND,			/* ?? */
	DATA_COMMAND,			/* ?? */
	CAS_WITH_DRIVE_COMMAND,		/* ?? */
	REFRESH_ALL_COMMAND
};

enum fileio_token_t {
	dram_type_token,
	datarate_token,
	clock_granularity_token,
	row_buffer_management_policy_token,
	auto_precharge_token,
	addr_mapping_scheme_token,
	transaction_ordering_policy_token,
	command_ordering_algorithm_token,
	refresh_policy_token,
	refresh_time_token,
	channel_width_token,
	posted_cas_token,
	chan_count_token,
	rank_count_token,
	bank_count_token,
	row_count_token,
	col_count_token,
	col_size_token,
	row_size_token,
	cacheline_size_token,
	per_bank_queue_depth_token,
	t_al_token,
	t_burst_token,
	t_cas_token,
	t_cac_token,
	t_cmd_token,
	t_cwd_token,
	t_faw_token,
	t_ras_token,
	t_rc_token,
	t_rcd_token,
	t_rfc_token,
	t_rp_token,
	t_rl_token,
	t_rrd_token,
	t_rtp_token,
	t_rtrs_token,
	t_wr_token,
	t_wtr_token,
	comment_token,
	PICOSECOND,
	NANOSECOND,
	MICROSECOND,
	MILLISECOND,
	SECOND,
	MEM_RD,
	MEM_WR,
	FETCH,
	IO_RD,
	IO_WR,
	BOFF,
	LOCK_RD,
	LOCK_WR,
	INT_ACK,
	unknown_token,
	deprecated_ignore_token
};

enum distribution_type_t {
	UNIFORM_DISTRIBUTION,
	GAUSSIAN_DISTRIBUTION,
	POISSON_DISTRIBUTION,
	NORMAL_DISTRIBUTION
};

enum event_type_t {
	TRANSACTION_ENQUEUE,		/* put transaction into channel queue */
	DRAM_COMMAND_ENQUEUE,		/* put DRAM command into per-bank queue */
	DRAM_COMMAND_EXECUTE		/* schedule DRAM command for execution */
};

/***********************************************************************
 Here are the data structures for the memory system.
 ***********************************************************************/

typedef struct addresses_t {
	int		virt_addr;	
	int		phys_addr;
	int		chan_id;		/* logical channel id */
	int		rank_id;		/* device id */
	int		bank_id;
	int		row_id;
	int		col_id;			/* column address */
} addresses_t;

typedef struct command_t {
	int             command;                        /* which command is this? */
	tick_t          start_time;                     /* when did the transaction start? */
	tick_t		enqueue_time;			/* when did it make it into the queues? */
	tick_t  	completion_time;
	addresses_t	addr;

	struct transaction_t	*host_t;	/* backward pointer to the original transaction */
	/** Added list of completion times in order to clean up code */
	tick_t  link_comm_tran_comp_time;
	tick_t  amb_proc_comp_time;
	tick_t  dimm_comm_tran_comp_time;
	tick_t  dram_proc_comp_time;
	tick_t  dimm_data_tran_comp_time;
	tick_t  amb_down_proc_comp_time;
	tick_t  link_data_tran_comp_time;

	/* Variables added for the FB-DIMM */
	int             bundle_id;              /* Bundle into which command is being sent - Do we need this ?? */
	unsigned int    tran_id;                /*      The transaction id number */
	int             data_word;              /* Which portion of data is returned i.e. entire cacheline or fragment thereof which portions are being sent*/
	int     	data_word_position;     /** Which part of the data transmission are we doing : postions include FIRST , MIDDLE, LAST **/
	int             refresh;                /** This is used to determine if the ras/prec are part of refresh **/
	int             posted_cas;             /** This is used to determine if the ras + cas were in the same bundle **/
	int		length;
}command_t;

typedef struct transaction_t {
	int		event_no;
	int		type;			/* transaction type */
	int		status;
	int		length;			/* how long? */
	tick_t		arrival_time;		/* time when first seen by memory controller in DRAM ticks */
	tick_t		completion_time;	/* time when transaction has completed in DRAM ticks */
	addresses_t	addr;
} transaction_t;

/*  bus_event_t is used with the file I/O interface to get bus events from input trace files */

typedef struct bus_event_t {   /* 6 DWord per event */
        int     attributes;     /* read/write/Fetch type stuff.  Not overloaded with other stuff */
        int     address;        /* assume to be <= 32 bits for now */
        double  timestamp;      /* time stamp will now have a large dynamic range, but only 53 bit precision */
} bus_event_t;

typedef struct queue_t {
	int 		depth;			/* capacity of the queue */
	int		count;
	int		head_ptr;
	int		tail_ptr;
	void		**entry;
} queue_t;

typedef struct bank_t {
	tick_t		last_ras_time;		/* when did last ras start? */
	tick_t		last_cas_time;		/* when did last cas start? */
	tick_t		last_casw_time;		/* when did last cas write start? */
	tick_t		last_prec_time;		/* when did last prec start? */
	tick_t		last_refresh_all_time;	/* must respect t_rfc. concurrent refresh takes time */
	int		last_cas_length;
	int		last_casw_length;
	int		row_id;			/* if the bank is open, what is the row id? */
	queue_t		*per_bank_q;		/* per bank queue */
	/* stats */ 
	int		ras_count;
	int		cas_count;
	int		casw_count;
} bank_t;

typedef struct rank_t {
	int		bank_count;
	bank_t		*bank;
	int		last_bank_id;		/* id of the last accessed bank of this rank*/
	tick_t		last_refresh_time;
	tick_t		last_cas_time;
	tick_t		last_casw_time;
	int		last_cas_length;
	int		last_casw_length;
	queue_t		*last_ras_times;	/* ras time queue. make tfaw computation easier */
} rank_t;

typedef struct dram_controller_t {
	tick_t		time;				/* channel time, allow for channel concurrency */
	rank_t 		*rank;  		
	int		refresh_row_index;		/* the row index to be refreshed */
	tick_t		last_refresh_time;		/* tells me when last refresh was done */
	int		last_rank_id;			/* id of the last accessed rank of this channel */
	queue_t		*channel_q;			/* unified system queue */
	queue_t		*refresh_q;
	queue_t		*history_q;			/* what were the last N commands to this channel? */
	queue_t		*completion_q;			/* completed_q, can send status back to BIU */
}dram_controller_t;

/* 
 *	Contains all of the configuration information.  
 */

typedef struct dram_system_configuration_t {
	FILE		*spd_file_ptr;
	int 		config_type;
	int		dram_type;
	double		datarate;
        int             row_buffer_management_policy;   /* row buffer management policy? OPEN/CLOSE, etc */
	int		auto_precharge;			/* issue cas and prec separately or together? */
	int		addr_mapping_scheme;		/* addr mapping scheme for physical to DRAM addr */
	int		command_ordering_algorithm;	/* strict or round robin? */
	int		read_write_grouping;
	int		seniority_age_limit;
	int		refresh_policy;			/*  */
	int		posted_cas;			/* TRUE/FALSE */
	int		clock_granularity;		

	int		chan_count;			/* How many logical channels are there ? */
	int 		rank_count;			/* How many ranks are there per channel ? */
	int		bank_count;			/* How many banks per device? */
	int		row_count;			/* how many rows per bank? */
	int		cachelines_per_row;		/* dependent variable */

	int		col_count;			/* Hwo many columns per row? */
	int		col_size;
	int		row_size;			/* how many bytes per row? (across one rank) */
	int		cacheline_size;			/* 32/64/128 etc */
	int		per_bank_queue_depth;		/* how deep? per bank */
	int		history_queue_depth;		/* keep track of per channel command history */
	int		completion_queue_depth;		/* keep track of per channel command history */
	int		transaction_queue_depth;	/* input transaction queue depth */
	int		event_queue_depth;		/* pending event queue depth */
	int		refresh_time;			/* loop time of refresh */
	int		refresh_queue_depth;		/* loop time of refresh */
	double		short_burst_ratio;
	double		read_percentage;
} dram_system_configuration_t;
							/* t_pp (min prec to prec of any bank) ignored  */
typedef struct dram_timing_specification_t {
	int		t_al;				/* additive latency, used with posted cas */
        int             t_burst;                        /* number of cycles utilized per cacheline burst */
        int             t_cas;                          /* delay between start of CAS and start of burst */
        int             t_cmd;                          /* command bus duration... */
	int             t_cwd;                          /* delay between end of CASW and start of burst */
	int             t_faw;                          /* four bank activation */
	int             t_int_burst;                    /* internal prefetch length of DRAM devices, 4 for DDR2, 8 for DDR3 */
	int             t_ras;                          /* interval between ACT and PRECHARGE to same bank */
	int             t_rc;                           /* t_rc is simply t_ras + t_rp */
	int             t_rcd;                          /* RAS to CAS delay of same bank */
	int             t_rfc;                          /* refresh cycle time */
	int             t_rp;                           /* interval between PRECHARGE and ACT to same bank */
	int             t_rrd;                          /* Row to row activation delay */
	int             t_rtp;                          /* read to precharge delay,  */
	int             t_rtrs;                         /* rank hand off penalty. */
	int             t_wr;                           /* write recovery time , time to restore data */
	int             t_wtr;                          /* write to read turnaround time */
} dram_timing_specification_t;

/* The way locality is defined is that if locality is 0, then next access to the same resource is
   guaranteed to be different (unless there is only 1 of that resource). Conversely, if the 
   locality is 1, then the next access to the same resource is guarenteed to be to the same location.
   row_locality should be set to something like 80 to 90%, and the other locality can be varied
   according to the needs of the simulation
*/

typedef struct input_stream_t {
	int		type;				/* trace type or random number generator */
	tick_t		time;				/* time reported by trace or recorded by random number generator */
	FILE 		*trace_file_ptr;
	double		chan_locality;
	double		rank_locality;
	double		bank_locality;
	double		row_locality;	
	double		read_percentage;		/* the percentage of accesses that are reads. should replace with access_distribution[] */
	double		short_burst_ratio;		/* long burst or short burst? */
	double		arrival_thresh_hold;
	int		average_interarrival_cycle_count;	/* used by random number generator */
	int		interarrival_distribution_model;
} input_stream_t;

/* pending event queue */

typedef struct event_t {
	int		event;				/* what kind of event is this? */
	tick_t		time;
	void		*event_ptr;			/* point to a transaction or command to be enqueued or executed */
} event_t;

/* 
 *  Algorithm specific data structures should go in here.
 */

typedef struct dram_command_algorithm_t {
	queue_t				*WHCC;				/* Wang Hop Command Chain */
	int				WHCC_offset[2];
	int				transaction_type[4];
	int				rank_id[2];
	int				ras_bank_id[4];
	int				cas_count[4];
} dram_command_algorithm_t;

/*
 *  	DRAM System has the configuration, timing, state, the transaction queue and the dram controller
 */

typedef struct dram_system_t {
	dram_system_configuration_t 	*config;
	dram_timing_specification_t 	*timing;
	dram_controller_t       	*channel;
	tick_t				time;		/* master clock */
	queue_t				*free_command_pool;
	queue_t				*free_transaction_pool;		
	queue_t				*free_event_pool;
	queue_t				*event_q;			/* pending event queue */
	dram_command_algorithm_t	*algorithm;
} dram_system_t;

/* simulation parameters */

typedef struct simulation_parameters_t {
	int	debug_flag;
	int	input_type;
	int	request_count;
} simulation_parameters_t;

/* statistics gathering structures */

typedef struct dram_statistics_t {
	FILE		*fout;
	int		start_no;
	int		end_no;
	double		start_time;
	double		end_time;
	int		valid_transaction_count;
	int		bo8_count;
	int		bo4_count;
	int		system_config_type;
} dram_statistics_t;

typedef struct STAT_BUCKET {
	int			count;
	double			delta_t;
	struct	STAT_BUCKET 	*next_bucket;
} STAT_BUCKET;

/* function calls used in file IO */

int     is_valid_hex_number(char *);
double  ascii2double(char *);
double  ascii2multiplier(char *);
int 	file_io_token(char *);
int	intlog2(int);
void	print_command(command_t *);
void 	print_command_type(int);
void	print_dram_stats(FILE *, dram_statistics_t *);
void 	print_system_config_summary(FILE *, dram_system_configuration_t *);
void 	print_system_config_detailed(FILE *, dram_system_configuration_t *);
void    print_system_timing_summary(FILE *, dram_timing_specification_t *);
void 	print_addresses(addresses_t *);
void 	print_transaction(transaction_t *);

/* system init calls */

dram_system_t *init_dram_system();
dram_system_configuration_t *init_dram_system_configuration();
dram_timing_specification_t *init_dram_timing_specification(int);
void  	set_dram_timing_specification(dram_timing_specification_t *, int);
dram_controller_t *init_dram_controllers(dram_system_configuration_t *);
simulation_parameters_t *init_simulation_parameters();
dram_statistics_t *init_dram_stats();
void reset_dram_stats();
void	read_command_line_config(int, char **, dram_system_t *, simulation_parameters_t *, input_stream_t *, dram_statistics_t *);
void	read_dram_config_from_file(dram_system_t *);
void 	reset_dram_system(dram_system_t *);
queue_t *init_q(int);
bank_t  *init_banks(dram_system_configuration_t *);
rank_t  *init_ranks(dram_system_configuration_t *);
dram_command_algorithm_t *init_algorithm(dram_system_t *);
void print_WHCC(queue_t *);

/* queueing functions  */

int	enqueue(queue_t *, void *);
void	*dequeue(queue_t *);
void	*q_read(queue_t *, int);
int	q_count(queue_t *);
int	q_freecount(queue_t *);
int	q_insert(queue_t *, void *, int);
void	*acquire_item(queue_t *, int);
void	release_item(queue_t *, void *, int);
int	event_enqueue(queue_t *, event_t *);

/* input stream headers */
input_stream_t *init_input_stream(dram_system_configuration_t *);

/* add transaction */
transaction_t *	get_next_input_transaction(dram_system_t *, input_stream_t *);
int	get_next_bus_event(FILE *, int, bus_event_t *);

int 	convert_address(dram_system_configuration_t *, addresses_t *);

int  queue_check(dram_system_t *, int);

/* random number generation headers -grabbed from internet by Katie Baynes*/

transaction_t *get_next_random_request(dram_system_t *, input_stream_t *);
double	box_muller(double m, double s);
double	poisson_rng(double m);
double 	gammln(double xx);

/* actual simulation headers */

void 	run_simulations(dram_system_t *, input_stream_t *, simulation_parameters_t *, dram_statistics_t *);
command_t *get_next_command(dram_system_t *, int);
int	set_read_write_type(dram_controller_t *, int, int);
int	pending_count(dram_controller_t *, int, int);
int	min_protocol_gap(int, dram_controller_t *, command_t *, dram_system_configuration_t *, dram_timing_specification_t *);
void 	execute_command(dram_system_t *, command_t *, int);
int	transaction2command(dram_system_t *, transaction_t *);

void 	collect_transaction_stats(dram_statistics_t *, transaction_t *);
int	find_oldest_channel(dram_system_t *);
void	update_system_time(dram_system_t *);

/* event handling headers */

