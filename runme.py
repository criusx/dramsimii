#!/usr/bin/python -O

from subprocess import Popen, PIPE, STDOUT
import sys
import os
import re
import gzip

runtype = sys.argv[1]
numcore = sys.argv[2]
benchmark = sys.argv[3]
starttime = 2200000000000
linesToBuffer = 16384

dramsimDirectory = os.path.join(os.path.expanduser("~"), 'dramsimii')
m5directory = os.path.join(os.path.expanduser("~"), 'm5')
m5binary = os.path.join(m5directory, 'build/ALPHA_FS/m5.opt')
m5Script = os.path.join(dramsimDirectory, 'm5/configs/example/fs.py')

m5flags = '--trace-flags=Cache --trace-start=%d' % (starttime)
m5scriptFlags = '--detailed -n %s --caches --l3cache -F 1000000000 -b %s' % (numcore, benchmark)

# ----
# main
# ----
if runtype == '-t':
	# -------------------
	# generate trace file
	# -------------------
	commandline = "%s %s %s %s" % (m5binary, m5flags, m5Script, m5scriptFlags)
	
	proc = int(numcore)
	
	print 'commandline: %s' % commandline
	print 'generating %s tracefile...' % benchmark
	p = Popen([commandline], shell = True, executable = "/bin/zsh", stdout = PIPE)
	
	pattern = re.compile("([0-9]+): system.l3cache: (ReadReq (ifetch)|ReadExReq|ReadReq|Writeback) ([0-9a-f]+).*")
	switchPattern = re.compile("Switched CPUS @ cycle = [0-9]+")

	filename = "%s_%s.gz" % (benchmark, numcore)

	if (os.path.isfile(filename) == True):
		counter = 0
		while (True):
			filename = "%s_%s_%03d.gz" % (benchmark, numcore, counter)
			if (os.path.isfile(filename) == False):
				outfile = gzip.open(filename, 'wb')
				break
			else:
				counter = counter + 1

	else:
		outfile = gzip.open(filename, 'wb')

	lines = []
	switched = False

	while True:
		newLine = p.stdout.readline()
    		if not newLine: break

		if switched == True:
			m = pattern.match(newLine)

			if m is not None:
				value = '2'
				if m.group(2) == "ReadReq" or m.group(2) == "ReadExReq":
					value = '0'
				elif m.group(2) == "Writeback":
					value = '1'

				lines.append(value + " " + m.group(4) + " " + m.group(1) + "\n")

				if (len(lines) > linesToBuffer):
					outfile.writelines(lines)
					lines = []
		elif switchPattern.match(newLine) is not None:
			switched = True


	p.wait()
	print '%s tracefile done...' % benchmark

elif runtype == '-d':
	# ---------------------
	# run m5 with dramsimii
	# ---------------------
	run_commandline = '/home/mutien/Tools/m5/build/ALPHA_FS/m5.opt /home/mutien/Tools/dramsimii/m5/configs/example/fsN.py --detailed -b ' + benchmark + ' -n ' + numcore + ' --caches --l3cache -F 10000000000 --mp "channels 2 dimms 2 ranks 2 banks 16 postedCAS true physicaladdressmappingpolicy sdramhiperf commandorderingalgorithm firstAvailableAge perbankqueuedepth 12 readwritegrouping true rowBufferPolicy openpageaggressive outfiledir /home/mutien/Tools/dramsimii/benchmarks/' + benchmark + ' blockSize 256 associativity 16 cacheSize 8192 usingCache true"'

	print 'commandline: %s' % run_commandline
	print 'running %s...' % benchmark
	os.system(run_commandline)
	print '%s done...' % benchmark

else:
	print 'usage: python runme.py <runtype> <num cores> <benchmark> <start time>'
	print 'runtype:'
	print '\t-t: generate tracefile'
	print '\t-d: run (set start time to 0)'
