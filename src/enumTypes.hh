// Copyright (C) 2010 University of Maryland.
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

#ifndef ENUMTYPES_HH
#define ENUMTYPES_HH

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

	enum RowBufferManagementPolicy
	{
		AUTO_PAGE, // same as OPEN PAGE, but close page after timer expires
		OPEN_PAGE, // keep page open indefinitely
		OPEN_PAGE_AGGRESSIVE, // improve upon simple open page
		CLOSE_PAGE, // close a row after an operation
		CLOSE_PAGE_AGGRESSIVE // improve upon regular close page
	};

	/// we can define various algorithms previously explored by Rixner, McKee et al. here.
	enum CommandOrderingAlgorithm
	{
		STRICT_ORDER, ///< maintains original ordering
		RANK_ROUND_ROBIN, ///< alternate ranks
		BANK_ROUND_ROBIN, ///< keep same ranks as long as possible, go down banks
		FIRST_AVAILABLE_AGE, ///< prioritizes whatever may issue soonest and uses age as a tie-breaker
		FIRST_AVAILABLE_RIFF, ///< prioritizes whatever may issue soonest and uses prioritizes reads over writes
		FIRST_AVAILABLE_QUEUE, ///< prioritizes whatever may issue soonest and uses queue pressure as the tie-breaker
		COMMAND_PAIR_RANK_HOPPING ///< Patented stuff. davewang202@yahoo.com ;)
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
		address_mapping_policy_token,
		auto_precharge_token,
		average_interarrival_cycle_count,
		bank_count_token,
		// cache
		cache_associativity_token,
		cache_blocksize_token,
		cache_hitlatency_token,
		cache_size_token,
		cache_replacementpolicy_token,
		cache_nmrutrackingcount_token,
		using_cache_token,
		fixed_cache_latency_token,
		//
		cachelines_per_row_token,
		cacheline_size_token,
		channel_count_token,
		channel_width_token,
		clock_granularity_token,
		col_count_token,
		col_size_token,
		command_ordering_algorithm_token,
		cpu_to_memory_clock_ratio,
		datarate_token,
		decode_window_token,
		dimm_count_token,
		dm_per_dram_token,
		dq_per_dram_token,
		dqs_per_dram_token,
		dram_type_token,
		epoch_token,
		frequency_spec_token,
		input_file_token,
		input_type_token,
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
		request_count_token,
		refresh_queue_depth_token,
		riff_token,
		row_buffer_management_policy_token,
		row_count_token,
		row_size_token,
		seniority_age_limit_token,
		short_burst_ratio_token,
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
		idd1_token,
		idd2p_token,
		idd2n_token,
		idd3p_token,
		idd3n_token,
		idd4r_token,
		idd4w_token,
		idd5_token
	};
}
#endif
