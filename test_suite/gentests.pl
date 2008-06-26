#!/usr/bin/perl

use strict;
use warnings;

sub randint {
	my $max = shift(@_);
	my $min = 0;

	if ($#_ == 0) {
		$min = shift(@_);
	}
	
	if ($min > $max) {
		my $temp = $max;
		$max = $min;
		$min = $temp;
	}
	
	return int(rand()*($max-$min)) + $min;
}

srand(5);

my $i;
for ($i=0; $i < 1000; $i++) {
	print "$i WRITE 0 ".randint(0)." ".randint(0)." ".randint(100)." ".randint(100)."\n";
}


