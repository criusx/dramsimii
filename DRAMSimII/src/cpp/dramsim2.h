/*
To do list:

1. add refresh
2. add open page support
3. Look at power up/down models.
4. re-add power models.
5. convert to event driven model
6. attach BIU, port to alpha-sim
7. convert passed pointers/values to references
8. standardize time variables
9. organize classes better, divide work more intelligently
10. move away from srand48, drand48 functions, they are deprecated
11. switch from machine dependent vars to definite types. e.g. unsigned int -> UINT32, __int64, etc
12. make vars private again and declare various functions as friends
13. added tranaction_type enum of control
14. make the queue a template class
15. make data private, but declare it friend for functions which access it frequently
*/
#ifndef DRAMSIM2_H
#define DRAMSIM2_H
#pragma once

#include <map>
#include <fstream>

using namespace std;

#define ABS(a) ((a) < 0 ? (-a) : (a))

#define tick_t long long

#define INVALID -1

#define PI 3.1415926535897932384626433832795

#define COMMAND_QUEUE_SIZE 32

#define DEBUG_FLAG

#define DEBUG_COMMAND

//#define DEBUG_TRANSACTION

//#define DEBUG_RAND

//#define DEBUG_MIN_PROTOCOL_GAP

//#define DEBUG_FLAG_2



// this is to take care of some nonstandard differences that arise due to having different platforms
// check ISO24731 for details on what rand_s() will be
#ifdef WIN32
#ifdef DEBUG_RAND
#define rand_s(a) (*a)=rand()
#define srand48 srand
#undef UINT_MAX
#define UINT_MAX RAND_MAX

#endif
#define srand48 srand
#else
#define rand_s(a) *a=lrand48()*2
#endif

/* This section defines the refresh policy */

enum refresh_policy_t
{
	NO_REFRESH, 
	BANK_CONCURRENT, 
	BANK_STAGGERED_HIDDEN 
};

enum row_buffer_policy_t
{
	AUTO_PAGE, /* same as OPEN PAGE, but close page after timer expires */
	OPEN_PAGE, /* keep page open indefinitely */
	CLOSE_PAGE
};

/// This section defines the address mapping scheme
/// The scheme dictates how a memory address is converted
/// to rank, bank, row, col, byte
enum address_mapping_scheme_t
{
	BURGER_BASE_MAP,
	CLOSE_PAGE_BASELINE,
	INTEL845G_MAP,
	OPEN_PAGE_BASELINE,
	SDRAM_BASE_MAP,
	SDRAM_CLOSE_PAGE_MAP,
	SDRAM_HIPERF_MAP
};

enum system_configuration_type_t
{
	BASELINE_CONFIG, // direct control. 1 or 2 ranks in DDR3
	FBD_CONFIG // fully buffered DIMMS
};

/// we can define various algorithms previously explored by Rixner, McKee et al. here.
enum ordering_algorithm_t
{
	STRICT_ORDER, // maintains original ordering
	RANK_ROUND_ROBIN, // alternate ranks 
	BANK_ROUND_ROBIN, // keep same ranks as long as possible, go down banks
	GREEDY, // greedy algorithm
	WANG_RANK_HOP // Patented stuff. davewang202@yahoo.com ;)
};

enum input_type_t
{
	K6_TRACE,
	MASE_TRACE,
	RANDOM,
	MASE,
	ALPHASIM,
	GEMS,
	UNKNOWN
};

enum input_status_t
{
	FAILURE,
	SUCCESS
};

enum dram_type_t
{
	DDR,
	DDR2,
	DDR3,
	DRDRAM,
	SDRAM
};

enum transaction_type_t
{
	IFETCH_TRANSACTION,
	WRITE_TRANSACTION,
	READ_TRANSACTION,
	PREFETCH_TRANSACTION,
	AUTO_REFRESH_TRANSACTION,
	PER_BANK_REFRESH_TRANSACTION,
	AUTO_PRECHARGE_TRANSACTION,
	CONTROL_TRANSACTION
};

enum command_type_t
{
	RAS_COMMAND,
	CAS_COMMAND,
	CAS_AND_PRECHARGE_COMMAND,
	CAS_WRITE_COMMAND,
	CAS_WRITE_AND_PRECHARGE_COMMAND,
	RETIRE_COMMAND, /* ?? */
	PRECHARGE_COMMAND,
	PRECHARGE_ALL_COMMAND, /* ?? */
	RAS_ALL_COMMAND, /* ?? */
	DRIVE_COMMAND, /* ?? */
	DATA_COMMAND, /* ?? */
	CAS_WITH_DRIVE_COMMAND, /* ?? */
	REFRESH_ALL_COMMAND
};

