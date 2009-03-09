#!/usr/bin/python -O

import re
import gzip
import sys
import os
import string
from threading import Thread, Lock
import Queue
#import tempfile
#import tarfile
import math
import bz2
from subprocess import Popen, PIPE, STDOUT
from array import array
import shutil
import cProfile, pstats, StringIO
from statsScripts import *


#globals
workerThreads = 2

Window = 8

resizeLock = Lock()

class CumulativePriorMovingAverage:
    def __init__(self):
        self.average = 0.0
        self.count = 0.0

    def add(self, count, value):
        self.count += 1.0
        self.average = self.average + ((count * value - self.average) / self.count)

    def clear(self):
        self.average = 0.0
        self.count = 0.0

class WeightedAverage:
    def __init__(self):
        self.count = 0
        self.total = 0

    def add(self, value, count):
        self.total += value * count
        self.count += count

    def clear(self):
        self.total = 0
        self.count = 0

    def average(self):
        return self.total / (self.count if self.count > 0 else 1)



class PriorMovingAverage:
    def __init__(self, size):
        self.data = array('f',)
        self.average = 0.0
        self.size = size

    def append(self, x):
        size = max(len(self.data), 1)
        oldestItem = self.average if len(self.data) < self.size else self.data[0]
        self.average = self.average - (oldestItem / size) + (x / size)
        if len(self.data) == self.size:
            self.data.pop(0)
        self.data.append(x)

    def get(self):
        return self.data

def gziplines(filename):
    from subprocess import Popen, PIPE

    if filename.endswith("gz"):
      compressedstream = Popen(['zcat', filename], stdout=PIPE)
    elif filename.endswith("bz2"):
        compressedstream = Popen(['bzcat', filename], stdout=PIPE)
    else:
        compressedstream = Popen(['cat', filename], stdout=PIPE)

    for line in compressedstream.stdout:
        yield line

def thumbnail(filelist, tempPath):
    for x in filelist:
        if x.endswith(extension):
            with resizeLock:
                file = os.path.join(tempPath, x)
                p1 = Popen(['convert', '-limit', 'memory', '512mb', '-resize', thumbnailResolution, file, '%s-thumb.png' % file[: - 4]])
                p2 = Popen("gzip -c -9 -f %s > %s" % (file, file + "z"), shell=True)
                p1.wait()
                p2.wait()
                try:
                    os.remove(file)
                except OSError, msg:
                    print OSError, msg

def processPower(filename):

    tempPath = os.path.join(os.getcwd(), filename[0:filename.find("-power")])

    try:
        if not os.path.exists(tempPath):
            os.mkdir(tempPath)
        elif not os.access(tempPath, os.W_OK):
            print "Cannot write to output dir: " + tempPath
    except:
        print "Could not create output dir: " + tempPath
        if not os.path.exists(tempPath):
            return
