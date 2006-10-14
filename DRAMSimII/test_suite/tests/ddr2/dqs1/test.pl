use strict;
use warnings;
use diagnostics;

#Must include this line to ignore redefinition of runTest
no warnings 'redefine';


sub runTest {
	### Change this to match memory file
	my $t_dqs = 2;
	my $t_burst = 8;



	my $line = -1;
	my @data = @_;

	my @status;

	my $start_time;
	my $start_rank;

	foreach (@data) {
		$line++;
		if(!/RAS/) {
			next;
		}

		/rank\[ *([a-fA-F0-9]+)\].+S\[ *([a-fA-F0-9]+)\]/;
		my $rank = hex $1;
		my $time = hex $2;	

		if(defined $start_rank and $start_rank != $rank and $time-$start_time < $t_dqs+$t_burst) {
			push (@status, "Problem with output line $line");
			if($status[0] ne 0) {
				unshift (@status,0);
			}
		}
		
		$start_time = $time;
		$start_rank = $rank;
	
	}
	
	if(!@status || $status[0] ne 0) {
		unshift (@status,1);
	}
	return @status;
}

1 #File must return 1 to indicate sucessful loading