enum file_io_token_t
{
	BOFF,
	FETCH,
	INT_ACK,
	IO_RD,
	IO_WR,
	LOCK_RD,
	LOCK_WR,
	MEM_RD,
	MEM_WR,
	MICROSECOND,
	MILLISECOND,
	NANOSECOND,
	PICOSECOND,
	SECOND,
	addr_mapping_scheme_token,
	auto_precharge_token,
	bank_count_token,
	cachelines_per_row_token,
	cacheline_size_token,
	chan_count_token,
	channel_width_token,
	clock_granularity_token,
	col_count_token,
	col_size_token,
	command_ordering_algorithm_token,
	comment_token,
	completion_queue_depth_token,
	datarate_token,
	debug_token,
	deprecated_ignore_token,
	dram_type_token,
	event_queue_depth_token,
	help_token,
	history_queue_depth_token,
	input_type_token,
	ordering_algorithm_token,
	output_file_token,
	per_bank_queue_depth_token,
	posted_cas_token,
	rank_count_token,
	read_percentage_token,
	read_write_grouping_token,
	refresh_policy_token,
	refresh_time_token,
	request_count_token,
	refresh_queue_depth_token,
	row_buffer_management_policy_token,
	row_count_token,
	row_size_token,
	seniority_age_limit_token,
	short_burst_ratio_token,
	system_configuration_type_token,
	t_al_token,
	t_burst_token,
	t_cac_token,
	t_cas_token,
	t_cmd_token,
	t_cwd_token,
	t_faw_token,
	t_ras_token,
	t_rc_token,
	t_rcd_token,
	t_rfc_token,
	t_rl_token,
	t_rp_token,
	t_rrd_token,
	t_rtp_token,
	t_rtrs_token,
	t_wr_token,
	t_wtr_token,
	transaction_ordering_policy_token,
	transaction_queue_depth_token,
	trace_file_token,
	unknown_token
};

enum distribution_type_t
{
	UNIFORM_DISTRIBUTION,
	GAUSSIAN_DISTRIBUTION,
	POISSON_DISTRIBUTION,
	NORMAL_DISTRIBUTION
};

enum event_type_t
{
	TRANSACTION_ENQUEUE, // put transaction into channel queue
	DRAM_COMMAND_ENQUEUE, // put DRAM command into per-bank queue
	DRAM_COMMAND_EXECUTE // schedule DRAM command for execution
};

// include the template code for queues
#include "queue.h"


#include "addresses.h"


#include "transaction.h"


#include "command.h"


/* pending event queue */

class event
{
public:
	int event_type; /* what kind of event is this? */
	tick_t time;
	void *event_ptr; /* point to a transaction or command to be enqueued or executed */
	event();
};

/* bus_event_t is used with the file I/O interface to get bus events from input trace files */

class bus_event /* 6 DWord per event */
{
public:
	enum transaction_type_t attributes;
	/* read/write/Fetch type stuff. Not overloaded with other stuff */
	int address; /* assume to be <= 32 bits for now */
	tick_t timestamp; /* time stamp will now have a large dynamic range, but only 53 bit precision */
	bus_event();
};

//////////////////////////////////////////////////////////////////////////
/// global functions
//////////////////////////////////////////////////////////////////////////
// overloaded insertion operator functions for printing various aspects of the dram system
ostream &operator<<(ostream &, const command_type_t &);
ostream &operator<<(ostream &, const command &);
ostream &operator<<(ostream &, const addresses &);
ostream &operator<<(ostream &, const transaction *&);
ostream &operator<<(ostream &, const address_mapping_scheme_t &);
// converts a string to a file_io_token
file_io_token_t file_io_token(const string &);

// converts a string to its corresponding magnitude representation
double ascii2multiplier(const string &);

// maps the inputs to file_io_tokens and corresponding strings to simplify initialization
void create_input_map(int ,char *[],map<enum file_io_token_t, string> &);
void create_input_map_from_input_file(map<enum file_io_token_t, string> &,ifstream &);

/// class declarations
class dram_system;
//////////////////////////////////////////////////////////////////////////
int inline log2(unsigned int input)
{
	int l2 = 0;
	for (input >>= 1; input > 0; input >>= 1)
	{
		++ l2;
	}
	return l2;
}

