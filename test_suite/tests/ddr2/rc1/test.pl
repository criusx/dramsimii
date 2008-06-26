use strict;
use warnings;
use diagnostics;

#Must include this line to ignore redefinition of runTest
no warnings 'redefine';


sub runTest {
	### Change this to match memory file
	my $t_rc = 38;



	my $line = -1;
	my @data = @_;

	my @status;

	

	my @start_time;
	foreach (@data) {
		$line++;
		if(!/RAS/) {
			next;
		}
		/bank\[ *([a-fA-F0-9]+)\].+S\[ *([a-fA-F0-9]+)\]/;
		my $bank = hex $1;
		my $time = hex $2;	

		my $bank_time = -1;
		$bank_time = $start_time[$bank] if defined $start_time[$bank];
		
		if ($bank_time != -1) {
			if($time-$bank_time < $t_rc) {
                push (@status, "Problem with output line $line");
				if($status[0] ne 0) {
					unshift (@status,0);
				}
            }
		}
		
		$start_time[$bank] = $time;
	}
	
	if(!@status || $status[0] ne 0) {
		unshift (@status,1);
	}
	return @status;
}

1 #File must return 1 to indicate sucessful loading
