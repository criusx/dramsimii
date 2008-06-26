use strict;
use warnings;
use diagnostics;

#Must include this line to ignore redefinition of runTest
no warnings 'redefine';


sub runTest {
	### Change this to match memory file
	my $t_faw = 25;



	my @data = @_;
	my $line = -1;
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

		my @rank_time = ();
		my $rank_time_ref = $start_time[$rank];
		if (defined $rank_time_ref) {
			@rank_time = @$rank_time_ref;
		}
		if ($#rank_time+1 == 4) {
			if($time-(pop @rank_time) < $t_faw) {
                push (@status, "Problem with output line $line");
				if($status[0] ne 0) {
					unshift (@status,0);
				}
            }
		}
		unshift (@rank_time,$time);
		
		$start_time[$rank] = \@rank_time;
	}

	if(!@status || $status[0] ne 0) {
		unshift (@status,1);
	}
	return @status;
}

1 #File must return 1 to indicate sucessful loading
