#!/usr/bin/python -O

from subprocess import Popen, PIPE, STDOUT
import signal
import sys
import os
import re
import gzip

runtype = sys.argv[1]
numcore = sys.argv[2]
benchmarkCommand = ''
benchmark = ''
benchmarkString = ''

if len(sys.argv) > 3:
	benchmarkString = "-b %s" % sys.argv[3]
	benchmark = sys.argv[3]

#starttime = 2800000000000
#starttime = 75501953411000
#for bootup
#starttime = 4744140934800
#for TPCC-UVA
#starttime = 24938083141000
numberToSkip = 10000000000000000
linesToBuffer = 32768

dramsimDirectory = os.path.join(os.path.expanduser("~"), 'dramsimii')
m5directory = os.path.join(os.path.expanduser("~"), 'm5')
m5binary = os.path.join(m5directory, 'build/ALPHA_FS/m5.opt')
m5Script = os.path.join(dramsimDirectory, 'm5/configs/example/fs.py')

#m5flags = '--trace-flags=Cache --trace-start=%d' % (starttime)
m5flags = ''
m5scriptFlags = '--detailed -n %s --caches --l3cache -F %d %s --mp "outfiletype none"' % (numcore, numberToSkip, benchmarkString)

# ----
# main
# ----
if runtype == '-t':
	# -------------------
	# generate trace file
	# -------------------
	commandline = "%s %s %s %s" % (m5binary, m5flags, m5Script, m5scriptFlags)

	proc = int(numcore)

	print 'commandline: %s, tracefile: %s' % (commandline, benchmark)

	
	pattern = re.compile("([0-9]+): system.l3cache: (ReadReq \(ifetch\)|ReadExReq|ReadReq|Writeback) ([0-9a-f]+).*")
	switchPattern = re.compile("Switched CPUS @ cycle = [0-9]+")

	filename = "%s_%s.gz" % (benchmark, numcore)
	dir = ""

	if (os.path.isfile(filename) == True):
		counter = 0
		while (True):
			filename = "%s_%s_%03d.gz" % (benchmark, numcore, counter)
			dirname = "%s_%s%03d" % (benchmark, numcore, counter)
			dir = os.path.join(os.curdir, dirname)
			if not os.path.isfile(filename) and not os.path.isdir(dir):
				outfile = gzip.open(filename, 'wb')
				if not os.mkdir(dirname):
					print "Could not create directory %s" % dir
					sys.exit(-1)
				break
			else:
				counter = counter + 1

	else:
		outfile = gzip.open(filename, 'wb')

	lines = []
	switched = False
	
	p = Popen([commandline], shell=True, executable="/bin/zsh", stdout=PIPE, cwd=dir)
	print "PID is %d" % p.pid


	while True:
		newLine = p.stdout.readline()
    		if not newLine: break

    		if not newLine[0].isdigit():
    			print newLine,

		if switched == True:
			m = pattern.match(newLine)

			if m is not None:
				if m.group(2) == "ReadReq (ifetch)":
					value = '2'
				elif m.group(2) == "Writeback":
					value = '1'
				else:
					value = '0'

				inpvalue = value + " " + m.group(3) + " " + m.group(1) + "\n"
				lines.append(inpvalue)

				if (len(lines) > linesToBuffer):
					outfile.writelines(lines)
					lines = []

		elif newLine.startswith("Switched"):
			print "Switched to timing mode"
			# m5's event handlers should be updated to setTraceFlag("Cache"); when the SIGUSR1 signal is detected
			p.send_signal(signal.SIGUSR1)
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
