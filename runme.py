#!/usr/bin/python -O

from subprocess import Popen, PIPE, STDOUT
import sys
import os

runtype = sys.argv[1]
numcore = sys.argv[2]
benchmark = sys.argv[3]
starttime = sys.argv[4]

dramsimDirectory = os.path.join(os.path.expanduser("~"),'dramsimii')
m5directory = os.path.join(os.path.expanduser("~"),'m5') 
m5binary = os.path.join(m5directory,'build/ALPHA_FS/m5.opt')
m5Script = os.path.join(dramsimDirectory,'m5/configs/example/fsN.py')

m5flags = ' --trace-flags=Cache --trace-start=' + starttime + '  --detailed -n' + numcore + ' --caches --l3cache -F 10000000000 -b ' + benchmark

sedCommand0 = "sed -n -e 's/ReadReq (ifetch)/2/p' -e 's/ReadExReq/0/p' -e 's/ReadReq/0/p' -e 's/Writeback/1/p'"
sedCommand1 = "sed -e 's/ [a-z]*$//' -e 's/:.*://'"
sedCommand2 = "sed -n -e 's/\(.*\) \(.*\) \(.*\)/\2 \3 \1/p'"

# ----
# main
# ----
if sys.argv[1] == '-t':
	# -------------------
	# generate trace file
	# -------------------
	#tracefile_commandline = m5binary + m5flags + "p \"" |&grep l3cache|gzip > /home/mutien/Tools/dramsimii/benchmarks/' + benchmark + '/' + benchmark + 'Stream.gz'
	commandline = m5binary + m5flags
	
	print 'commandline: %s' %tracefile_commandline
	print 'generating %s tracefile...' % benchmark
	#p = Popen([tracefile_commandline], shell=True, executable="/bin/bash")
	p = Popen([commandline], shell=True, executable="/bin/bash", stdout=PIPE, stderr=PIPE)
	out,err = p.communicate()
	
	pattern = re.compile("([0-9]+): system.l3cache: (ReadReq (ifetch)|ReadExReq|ReadReq|Writeback) ([0-9a-f]).*")
	
	while True:
		newLine = out.readline()
		
    	if not next_line:
			break
		
		print pattern.match(newLine) 
		
	p.wait()     
	print '%s tracefile done...' %benchmark

elif sys.argv[1] == '-d':
	# ---------------------
	# run m5 with dramsimii
	# ---------------------
	run_commandline = '/home/mutien/Tools/m5/build/ALPHA_FS/m5.opt /home/mutien/Tools/dramsimii/m5/configs/example/fsN.py --detailed -b ' + benchmark + ' -n ' + numcore + ' --caches --l3cache -F 10000000000 --mp "channels 2 dimms 2 ranks 2 banks 16 postedCAS true physicaladdressmappingpolicy sdramhiperf commandorderingalgorithm firstAvailableAge perbankqueuedepth 12 readwritegrouping true rowBufferPolicy openpageaggressive outfiledir /home/mutien/Tools/dramsimii/benchmarks/' + benchmark + ' blockSize 256 associativity 16 cacheSize 8192 usingCache true"'

	print 'commandline: %s' %run_commandline
	print 'running %s...' %benchmark
	os.system(run_commandline)
	print '%s done...' %benchmark

else:
	print 'usage: python runme.py <runtype> <num cores> <benchmark> <start time>'
	print 'runtype:'
	print '\t-t: generate tracefile'
	print '\t-d: run (set start time to 0)'
