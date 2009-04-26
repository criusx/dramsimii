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

tracesDir = '/home/crius/m5'
traceType = 'dramsim'
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
traces = ['lbm000-trace.gz']
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

dramSimExe = 'dramSimII.profuse'
dramSimPath = '/home/crius/m5/src/mem/DRAMsimII/build'
m5Path = '/home/crius/m5/build/ALPHA_SE/'
m5SEConfigFile = '/home/crius/m5/configs/example/dramsim.py'
m5FSPath = '/home/crius/m5/build/ALPHA_FS/'
m5FSScript = '/home/crius/m5/configs/example/dramsimfs.py'
m5Exe = 'm5.fast'
outputDir = '/home/crius/results/mappingStudy15'
memorySettings = '/home/crius/m5/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml'
commandLine = '%s --config-file %s --modifiers "channels %d ranks %d banks %d physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %d perbankqueuedepth %d requestcount %d tfaw %d outfiledir %s %s"'
fScommandParameters = "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s perbankqueuedepth %s outfiledir %s"
queueName = 'default'
submitString = '''echo 'time %%s' | qsub -q %s -o %%s -e %%s -N "%%s"''' % (queueName)
#m5CommandLine = '%s /home/crius/m5-stable/configs/example/dramsim.py -f %s -b mcf --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s"'
m5SECommandLine = '%s %s -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %d ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s perbankqueuedepth %s outfiledir %s"'
m5CommandLine = '%s %s -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s outfiledir %s"'
m5FSCommandLine = '%s %s -b %%s -F 10000000000 --nopre --mp "%%s"' % (os.path.join(m5FSPath, m5Exe), m5FSScript)
channels = [2]
ranks = [4]
banks = [16]
tFAW = [28]
#addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'closepagebaseline', 'closepagelowlocality', 'closepagehighlocality', 'closepagebaselineopt']
#addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'closepagelowlocality', 'closepagehighlocality', 'closepagebaselineopt']
#addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'closepagelowlocality', 'closepagebaselineopt']
#addressMappingPolicy = ['closepagebaseline', 'sdramhiperf']
#addressMappingPolicy = ['closepagebaselineopt']
addressMappingPolicy = ['closepagebaselineopt','closepagelowlocality']
#addressMappingPolicy = ['closepagebaseline']
#commandOrderingAlgorithm = ['firstAvailable', 'bankroundrobin', 'rankroundrobin', 'strict']
#commandOrderingAlgorithm = ['firstAvailable', 'bankroundrobin', 'rankroundrobin']
commandOrderingAlgorithm = ['firstAvailable']
interarrivalCycleCount = [1]
#perBankQueueDepth = range(8, 16, 4)
perBankQueueDepth = [12]
requests = [9223372036854775808]
#benchmarks = ['calculix', 'milc', 'lbm', 'mcf', 'stream', 'bzip2', 'sjeng', 'xalancbmk', 'GemsFDTD']
#benchmarks = ['stream']
benchmarks = ['lbm']

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "tsrf")
    except getopt.GetoptError, err:
        print str(err)
        print "-t for trace, -s for syscall emulation, -r for random, -f for full-system"
        sys.exit(-2)

    ds2executable = os.path.join(dramSimPath, dramSimExe)
    executable = os.path.join(m5Path, m5Exe)

    counting = False
    count = 0

    for a in channels:
        for b in ranks:
            for c in banks:
                for d in addressMappingPolicy:
                    for e in commandOrderingAlgorithm:
                            for g in perBankQueueDepth:
                                for j in tFAW:
                                    for opt, arg in opts:
                                        # trace file
                                        if opt == '-t':
                                            for i in traces:
                                                currentTrace = os.path.join(tracesDir, i)
                                                currentCommandLine = commandLine % (ds2executable, memorySettings, a, b, c, d, e, 0, g, 135000000000000, j, outputDir, "inputfiletype %s inputfile %s outfile %s" % (traceType, currentTrace, i))
                                                submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                                #print submitCommandLine
                                                #sys.exit(0)
                                                if not counting:
                                                    os.system(submitCommandLine)
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
                                                    currentCommandLine = commandLine % (ds2executable, memorySettings, a, b, c, d, e, f, g, h, outputDir, requests[0])
                                                    submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                                    print submitCommandLine
                                                    sys.exit(-1)
                                                    os.system(submitCommandLine)

                                        # full system
                                        elif opt == '-f':
                                            for i in benchmarks:
                                                currentCommandLine = m5FSCommandLine % (i, fScommandParameters % (a, b, c, d, e, g, outputDir))
                                                submitCommand = submitString % (currentCommandLine, outputDir, outputDir, i)
                                                #print submitCommand
                                                os.system(submitCommand)

                                    #sys.exit(2)


                                    #print submitCommandLine

    if counting:
        print "would be %d jobs." % count


if __name__ == "__main__":

     main()



