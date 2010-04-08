#!/usr/bin/python

import sys
import os
import string
from threading import Thread
import Queue
import tempfile
import math
import getopt
import re
from subprocess import Popen, PIPE, STDOUT
from array import array
import shutil

######################################################################################
# the name of the queue to submit these jobs to
queueName = 'default'

# the string that sends this command to the queue
submitString = '''echo 'time %%s' | qsub -q %s -o %%s -e %%s -N "%%s"''' % (queueName)

def submitCommand(commandLine, name):
        os.system(commandLine)
        submitCommand = submitString % (commandLine, outputDir, outputDir, name)
        #print commandLine
        #sys.exit(0)
        #os.system(submitCommand)
######################################################################################

# the directory where the traces are
tracesDir = os.path.join(os.path.expanduser("~"),'benchmarks/dsTraces/8MB/16WAY/long')

# the format of the traces
traceType = 'dramsim'

# the list of trace files to run
#traces = ['bzip2003-trace.gz',
#'cactusADM001-trace.gz',
#'calculix000-trace.gz',
#'dealII000-trace.gz',
#'GemsFDTD000-trace.gz',
#'lbm000-trace.gz',
#'mcf000-trace.gz',
#'milc000-trace.gz',
#'namd000-trace.gz',
#'omnetpp000-trace.gz',
#'soplex000-trace.gz']

#traces = ['lbm000-trace.gz', 'mcf000-trace.gz', 'milc000-trace.gz']

# the name of the DRAMsimII executable
dramSimExe = 'dramSimII.opt'

# the path to the DRAMsimII executable
dramSimPath = os.path.join(os.path.expanduser("~"),'dramsimii/bin')

# the path to M5/SE executable
m5Path = os.path.join(os.path.expanduser("~"),'m5/build/ALPHA_SE/')

# the config file for the M5/SE executable
m5SEConfigFile = os.path.join(os.path.expanduser("~"),'m5/configs/example/dramsim.py')

# the path to the M5/FS executable
m5FSPath = os.path.join(os.path.expanduser("~"),'m5/build/ALPHA_FS/')

# the config file for the M5/FS executable
m5FSScript = os.path.join(os.path.expanduser("~"),'m5/configs/example/dramsimfs.py')

# the executable for M5/(FS|SE)
m5Exe = os.path.join(m5FSPath,'m5.fast')

# the directory where the simulation outputs should be written
outputDir = os.path.join(os.path.expanduser("~"),'results/test')

# the file that describes the base memory settings
memorySettings = os.path.join(os.path.expanduser("~"),'dramsimii/memoryDefinitions/DDR2-800-sg125E.xml')

# the command line to pass to the DRAMsimII simulator to modify parameters
commandLine = '%s --config-file %s --modifiers "channels %d dimms %d ranks %d banks %d physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %d perbankqueuedepth %d requestcount %d tfaw %d rowBufferPolicy %s outfiledir %s %s"'

# the command line parameters for running in FS mode
fScommandParameters = "channels %s dimms %d ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s perbankqueuedepth %s readwritegrouping %s rowBufferPolicy %s outfiledir %s"

#m5CommandLine = '%s /home/crius/m5-stable/configs/example/dramsim.py -f %s -b mcf --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s"'
m5SECommandLine = '%s %s -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %d ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s perbankqueuedepth %s outfiledir %s"'
m5CommandLine = '%s %s -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s outfiledir %s"'

# command line to setup full system runs
m5FSCommandLine = '%s %s -b %%s -F 10000000000 --mp "%%s"' % (m5Exe, m5FSScript)

addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'closepagebaseline', 'closepagelowlocality', 'closepagehighlocality', 'closepagebaselineopt']

commandOrderingAlgorithm = ['bankroundrobin', 'rankroundrobin', 'firstAvailableAge', 'firstAvailableRIFF', 'firstAvailableQueue', 'commandPairRankHop', 'strict']

rowBufferManagementPolicy = ['openpageaggressive', 'openpage', 'closepage','closepageaggressive']

interarrivalCycleCount = [4]

perBankQueueDepth = [12]

readWriteGrouping = ['true']

requests = [5000000]

benchmarks = ['calculix', 'milc', 'lbm', 'mcf', 'stream', 'bzip2', 'sjeng', 'xalancbmk', 'GemsFDTD']
#benchmarks = ['bzip2', 'GemsFDTD']

# options for the run
channels = [2]
dimms = [2,4]
ranks = [1,2]
banks = [16]
tFAW = [28]

# per-DIMM cache parameters
associativity = [8, 16, 32]
cacheSizes = [8192, 16384]
blockSize = [64, 128, 256]
hitLatency = [5]
replacementPolicies = ['nmru','lru','mru']
nmruTrackingCounts = [4,6]

