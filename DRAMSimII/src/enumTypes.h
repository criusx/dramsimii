#ifndef ENUMTYPES_H
#define ENUMTYPES_H
#pragma once

// This section defines the refresh policy
enum refresh_policy_t
{
	NO_REFRESH, 
	BANK_CONCURRENT, 
	BANK_STAGGERED_HIDDEN 
};

enum row_buffer_policy_t
{
	AUTO_PAGE, // same as OPEN PAGE, but close page after timer expires
	OPEN_PAGE, // keep page open indefinitely
	CLOSE_PAGE
};

// This section defines the address mapping scheme
// The scheme dictates how a memory address is converted
// to rank, bank, row, col, byte
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

// we can define various algorithms previously explored by Rixner, McKee et al. here.
enum command_ordering_algorithm_t
{
	STRICT_ORDER, // maintains original ordering
	RANK_ROUND_ROBIN, // alternate ranks 
	BANK_ROUND_ROBIN, // keep same ranks as long as possible, go down banks
	GREEDY, // greedy algorithm
	WANG_RANK_HOP // Patented stuff. davewang202@yahoo.com ;)
};

enum transaction_ordering_algorithm_t
{
	RIFF,
	STRICT
};

enum input_type_t
{
	K6_TRACE,
	MASE_TRACE,
	RANDOM,
	MASE,
	MAPPED,
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
	t_buffer_delay_token,
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
	t_refi_token,
	t_rp_token,
	t_rrd_token,
	t_rtp_token,
	t_rtrs_token,
	t_wr_token,
	t_wtr_token,
	transaction_ordering_policy_token,
	transaction_queue_depth_token,
	trace_file_token,
	unknown_token,
	// power config tokens
	vdd_token,
	max_vcc_token,
	idd0_token,
	idd2p_token,
	idd2n_token,
	idd3p_token,
	idd3n_token,
	idd4r_token,
	idd4w_token,
	idd5_token
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
	TRANSACTION_ENQUEUE,	// put transaction into channel queue
	DRAM_COMMAND_ENQUEUE,	// put DRAM command into per-bank queue
	DRAM_COMMAND_EXECUTE	// schedule DRAM command for execution
};

#endif
