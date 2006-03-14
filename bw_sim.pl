#!/usr/bin/perl

$pwd 			= `pwd`;
chop($pwd);
$SRC_DIR		= $pwd;
$TRACE_DIR		= "/home/davewang/btv/traces";
$REQUEST_COUNT		= "100000";
$SPD_DIR		= "$SRC_DIR/mem_system_def";
$SPD_FILE		= "DDR2-800-5-5-5.spd";
$DEBUG_FLAG		= "";
$TIMING_FLAGS		= "";
$CONFIG_FLAGS		= "-config:rank 1 -config:ordering_algorithm bank_round_robin";
$EXEC_NAME		= "dramsim2";

$starttime = time();
if($ARGV[0] eq "k6test"){
	print "testing K6 trace read\n";
	$INPUT_TYPE		= "k6";
	$file 			= "quake3/q3_04.trc"; 
	`$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -trace_file $TRACE_DIR/$file -dram:spd_input $SPD_DIR/$SPD_FILE $DEBUG_FLAG -request_count $REQUEST_COUNT`;
} elsif ($ARGV[0] eq "masetest") {
	print "testing mase trace read\n";
	$INPUT_TYPE		= "mase";
	$file 			= "equake.trc"; 
	`$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -trace_file $TRACE_DIR/$file -dram:spd_input $SPD_DIR/$SPD_FILE $DEBUG_FLAG -request_count $REQUEST_COUNT`;
} elsif ($ARGV[0] eq "run") {
	print "running\n";
	$INPUT_TYPE		= "random";
	for($rank_count = 1; $rank_count <= 2; $rank_count++){
	    for($bank_count = 8; $bank_count <= 16; $bank_count+=8){
		for($q_depth = 6; $q_depth <= 12; $q_depth+= 2){
		    for($t_rtrs = 2; $t_rtrs <=4; $t_rtrs += 2){
			$CONFIG_FLAGS	= "-config:rank $rank_count -config:bank $bank_count -config:ordering_algorithm rank_round_robin -config:queue_depth $q_depth -timing:rtrs $t_rtrs";
			print "$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -dram:spd_input $SPD_DIR/$SPD_FILE $CONFIG_FLAGS -request_count $REQUEST_COUNT";
			`$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -dram:spd_input $SPD_DIR/$SPD_FILE $CONFIG_FLAGS -request_count $REQUEST_COUNT`;
			$CONFIG_FLAGS	= "-config:rank $rank_count -config:bank $bank_count -config:ordering_algorithm bank_round_robin -config:queue_depth $q_depth -timing:rtrs $t_rtrs";
			print "$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -dram:spd_input $SPD_DIR/$SPD_FILE $CONFIG_FLAGS -request_count $REQUEST_COUNT";
			`$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -dram:spd_input $SPD_DIR/$SPD_FILE $CONFIG_FLAGS -request_count $REQUEST_COUNT`;
			$CONFIG_FLAGS	= "-config:rank $rank_count -config:bank $bank_count -config:ordering_algorithm wang_hop -config:queue_depth $q_depth -timing:rtrs $t_rtrs";
			print "$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -dram:spd_input $SPD_DIR/$SPD_FILE $CONFIG_FLAGS -request_count $REQUEST_COUNT";
			#`$SRC_DIR/$EXEC_NAME -input_type $INPUT_TYPE -dram:spd_input $SPD_DIR/$SPD_FILE $CONFIG_FLAGS -request_count $REQUEST_COUNT`;
		    }
		}
	    }
	}
} else {
	print "option $ARGV[0] not recognized\n";
}
