#!/usr/bin/perl


use strict;
use warnings;
use diagnostics;

##########

use Cwd;

##########
# Configuration
#
##########

my $testdir = getcwd;			# Base directory of test suite
my $progdir = $testdir . "/../";	# Location of DRAMsim executable

my $outputfile = "$testdir/output"; # File to save simulator output

my $trace_type = "mase";
my $trace_file = "$testdir/gzip.trc";


my $cmd = "dramsim2 -input_type $trace_type -trace_file $trace_file -dram:spd_input $testdir/DDR2-667-4-4-4.spd -config:rank 8 -config:bank 8 -config:ordering_algorithm strict -config:queue_depth 6 -timing:rtrs 2 -request_count 1000 -no_refresh";


#########
# Main Program
#
#########

chdir $progdir;

my @data;
open (OUTPUT, "$cmd 2>&1 |"); # Run simulator
open (OUTPUTFILE, ">$outputfile");
while(<OUTPUT>) { # Save simulator data in file and @data
	print OUTPUTFILE $_;
	chomp;
	push(@data,$_);
}

close OUTPUTFILE;
close OUTPUT;

chdir $testdir;


my @testList = loadTests(); #load list of tests

print "\n".scalar(@testList)." tests to run\n";
foreach (@testList) {
    print "$_ ";
}
print "\n------------------\n";

my $failed = 0;
foreach (@testList) {
    if(!processTest($_)) {
		$failed++;
	}
}

if ($failed) {
	print "\n$failed ".(($failed==1)?"test":"tests")." failed. Check $outputfile for simulator data\n";
}

##########
# loadTests
# 
# retrive names of test subfolders
##########
sub loadTests {
	my @testList;

    open(INDEXFILE,"test-index") || die "\"test-index\" does not exist.";
    while(<INDEXFILE>) {
        chomp;
    	s/\s*#.*$//;						#remove comments
		push @testList, $_ unless $_ eq "";	#ignore blank lines    
    }
    close INDEXFILE;

	return @testList;
}
#########
# processTest
#
# Pass test number to run
# Calls runTest sub of test.pl from each testing directory
#########
sub processTest {
	my $testNumber = shift(@_);	
	
	print "Test $testNumber: ";
	
	# Give warning if test not implemented correctly
	# and allow for methods that all tests can share
	use genericTest;
	our @ISA = qw(dram_test);
	
	
	my $testpath = "tests/$testNumber/test.pl";	
	
	#Basic error checking
	if (!-e $testpath)
	{ 
		
		print "FAIL - Missing test.pl\n";
		return 0;
	}

	if(!-r $testpath) {
		print "FAIL - test.pl not readable\n";
		return 0;
	}

	#Load actual test file
	require "tests/$testNumber/test.pl";
	#Run test
	my @status = main->runTest(@data);	
	
	chdir $testdir;	
	
	# Check result of test

	if(!@status) {
		print "FAIL - Test returned nothing\n";
		return 0;
	}	
	
	if(shift @status == 1) {
		print "OK\n";
		
		foreach (@status) {
			print "\t$_\n";
		}
		
		return 1;
	} else {
		print "FAIL\n";

		foreach (@status) {
			print "\t$_\n";
		}

		return 0;
	}
}

##########
