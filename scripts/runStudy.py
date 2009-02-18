#!/usr/bin/python -O

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

tracesDir = '/home/crius/benchmarks/addressTraces/'
traces = ['mase_256K_64_4G/ammp.trc.gz',
'mase_256K_64_4G/equake.trc.gz',
'mase_256K_64_4G/mgrid.trc.gz',
'mase_256K_64_4G/10GB_art.trc.gz',
'mase_256K_64_4G/vortex.trc.gz',
'mase_256K_64_4G/art.trc.gz',
'mase_256K_64_4G/gcc.trc.gz',
'mase_256K_64_4G/parser.trc.gz',
'mase_256K_64_4G/gzip.trc.gz',
'mase_256K_64_4G/galgel.trc.gz',
'mase_4M_32_2G/ammp.trc.gz',
'mase_4M_32_2G/equake.trc.gz',
'mase_4M_32_2G/mgrid.trc.gz',
'mase_4M_32_2G/vortex.trc.gz',
'mase_4M_32_2G/art.trc.gz',
'mase_4M_32_2G/gcc.trc.gz',
'mase_4M_32_2G/parser.trc.gz',
'mase_4M_32_2G/gzip.trc.gz',
'mase_4M_32_2G/galgel.trc.gz',
'mase_4M_64_2G/ammp.trc.gz',
'mase_4M_64_2G/equake.trc.gz',
'mase_4M_64_2G/mgrid.trc.gz',
'mase_4M_64_2G/vortex.trc.gz',
'mase_4M_64_2G/art.trc.gz',
'mase_4M_64_2G/gcc.trc.gz',
'mase_4M_64_2G/parser.trc.gz',
'mase_4M_64_2G/gzip.trc.gz',
'mase_4M_64_2G/galgel.trc.gz',
'mase_256K_128_2G/ammp.trc.gz',
'mase_256K_128_2G/equake.trc.gz',
'mase_256K_128_2G/mgrid.trc.gz',
'mase_256K_128_2G/vortex.trc.gz',
'mase_256K_128_2G/art.trc.gz',
'mase_256K_128_2G/gcc.trc.gz',
'mase_256K_128_2G/parser.trc.gz',
'mase_256K_128_2G/gzip.trc.gz',
'mase_256K_128_2G/galgel.trc.gz',
'mase_4M_128_2G/ammp.trc.gz',
'mase_4M_128_2G/equake.trc.gz',
'mase_4M_128_2G/mgrid.trc.gz',
'mase_4M_128_2G/vortex.trc.gz',
'mase_4M_128_2G/art.trc.gz',
'mase_4M_128_2G/gcc.trc.gz',
'mase_4M_128_2G/parser.trc.gz',
'mase_4M_128_2G/gzip.trc.gz',
'mase_4M_128_2G/galgel.trc.gz',
'mase_256K_32_2G/ammp.trc.gz',
'mase_256K_32_2G/equake.trc.gz',
'mase_256K_32_2G/gzip.trc.gz',
'mase_256K_32_2G/galgel.trc.gz',
'mase_256K_64_2G/ammp.trc.gz',
'mase_256K_64_2G/equake.trc.gz',
'mase_256K_64_2G/mgrid.trc.gz',
'mase_256K_64_2G/vortex.trc.gz',
'mase_256K_64_2G/art.trc.gz',
'mase_256K_64_2G/gcc.trc.gz',
'mase_256K_64_2G/parser.trc.gz',
'mase_256K_64_2G/gzip.trc.gz',
'mase_256K_64_2G/galgel.trc.gz']

dramSimExe = 'dramSimII.opt'
dramSimPath = '/home/crius/m5-stable/src/mem/DRAMsimII/build'
m5Path = '/home/crius/m5-stable/build/ALPHA_SE/'
m5Exe = 'm5.fast'
outputDir = '/home/crius/results/mappingStudy12'
memorySettings = '/home/crius/m5-stable/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml'
commandLine = '%s --config-file %s --modifiers "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s %s"'

#m5CommandLine = '%s /home/crius/m5-stable/configs/example/dramsim.py -f %s -b mcf --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s"'
m5CommandLine = '%s /home/crius/m5-stable/configs/example/dramsim.py -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s"'
channels = [2]
ranks = [2, 4, 8]
banks = [8, 16]
addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'sdramclosepage', 'closepagelowlocality', 'closepagehighlocality', 'closepagebaselineopt']
#commandOrderingAlgorithm = ['greedy','bankroundrobin','rankroundrobin']
commandOrderingAlgorithm = ['greedy']
interarrivalCycleCount = [1]
#perBankQueueDepth = range(8, 16, 4)
perBankQueueDepth = [12]
requests = [13500000]
benchmarks = ['calculix', 'libquantum', ]

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "tbr")
    except getopt.GetoptError, err:
        print str(err)
        print "-t for trace, -b for benchmark, -r for random"
        sys.exit(-2)

    ds2executable = os.path.join(dramSimPath, dramSimExe)
    executable = os.path.join(m5Path, m5Exe)

    for a in channels:
        for b in ranks:
            for c in banks:
                for d in addressMappingPolicy:
                    for e in commandOrderingAlgorithm:
                        for f in interarrivalCycleCount:
                            for g in perBankQueueDepth:
                                for h in requests:
                                    for opt, arg in opts:
                                        # trace file
                                        if opt == '-t':
                                            for i in traces:
                                                currentTrace = os.path.join(tracesDir, i)
                                                currentCommandLine = commandLine % (ds2executable, memorySettings, a, b, c, d, e, f, g, h, outputDir, "inputfiletype mase inputfile %s" % currentTrace)
                                                submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                                #print submitCommandLine
                                                #sys.exit(0)
                                                os.system(submitCommandLine)

                                        # m5 benchmark
                                        elif opt == '-b':
                                            currentCommandLine = m5CommandLine % (executable, memorySettings, a, b, c, d, e, f, g, h, outputDir)
                                            submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                            print submitCommandLine

                                            os.system(submitCommandLine)

                                        # random input
                                        elif opt == '-r':
                                            currentCommandLine = commandLine % (ds2executable, memorySettings, a, b, c, d, e, f, g, h, outputDir, requests[0])
                                            submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyMap"''' % (currentCommandLine, outputDir, outputDir)
                                            print submitCommandLine

                                            os.system(submitCommandLine)

                                    #sys.exit(2)


                                    #print submitCommandLine


if __name__ == "__main__":

     main()