/* t_pp (min prec to prec of any bank) ignored */
class dram_timing_specification
{
private:
	int t_rtrs; // rank hand off penalty.
	int t_al; // additive latency, used with posted cas
	int t_burst; // number of cycles utilized per cacheline burst
	int t_cas; // delay between start of CAS and start of burst
	int t_cwd; // delay between end of CASW and start of burst
	int t_faw; // four bank activation
	int t_ras; // interval between ACT and PRECHARGE to same bank
	int t_rc; // t_rc is simply t_ras + t_rp
	int t_rcd; // RAS to CAS delay of same bank
	int t_rfc; // refresh cycle time
	int t_rp; // interval between PRECHARGE and ACT to same bank
	int t_rrd; // Row to row activation delay
	int t_rtp; // read to precharge delay
	int t_wr; // write recovery time , time to restore data
	int t_wtr; // write to read turnaround time
	int t_cmd; // command bus duration...
	int t_int_burst; // internal prefetch length of DRAM devices, 4 for DDR2, 8 for DDR3

public:
	dram_timing_specification(map<file_io_token_t, string> &);
	friend ostream &operator<<( ostream&, const dram_timing_specification &);
	friend dram_system;
};


class bank_c
{
public:
	tick_t last_ras_time; // when did last ras start?
	tick_t last_cas_time; // when did last cas start?
	tick_t last_casw_time; // when did last cas write start?
	tick_t last_prec_time; // when did last prec start?
	tick_t last_refresh_all_time; // must respect t_rfc. concurrent refresh takes time
	int last_cas_length;
	int last_casw_length;
	int row_id; // if the bank is open, what is the row id?
	queue<command> per_bank_q; // per bank queue
	// stats
	int ras_count;
	int cas_count;
	int casw_count;
	//public:
	bank_c();
	void init_banks(int);
};

class rank_c
{
public:
	int bank_count;

	int last_bank_id; /* id of the last accessed bank of this rank*/
	tick_t last_refresh_time;
	tick_t last_cas_time;
	tick_t last_casw_time;
	int last_cas_length;
	int last_casw_length;
	queue<tick_t> last_ras_times; // ras time queue. useful to determine if t_faw is met
	bank_c *bank;

	~rank_c();
	rank_c();
	void init_ranks(int,int);
};

class dram_channel
{
private:
	tick_t time; // channel time, allow for channel concurrency
	rank_c *rank; // pointer to the array of ranks
	int refresh_row_index; // the row index to be refreshed
	tick_t last_refresh_time; // tells me when last refresh was done
	int last_rank_id; // id of the last accessed rank of this channel
	queue<transaction> transaction_q;// unified system queue
	queue<transaction> refresh_q; // queue of refresh transactions
	queue<command> history_q; // what were the last N commands to this channel?
	queue<transaction> completion_q;// completed_q, can send status back to memory controller

public:
	// the get_ functions
	rank_c *get_rank(const unsigned rank_num) const { return &(rank[rank_num]); }
	tick_t get_time() const { return time; }
	void set_time(tick_t new_time) { time = new_time; }
	int get_last_rank_id() const { return last_rank_id; }
	transaction *get_transaction() { return transaction_q.dequeue(); } // remove and return the oldest transaction
	transaction *read_transaction() { return transaction_q.read_back(); } // read the oldest transaction without affecting the queue
	input_status_t enqueue(transaction *in) { return transaction_q.enqueue(in); }
	input_status_t complete(transaction *in) { return completion_q.enqueue(in); }
	transaction *get_oldest_completed() { return completion_q.dequeue(); }
	command *get_most_recent_command() const { return history_q.newest(); } // get the most recent command from the history queue
	void record_command( command *, queue<command> &);

	dram_channel();
	~dram_channel();
	void init_controller(int, int, int, int, int, int, int);
	enum transaction_type_t set_read_write_type(const int,const int) const;
};

/* simulation parameters */

class simulation_parameters 
{
private:
	int request_count;
	int input_type;

public:
	simulation_parameters(map<file_io_token_t,string> &);
	inline int get_request_count() {return request_count;}
};

