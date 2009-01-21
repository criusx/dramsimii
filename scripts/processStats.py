#!/usr/bin/python -O
##!/usr/bin/python

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

#globals
workerThreads = 3

IPCWindow = 20

processPerEpochGraphs = False

#terminal = 'set terminal png font "VeraMono,10" transparent nointerlace truecolor  size 1700, 1024 nocrop enhanced\n'
#extension = 'png'

terminal = 'set terminal svg size 1600,768 dynamic enhanced fname "VeraMono" fsize 10\n'
extension = 'svg'

#terminal = 'set terminal postscript eps enhanced color font "VeraMono, 10"'
#extension = 'eps'

class WeightedAverage:
    def __init__(self):
        self.total = 0
        self.count = 0

    def add(self, count, value):
        self.total += count * value
        self.count += count

    def average(self):
        if self.count > 0:
            return self.total / self.count
        else:
            return 0

    def clear(self):
        self.total = 0
        self.count = 0

class RingBuffer:
    def __init__(self, size):
        self.data = array('f',[0.0 for i in xrange(size)])

    def append(self, x):
        self.data.pop(0)
        self.data.append(x)

    def get(self):
        return self.data

    def average(self):
        if len(self.data) > 0:
            return sum(self.data) / len(self.data)
        else:
            return 0.0

    def finishAverage(self):
        self.data.pop(0)
        return self.average()

