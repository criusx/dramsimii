#!/usr/bin/python

import gzip
import sys
import os
import string
from threading import Thread
import Queue
import tempfile
import tarfile
import math
import getopt
import bz2
import re
from subprocess import Popen, PIPE, STDOUT
from array import array
import shutil

# the directory where the traces are
tracesDir = '/home/crius/benchmarks/dsTraces/8MB/16WAY'

# the format of the traces
traceType = 'dramsim'

# the list of trace files to run
traces = ['bzip2003-trace.gz',
'cactusADM001-trace.gz',
'calculix000-trace.gz',
'dealII000-trace.gz',
'GemsFDTD000-trace.gz',
'lbm000-trace.gz',
'mcf000-trace.gz',
'milc000-trace.gz',
'namd000-trace.gz',
'omnetpp000-trace.gz',
'soplex000-trace.gz']

traces = ['lbm000-trace.gz', 'mcf000-trace.gz', 'milc000-trace.gz']
#tracesDir = '/home/crius/benchmarks/addressTraces/'
#traces = ['mase_256K_64_4G/ammp.trc.gz',
#'mase_256K_64_4G/equake.trc.gz',
#'mase_256K_64_4G/mgrid.trc.gz',
#'mase_256K_64_4G/10GB_art.trc.gz',
#'mase_256K_64_4G/vortex.trc.gz',
#'mase_256K_64_4G/art.trc.gz',
#'mase_256K_64_4G/gcc.trc.gz',
#'mase_256K_64_4G/parser.trc.gz',
#'mase_256K_64_4G/gzip.trc.gz',
#'mase_256K_64_4G/galgel.trc.gz',
#'mase_4M_32_2G/ammp.trc.gz',
#'mase_4M_32_2G/equake.trc.gz',
#'mase_4M_32_2G/mgrid.trc.gz',
#'mase_4M_32_2G/vortex.trc.gz',
#'mase_4M_32_2G/art.trc.gz',
#'mase_4M_32_2G/gcc.trc.gz',
#'mase_4M_32_2G/parser.trc.gz',
#'mase_4M_32_2G/gzip.trc.gz',
#'mase_4M_32_2G/galgel.trc.gz',
#'mase_4M_64_2G/ammp.trc.gz',
#'mase_4M_64_2G/equake.trc.gz',
#'mase_4M_64_2G/mgrid.trc.gz',
#'mase_4M_64_2G/vortex.trc.gz',
#'mase_4M_64_2G/art.trc.gz',
#'mase_4M_64_2G/gcc.trc.gz',
#'mase_4M_64_2G/parser.trc.gz',
#'mase_4M_64_2G/gzip.trc.gz',
#'mase_4M_64_2G/galgel.trc.gz',
#'mase_256K_128_2G/ammp.trc.gz',
#'mase_256K_128_2G/equake.trc.gz',
#'mase_256K_128_2G/mgrid.trc.gz',
#'mase_256K_128_2G/vortex.trc.gz',
#'mase_256K_128_2G/art.trc.gz',
#'mase_256K_128_2G/gcc.trc.gz',
#'mase_256K_128_2G/parser.trc.gz',
#'mase_256K_128_2G/gzip.trc.gz',
#'mase_256K_128_2G/galgel.trc.gz',
#'mase_4M_128_2G/ammp.trc.gz',
#'mase_4M_128_2G/equake.trc.gz',
#'mase_4M_128_2G/mgrid.trc.gz',
#'mase_4M_128_2G/vortex.trc.gz',
#'mase_4M_128_2G/art.trc.gz',
#'mase_4M_128_2G/gcc.trc.gz',
#'mase_4M_128_2G/parser.trc.gz',
#'mase_4M_128_2G/gzip.trc.gz',
#'mase_4M_128_2G/galgel.trc.gz',
#'mase_256K_32_2G/ammp.trc.gz',
#'mase_256K_32_2G/equake.trc.gz',
#'mase_256K_32_2G/gzip.trc.gz',
#'mase_256K_32_2G/galgel.trc.gz',
#'mase_256K_64_2G/ammp.trc.gz',
#'mase_256K_64_2G/equake.trc.gz',
#'mase_256K_64_2G/mgrid.trc.gz',
#'mase_256K_64_2G/vortex.trc.gz',
#'mase_256K_64_2G/art.trc.gz',
#'mase_256K_64_2G/gcc.trc.gz',
#'mase_256K_64_2G/parser.trc.gz',
#'mase_256K_64_2G/gzip.trc.gz',
#'mase_256K_64_2G/galgel.trc.gz']

