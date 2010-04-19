#!/usr/bin/python

import sys
import os
import string
from threading import Thread
from string import Template
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
queueName = 'b_1cpu'

# the string that sends this command to the queue
#submitString = '''echo 'time %%s' | qsub -q %s -o %%s -e %%s -N "%%s"''' % (queueName)
submitString = '''echo '%%s' | qsub -q %s -o %%s -e %%s -N "%%s"''' % (queueName)

def submitCommand(commandLine, name):
        #os.system(commandLine)
        submitCommand = submitString % (commandLine, outputDir, outputDir, name)
        #print commandLine
        #sys.exit(0)
        os.system(submitCommand)
######################################################################################

# the directory where the traces are
tracesDir = os.path.join(os.path.expanduser("~"), 'benchmarks/dsTraces/6MB/24WAY')

# the format of the traces
traceType = 'dramsim'

#traces = ['lbm000-trace.gz', 'mcf000-trace.gz', 'milc000-trace.gz']

# the name of the DRAMsimII executable
dramSimExe = 'dramSimII.opt'

# the path to the DRAMsimII executable
ds2executable = os.path.join(os.path.join(os.path.expanduser("~"), 'dramsimii/bin'), dramSimExe)

# the path to M5/SE executable
m5SeExecutable = os.path.join(os.path.join(os.path.expanduser("~"), 'm5/build/ALPHA_SE/'), 'm5.fast')

# the path to M5/SE executable
m5FsExecutable = os.path.join(os.path.join(os.path.expanduser("~"), 'm5/build/ALPHA_FS/'), 'm5.fast')

# the config file for the M5/SE executable
m5SEConfigFile = os.path.join(os.path.expanduser("~"), 'm5/configs/example/dramsim.py')

# the config file for the M5/FS executable
m5FsScript = os.path.join(os.path.expanduser("~"), 'm5/configs/example/dramsimfs.py')

# the directory where the simulation outputs should be written
outputDir = os.path.join(os.path.expanduser("~"), 'results/Cypress/studyC')

# the file that describes the base memory settings
memorySettings = os.path.join(os.path.expanduser("~"), 'dramsimii/memoryDefinitions/DDR2-800-sg125E.xml')

# the command line to pass to the DRAMsimII simulator to modify parameters
commandLine = '%s --config-file %s --modifiers "channels %d dimms %d ranks %d banks %d postedCAS %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %d perbankqueuedepth %d requestcount %d tfaw %d rowBufferPolicy %s outfiledir %s %s"'

m5SeCommandLine = '%s %s -f %s -c /home/crius/benchmarks/stream/stream-short-opt --mp "channels %d ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s perbankqueuedepth %s outfiledir %s"'

# command line to setup full system runs
m5FsCommandLine = Template(m5FsExecutable + " " + m5FsScript + ' -b $benchmark -F 10000000000')

# the command line parameters for running in FS mode
fsCommandParameters = Template('channels $channels dimms $dimms ranks $ranks banks $banks postedCAS $postedCas physicaladdressmappingpolicy $amp commandorderingalgorithm $coa perbankqueuedepth $pbqd readwritegrouping $rwg rowBufferPolicy $rbmp outfiledir $output')

addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'closepagebaseline', 'closepagelowlocality', 'closepagehighlocality', 'closepagebaselineopt']

commandOrderingAlgorithm = ['firstAvailableAge', 'bankroundrobin', 'rankroundrobin', 'firstAvailableRIFF', 'firstAvailableQueue', 'commandPairRankHop', 'strict']

rowBufferManagementPolicy = []
#rowBufferManagementPolicy += ['openpageaggressive']
rowBufferManagementPolicy += ['openpage']
rowBufferManagementPolicy += ['closepage']
#rowBufferManagementPolicy += ['closepageaggressive']

interarrivalCycleCount = [4]

perBankQueueDepth = [12]

readWriteGrouping = ['true']

postedCas = ['true', 'false']

requests = [5000000]

benchmarks = []
benchmarks += ['calculix']
benchmarks += ['milc']
benchmarks += ['lbm']
benchmarks += ['mcf']
benchmarks += ['stream']
benchmarks += ['bzip2']
benchmarks += ['sjeng']
benchmarks += ['xalancbmk']
benchmarks += ['GemsFDTD']

# options for the run
channels = [2]
dimms = []
dimms += [2]
#dimms += [4]
ranks = []
#ranks += [1]
ranks += [2]
banks = [16]
tFAW = [28]

# per-DIMM cache parameters
associativity = []
associativity += [8]
#associativity += [24]
#associativity += [16]
#associativity += [32]
cacheSizes = []
cacheSizes += [8192]
cacheSizes += [16384]
#cacheSizes += [24576]
blockSize = []
blockSize += [64]
#blockSize += [128]
blockSize += [256]
hitLatency = [5]
replacementPolicies = []
replacementPolicies += ['nmru']
#replacementPolicies += ['lru']
#replacementPolicies += ['mru']
nmruTrackingCounts = []
#nmruTrackingCounts += [4]
nmruTrackingCounts += [6]

