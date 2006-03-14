#include "dramsim.h"

dram_statistics_t *init_dram_stats(){
	dram_statistics_t *stats;
	stats = (dram_statistics_t *)calloc(1,sizeof(dram_statistics_t));
	reset_dram_stats(stats);
	return stats;
}

void reset_dram_stats(dram_statistics_t *stats){
	stats->fout     = stdout;
	stats->start_no = 0;
	stats->end_no   = 0;
	stats->start_time = 0;
	stats->end_time = 0;
	stats->valid_transaction_count = 0;
	stats->bo8_count = 0;
	stats->bo4_count = 0;
}


void collect_transaction_stats(dram_statistics_t *stats, transaction_t *this_t){
	if(this_t->length == 8){
		stats->bo8_count++;
	} else {
		stats->bo4_count++;
	}
}