# the name of the DRAMsimII executable
dramSimExe = 'dramSimII.opt'

# the path to the DRAMsimII executable
dramSimPath = '/home/crius/dramsimii/bin'

# the path to M5/SE executable
m5Path = '/home/crius/m5/build/ALPHA_SE/'

# the config file for the M5/SE executable
m5SEConfigFile = '/home/crius/m5/configs/example/dramsim.py'

# the path to the M5/FS executable
m5FSPath = '/home/crius/m5/build/ALPHA_FS/'

# the config file for the M5/FS executable
m5FSScript = '/home/crius/m5/configs/example/dramsimfs.py'

# the executable for M5/(FS|SE)
m5Exe = 'm5.fast'

# the directory where the simulation outputs should be written
outputDir = '/home/crius/results/Cypress/4'

# the file that describes the base memory settings
memorySettings = '/home/crius/dramsimii/memoryDefinitions/DDR2-800-sg125E.xml'

# the command line to pass to the DRAMsimII simulator to modify parameters
commandLine = '%s --config-file %s --modifiers "channels %d ranks %d banks %d physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %d perbankqueuedepth %d requestcount %d tfaw %d rowBufferPolicy %s outfiledir %s %s"'

# the command line parameters for running in FS mode
fScommandParameters = "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s perbankqueuedepth %s readwritegrouping %s rowBufferPolicy %s outfiledir %s"

# the name of the queue to submit these jobs to
queueName = 'default'

# the string that sends this command to the queue
submitString = '''echo 'time %%s' | qsub -q %s -o %%s -e %%s -N "%%s"''' % (queueName)

#m5CommandLine = '%s /home/crius/m5-stable/configs/example/dramsim.py -f %s -b mcf --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s"'
m5SECommandLine = '%s %s -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %d ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s perbankqueuedepth %s outfiledir %s"'
m5CommandLine = '%s %s -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s outfiledir %s"'

# command line to setup full system runs
m5FSCommandLine = '%s %s -b %%s -F 10000000000 --mp "%%s"' % (os.path.join(m5FSPath, m5Exe), m5FSScript)

# options for the run
channels = [2]
ranks = [4]
banks = [16]
tFAW = [28]

addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'closepagebaseline', 'closepagelowlocality', 'closepagehighlocality', 'closepagebaselineopt']

commandOrderingAlgorithm = ['bankroundrobin', 'rankroundrobin', 'firstAvailableAge', 'firstAvailableRIFF', 'firstAvailableQueue', 'commandPairRankHop', 'strict']
#commandOrderingAlgorithm = ['commandPairRankHop', 'strict']

rowBufferManagementPolicy = ['openpageaggressive', 'openpage', 'closepage','closepageaggressive']
#rowBufferManagementPolicy = ['closepage','closepageaggressive']

interarrivalCycleCount = [4]

#perBankQueueDepth = range(8, 16, 4)
perBankQueueDepth = [12]

readWriteGrouping = ['true']

requests = [5000000]

#benchmarks = ['calculix', 'milc', 'lbm', 'mcf', 'stream', 'bzip2', 'sjeng', 'xalancbmk', 'GemsFDTD']
benchmarks = ['bzip2', 'GemsFDTD']

# per-DIMM cache parameters
associativity = [8, 16, 32]
cacheSizes = [2048, 4096, 8192, 16384]
blockSize = [64, 128, 256]
hitLatency = [5]