###############################################################################

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "tsrfc")
    except getopt.GetoptError, err:
        print str(err)
        print "-t for trace, -s for syscall emulation, -r for random, -f for full-system, -c for cache simulation"
        sys.exit(-2)


    for opt, arg in opts:
        if opt == '-t' or opt == '-c':
            if not os.path.exists(ds2executable):
                print "cannot find DRAMsimII executable: " + ds2executable
                sys.exit(-3)
            if not os.access(ds2executable, os.X_OK):
                print "cannot execute DRAMsimII executable: " + ds2executable
                sys.exit(-3)
        elif opt == '-f':
            if not os.path.exists(m5FsExecutable):
                print "cannot find m5 executable: " + m5FsExecutable
                sys.exit(-3)
            if not os.access(m5FsExecutable, os.X_OK):
                print "cannot execute m5 executable: " + m5FsExecutable
                sys.exit(-3)

    counting = False
    count = 0

    for opt, arg in opts:

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
                                                for pc in postedCas:
                                                    for rbmp in rowBufferManagementPolicy:

                                                        currentTrace = os.path.join(tracesDir, t)
                                                        currentCommandLine = commandLine % (ds2executable, memorySettings, channel, dimm, rank,
                                                                                             banks[0], pc, addressMappingPolicy[0], commandOrderingAlgorithm[0],
                                                                                              0, perBankQueueDepth[0], 135000000000000, tFAW[0],
                                                                                               rbmp, outputDir,
                                                                                               ("inputfiletype %s inputfile %s outfile %s blockSize %s cacheSize %s " +
                                                                                               "hitLatency %s associativity %s readPercentage .8 replacementPolicy %s " +
                                                                                               "%%s") %
                                                                                               (traceType, currentTrace, t, blkSz, size, hitLat, assoc, replacementPolicy))

                                                        if replacementPolicy == 'nmru':
                                                            for trackingCount in nmruTrackingCounts:
                                                                newCommandLine = currentCommandLine % ("nmruTrackingCount %d" % trackingCount)
                                                                submitCommand(newCommandLine, t + replacementPolicy + str(trackingCount))
                                                                count += 1
                                                        else:
                                                            newCommandLine = currentCommandLine % ""
                                                            submitCommand(newCommandLine, t + replacementPolicy)
                                                            count += 1

            print str(count) + " simulations to be run."

        else:
            for channel in channels:
                for dimm in dimms:
                    for rank in ranks:
                        for bank in banks:
                            for amp in addressMappingPolicy:
                                for coa in commandOrderingAlgorithm:
                                    for pbqd in perBankQueueDepth:
                                        for j in tFAW:
                                            for rwg in readWriteGrouping:
                                                for rbmp in rowBufferManagementPolicy:
                                                    for pc in postedCas:
                                                        for benchmark in benchmarks:
                                                            for opt, arg in opts:

                                                                # trace file
                                                                if opt == '-t':
                                                                    for t in traces:
                                                                        currentTrace = os.path.join(tracesDir, t)
                                                                        currentCommandLine = commandLine % (ds2executable, memorySettings, a, dimm, b, c, d, e, 0, g, 135000000000000, j, l, outputDir, "inputfiletype %s inputfile %s outfile %s" % (traceType, currentTrace, t))

                                                                        submitCommand(currentCommandLine, t)
                                                                        count += 1

                                                                # syscall emulation
                                                                elif opt == '-s':
                                                                    currentCommandLine = m5SECommandLine % (executable, m5SEConfigFile, memorySettings, a, dimm, b, c, d, e, g, outputDir)

                                                                    submitCommand(currentCommandLine, i)

                                                                # random input
                                                                elif opt == '-r':
                                                                    for f in interarrivalCycleCount:
                                                                        for h in requests:
                                                                            currentCommandLine = commandLine % (ds2executable, memorySettings, a, dimm, b, c, d, e, f, g, h, j, l, outputDir, "")
                                                                            submitCommand(currentCommandLine, "%d%s%d" % (f, i, h))

                                                                # full system
                                                                elif opt == '-f':
                                                                    currentCommandLine = \
                                                                    m5FsCommandLine.substitute(benchmark=benchmark) + ' --mp "' + fsCommandParameters.substitute(channels=channel, dimms=dimm, ranks=rank, banks=bank, \
                                                                                                                                  amp=amp, coa=coa, pbqd=pbqd, rwg=rwg, rbmp=rbmp, \
                                                                                                                                  output=outputDir, benchmark=benchmark, postedCas=pc) + '"'
                                                                    #                                (i, fScommandParameters % (channel, dimm, rank, bank, amp, coa, pbqd, rwg, rbmp, outputDir))
                                                                    submitCommand(currentCommandLine, benchmark)

                                                                # variations of the per-DIMM cache
                                                                elif opt == '-c':
                                                                    for t in traces:
                                                                        for blkSz in blockSize:
                                                                            for hitLat in hitLatency:
                                                                                for assoc in associativity:
                                                                                    for numSets in numberSets:
                                                                                        currentTrace = os.path.join(tracesDir, t)
                                                                                        currentCommandLine = commandLine % (ds2executable, memorySettings, a, dimms[0], b, c, d, e, 0, g, 135000000000000, j, l, outputDir, "inputfiletype %s inputfile %s outfile %s blockSize %s numberSets %s hitLatency %s associativity %s readPercentage .8" % (traceType, currentTrace, t, blkSz, numSets, hitLat, assoc))
                                                                                        submitCommand(currentCommandLine, t)

                                                            #sys.exit(2)


                                                            #print submitCommandLine



if __name__ == "__main__":

     main()