def processPower(filename):
    # remember the temp directory
    tempPath = tempfile.mkdtemp(prefix="processStats")

    # setup the script headers
    scripts = ['''
    %s
    unset border
    set key outside center top horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5
    set autoscale xfixmin
    set autoscale xfixmax
    set yrange [0:*] noreverse nowriteback

    set ytics out

    set multiplot
    set ylabel "Power Dissipated (mW)" offset character .05, 0,0 font "VeraMono,12" textcolor lt -1 rotate by 90
    set size 1.0, 0.75
    set origin 0.0, 0.25
    unset xtics
    set boxwidth 0.95 relative
    set style fill  solid 1.00 noborder
    set style data histograms
    #set style data filledcurves below x1
    set style histogram rowstacked title offset 0,0,0
    ''' % terminal,'''
    set size 1.0, 0.27
    set origin 0.0, 0.0
    set autoscale xfixmin
    set autoscale xfixmax
    set yrange [0:*] noreverse nowriteback
    unset title
    set ytics out
    set xtics out
    set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5
    set ylabel "Power Dissipated (mW)" offset character .05, 0,0 font "VeraMono,12" textcolor lt -1 rotate by 90
    set xlabel "Time (s)" font "VeraMono,12"

    set boxwidth 0.95 relative
    unset x2tics

    plot '-' u 1:2 t "Cumulative Average" w lines lw 2.00, '-' u 1:2 t "Total Power" w boxes, '-' u 1:2 t "Running Average" w lines lw 2.00
    ''']

    writing = 0
    p = Popen(['gnuplot'], stdin=PIPE, stdout=PIPE, shell=False)

    # list of arrays
    values = []
    averageValues = array('f')
    instantValues = array('f')
    windowValues = array('f')
    times = array('f')
    powerWindow = RingBuffer(IPCWindow)
    totalPower = 0.0
    epochPower = 0.0
    channels = 0
    ranks = 0
    epochNumber = 0

    try:
        if filename.endswith("gz"):
            basefilename = filename.split('.gz')[0]
            outFileBZ2 = os.path.join(os.getcwd(),  filename.split('.gz')[0] + ".tar.bz2")
            compressedstream = gzip.open(filename, 'rb')
        else:
            basefilename = filename.split('.bz2')[0]
            outFileBZ2 = os.path.join(os.getcwd(),  filename.split('.bz2')[0] + ".tar.bz2")
            compressedstream = bz2.open(filename, 'rb')

        splitter = re.compile('([\[\]])')
        splitter2 = re.compile(' ')

        for line in compressedstream:
            if line[0] == '-':
              # normal lines
                if line[1] == 'P':
                    startVal = line.find("{")
                    if startVal > 0:
                        endVal = line.find("}")
                        thisPower = float(line[startVal + 1:endVal])
                        values[writing].append(thisPower)

                        writing = (writing + 1) % (valuesPerEpoch)
                        epochPower += thisPower
                        totalPower += thisPower
                        if writing == 0:
                            epochNumber += 1
                            averageValues.append(totalPower / epochNumber)
                            instantValues.append(epochPower)
                            powerWindow.append(epochPower)
                            windowValues.append(powerWindow.average())
                            epochPower = 0.0

                elif line.startswith('----Epoch'):
                    splitLine = splitter2.split(line)
                    epochTime = float(splitLine[1])

                elif line.startswith('----Command Line:'):
                    commandLine = line.strip().split(":")[1]
                    print commandLine
                    p.stdin.write("set title \"{/=12 Power Consumed vs. Time}\\n{/=10 %s}\"  offset character 0, -1, 0 font \"VeraMono,14\" norotate\n" % commandLine)

                elif line[1] == '+':
                    p.stdin.write('set output "' + basefilename + "." + extension + '\n')
                    p.stdin.write(scripts[0])

                    splitLine = splitter.split(line)
                    channels = int(splitLine[2])
                    ranks = int(splitLine[6])

                    p.stdin.write('plot ')

                    for a in range(channels):
                        for b in range(2 * ranks):
                           p.stdin.write('"-" using 1 title "%s ch[%d]rk[%d]"' % ("P_{sys}(ACT-STBY)" if b % 2 == 0 else "P_{sys}(ACT)", a, b))
                           p.stdin.write(",")
                        for b in range(2):
                            p.stdin.write('"-" using 1 title "P_{sys} ch[%d](%s)"' % (a, "RD" if b % 2 == 0 else "WR"))
                            if (a < channels - 1) or (b < 1):
                                p.stdin.write(",")
                        #if (a == channels - 1):
                    p.stdin.write("\n")
                    # plus the RD and WR values
                    valuesPerEpoch = channels * (2 * ranks + 2)
                    for i in range(valuesPerEpoch):
                        values.append(array('f'))

    except IOError, strerror:
        print "I/O error", strerror
    except OSError, strerror:
        print "OS error", strerror
    except IndexError, strerror:
        print "IndexError",strerror

    for u in values:
        for v in u:
            p.stdin.write("%f\n" % v)
        p.stdin.write("e\n")

    p.stdin.write(scripts[1])

    epochNumber = 0
    for u in averageValues:
        p.stdin.write("%f %f\n" % (epochNumber * epochTime, u))
        epochNumber += 1
    p.stdin.write("e\n")
    epochNumber = 0
    for u in instantValues:
        p.stdin.write("%f %f\n" % (epochNumber * epochTime, u))
        epochNumber += 1
    p.stdin.write("e\n")

    for i in range(0,len(windowValues)):
        p.stdin.write("%f %f\n" % (max(i - IPCWindow / 2.0, i / 2.0) * epochTime, windowValues[i]))
    for i in range(len(windowValues),len(windowValues) + IPCWindow / 2):
        p.stdin.write("%f %f\n" % (max(i - IPCWindow / 2.0, i / 2.0) * epochTime, powerWindow.finishAverage()))
    p.stdin.write("e\n")

    p.stdin.write("unset multiplot\nunset output\nexit\n")
    p.wait()

    files = os.listdir(tempPath)

    for x in files:
        fullPath = os.path.join(tempPath,x)
        os.remove(fullPath)

    os.rmdir(tempPath)


    # make a big file of them all
    #os.system("tar cjf " + outFileBZ2 + " --remove-files " + string.join(fileList, ' '))


