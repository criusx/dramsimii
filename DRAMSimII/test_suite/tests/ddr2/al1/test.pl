use strict;
use warnings;
use diagnostics;

#Must include this line to ignore redefinition of runTest
no warnings 'redefine';


sub runTest {
	### Change this to match memory file
	my $t_al = 6;


	my $line = -1;
	my @data = @_;

	my @status;

	my $start_time;
	foreach (@data) {
		$line++;
		if(/RAS/) {
			/S\[ *([a-fA-F0-9]+)\]/;
			$start_time = hex $1;
			next;
		}

		if(!/CAS/) {
			next;
		}

		/S\[ *([a-fA-F0-9]+)\]/;
		my $time = hex $1;	

		if($time-$start_time < $t_al) {
			push (@status, "Problem with output line $line");
			if($status[0] ne 0) {
				unshift (@status,0);
			}
		}
		
	}
	
	if(!@status || $status[0] ne 0) {
		unshift (@status,1);
	}
	return @status;
}

1 #File must return 1 to indicate sucessful loading
