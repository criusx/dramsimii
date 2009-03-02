// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
// 
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ENUMTYPES_H
#define ENUMTYPES_H
#pragma once

namespace DRAMsimII
{
	// This section defines the refresh policy
	enum RefreshPolicy
	{
		NO_REFRESH, // do not simulate refresh operations
		BANK_CONCURRENT, //
		BANK_STAGGERED_HIDDEN, //
		ONE_CHANNEL_ALL_RANK_ALL_BANK // standard refresh, close banks, issue refresh command to activate/precharge a bank based upon the DRAM refresh counter
	};

	enum RowBufferPolicy
	{
		AUTO_PAGE, // same as OPEN PAGE, but close page after timer expires
		OPEN_PAGE, // keep page open indefinitely
		OPEN_PAGE_AGGRESSIVE, // improve upon simple open page
		CLOSE_PAGE, // close a row after an operation
		CLOSE_PAGE_AGGRESSIVE // improve upon regular close page
	};

	enum SystemConfigurationType
	{
		BASELINE_CONFIG, // direct control. 1 or 2 ranks in DDR3
		FBD_CONFIG // fully buffered DIMMS
	};

	// we can define various algorithms previously explored by Rixner, McKee et al. here.
	enum CommandOrderingAlgorithm
	{
		STRICT_ORDER, // maintains original ordering
		RANK_ROUND_ROBIN, // alternate ranks 
		BANK_ROUND_ROBIN, // keep same ranks as long as possible, go down banks
		FIRST_AVAILABLE, // greedy algorithm
		WANG_RANK_HOP // Patented stuff. davewang202@yahoo.com ;)
	};

	enum TransactionOrderingAlgorithm
	{
		RIFF, ///< read and instruction fetch first
		STRICT ///< FIFO
	};

	enum OutputFileType
	{
		COUT, ///< print to stdout
		GZ, ///< print to a gzip file
		BZ, ///< print to a bzip2 file
		UNCOMPRESSED, ///< just write to a normal file
		NONE ///< don't print diagnostic messages
	};


	enum DRAMType
	{
		DDR,
		DDR2,
		DDR3,
		DRDRAM,
		SDRAM
	};

	enum FileIOToken
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
		average_interarrival_cycle_count,
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
		cpu_to_memory_clock_ratio,
		datarate_token,
		debug_token,
		deprecated_ignore_token,
		dm_per_dram_token,
		dq_per_dram_token,
		dqs_per_dram_token,
		dram_type_token,
		dbreporting_token,
		event_queue_depth_token,
		epoch_token,
		frequency_spec_token,
		help_token,
		history_queue_depth_token,
		input_file_token,
		input_type_token,
		ordering_algorithm_token,
		output_file_token,
		output_file_dir_token,
		output_file_type_token,
		p_dq_rd_token,
		p_dq_wr_token,
		p_dq_rd_oth_token,
		p_dq_wr_oth_token,
		per_bank_queue_depth_token,
		posted_cas_token,
		random_distribution_token,
		rank_count_token,
		read_percentage_token,
		read_write_grouping_token,
		refresh_policy_token,
		refresh_time_token,
		request_count_token,
		refresh_queue_depth_token,
		riff_token,
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
		t_dqs_token,
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

	enum FrameType
	{
		COMMAND,				// contains only commands, A, B, C
		COMMAND_AND_DATA,		// contains a command in the A slot and data in the B, C slots
		SYNC					// a sync command
	};
}
#endif