def processStats(filename):
    # remember the temp directory
    tempPath = tempfile.mkdtemp(prefix="processStats")

    # start counting lines
    #process = Popen(["zcat " + filename ], stdout=PIPE, shell=True)
    #process2 = Popen(["wc -l"], stdin=process.stdout, stdout=PIPE, shell=True)


    basicSetup = '''
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    set autoscale xfixmax
    set autoscale xfixmin
    set xtics nomirror out
    set key center top horizontal reverse Left
    set style fill solid noborder
    set boxwidth 0.95 relative
    set ytics out
    '''

    # setup the script headers
    scripts = [terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set logscale y
    set style fill solid 1.00 noborder
    set xlabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Transactions with this Execution Time"
    set title "Transaction Latency\\n%s"  offset character 0, -1, 0 font "" norotate
        ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    set style fill solid 1.00 noborder
    set xlabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Commands with this Execution Time"
    set title "Command Execution Time\\n%s"  offset character 0, -1, 0 font "" norotate
        ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set logscale y
    set style fill  solid 1.00 noborder
    set xlabel "Time From Enqueue To Execution (ns)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Commands with this Latency"
    set title "Command Latency\\n%s"  offset character 0, -1, 0 font "" norotate
    ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    set mxtics
    set style fill  solid 1.00 noborder
    set xlabel "Time (s)"
    set ylabel "Working Set Size" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set title "Working Set Size vs Time\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "workingSet.''' + extension + '''"
    plot '-' using 1:2 t "Working Set Size" with boxes
    ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set output "bandwidth.''' + extension + '''"
    set multiplot
    unset xtics
    set size 1.0, 0.66
    set origin 0.0, 0.33
    set style data histograms
    set style histogram rowstacked title offset 0,0,0
    set ylabel "Bandwidth (bytes per second)"
    set title "System Bandwidth\\n%s"  offset character 0, -1, 0 font "" norotate
    plot '-' using 2 title "Read Bytes", '-' using 2 title "Write Bytes", '-' using 1:2 title "Average Bandwidth" with lines
    ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set logscale y
    set xlabel "PC Value" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Total Latency (cycles)"
    set title "Total Latency vs. PC Value\\n%s"  offset character 0, -1, 0 font "" norotate
    #set style fill solid 1.00 border 0
    set format x "0x1%%x"
    set style fill solid 1.00 noborder
    ''', terminal + basicSetup + '''
    set output "averageIPCandLatency.''' + extension + '''"
    set multiplot
    set size 1.0, 0.5
    set origin 0.0, 0.0
    #%s
    set yrange [0 : *] noreverse nowriteback
    set xlabel "Time (s)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Instructions Per Cycle"
    set title "Average IPC vs. Time\\n" offset character 0, -1, 0 font "" norotate
    set style fill solid 1.00 noborder
    plot '-' using 1:2 title "IPC" with impulses, '-' using 1:2 title "Cumulative Average IPC" with lines, '-' using 1:2 title "Moving Average IPC" with lines
    ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    set xlabel "Time (s)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Hit Rate"
    set title "Hit Rate of Open Rows vs. Time\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "rowHitRate.''' + extension + '''"
    plot '-' using 1:2 title "Hit Rate" with filledcurve below x1, '-' using 1:2 title "Cumulative Average Hit Rate" with lines lw 1.250, '-' using 1:2 title "Moving Average" with lines lw 1.250
    '''
    ]
    averageTransactionLatencyScript = basicSetup + '''
    set size 1.0, 0.5
    set origin 0.0, 0.5
    set yrange [0 : *] noreverse nowriteback
    set xlabel "Time (s)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Latency (ns)"
    set title "Transaction Latency\\n%s"  offset character 0, -1, 0 font "" norotate
    set style fill solid 1.00 noborder
    plot '-' using 1:2 title "Latency" with impulses, '-' using 1:2 title "Cumulative Average Latency" with lines lw 1.25, '-' using 1:2 title "Moving Average" with lines lw 1.25
    '''
    smallIPCGraph = basicSetup + '''
    set size 1.0, 0.35
    set origin 0.0, 0.0
    set yrange [0 : *] noreverse nowriteback
    set xlabel "Time (s)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "IPC"
    set format y "%-6f"
    set title "IPC vs. Time"  offset character 0, -1, 0 font "" norotate
    set style fill solid 1.00 noborder
    plot '-' using 1:2 title "IPC" with impulses, '-' using 1:2 title "Cumulative Average IPC" with lines, '-' using 1:2 title "Moving Average IPC" with lines
    '''
    cacheGraphA = '''
    %s
    %s
    set output "cacheData.%s"
    set y2tics
    set format x
    set yrange [0 : 1] noreverse nowriteback
    set multiplot
    set size 1.0, 0.333
    set origin 0.0, 0.666
    unset xlabel
    set ylabel "Miss Rate"
    set y2label "Access Count"
    set title "Miss Rate of L1 ICache\\n%s"  offset character 0, -1, 0 font "" norotate
    plot  '-' using 1:2 title "Access Count" axes x2y2 with impulses, '-' using 1:2 title "Miss Rate" with lines lw 1.0
    ''' % (terminal, basicSetup, extension, "%s")

    cacheGraphB = '''
    set size 1.0, 0.333
    set origin 0.0, 0.333
    set yrange [0 : *] noreverse nowriteback
    unset xlabel
    set ylabel "Miss Rate"
    set y2label "Access Count"
    set title "Miss Rate of L1 DCache"  offset character 0, -1, 0 font "" norotate
    plot  '-' using 1:2 title "Access Count" axes x2y2 with impulses, '-' using 1:2 title "Miss Rate" with lines lw 1.0
    '''
    cacheGraphC = '''
    set size 1.0, 0.333
    set origin 0.0, 0.0
    set yrange [0 : *] noreverse nowriteback
    set xlabel "Time (s)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Miss Rate"
    set y2label "Access Count"
    set title "Miss Rate of L2 Cache"  offset character 0, -1, 0 font "" norotate
    plot  '-' using 1:2 title "Access Count" axes x2y2 with impulses, '-' using 1:2 title "Miss Rate" with lines lw 1.0
    '''
    pcVsLatencyGraph = terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set logscale y
    set xlabel "PC Value" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Total Latency (cycles)"
    set title "Total Latency Due to Reads vs. PC Value\\n%s"  offset character 0, -1, 0 font "" norotate
    #set style fill solid 1.00 border 0
    set format x "0x1%%x"
    set style fill solid 1.00 noborder
    set output "latencyVsPc.%s"
    plot '-' using 1:2:(1) t "Total Latency" with boxes
    '''
    transactionGraph = terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set logscale y
    set format x
    set style fill solid 1.00 noborder
    set xlabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Transactions with this Execution Time"
    set title "Total Transaction Latency\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "transactionLatencyDistribution.%s"
    plot '-' using 1:2:(1) t "Total Latency" with boxes
    '''

    commandTurnaroundCounter = 0
    cmdCounter = 0
    workingSetCounter = 0
    rwTotalCounter = 0
    ipcCounter = 0

    # what type we are writing to
    writing = 0
    # the list of files to be compressed
    fileList = []

    # the list of files to be removed
    # one instance of gnuplot per script that needs this
    gnuplot = []

    latencyVsPC = 0
    latencyVsPCDict = {}

    averageTransactionLatency = WeightedAverage()
    transactionLatency = array('f')
    movingTransactionLatency = array('f')
    movingTransactionBuffer = RingBuffer(IPCWindow)
    cumulativeTransactionLatency = array('f')
    totalTransactionLatency = 0.1
    distTransactionLatency = {}
    transCounter = 0

    #hitMissCounter = 0
    hitMissTotal = 0.0
    hitTotal = 0.0
    hitMissBuffer = RingBuffer(IPCWindow)
    hitMissValues = []
    hitMissValues.append(array('f'))
    hitMissValues.append(array('f'))
    hitMissValues.append(array('f'))
    hitMissValues.append(array('f'))

    iCacheHits = array('i')
    iCacheMisses = array('i')
    iCacheMissLatency = array('l')
    dCacheHits = array('i')
    dCacheMisses = array('i')
    dCacheMissLatency = array('l')
    l2CacheHits = array('i')
    l2CacheMisses = array('i')
    l2CacheMissLatency = array('l')
    l2MshrHits = array('i')
    l2MshrMisses = array('i')
    l2MshrMissLatency = array('l')

    bandwidthCounter = 0
    bandwidthTotal = 0.0
    bandwidthValues = []
    bandwidthValues.append(array('l'))
    bandwidthValues.append(array('l'))
    bandwidthValues.append(array('f'))

    ipcValues = []
    ipcValues.append(array('f'))
    ipcValues.append(array('f'))
    ipcValues.append(array('f'))
    ipcBuffer = RingBuffer(IPCWindow)
    ipcTotal = 0.0

    outFileBZ2 = os.path.join(os.getcwd(),  filename.split('.gz')[0] + ".tar.bz2")

    started = False
    ipcLinesWritten = 0
    writeLines = 0
    epochTime = 0.0

    period = 0.0

    #process2.wait()

    #lineCount = max(int(process2.stdout.readline()),1)

    # counter to keep track of how many lines are written to each file
    #linesWritten = 0
    #print 'Generate graphs'

    try:
        if filename.endswith("gz"):
            compressedstream = gzip.open(filename, 'rb')
        elif filename.endswith("bz2"):
            compressedstream = bz2.open(filename, 'rb')
        else:
            return

        #line = compressedstream.readline()
        #while line:
        for line in compressedstream:
            if line[0] == '-':
                if processPerEpochGraphs:
                    if writing == 1:
                        #if (linesWritten == 0):
                            #gnuplot[0].stdin.write("0 1.01\n");
                        #linesWritten = 0
                        gnuplot[0].stdin.write("e\nunset output\n")
                    elif writing == 4:
                        #if (linesWritten == 0):
                            #gnuplot[1].stdin.write("0 1.01\n");
                        #linesWritten = 0
                        gnuplot[1].stdin.write("e\nunset output\n")
                    elif writing == 6:
                        #if (linesWritten == 0):
                            #gnuplot[2].stdin.write("0 1.01\n");
                        #linesWritten = 0
                        gnuplot[2].stdin.write("e\nunset output\n")
                    elif writing == 8:
                        #if (linesWritten == 0):
                            #gnuplot[5].stdin.write("0 1.01\n");
                        #linesWritten = 0
                        gnuplot[5].stdin.write("e\nunset output\n")
                    #elif writing == 9:
                        #linesWritten = 0

                if writing == 1:
                        # append the values for this time
                        transactionLatency.append(averageTransactionLatency.average())
                        # append the moving average
                        movingTransactionBuffer.append(averageTransactionLatency.average())
                        movingTransactionLatency.append(movingTransactionBuffer.average())
                        totalTransactionLatency += averageTransactionLatency.average()
                        cumulativeTransactionLatency.append(totalTransactionLatency / transCounter)

                writing = 0
                #line = line.strip()

                if started == False:
                    # extract the command line, set the writers up
                    if line.startswith('----Command Line:'):
                        commandLine = line.strip().split(":")[1]

                        for i in scripts:
                            p = Popen(['gnuplot'], stdin=PIPE, shell=False, cwd=tempPath)
                            p.stdin.write(i % commandLine.strip())
                            gnuplot.append(p)
                        started = True
                        print commandLine
                elif line.startswith("----M5 Stat:"):

                    splitLine = line.split()
                    #print splitLine[0], splitLine[1], splitLine[2], splitLine[3]
                    try:
                        value = int(splitLine[3])
                    except ValueError:
                        value = long(float(splitLine[3]))

                    if splitLine[2] == "system.cpu.dcache.overall_hits":
                        dCacheHits.append(value)
                    elif splitLine[2] == "system.cpu.dcache.overall_miss_latency":
                        dCacheMissLatency.append(value)
                    elif splitLine[2] == "system.cpu.dcache.overall_misses":
                        dCacheMisses.append(value)
                    elif splitLine[2] == "system.cpu.icache.overall_hits":
                        iCacheHits.append(value)
                    elif splitLine[2] == "system.cpu.icache.overall_miss_latency":
                        iCacheMissLatency.append(value)
                    elif splitLine[2] == "system.cpu.icache.overall_misses":
                        iCacheMisses.append(value)
                    elif splitLine[2] == "system.l2.overall_hits":
                        l2CacheHits.append(value)
                    elif splitLine[2] == "system.l2.overall_miss_latency":
                        l2CacheMissLatency.append(value)
                    elif splitLine[2] == "system.l2.overall_misses":
                        l2CacheMisses.append(value)
                    elif splitLine[2] == "system.l2.overall_mshr_hits":
                        l2MshrHits.append(value)
                    elif splitLine[2] == "system.l2.overall_mshr_miss_latency":
                        l2MshrMissLatency.append(value)
                    elif splitLine[2] == "system.l2.overall_mshr_misses":
                        l2MshrMisses.append(value)
                    else:
                        print "missed something: %s" % splitLine[2]

                elif line[4] == 'E':
                    epochTime = float(line.split()[1])
                elif line.startswith("----Datarate"):
                    period = 1 / float(line.strip().split()[1]) / 1E-9
                else:
                    if line.startswith("----Transaction Latency"):
                        if processPerEpochGraphs:
                            outFile = "transactionExecution" + '%05d' % transCounter + "." + extension
                            gnuplot[0].stdin.write("set output './%s'\nplot '-' using 1:2:(1) with boxes\n" % outFile)
                            fileList.append(os.path.join(tempPath,outFile))
                        averageTransactionLatency.clear()
                        transCounter += 1
                        writing = 1
                    elif line[4] == 'W':
                        writing = 2
                    elif line[4] == 'D':
                        writing = 3
                    elif line[4] == 'C' and line[5] == 'M':
                        if processPerEpochGraphs:
                            outFile = "commandExecution%05d.%s" % (cmdCounter, extension)
                            gnuplot[1].stdin.write("set output './%s'\nplot '-' using 1:2:(1)  with boxes\n" % outFile)
                            cmdCounter += 1
                            fileList.append(os.path.join(tempPath,outFile))
                        writing = 4
                        #if cmdCounter > 50:
                        #    break
                    #elif line == '----R W Total----':
                    #    writing = 5
                    elif line[4] == 'C' and line[5] == 'o':
                        if processPerEpochGraphs:
                            outFile = "commandTurnaround%05d.%s" % (commandTurnaroundCounter, extension)
                            gnuplot[2].stdin.write("set output './%s'\nplot '-' using 1:2:(1)   with boxes\n" % outFile)
                            commandTurnaroundCounter += 1
                            fileList.append(os.path.join(tempPath,outFile))
                        writing = 6
                    elif line[4] == 'B':
                        writing = 7
                    elif line[4] == 'A':
                        if processPerEpochGraphs:
                            outFile = "latencyVsPC%05d.%s" % (latencyVsPC, extension)
                            gnuplot[5].stdin.write("set output './%s'\nplot '-' using 1:2:(1)   with boxes\n" % outFile)
                            latencyVsPC += 1
                            fileList.append(os.path.join(tempPath,outFile))
                        writing = 8
                    elif line[4] == 'I':
                        ipcLinesWritten = 0
                        writing = 9
                    elif line.startswith("----Row"):
                        writing = 10


            # data in this section
            else:
                # transaction latency
                if writing == 1:
                    splitLine = line.split()
                    latency = int(splitLine[1])
                    quantity = int(splitLine[0])
                    averageTransactionLatency.add(latency, quantity)
                    try:
                        distTransactionLatency[latency] += quantity
                    except KeyError:
                        distTransactionLatency[latency] = quantity

                # working set
                elif writing == 2:
                    if len(line) > 1:
                        gnuplot[3].stdin.write("%f %s" % (workingSetCounter * epochTime,line))
                        workingSetCounter += 1

                # bandwidth
                elif writing == 7:
                    newLine = line.strip().split()
                    bandwidthValues[0].append(long(float(newLine[0])))
                    bandwidthValues[1].append(long(float(newLine[1])))
                    bandwidthTotal += float(newLine[0]) + float(newLine[1])
                    bandwidthCounter += 1
                    bandwidthValues[2].append(bandwidthTotal / bandwidthCounter)

                # PC vs latency
                elif writing == 8:
                    splitLine = line.split()
                    PC = int(splitLine[0],16) - 4294967296
                    numberAccesses = float(splitLine[1])
                    try:
                        latencyVsPCDict[PC] += numberAccesses
                    except KeyError:
                        latencyVsPCDict[PC] = numberAccesses

                elif writing == 9:
                    if (ipcLinesWritten < 1):
                        # append the values for this time
                        if line == "nan":
                            currentValue = 0.0
                        else:
                            currentValue = float(line)
                        ipcValues[0].append(currentValue)
                        # append the moving average
                        ipcTotal += currentValue
                        ipcCounter += 1
                        ipcValues[1].append(ipcTotal / ipcCounter)
                        ipcBuffer.append(currentValue)
                        ipcValues[2].append(ipcBuffer.average())
                    ipcLinesWritten += 1

                elif writing == 10:
                    newLine = line.strip().split()
                    hitCount = max(float(newLine[0]),1)
                    missCount = max(float(newLine[1]),1)
                    hitMissValues[0].append(missCount / (hitCount + missCount))
                    hitMissValues[1].append(hitCount / (hitCount + missCount))
                    #hitMissCounter += 1
                    hitMissTotal += hitCount + missCount
                    hitTotal += hitCount
                    hitMissValues[2].append(hitTotal / hitMissTotal)
                    hitMissBuffer.append(hitCount / (hitCount + missCount))
                    hitMissValues[3].append(hitMissBuffer.average())

                if processPerEpochGraphs:
                    if writing == 1:
                        gnuplot[0].stdin.write(line)
                    elif writing == 4:
                        gnuplot[1].stdin.write(line)
                    elif writing == 6:
                        gnuplot[2].stdin.write(line)
                    elif writing == 8:
                        splitLine = line.split()
                        gnuplot[5].stdin.write("%d %d\n" % (int(splitLine[0], 16) - 4294967296, (float(splitLine[1]) * float(splitLine[2]))))

            if workingSetCounter > 5000:
                break
            #line = compressedstream.readline()

        compressedstream.close()

    except IOError, strerror:
        print "I/O error", strerror, "\nThis archive is probably truncated."
    except OSError, strerror:
        print "OS error", strerror
    except IndexError, strerror:
        print "IndexError",strerror

    if started == True:
        # close any files being written
        if writing == 1:
            gnuplot[0].stdin.write("e\nunset output\n")
        elif writing == 4:
            gnuplot[1].stdin.write("e\nunset output\n")
        elif writing == 6:
            gnuplot[2].stdin.write("e\nunset output\n")
        elif writing == 8:
            gnuplot[5].stdin.write("e\nunset output\n")

        # finish up the moving averages
        for i in range(0, IPCWindow / 2):
            movingTransactionLatency.append(movingTransactionBuffer.finishAverage())
            ipcValues[2].append(ipcBuffer.finishAverage())
            hitMissValues[3].append(hitMissBuffer.finishAverage())

        # make the PC vs latency graph
        gnuplot[0].stdin.write(pcVsLatencyGraph % (commandLine, extension))

        for u in latencyVsPCDict:
            gnuplot[0].stdin.write("%d %f\n" % (u, latencyVsPCDict[u]))

        gnuplot[0].stdin.write("e\nunset output\n")

        # make the transaction latency graph
        gnuplot[0].stdin.write(transactionGraph % (commandLine, extension))

        for u in distTransactionLatency:
            gnuplot[0].stdin.write("%d %f\n" % (u, distTransactionLatency[u]))

        gnuplot[0].stdin.write("e\nunset output\n")

        # make the bandwidth graph
        for t in bandwidthValues:
            for i in range(0, len(t)):
                 gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, t[i]))
            gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write(smallIPCGraph)

        # make the ipc graph
        minRange = min(len(ipcValues[0]),len(ipcValues[1]),len(ipcValues[2]))

        for i in range(0,minRange):
            gnuplot[4].stdin.write("%f %f\n" % (epochTime * i, ipcValues[0][i]))
        gnuplot[4].stdin.write("e\n")

        for i in range(0,minRange):
            gnuplot[4].stdin.write("%f %f\n" % (epochTime * i, ipcValues[1][i]))
        gnuplot[4].stdin.write("e\n")

        for i in range(0,minRange + IPCWindow / 2):
            gnuplot[4].stdin.write("%f %f\n" % (epochTime * max(i - IPCWindow / 2.0, i / 2.0), ipcValues[2][i]))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write("unset multiplot\nunset output\n")

        # make the cache graph
        gnuplot[4].stdin.write(cacheGraphA % commandLine)

        for i in range(0, min(len(iCacheMisses),len(iCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, 1.0 * (iCacheMisses[i] + iCacheHits[i]) ))
        gnuplot[4].stdin.write("e\n")

        for i in range(0, min(len(iCacheMisses),len(iCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, iCacheMisses[i] / (iCacheMisses[i] + iCacheHits[i] + 1.0)))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write(cacheGraphB)
        for i in range(0, min(len(dCacheMisses),len(dCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, 1.0 * (dCacheMisses[i] + dCacheHits[i])))
        gnuplot[4].stdin.write("e\n")

        for i in range(0, min(len(dCacheMisses),len(dCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, dCacheMisses[i] / (dCacheMisses[i] + dCacheHits[i] + 1.0)))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write(cacheGraphC)

        for i in range(0, min(len(l2CacheMisses),len(l2CacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, 1.0 * (l2CacheMisses[i] + l2CacheHits[i]) ))
        gnuplot[4].stdin.write("e\n")


        for i in range(0, min(len(l2CacheMisses),len(l2CacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, l2CacheMisses[i] / (l2CacheMisses[i] + l2CacheHits[i] + 1.0)))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write("unset multiplot\nunset output\n")

        # make the ipc graph
        minRange = min(len(ipcValues[0]),len(ipcValues[1]),len(ipcValues[2]))

        for i in range(0,minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, ipcValues[0][i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0,minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, ipcValues[1][i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0,minRange + IPCWindow / 2):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * max(i - IPCWindow / 2.0, i / 2.0), ipcValues[2][i]))
        gnuplot[6].stdin.write("e\n")

        gnuplot[6].stdin.write(averageTransactionLatencyScript % commandLine)

        # make the transaction latency graph
        minRange = min(len(transactionLatency),len(cumulativeTransactionLatency),len(movingTransactionLatency))

        for i in range(0,minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, period * transactionLatency[i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0,minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, period * cumulativeTransactionLatency[i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0,minRange + IPCWindow / 2):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * max(i - IPCWindow / 2.0, i / 2.0), period * movingTransactionLatency[i]))
        gnuplot[6].stdin.write("e\n")

        gnuplot[6].stdin.write("unset multiplot\nunset output\n")

        # make the hit-miss graph
        minRange = min(len(hitMissValues[0]),len(hitMissValues[1]),len(hitMissValues[2]),len(hitMissValues[3]))

        for i in range(0, minRange):
            gnuplot[7].stdin.write("%f %f\n" % (epochTime * i, hitMissValues[1][i]))

        gnuplot[7].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[7].stdin.write("%f %f\n" % (epochTime * i, hitMissValues[2][i]))

        gnuplot[7].stdin.write("e\n")

        for i in range(0, minRange + IPCWindow / 2):
            gnuplot[7].stdin.write("%f %f\n" % (epochTime * max(i - IPCWindow / 2.0, i / 2.0), hitMissValues[3][i]))

        gnuplot[7].stdin.write("e\nunset output\n")

        # close outputs that are written to once per epoch
        gnuplot[3].stdin.write("e\nunset output\n")
        gnuplot[4].stdin.write("unset output\n")

        fileList.append(os.path.join(tempPath,"workingSet." + extension))
        fileList.append(os.path.join(tempPath,"bandwidth." + extension))
        fileList.append(os.path.join(tempPath,"averageIPCandLatency." + extension))
        fileList.append(os.path.join(tempPath,"rowHitRate." + extension))
        fileList.append(os.path.join(tempPath,"cacheData." + extension))
        fileList.append(os.path.join(tempPath,"latencyVsPc." + extension))
        fileList.append(os.path.join(tempPath,"transactionLatencyDistribution." + extension))

        for b in gnuplot:
            b.stdin.write('\nexit\n')
            b.wait()

    # make a big file of them all
    #print 'Compress graphs'
    outputFile = tarfile.open(outFileBZ2,'w:bz2')
    for name in fileList:
        outputFile.add(name, os.path.basename(name))

    outputFile.close()

    files = os.listdir(tempPath)

    for x in files:
        fullPath = os.path.join(tempPath,x)
        os.remove(fullPath)

    os.rmdir(tempPath)
    #os.system("tar cjf " + outFileBZ2 + " --totals " + " ".join(fileList))

    #os.chdir(originalPath)
    #print os.getcwd()

workQueue = Queue.Queue()

def worker():
    #while not workQueue.empty():
    while True:
        try:
            item = workQueue.get()
            if item.endswith("stats.gz") or item.endswith("stats.bz2"):
                processStats(item)
            elif item.endswith("power.gz") or item.endswith("power.bz2"):
                processPower(item)
            workQueue.task_done()
        except str:
            print str

def main():
    for i in range(workerThreads):
        t = Thread(target=worker)
        t.setDaemon(True)
        t.start()

    for u in sys.argv:
        if u.endswith("gz") or u.endswith("bz2"):
            #print u
            workQueue.put(u)

    workQueue.join()

if __name__ == "__main__":
     os.environ["GDFONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"

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