#todo: make the gnuplot subprocesses common for the entire run, not per zip file
    writing = 0
    p = Popen(['gnuplot'], stdin=PIPE, stdout=PIPE, shell=False)
    p.stdin.write(terminal)
    p2 = Popen(['gnuplot'], stdin=PIPE, stdout=PIPE, shell=False)
    p2.stdin.write(terminal)

    filesGenerated = []

    # list of arrays
    values = []
    averageValues = array('f')
    instantValues = array('f')
    windowValues = array('f')
    times = array('f')
    powerMovingAverage = PriorMovingAverage(Window)
    cumulativePower = CumulativePriorMovingAverage()
    epochPower = 0.0
    channels = 0
    ranks = 0

    try:
        if filename.endswith("gz"):
            outFileBZ2 = os.path.join(tempPath, filename.split('.gz')[0] + ".tar.bz2")
            basefilename = os.path.join(tempPath, filename.split('.gz')[0])
        elif filename.endswith("bz2"):
            outFileBZ2 = os.path.join(tempPath, filename.split('.bz2')[0] + ".tar.bz2")
            basefilename = os.path.join(tempPath, filename.split('.bz2')[0])
        else:
            outFileBZ2 = os.path.join(tempPath, filename + ".tar.bz2")
            basefilename = filename

        splitter = re.compile('([\[\]])')
        splitter2 = re.compile(' ')

        for line in gziplines(filename):
            if line[0] == '-':
              # normal lines
                if line[1] == 'P':
                    powerNumber = powerRegex.search(line)
                    if powerNumber:
                        thisPower = float(powerNumber.group(0))
                        values[writing].append(thisPower)

                        writing = (writing + 1) % (valuesPerEpoch)
                        epochPower += thisPower
                        if writing == 0:
                            cumulativePower.add(1.0, epochPower)
                            averageValues.append(cumulativePower.average)
                            instantValues.append(epochPower)
                            powerMovingAverage.append(epochPower)
                            windowValues.append(powerMovingAverage.average)
                            epochPower = 0.0

                elif line.startswith('----Epoch'):
                    splitLine = splitter2.split(line)
                    epochTime = float(splitLine[1])

                elif line.startswith('----Command Line:'):
                    commandLine = line.strip().split(":")[1]
                    print commandLine

                    p.stdin.write("set title \"{/=12 Power Consumed vs. Time}\\n{/=10 %s}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n" % commandLine)

                    fileName = os.path.join(tempPath, "energy." + extension)
                    filesGenerated.append(fileName)

                    p2.stdin.write("%s\n" % (powerScripts[2] % fileName))
                    p2.stdin.write("set title \"{/=12 Energy vs. Time}\\n{/=10 %s}\"  offset character 0, -1, 0 font \"Arial,14\" norotate\n" % commandLine)
                    p2.stdin.write('''plot '-' u 1:2 sm csp t "Energy (P t)" w lines lw 2.00, '-' u 1:2 sm csp t "IBM Energy (P^{2} t^{2})" w lines lw 2.00\n''')

                elif line[1] == '+':
                    fileName = os.path.join(tempPath, "power." + extension)
                    filesGenerated.append(fileName)
                    p.stdin.write('set output "%s"\n' % fileName)
                    p.stdin.write(powerScripts[0])

                    splitLine = splitter.split(line)
                    channels = int(splitLine[2])
                    ranks = int(splitLine[6])

                    p.stdin.write('plot ')

                    for a in range(channels):
                        for b in ['ACT-STBY', 'ACT', 'PRE-STBY', 'RD', 'WR']:
                            p.stdin.write('"-" using 1 axes x2y1 title "P_{sys}(%s) ch[%d]",' % (b, a))

                    p.stdin.write("'-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n")
                    # plus the RD and WR values
                    valuesPerEpoch = channels * 5
                    for i in range(valuesPerEpoch):
                        values.append(array('f'))

    except IOError, strerror:
        print "I/O error", strerror
    except OSError, strerror:
        print "OS error", strerror
    except IndexError, strerror:
        print "IndexError", strerror


    # write the divided power data
    for u in values:
        for v in u:
            p.stdin.write("%f\n" % v)
        p.stdin.write("e\n")
    p.stdin.write("0 0\n%f 0.2\ne\n" % (len(values[0]) * epochTime))

    p.stdin.write(powerScripts[1])

    epochNumber = 0.0
    for u in averageValues:
        p.stdin.write("%f %f\n" % (epochNumber, u))
        epochNumber += epochTime
    p.stdin.write("e\n")
    epochNumber = 0.0
    for u in instantValues:
        p.stdin.write("%f %f\n" % (epochNumber, u))
        epochNumber += epochTime
    p.stdin.write("e\n")

    epochNumber = 0.0
    for u in windowValues:
        p.stdin.write("%f %f\n" % (epochNumber, u))
        epochNumber += epochTime
    p.stdin.write("e\n")

    p.stdin.write("unset multiplot\nunset output\nexit\n")

    epochNumber = 0.0
    for u in instantValues:
        p2.stdin.write("%f %f\n" % (epochNumber, u * epochTime))
        epochNumber += epochTime
    p2.stdin.write("e\n")

    epochNumber = 0.0
    for u in instantValues:
        p2.stdin.write("%f %f\n" % (epochNumber, u * u * epochTime * epochTime))
        epochNumber += epochTime
    p2.stdin.write("e\n")

    p2.stdin.write(powerScripts[3])
    epochNumber = 0.0
    for u in instantValues:
        p2.stdin.write("%f %f\n" % (epochNumber, u * epochTime * epochTime))
        epochNumber += epochTime
    p2.stdin.write("e\n")

    epochNumber = 0.0
    for u in instantValues:
        p2.stdin.write("%f %f\n" % (epochNumber, u * u * epochTime * epochTime * epochTime))
        epochNumber += epochTime
    p2.stdin.write("e\nunset multiplot\nunset output\nexit\n")

    p.wait()
    p2.wait()

    thumbnail(filesGenerated, tempPath)

    o = open(os.path.join(tempPath, basefilename[0:basefilename.find("-power")] + ".html"), "w")
    templateF = open(os.path.join(os.path.dirname(os.path.realpath(__file__)), "template.html"))
    template = templateF.read()
    templateF.close()
    o.write(re.sub("@@@", commandLine, template))
    o.close()

    o = open(os.path.join(tempPath, ".htaccess"), "w")
    accessFileF = open(os.path.join(os.path.dirname(os.path.realpath(__file__)), ".htaccess"))
    accessFile = accessFileF.read()
    accessFileF.close()
    o.write(accessFile)
    o.close()

    # make a big file of them all
    #os.system("tar cjf " + outFileBZ2 + " --remove-files " + string.join(fileList, ' '))


