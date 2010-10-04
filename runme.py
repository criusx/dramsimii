#!/usr/bin/python -O

from subprocess import Popen, PIPE, STDOUT
import sys
import os
import re
import gzip

runtype = sys.argv[1]
numcore = sys.argv[2]
benchmark = sys.argv[3]
starttime = sys.argv[4]

dramsimDirectory = os.path.join(os.path.expanduser("~"), 'dramsimii')
m5directory = os.path.join(os.path.expanduser("~"), 'm5')
m5binary = os.path.join(m5directory, 'build/ALPHA_FS/m5.opt')
m5Script = os.path.join(dramsimDirectory, 'm5/configs/example/fs.py')

m5flags = '--trace-flags=Cache --trace-start=' + starttime
m5scriptFlags = '--detailed -n' + numcore + ' --caches --l3cache -F 10000000000 -b ' + benchmark

#sedCommand0 = "sed -n -e 's/ReadReq (ifetch)/2/p' -e 's/ReadExReq/0/p' -e 's/ReadReq/0/p' -e 's/Writeback/1/p'"
#sedCommand1 = "sed -e 's/ [a-z]*$//' -e 's/:.*://'"
#sedCommand2 = "sed -n -e 's/\(.*\) \(.*\) \(.*\)/\2 \3 \1/p'"

# ----
# main
# ----
if runtype == '-t':
	# -------------------
	# generate trace file
	# -------------------
	#tracefile_commandline = m5binary + m5flags + "p \"" |&grep l3cache|gzip > /home/mutien/Tools/dramsimii/benchmarks/' + benchmark + '/' + benchmark + 'Stream.gz'
	commandline = "%s %s %s %s" % (m5binary, m5flags, m5Script, m5scriptFlags)

	print 'commandline: %s' % commandline
	print 'generating %s tracefile...' % benchmark
	#p = Popen([tracefile_commandline], shell=True, executable="/bin/bash")
	p = Popen([commandline], shell = True, executable = "/bin/zsh", stdout = PIPE)
	#out, err = p.communicate()
	#print out, err

	pattern = re.compile("([0-9]+): system.l3cache: (ReadReq (ifetch)|ReadExReq|ReadReq|Writeback) ([0-9a-f]+).*")

	filename = "%s_%s.gz" % (benchmark, numcore)

	if (os.path.isfile(filename) == True):
		counter = 0
		while (True):
			filename = "%s_%s_%03d.gz" % (benchmark, numcore, counter)
			if (os.path.isfile(filename) == False):
				print "going with " + filename
				outfile = gzip.open(filename, 'wb')
				break
			else:
				counter = counter + 1
				print "counter at %d" % (counter)

	else:
		print "did not find"
		outfile = gzip.open(filename, 'wb')

	lines = []

	while True:
		#newLine = out.readline()
		newLine = p.stdout.readline()
    		if not newLine: break

		#print newLine
		m = pattern.match(newLine)
		if m is not None:
			value = '2'
			if m.group(2) == "ReadReq" or m.group(2) == "ReadExReq":
				value = '0'
			elif m.group(2) == "Writeback":
				value = '1'

			lines.append(value + " " + m.group(4) + " " + m.group(1))

			if (len(lines) > 8192):
				outfile.writelines(lines)
				lines = []

	print "Finished running, waiting to finish"
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