class input_stream_c 
{
private:
	enum input_type_t input_token(const string&) const;
	enum input_type_t type; /* trace type or random number generator */
	double chan_locality;
	double rank_locality;
	double bank_locality;
	tick_t time; /* time reported by trace or recorded by random number generator */
	double row_locality; 
	double read_percentage; /* the percentage of accesses that are reads. should replace with access_distribution[] */
	double short_burst_ratio; /* long burst or short burst? */
	double arrival_thresh_hold;
	int average_interarrival_cycle_count; /* used by random number generator */
	distribution_type_t interarrival_distribution_model;
	double box_muller(double, double);
	double poisson_rng (double);
	double gammaln(double) const;
	enum input_status_t get_next_bus_event(bus_event &);
	//enum input_status_t get_next_input_transaction(transaction*);
	ifstream trace_file;

public: 
	input_stream_c(map<file_io_token_t,string> &);
	inline enum input_type_t getType() { return type; }

	//friends
	friend class dram_system;
};




/*
* Algorithm specific data structures should go in here.
*/

class dram_algorithm
{

protected:
	queue<command> WHCC; /// Wang Hop Command Chain 
	int WHCC_offset[2];
	int transaction_type[4];
	int rank_id[2];
	int ras_bank_id[4];
	int cas_count[4];

public:
	dram_algorithm();
	dram_algorithm(const dram_algorithm &);
	void init(queue<command> &, int, int, int); 
	friend dram_system;
};

class dram_statistics
{

private:
	tick_t end_time;
	int valid_transaction_count;
	fstream *fout;
	int start_no;
	int end_no;
	double start_time; 
	int bo8_count;
	int bo4_count;
	int system_config_type;

public:
	void collect_transaction_stats(transaction*);
	dram_statistics();
	inline void set_end_time(tick_t et) {end_time = et;}
	inline void set_valid_trans_count(int vtc) {valid_transaction_count = vtc;}
	friend ostream &operator<<(ostream &, const dram_statistics &);
};



class dram_system_configuration
{ 

private:
	ifstream spd_file_ptr;
	ordering_algorithm_t command_ordering_algorithm;// strict or round robin 
	int per_bank_queue_depth; // command queue size
	system_configuration_type_t config_type;
	int refresh_time; // loop time of refresh 
	refresh_policy_t refresh_policy; // 
	int col_size;
	int row_size; // bytes per row (across one rank) 
	int row_count; // rows per bank
	int col_count; // columns per row
	int cacheline_size; // 32/64/128 etc 
	int history_queue_depth; // keep track of per channel command history 
	int completion_queue_depth; // keep track of per channel command history 
	int transaction_queue_depth; // input transaction queue depth 
	int event_queue_depth; // pending event queue depth 
	int refresh_queue_depth; // loop time of refresh 
	int seniority_age_limit;
	dram_type_t dram_type; 
	row_buffer_policy_t row_buffer_management_policy; // row buffer management policy? OPEN/CLOSE, etc 
	address_mapping_scheme_t addr_mapping_scheme; // addr mapping scheme for physical to DRAM addr 
	double datarate;
	bool posted_cas; // TRUE/FALSE 
	bool read_write_grouping;
	bool auto_precharge; // issue cas and prec separately or together? 
	int clock_granularity; 
	int cachelines_per_row; // dependent variable 
	int chan_count; // How many logical channels are there ? 
	int rank_count; // How many ranks are there per channel ? 
	int bank_count; // How many banks per device? 
	double short_burst_ratio;
	double read_percentage;

public:
	dram_system_configuration(map<file_io_token_t,string> &);

	// friends
	friend dram_system;
	friend ostream &operator<<(ostream &, const dram_system &);
};

class dram_system
{
private:
	dram_system_configuration system_config;
	dram_timing_specification timing_specification;
	simulation_parameters sim_parameters;
	dram_statistics statistics;
	dram_algorithm algorithm;

	string output_filename;
	input_stream_c input_stream;
	dram_channel *channel;
	tick_t time; // master clock
	queue<command> free_command_pool; // command objects are stored here to avoid allocating memory at runtime
	queue<transaction> free_transaction_pool; // same for transactions
	queue<event> free_event_pool; // same for events
	queue<event> event_q; /* pending event queue */

	void read_dram_config_from_file();

	void set_dram_timing_specification(enum dram_type_t);
	command *getNextCommand(const int);
	enum input_status_t transaction2commands(transaction*);
	int minProtocolGap(const int,const command *) const;
	int find_oldest_channel() const;
	void executeCommand(command *, const int);
	void update_system_time();
	int convert_address(addresses &);
	enum input_status_t get_next_input_transaction(transaction *&);
	void get_next_random_request(transaction *);

public:
	explicit dram_system(map<file_io_token_t,string> &);
	~dram_system();
	friend ostream &operator<<(ostream &, const dram_system &);
	void run_simulations();
	void run_simulations2();
};

#endif