###############################################################################

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "tsrfc")
    except getopt.GetoptError, err:
        print str(err)
        print "-t for trace, -s for syscall emulation, -r for random, -f for full-system, -c for cache simulation"
        sys.exit(-2)

    ds2executable = os.path.join(dramSimPath, dramSimExe)
    executable = os.path.join(m5Path, m5Exe)

    for opt, arg in opts:
        if opt == '-t' or opt == '-c':
            if not os.path.exists(ds2executable):
                print "cannot find DRAMsimII executable: " + ds2executable
                sys.exit(-3)
            if not os.access(ds2executable, os.X_OK):
                print "cannot execute DRAMsimII executable: " + ds2executable
                sys.exit(-3)
        elif opt == '-f':
            if not os.path.exists(m5Exe):
                print "cannot find m5 executable: " + m5Exe
                sys.exit(-3)
            if not os.access(m5Exe, os.X_OK):
                print "cannot execute m5 executable: " + m5Exe
                sys.exit(-3)

    counting = False
    count = 0

    for opt, arg in opts:
        if opt != '-f':
            global benchmarks
            benchmarks = ['']

    for opt,arg in opts:
        print opt
    count = 0
    for opt,arg in opts:
        if opt == '-c':
            traces = []
            for a in os.listdir(tracesDir):
                if a.endswith("trace.gz"):
                    traces += [a]

            for channel in channels:
                for dimm in dimms:
                    for rank in ranks:
                        for t in traces:
                            for blkSz in blockSize:
                                for hitLat in hitLatency:
                                    for assoc in associativity:
                                        for size in cacheSizes:
                                            for replacementPolicy in replacementPolicies:

                                                currentTrace = os.path.join(tracesDir, t)
                                                currentCommandLine = commandLine % (ds2executable, memorySettings, channel, dimm, rank,
                                                                                     banks[0], addressMappingPolicy[0],commandOrderingAlgorithm[0],
                                                                                      0, perBankQueueDepth[0], 135000000000000, tFAW[0],
                                                                                       rowBufferManagementPolicy[0], outputDir,
                                                                                       ("inputfiletype %s inputfile %s outfile %s blockSize %s cacheSize %s " +
                                                                                       "hitLatency %s associativity %s readPercentage .8 replacementPolicy %s " +
                                                                                       "%%s") %
                                                                                       (traceType, currentTrace, t, blkSz, size, hitLat, assoc, replacementPolicy))

                                                if replacementPolicy == 'nmru':
                                                    for trackingCount in nmruTrackingCounts:
                                                        newCommandLine = currentCommandLine % ("nmruTrackingCount %d" % trackingCount)
                                                        submitCommand(newCommandLine,t+replacementPolicy+str(trackingCount))
                                                        count += 1
                                                else:
                                                    newCommandLine = currentCommandLine % ""
                                                    submitCommand(newCommandLine, t+replacementPolicy)
                                                    count += 1
            sys.exit(0)
        print count



    for i in benchmarks:
        for a in channels:
            for b in ranks:
                for c in banks:
                    for d in addressMappingPolicy:
                        for e in commandOrderingAlgorithm:
                                for g in perBankQueueDepth:
                                    for j in tFAW:
                                        for k in readWriteGrouping:
                                            for l in rowBufferManagementPolicy:
                                                for opt, arg in opts:
                                                    #print opt, arg, e, g, j, k, l
                                                    # trace file
                                                    if opt == '-t':
                                                        for t in traces:
                                                            currentTrace = os.path.join(tracesDir, t)
                                                            currentCommandLine = commandLine % (ds2executable, memorySettings, a, dimms[0], b, c, d, e, 0, g, 135000000000000, j, l, outputDir, "inputfiletype %s inputfile %s outfile %s" % (traceType, currentTrace, t))

                                                            submitCommand(currentCommandLine,t)
                                                            count += 1

                                                    # syscall emulation
                                                    elif opt == '-s':
                                                        currentCommandLine = m5SECommandLine % (executable, m5SEConfigFile, memorySettings, a, dimms[0], b, c, d, e, g, outputDir)

                                                        submitCommand(currentCommandLine,i)

                                                    # random input
                                                    elif opt == '-r':
                                                        for f in interarrivalCycleCount:
                                                            for h in requests:
                                                                currentCommandLine = commandLine % (ds2executable, memorySettings, a, dimms[0], b, c, d, e, f, g, h, j, l, outputDir, "")
                                                                submitCommand(currentCommandLine,"%d%s%d" % (f,i,h))

                                                    # full system
                                                    elif opt == '-f':
                                                        currentCommandLine = m5FSCommandLine % (i, fScommandParameters % (a, dimms[0], b, c, d, e, g, k, l,  outputDir))
                                                        submitCommand(currentCommandLine,i)

                                                    # variations of the per-DIMM cache
                                                    elif opt == '-c':
                                                        for t in traces:
                                                            for blkSz in blockSize:
                                                                for hitLat in hitLatency:
                                                                    for assoc in associativity:
                                                                        for numSets in numberSets:
                                                                            currentTrace = os.path.join(tracesDir, t)
                                                                            currentCommandLine = commandLine % (ds2executable, memorySettings, a, dimms[0], b, c, d, e, 0, g, 135000000000000, j, l, outputDir, "inputfiletype %s inputfile %s outfile %s blockSize %s numberSets %s hitLatency %s associativity %s readPercentage .8" % (traceType, currentTrace, t, blkSz, numSets, hitLat, assoc))
                                                                            submitCommand(currentCommandLine,t)

                                                #sys.exit(2)


                                                #print submitCommandLine

    if counting:
        print "would be %d jobs." % count


if __name__ == "__main__":

     main()



