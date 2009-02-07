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
import bz2
import re
from subprocess import Popen, PIPE, STDOUT
from array import array
import shutil

dramSimExe = 'dramSimII.opt'
dramSimPath = '/home/crius/m5-stable/src/mem/DRAMsimII/build'
m5Path = '/home/crius/m5-stable/build/ALPHA_SE/'
m5Exe = 'm5.fast'
outputDir = '/home/crius/results/mappingStudy10'
memorySettings = '/home/crius/m5-stable/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml'
commandLine = '%s --config-file %s --modifiers "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s"'
m5CommandLine = '%s /home/crius/m5-stable/configs/example/dramsim.py -f %s -b mcf --mp "channels %s ranks %s banks %s physicaladdressmappingpolicy %s commandorderingalgorithm %s averageinterarrivalcyclecount %s perbankqueuedepth %s requestcount %s outfiledir %s"'
channels = [2]
ranks = [4, 8]
banks = [8, 16]
addressMappingPolicy = ['sdramhiperf', 'sdrambase', 'sdramclosepage', 'closepagelowlocality', 'closepagehighlocality']
#commandOrderingAlgorithm = ['greedy','bankroundrobin','rankroundrobin']
commandOrderingAlgorithm = ['greedy']
interarrivalCycleCount = [1]
#perBankQueueDepth = range(8, 16, 4)
perBankQueueDepth = [12]
requests = [13500000]

def main():
    #executable = os.path.join(dramSimPath,dramSimExe)
    executable = os.path.join(m5Path,m5Exe)
    #cmdLine = commandLine
    cmdLine = m5CommandLine

    for a in channels:
        for b in ranks:
            for c in banks:
                for d in addressMappingPolicy:
                    for e in commandOrderingAlgorithm:
                        for f in interarrivalCycleCount:
                            for g in perBankQueueDepth:
                                for h in requests:
                                    currentCommandLine = cmdLine % (executable, memorySettings, a, b, c,
                                                                        d, e, f, g, h, outputDir)
                                    submitCommandLine = '''echo 'time %s' | qsub -q default -o %s -e %s -N "studyCmcf"''' % (currentCommandLine, outputDir, outputDir)
                                    os.system(submitCommandLine)
                                    #print submitCommandLine


if __name__ == "__main__":
     #os.environ["GDFONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"

     main()
     # This is the main function for profiling
     # We've renamed our original main() above to real_main()
     #import cProfile, pstats, StringIO
     #prof = cProfile.Profile()
     #prof = prof.runctx("main()", globals(), locals())
     #stream = StringIO.StringIO()
     #stats = pstats.Stats(prof, stream=stream)
     #stats.sort_stats("time")  # Or cumulative
     #stats.print_stats()  # 80 = how many to print
     # The rest is optional.
     #stats.print_callees()
     #stats.print_callers()
     #print stream.getvalue()
     #logging.info("Profile data:\n%s", stream.getvalue())


