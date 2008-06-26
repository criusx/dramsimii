use strict;
use warnings;
use diagnostics;

#Must include this line to ignore redefinition of runTest
no warnings 'redefine';


sub runTest {
	### Change this to match memory file
	my $t_rrd = 5;



	my $line = -1;
	my @data = @_;

	my @status;

	

	my @start_time;
	foreach (@data) {
		$line++;
		if(!/RAS/) {
			next;
		}
		/rank\[ *([a-fA-F0-9]+)\].+S\[ *([a-fA-F0-9]+)\]/;
		my $rank = hex $1;
		my $time = hex $2;	

		my $rank_time = -1;
		$rank_time = $start_time[$rank] if defined $start_time[$rank];

		if ($rank_time != -1) {
			if($time-$rank_time < $t_rrd) {
                push (@status, "Problem with output line $line");
				if($status[0] ne 0) {
					unshift (@status,0);
				}
            }
		}
		
		$start_time[$rank] = $time;
	}
	
	if(!@status || $status[0] ne 0) {
		unshift (@status,1);
	}
	return @status;
}

1 #File must return 1 to indicate sucessful loading
