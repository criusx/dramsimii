package dram_test;

	sub new {
	}

#Stub which fails incorrectly specified tests
	sub runTest {
		print "*** This test not implemented! ==> ";
		return 0;
	}

#ALWAYS call with main->executeTest

##########
# executeTest
# Arguments:
#	path to test folder
#	path to dramsim executable
#	other options
#
# returns simulator output
##########
	sub executeTest {
	$class = shift(@_);
	$myDir = shift(@_);
	$progDir = shift(@_);
	$override = shift(@_);
	
	if (!defined($override)) {
		$override = "";
	}

	if ($class ne "main" || $myDir eq "" || $progDir eq "") {
		die "Invalid arguments";
	}

	# Find number of instructions in 'input' file
	my $MAX_INST_COUNT     = 0;

	my $INPUT_FILE = "$myDir/input";
	my $INPUT_FLAG = " -mapped_trace_file $INPUT_FILE ";

	open(INPUT, "$INPUT_FILE") or return 0; 
		while (<INPUT>) {
		$MAX_INST_COUNT++;
		#print "$MAX_INST_COUNT=> $_";
		}
	close(INPUT);
	
	#Set default parameters

	my $dram_amb_buffers = ($override=~/-dram:amb_buffers/)?"":" -dram:amb_buffers 8 ";
	my $dram_rank_count = ($override=~/-dram:rank_count/)?"":" -dram:rank_count 2 ";
	my $dram_refresh = ($override=~/-dram:refresh/)?"":" -dram:refresh 0 ";
	my $biu_trans_sel = ($override=~/-biu:trans_sel/)?"":"-biu:trans_sel riff";
	my $dram_refresh_issue_policy = ($override=~/-dram:refresh_issue_policy/)?"":"-dram:refresh_issue_policy priority";
	my $dram_row_buffer_management_policy = ($override=~/-dram:row_buffer_management_policy/)?"":"-dram:row_buffer_management_policy close_page";
	my $dram_address_mapping_policy = ($override=~/-dram:address_mapping_policy/)?"":"-dram:address_mapping_policy sdram_hiperf_map";
	my $max_trans = ($override=~/-max:trans/)?"":"-max:trans $MAX_INST_COUNT";
	my $cpu_frequency = ($override=~/-cpu:frequency/)?"":" -cpu:frequency 4000 ";
	my $dram_spd_input = ($override=~/-dram:spd_input/)?"":" -dram:spd_input $progDir/test_suite/DDR2-667-4-4-4.spd ";
	my $debug = ($override=~/-debug:issue/)?"":"  -debug:issue ";
	
	my $CONFIG_FLAG = " $dram_amb_buffers $dram_rank_count $dram_refresh $biu_trans_sel $dram_refresh_issue_policy $dram_row_buffer_management_policy $dram_address_mapping_policy $max_trans $dram_spd_input $debug $override";
	
			chdir $progDir || die "$progDir";

			
			my $CMD = "DRAMsim $CONFIG_FLAG $INPUT_FLAG ";	

	my @data;


	#Run simulation, save output
	open (OUTPUT, "$CMD 2>&1 |");
	open (OUTPUTFILE, ">$myDir/output");
	print OUTPUTFILE "$CMD\n\n\n";
	while(<OUTPUT>) {
		print OUTPUTFILE $_;
		chomp;
		push(@data,$_);
	}
	close OUTPUTFILE;		
	close OUTPUT;	
	chdir $myDir;
	return @data;
	}


#ALWAYS call with main->orderTest

##########
# executeTest
#Implementation of a generic test where only
#order of transactions matter
# Arguments:
#   path to test folder
#   path to dramsim executable
#   other options
#
# returns 1 or 0 (pass/fail)
#########
	sub orderTest {
	@data=executeTest(@_);
	
	open (EXPECTED, "$myDir/expected");
	@expected = <EXPECTED>;
	close EXPECTED;

	foreach (@expected) {
		chomp;
	}	


	foreach(@data) {
		if (/$expected[0]/) {
			shift @expected;
			last if scalar @expected == 0;
		}
	}	

	#If all rules are satified, @expected should equal 0 (or false)
	return (@expected == 0);
	}
# Return 1 to signify sucessful loading
1