def processStats(filename):

    tempPath = os.path.join(os.getcwd(), filename[0:filename.find("-stats")])

    try:
        if not os.path.exists(tempPath):
            os.mkdir(tempPath)
        elif not os.access(tempPath, os.W_OK):
            print "Cannot write to output dir: " + tempPath
    except:
        print "Could not create output dir: " + tempPath
        if not os.path.exists(tempPath):
            return

    channels = []
    channelTotals = array('i')
    ranks = []
    rankTotals = array('i')
    banks = []
    bankTotals = array('i')

    channelLatencies = []
    channelLatencyTotals = array('i')
    rankLatencies = []
    rankLatencyTotals = array('i')
    bankLatencies = []
    bankLatencyTotals = array('i')

    channelDistribution = []
    channelLatencyDistribution = []

    commandTurnaroundCounter = 0
    cmdCounter = 0
    workingSetCounter = 0
    rwTotalCounter = 0
    ipcCounter = 0

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
    movingTransactionBuffer = PriorMovingAverage(Window)
    cumulativeTransactionLatency = array('f')
    perEpochTransactionCount = 0
    transactionCount = array('i')
    totalTransactionLatency = 0.1
    distTransactionLatency = {}
    transCounter = 0

    hitMissTotal = CumulativePriorMovingAverage()
    hitMissBuffer = PriorMovingAverage(Window)
    hitMissValues = [array('f'), array('f'), array('f'), array('f')]

    iCacheHits = array('i')
    iCacheMisses = array('i')
    iCacheMissLatency = array('L')
    dCacheHits = array('i')
    dCacheMisses = array('i')
    dCacheMissLatency = array('d')
    l2CacheHits = array('i')
    l2CacheMisses = array('i')
    l2CacheMissLatency = array('d')
    l2MshrHits = array('i')
    l2MshrMisses = array('i')
    l2MshrMissLatency = array('d')

    bandwidthTotal = CumulativePriorMovingAverage()
    bandwidthValues = [array('d'), array('d'), array('f')]

    ipcValues = [array('f'), array('f'), array('f')]
    ipcBuffer = PriorMovingAverage(Window)
    ipcTotal = CumulativePriorMovingAverage()

    started = False
    ipcLinesWritten = 0
    writeLines = 0
    epochTime = 0.0

    period = 0.0

    if filename.endswith("gz"):
        basefilename = os.path.join(tempPath, filename.split('.gz')[0])
    else:
        basefilename = os.path.join(tempPath, filename.split('.bz2')[0])

    # what type we are writing to
    writing = 0

    try:
        for line in gziplines(filename):
            if line[0] == '-':

                if writing == 1:
                    # append the values for this time
                    transactionLatency.append(averageTransactionLatency.average())
                    # append the moving average
                    movingTransactionBuffer.append(averageTransactionLatency.average())
                    movingTransactionLatency.append(movingTransactionBuffer.average)
                    totalTransactionLatency += averageTransactionLatency.average()
                    transactionCount.append(perEpochTransactionCount)
                    perEpochTransactionCount = 0
                    cumulativeTransactionLatency.append(totalTransactionLatency / transCounter)

                if writing == 14:
                    for channel in channelDistribution:
                        for rank in channel:
                            total = 0
                            for i in range(len(rank) - 1):
                                total += rank[i][ - 1]
                            rank[ - 1].append(total if total > 0 else 1)

                if writing == 15:
                    for channel in channelLatencyDistribution:
                        for rank in channel:
                            total = 0
                            for i in range(len(rank) - 1):
                                total += rank[i][ - 1]
                            rank[ - 1].append(total if total > 0 else 1)

                writing = 0

                if started == False:
                    # extract the command line, set the writers up
                    if line.startswith('----Command Line:'):
                        commandLine = line.strip().split(":")[1]

                        for i in statsScripts:
                            p = Popen(['gnuplot'], stdin=PIPE, shell=False, cwd=tempPath)
                            p.stdin.write(i % commandLine.strip())
                            #p.stdin.write(basicSetup)
                            gnuplot.append(p)
                        started = True

                        numberChannels = int(channelRegex.search(line).group(0))
                        numberRanks = int(rankRegex.search(line).group(0))
                        numberBanks = int(bankRegex.search(line).group(0))

                        for i in range(numberChannels):
                            channelLatencyDistribution.append([])
                            channelDistribution.append([])
                            for j in range(numberRanks):
                                channelLatencyDistribution[ - 1].append([])
                                channelDistribution[ - 1].append([])
                                for k in range(numberBanks + 1):
                                    channelLatencyDistribution[ - 1][ - 1].append(array('d'))
                                    channelDistribution[ - 1][ - 1].append(array('d'))

                        for i in range(numberChannels):
                            channels.append(array('i'))
                        for i in range(numberRanks):
                            ranks.append(array('i'))
                        for i in range(numberBanks):
                            banks.append(array('i'))

                        print commandLine
                elif line.startswith("----M5 Stat:"):

                    splitLine = line.split()
                    try:
                        value = int(splitLine[3])
                    except ValueError:
                        value = long(float(splitLine[3]))
                    except:
                        value = 0

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
                    period = 1 / float(line.strip().split()[1]) / 0.000000001
                else:
                    if line.startswith("----Transaction Latency"):
                        averageTransactionLatency.clear()
                        transCounter += 1
                        writing = 1
                    elif line[4] == 'W':
                        writing = 2
                    elif line[4] == 'D':
                        writing = 3
                    elif line[4] == 'C' and line[5] == 'M':
                        writing = 4
                    elif line[4] == 'C' and line[5] == 'o':
                        writing = 6
                    elif line.startswith("----Band"):
                        writing = 7
                    elif line[4] == 'A':
                        writing = 8
                    elif line[4] == 'I':
                        ipcLinesWritten = 0
                        writing = 9
                    elif line.startswith("----Row"):
                        writing = 10

                    elif line.startswith("----Channel"):

                        channelTotals.append(1)
                        for x in channels:
                            x.append(0)

                        writing = 11
                    elif line.startswith("----Rank"):

                        rankTotals.append(1)
                        for x in ranks:
                            x.append(0)

                        writing = 12

                    elif line.startswith("----Bank"):

                        bankTotals.append(1)
                        for x in banks:
                            x.append(0)

                        writing = 13

                    elif line.startswith("----Utilization"):

                        channelTotals.append(1)
                        rankTotals.append(1)
                        bankTotals.append(1)
                        for x in channels:
                            x.append(0)
                        for x in ranks:
                            x.append(0)
                        for x in banks:
                            x.append(0)

                        writing = 14

                    elif line.startswith("----Latency Breakdown"):

                        writing = 15

                    else:
                        writing = 0

            # data in this section
            else:
                # transaction latency
                if writing == 1:
                    splitLine = line.split()
                    latency = int(splitLine[0])
                    count = int(splitLine[1])
                    averageTransactionLatency.add(latency, count)
                    perEpochTransactionCount += count
                    try:
                        distTransactionLatency[latency] += count
                    except KeyError:
                        distTransactionLatency[latency] = count

                # working set size
                elif writing == 2:
                    if len(line) > 2:
                        gnuplot[3].stdin.write("%f %s\n" % (workingSetCounter * epochTime, line.strip()))
                        workingSetCounter += 1

                # bandwidth
                elif writing == 7:
                    newLine = line.strip().split()
                    readValue = float(newLine[0])
                    writeValue = float(newLine[1])
                    bandwidthValues[0].append(long(newLine[0]))
                    bandwidthValues[1].append(long(newLine[1]))

                    bandwidthTotal.add(1, float(readValue) + float(writeValue))
                    bandwidthValues[2].append(bandwidthTotal.average)

                # PC vs latency
                elif writing == 8:
                    splitLine = line.split()
                    # ignore stack addresses for now
                    if len(splitLine[0]) < 10:
                        PC = long(splitLine[0], 16)
                        # - 4294967296
                        #print "%s %x" % (splitLine[0], PC)
                        numberAccesses = float(splitLine[1])
                        try:
                            latencyVsPCDict[PC] += numberAccesses
                        except KeyError:
                            latencyVsPCDict[PC] = numberAccesses

                # IPC
                elif writing == 9:
                    if (ipcLinesWritten < 1):
                        # append the values for this time
                        try:
                            if line.startswith('nan'):
                                currentValue = 0.0
                            else:
                                currentValue = float(line)
                        except:
                            currentValue = 0.0
                        # python 2.6 if isnan(currentValue):
                        # print currentValue, NaN, line
                        ipcValues[0].append(currentValue)
                        # append the moving average
                        ipcTotal.add(1, currentValue)
                        ipcValues[1].append(ipcTotal.average)
                        ipcBuffer.append(currentValue)
                        ipcValues[2].append(ipcBuffer.average)
                    ipcLinesWritten += 1

                # hit and miss values
                elif writing == 10:
                    newLine = line.strip().split()
                    hitCount = max(float(newLine[0]), 1)
                    missCount = max(float(newLine[1]), 1)
                    hitMissValues[0].append(missCount / (hitCount + missCount))
                    hitMissValues[1].append(hitCount / (hitCount + missCount))
                    hitMissTotal.add(hitCount / (hitCount + missCount), 1)
                    hitMissValues[2].append(hitMissTotal.average)
                    hitMissBuffer.append(hitCount / (hitCount + missCount))
                    hitMissValues[3].append(hitMissBuffer.average)

                # channel breakdown
                elif writing == 11:
                    newLine = line.strip().split()
                    channels[int(newLine[0])][ - 1] = int(newLine[1])
                    channelTotals[ - 1] += int(newLine[1])

                # rank breakdown
                elif writing == 12:
                    newLine = line.strip().split()
                    ranks[int(newLine[0])][ - 1] = int(newLine[1])
                    rankTotals[ - 1] += int(newLine[1])

                # bank breakdown
                elif writing == 13:
                    newLine = line.strip().split()
                    banks[int(newLine[0])][ - 1] = int(newLine[1])
                    bankTotals[ - 1] += int(newLine[1])

                # new style per-bank breakdown
                elif writing == 14:
                    groups = combinedRegex.match(line).groups()

                    channel = int(groups[0])
                    rank = int(groups[1])
                    bank = int(groups[2])
                    value = int(groups[3])

                    channelDistribution[channel][rank][bank].append(value)

                    if len(groups) == 4:
                        channels[channel][ - 1] += value
                        ranks[rank][ - 1] += value
                        banks[bank][ - 1] += value
                        channelTotals[ - 1] += value
                        rankTotals[ - 1] += value
                        bankTotals[ - 1] += value

                # latency distribution graphs
                elif writing == 15:
                    groupsD = combinedRegex.match(line).groups()

                    channelD = int(groupsD[0])
                    rankD = int(groupsD[1])
                    bankD = int(groupsD[2])
                    valueD = int(groupsD[3])

                    channelLatencyDistribution[channelD][rankD][bankD].append(valueD)


    except IOError, strerror:
        print "I/O error", strerror, "\nThis archive is probably truncated."
    except OSError, strerror:
        print "OS error", strerror
    except IndexError, strerror:
        print "IndexError", strerror

    if started == True:
        # close any files being written
        if writing == 14:
            for channel in channelDistribution:
                for rank in channel:
                    total = 0
                    for i in range(len(rank) - 1):
                        total += rank[i][ - 1]
                    rank[ - 1].append(total if total > 0 else 1)

        if writing == 15:
            for channel in channelLatencyDistribution:
                for rank in channel:
                    total = 0
                    for i in range(len(rank) - 1):
                        total += rank[i][ - 1]
                    rank[ - 1].append(total if total > 0 else 1)

        # make the address latency distribution per channel graphs
        channelID = 0
        for channel in channelLatencyDistribution:
            #for each channel make its own graph
            gnuplot[2].stdin.write(subAddrDistroA % ("addressLatencyDistribution%d" % channelID))
            fileList.append('addressLatencyDistribution%d.%s' % (channelID, extension))
            channelID += 1
            rankFraction = 1.0 / len(channel)
            offset = 1 - rankFraction
            rankID = 0
            for rank in channel:
                gnuplot[2].stdin.write(subAddrDistroB % (rankFraction, offset, rankID, "" if rankID > 0 else "\\n%s" % commandLine))
                rankID += 1
                offset -= rankFraction
                if rankID < len(channel):
                    gnuplot[2].stdin.write('\nunset key\nunset xlabel\n')
                else:
                    gnuplot[2].stdin.write('\nset xlabel "Time (s)" offset 0,0.6\nset key outside center bottom horizontal reverse Left\n')
                    pass
                gnuplot[2].stdin.write('plot ')
                for a in range(len(rank) - 1):
                    gnuplot[2].stdin.write('"-" using 1 "%%lf" axes x2y1 t "bank_{%d}  ",' % a)
                gnuplot[2].stdin.write('"-" using 1:2 axes x1y1 notitle with points pointsize 0.01\n')
                #print len(v) - 1
                #for each bank
                for bank in range(len(rank) - 1):
                    #print len(rank[z])
                    for epoch in range(len(rank[bank])):
                        gnuplot[2].stdin.write("%f\n" % (1.0 * rank[bank][epoch] / rank[ - 1][epoch]))
                        #gnuplot[2].stdin.write("%f\n" % (1.0 * v[z][y]))
                        #if y ==12 or y==11 or y == 13:
                        #if z == 4:
                        #if epoch == 10:
                        #    print rank[-1][epoch], 1.0 * rank[bank][epoch], bank, epoch, len(rank[-1])
                        #print rank[-1]
                        #print (1.0 * v[z][y] / v[ - 1][y])
                    gnuplot[2].stdin.write("e\n")
                gnuplot[2].stdin.write('%lf 0.2\ne\n' % (len(rank[0]) * epochTime))
            gnuplot[2].stdin.write("\nunset multiplot\nunset output\n")


        # make the address distribution per channel graphs
        channelID = 0
        for channel in channelDistribution:
            #for each channel make its own graph
            gnuplot[5].stdin.write(subAddrDistroA % ("addressDistribution%d" % channelID))
            fileList.append('addressDistribution%d.%s' % (channelID, extension))
            channelID += 1
            rankFraction = 1.0 / len(channel)
            offset = 1 - rankFraction
            rankID = 0
            for rank in channel:
                gnuplot[5].stdin.write(subAddrDistroB % (rankFraction, offset, rankID, "" if rankID > 0 else "\\n%s" % commandLine))
                rankID += 1
                offset -= rankFraction
                if rankID < len(channel):
                    gnuplot[5].stdin.write('\nunset key\nunset xlabel\n')
                else:
                    gnuplot[5].stdin.write('\nset xlabel "Time (s)" offset 0,0.6\nset key outside center bottom horizontal reverse Left\n')
                    pass
                gnuplot[5].stdin.write('plot ')
                for a in range(len(rank) - 1):
                    gnuplot[5].stdin.write('"-" using 1 "%%lf" axes x2y1 t "bank_{%d}  ",' % a)
                gnuplot[5].stdin.write('"-" using 1:2 axes x1y1 notitle with points pointsize 0.01\n')
                for bank in range(len(rank) - 1):
                    #for each bank
                    for epoch in range(len(rank[bank])):
                        gnuplot[5].stdin.write("%f\n" % (1.0 * rank[bank][epoch] / rank[ - 1][epoch]))
                        #if epoch == 10:
                        #    print rank[-1][epoch], 1.0 * rank[bank][epoch], bank, epoch, len(rank[-1])

                        #if y == 4:
                        #    print v[-1][y], 1.0 * v[z][y]
                    gnuplot[5].stdin.write("e\n")
                gnuplot[5].stdin.write('%lf 0.2\ne\n' % (len(rank[0]) * epochTime))
            gnuplot[5].stdin.write("\nunset multiplot\nunset output\n")



        # make the address distribution graphs
        gnuplot[1].stdin.write(addressDistroA % commandLine)
        gnuplot[1].stdin.write('plot ')
        for a in range(len(channels)):
            gnuplot[1].stdin.write('"-" using 1 \'%%lf\' axes x2y1 t "ch[%d]",' % a)
        gnuplot[1].stdin.write('"-" using 1:2 axes x1y1 notitle with points pointsize 0.01\n')

        for x in channels:
            for y in range(len(x)):
                gnuplot[1].stdin.write("%lf\n" % (1.0 * x[y] / channelTotals[y]))
            gnuplot[1].stdin.write("e\n")
        gnuplot[1].stdin.write('%lf 0.2\ne\n' % (len(channelTotals) * epochTime))

        gnuplot[1].stdin.write(addressDistroB)
        gnuplot[1].stdin.write('plot ')
        for a in range(len(ranks)):
            gnuplot[1].stdin.write('"-" using 1 \'%%lf\' axes x2y1 t "rk[%d]",' % a)
        gnuplot[1].stdin.write('"-" using 1:2 axes x1y1 notitle with points pointsize 0.01\n')

        for x in ranks:
            for y in range(len(x)):
                gnuplot[1].stdin.write("%lf\n" % (1.0 * x[y] / rankTotals[y]))
            gnuplot[1].stdin.write("e\n")
        gnuplot[1].stdin.write('%lf 0.4\ne\n' % (len(channelTotals) * epochTime))

        gnuplot[1].stdin.write(addressDistroC)
        gnuplot[1].stdin.write('plot ')
        for a in range(len(banks)):
            gnuplot[1].stdin.write('"-" using 1 \'%%lf\' axes x2y1 t "bk[%d]",' % a)
        gnuplot[1].stdin.write('"-" using 1:2 axes x1y1 notitle with points pointsize 0.01\n')

        for x in banks:
            for y in range(len(x)):
                gnuplot[1].stdin.write("%lf\n" % (1.0 * x[y] / bankTotals[y]))
            gnuplot[1].stdin.write("e\n")
        gnuplot[1].stdin.write('%lf 0.6\ne\n' % (len(channelTotals) * epochTime))

        gnuplot[1].stdin.write("unset multiplot\nunset output\n")

        # make the PC vs latency graph
        gnuplot[0].stdin.write(pcVsLatencyGraph0 % (commandLine, extension))

        highValues = []
        for u in latencyVsPCDict.keys():
            if u < 0x100000000:
                gnuplot[0].stdin.write("%lf %f\n" % (u, period * latencyVsPCDict[u]))
            else:
                highValues.append(u)

        gnuplot[0].stdin.write("\ne\n%s\n" % pcVsLatencyGraph1)

        for u in highValues:
            gnuplot[0].stdin.write("%lf %f\n" % (u - 4294967296, period * latencyVsPCDict[u]))

        gnuplot[0].stdin.write("e\nunset multiplot\nunset output\n")

        # make the transaction latency distribution graph
        gnuplot[0].stdin.write(transactionGraph % (commandLine, extension))

        for u in distTransactionLatency.keys():
            gnuplot[0].stdin.write("%d %f\n" % (u * period, distTransactionLatency[u]))

        gnuplot[0].stdin.write("e\nunset output\n")

        # make the bandwidth graph
        for t in bandwidthValues[0]:
            gnuplot[4].stdin.write("%f\n" % t)
        gnuplot[4].stdin.write("e\n")

        for t in bandwidthValues[1]:
            gnuplot[4].stdin.write("%f\n" % t)
        gnuplot[4].stdin.write("e\n")

        count = 0
        for t in bandwidthValues[2]:
            gnuplot[4].stdin.write("%f %f\n" % (count * epochTime, t))
            count += 1

        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write(smallIPCGraph)

        # make the ipc graph
        minRange = min(len(ipcValues[0]), len(ipcValues[1]), len(ipcValues[2]))

        for i in range(0, minRange):
            gnuplot[4].stdin.write("%f %f\n" % (epochTime * i, ipcValues[0][i]))
        gnuplot[4].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[4].stdin.write("%f %f\n" % (epochTime * i, ipcValues[1][i]))
        gnuplot[4].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[4].stdin.write("%f %f\n" % (epochTime * i, ipcValues[2][i]))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write("unset multiplot\nunset output\n")

        # make the cache graph
        gnuplot[4].stdin.write(cacheGraphA % commandLine)

        for i in range(0, min(len(iCacheMisses), len(iCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, 1.0 * (iCacheMisses[i] + iCacheHits[i])))
        gnuplot[4].stdin.write("e\n")

        for i in range(0, min(len(iCacheMisses), len(iCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, iCacheMisses[i] / (iCacheMisses[i] + iCacheHits[i] + 1.0)))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write(cacheGraphB)
        for i in range(0, min(len(dCacheMisses), len(dCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, 1.0 * (dCacheMisses[i] + dCacheHits[i])))
        gnuplot[4].stdin.write("e\n")

        for i in range(0, min(len(dCacheMisses), len(dCacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, dCacheMisses[i] / (dCacheMisses[i] + dCacheHits[i] + 1.0)))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write(cacheGraphC)

        for i in range(0, min(len(l2CacheMisses), len(l2CacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, 1.0 * (l2CacheMisses[i] + l2CacheHits[i])))
        gnuplot[4].stdin.write("e\n")

        for i in range(0, min(len(l2CacheMisses), len(l2CacheHits))):
            gnuplot[4].stdin.write("%f %f\n" % (i * epochTime, l2CacheMisses[i] / (l2CacheMisses[i] + l2CacheHits[i] + 1.0)))
        gnuplot[4].stdin.write("e\n")

        gnuplot[4].stdin.write("unset multiplot\nunset output\n")

        # make the ipc graph
        minRange = min(len(ipcValues[0]), len(ipcValues[1]), len(ipcValues[2]))

        for i in range(0, minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, ipcValues[0][i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, ipcValues[1][i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, ipcValues[2][i]))
        gnuplot[6].stdin.write("e\n")

        gnuplot[6].stdin.write(averageTransactionLatencyScript % commandLine)

        # make the transaction latency graph
        minRange = min(len(transactionLatency), len(cumulativeTransactionLatency),
                       len(movingTransactionLatency), len(transactionCount))

        for i in range(0, minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, period * transactionLatency[i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i, period * cumulativeTransactionLatency[i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i , period * movingTransactionLatency[i]))
        gnuplot[6].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[6].stdin.write("%f %f\n" % (epochTime * i , transactionCount[i]))
        gnuplot[6].stdin.write("e\n")

        gnuplot[6].stdin.write("unset multiplot\nunset output\n")

        # make the hit-miss graph
        minRange = min(len(hitMissValues[0]), len(hitMissValues[1]), len(hitMissValues[2]), len(hitMissValues[3]))

        for i in range(0, minRange):
            gnuplot[7].stdin.write("%f %f\n" % (epochTime * i, hitMissValues[1][i]))

        gnuplot[7].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[7].stdin.write("%f %f\n" % (epochTime * i, hitMissValues[2][i]))

        gnuplot[7].stdin.write("e\n")

        for i in range(0, minRange):
            gnuplot[7].stdin.write("%f %f\n" % (epochTime * i , hitMissValues[3][i]))

        gnuplot[7].stdin.write("e\nunset output\n")

        # close outputs that are written to once per epoch
        gnuplot[3].stdin.write("e\nunset output\n")
        gnuplot[4].stdin.write("unset output\n")

        # make a list of all the files to be processed
        fileList.append(os.path.join(tempPath, "workingSet." + extension))
        fileList.append(os.path.join(tempPath, "bandwidth." + extension))
        fileList.append(os.path.join(tempPath, "averageIPCandLatency." + extension))
        fileList.append(os.path.join(tempPath, "rowHitRate." + extension))
        fileList.append(os.path.join(tempPath, "cacheData." + extension))
        fileList.append(os.path.join(tempPath, "latencyVsPc." + extension))
        fileList.append(os.path.join(tempPath, "transactionLatencyDistribution." + extension))
        fileList.append(os.path.join(tempPath, "addressDistribution." + extension))

        # close out gnuplot
        for b in gnuplot:
            b.stdin.write('\nexit\n')
            b.wait()

    # make a big file of them all
    #print 'Compress graphs'
    #outputFile = tarfile.open(os.path.join(tempPath,outFileBZ2),'w:bz2')
    #for name in fileList:
    #    outputFile.add(name, os.path.basename(name))

    #outputFile.close()

    thumbnail(fileList, tempPath)

    #files = os.listdir(tempPath)
    #htmlFile = os.path.join(os.path.abspath(sys.path[0]),"template.html")

    #shutil.copy(htmlFile, os.path.join(tempPath,basefilename[0:basefilename.find("-stats")] + ".html"))
    o = open(os.path.join(tempPath, basefilename[0:basefilename.find("-stats")] + ".html"), "w")
    templateF = open(os.path.join(os.path.dirname(os.path.realpath(__file__)), "template.html"))
    template = templateF.read()
    template = re.sub("@@@", commandLine, template)
    distroStrings = ''
    for i in range(len(channels)):
        distroStrings += addressDistroString % (i, i, i)
    for i in range(len(channels)):
        distroStrings += addressLatencyString % (i, i, i)
    o.write(re.sub("\+\+\+", distroStrings, template))
    o.close()
    templateF.close()

    o = open(os.path.join(tempPath, ".htaccess"), "w")
    accessFileF = open(os.path.join(os.path.dirname(os.path.realpath(__file__)), ".htaccess"))
    accessFile = accessFileF.read()
    o.write(accessFile)
    o.close()
    accessFileF.close()

    #os.rmdir(tempPath)
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
                #prof = cProfile.Profile().runctx("processStats(item)", globals(), locals())
                #stream = StringIO.StringIO()
                #stats = pstats.Stats(prof, stream=stream)
                #stats.sort_stats("time")  # Or cumulative
                #stats.print_stats()  # 80 = how many to print
                #print stream.getvalue()

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

    files = {}
    for u in sys.argv:
        if u.endswith("power.gz") or u.endswith("power.bz2") or u.endswith("stats.gz") or u.endswith("stats.bz2"):
            v = u[0:u.rfind("-")]
            commandLine = v

            if u.endswith("gz"):
                compressedstream = gzip.open(u, 'rb')
            else:
                compressedstream = bz2.open(u, 'rb')
            lineCounter = 0

            try:
                for line in compressedstream:

                    if line.startswith('----Command Line:'):
                        commandLine = line.strip().split(":")[1]
                        urlString = '<a href="%s/%s.html">%%s</a>' % (v, v)

                        commandLine = re.sub("_{}", "", commandLine).split()
                        #line = "<tr>"
                        line = ""
                        for x in commandLine:
                            start = x.find('[')
                            end = x.find(']')
                            if start == - 1 or end == - 1:
                                benchmarkName = x
                            else:
                                benchmarkName = x[start + 1:end]
                            if decoder.has_key(benchmarkName):
                                benchmarkName = decoder[benchmarkName]
                            line += "<td>" + urlString % benchmarkName + "</td>"
                        files[v] = line
                        workQueue.put(u)
                        break
                    lineCounter += 1
                    if lineCounter > 30:
                        break
            except IOError, strerror:
                pass
            except OSError, strerror:
                pass
            finally:
                compressedstream.close()

    o = open("result.html", "w")
    data = open(os.path.join(os.path.dirname(os.path.realpath(__file__)), "template2.html")).read()
    filelist = ""

    for x in files:
        filelist += "<tr> %s </tr>" % files[x]
    o.write(re.sub("@@@", filelist, data))
    o.close()

    #os.system("cp -r %s ." % os.path.join(os.path.abspath(sys.path[0]),"js"))
    try:
        shutil.copytree(os.path.join(os.path.dirname(os.path.realpath(__file__)), "js"), os.path.join(os.getcwd(), "js"))
    except OSError:
        pass

    workQueue.join()

if __name__ == "__main__":
     os.environ["GDFONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"
     os.environ["GNUPLOT_FONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"

     main()


     # This is the main function for profiling
     # We've renamed our original main() above to real_main()
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