#traces = ['bzip2-trace.gz', 'lbm-trace.gz', 'mcf-trace.gz']
#traces = ['mcf001-trace.gz','namd002-trace.gz','omnetpp002-trace.gz']
#traces = ['milcLong001-trace.gz', 'lbmLong-trace.gz', 'mcfLong000-trace.gz', 'xalancbmkLong005-trace.gz', 'bzip2-trace.gz']
traces = ['lbmLong-trace.gz']

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
            for t in traces:
                for blkSz in blockSize:
                    for hitLat in hitLatency:
                        for assoc in associativity:
                            for size in cacheSizes:
                                currentTrace = os.path.join(tracesDir, t)
                                currentCommandLine = commandLine % (ds2executable, memorySettings, channels[0], ranks[0], banks[0], addressMappingPolicy[0],
                                                                     commandOrderingAlgorithm[0], 0, perBankQueueDepth[0], 135000000000000, tFAW[0], rowBufferManagementPolicy[0], outputDir, "inputfiletype %s inputfile %s outfile %s blockSize %s cacheSize %s hitLatency %s associativity %s readPercentage .8" % (traceType, currentTrace, t, blkSz, size, hitLat, assoc))
                                #submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                submitCommand = submitString % (currentCommandLine, outputDir, outputDir, t)
                                #print currentCommandLine
                                #sys.exit(0)
                                #if not counting:
                                os.system(submitCommand)
                                #os.system(currentCommandLine)
                                #else:
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
                                                            currentCommandLine = commandLine % (ds2executable, memorySettings, a, b, c, d, e, 0, g, 135000000000000, j, l, outputDir, "inputfiletype %s inputfile %s outfile %s" % (traceType, currentTrace, t))
                                                            #submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                                            submitCommand = submitString % (currentCommandLine, outputDir, outputDir, t)
                                                            #print currentCommandLine
                                                            #sys.exit(0)
                                                            if not counting:
                                                                os.system(submitCommand)
                                                                #os.system(currentCommandLine)
                                                            else:
                                                                count += 1

                                                    # syscall emulation
                                                    elif opt == '-s':
                                                        currentCommandLine = m5SECommandLine % (executable, m5SEConfigFile, memorySettings, a, b, c, d, e, g, outputDir)
                                                        submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                                        #print submitCommandLine

                                                        os.system(submitCommandLine)

                                                    # random input
                                                    elif opt == '-r':
                                                        for f in interarrivalCycleCount:
                                                            for h in requests:
                                                                currentCommandLine = commandLine % (ds2executable, memorySettings, a, b, c , d, e, f, g, h, j, l, outputDir, "")
                                                                #print currentCommandLine
                                                                #os.system("gdb --args " + currentCommandLine)
                                                                #retVal = os.system(currentCommandLine)
                                                                #if retVal != 0:
                                                                #    sys.exit(-1)

                                                                submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                                                #print submitCommandLine
                                                                #sys.exit(-1)
                                                                if not counting:
                                                                    os.system(submitCommandLine)
                                                                else:
                                                                    count += 1

                                                    # full system
                                                    elif opt == '-f':
                                                        currentCommandLine = m5FSCommandLine % (i, fScommandParameters % (a, b, c, d, e, g, k, l,  outputDir))
                                                        submitCommand = submitString % (currentCommandLine, outputDir, outputDir, i)
                                                        print currentCommandLine
                                                        #print submitCommand
                                                        #sys.exit(0)
                                                        #os.system(currentCommandLine)
                                                        #os.system(submitCommand)

                                                    # variations of the per-DIMM cache
                                                    elif opt == '-c':
                                                        for t in traces:
                                                            for blkSz in blockSize:
                                                                for hitLat in hitLatency:
                                                                    for assoc in associativity:
                                                                        for numSets in numberSets:
                                                                            currentTrace = os.path.join(tracesDir, t)
                                                                            currentCommandLine = commandLine % (ds2executable, memorySettings, a, b, c, d, e, 0, g, 135000000000000, j, l, outputDir, "inputfiletype %s inputfile %s outfile %s blockSize %s numberSets %s hitLatency %s associativity %s readPercentage .8" % (traceType, currentTrace, t, blkSz, numSets, hitLat, assoc))
                                                                            #submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                                                            submitCommand = submitString % (currentCommandLine, outputDir, outputDir, t)
                                                                            print currentCommandLine
                                                                            #sys.exit(0)
                                                                            if not counting:
                                                                                #os.system(submitCommand)
                                                                                os.system(currentCommandLine)
                                                                            else:
                                                                                count += 1

                                                #sys.exit(2)


                                                #print submitCommandLine

    if counting:
        print "would be %d jobs." % count


if __name__ == "__main__":

     main()



