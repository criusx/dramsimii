use strict;
use warnings;
use diagnostics;

#Must include this line to ignore redefinition of runTest
no warnings 'redefine';


sub runTest {
	### Change this to match memory file
	my $t_ras = 30;



	my $line = -1;
	my @data = @_;
	my @status;

	

	foreach (@data) {
		$line++;
		if(!/RAS/) {
			next;
		}
		/T\[ *([a-fA-F0-9]+)\]/;
		my $elapsed = hex $1;	

		if($elapsed < $t_ras) {
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
