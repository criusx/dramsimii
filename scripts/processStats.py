#!/usr/bin/python -O
##!/usr/bin/python

import gzip
import sys
import os
import string
import subprocess
from threading import Thread
import Queue
import tempfile
import tarfile
import math
import bz2
import re
from subprocess import Popen, PIPE, STDOUT

#globals
workerThreads = 3

IPCWindow = 20

processPerEpochGraphs = False

terminal = 'set terminal png font "VeraMono,10" transparent nointerlace truecolor  size 1700, 1024 nocrop enhanced\n'
extension = 'png'

#terminal = 'set terminal svg size 1600,768 dynamic enhanced fname "VeraMono" fsize 10\n'
#extension = 'svg'

#terminal = 'set terminal postscript eps enhanced color font "VeraMono, 10"'
#extension = 'eps'



class RingBuffer:
    def __init__(self, size):
        self.data = [0.0 for i in xrange(size)]

    def append(self, x):
        self.data.pop(0)
        self.data.append(x)

    def get(self):
        return self.data

def processPower(filename):
    # remember the temp directory
    tempPath = tempfile.mkdtemp(prefix="processStats")

    os.environ["GDFONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"

    # setup the script headers
    scripts = [terminal + '''
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    set key center rmargin Right reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5
    #set xrange [0:*] noreverse nowriteback
    set autoscale xfixmin
    set autoscale xfixmax
    set yrange [0:*] noreverse nowriteback

    set ytics out
    #set xtics out
    #set mxtics
    #set logscale y

    set ylabel "Power Dissipated (mW)" offset character .05, 0,0 font "VeraMono,12" textcolor lt -1 rotate by 90
    set xlabel "Epoch" font "VeraMono,12"

    set boxwidth 1.00 relative
    set style fill  solid 1.00 noborder
    set style data histograms
    set style histogram rowstacked title offset 0,0,0
    ''']
    writing = 0
    # one instance of gnuplot per script that needs this
    gnuplot = []
    #counter = 0
    for i in scripts:
        p = Popen(['gnuplot'], stdin=PIPE, stdout=PIPE, shell=False)
        p.stdin.write(i)
        gnuplot.append(p)

    values = []
    channels = 0
    ranks = 0

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
                        values[writing].append(line[startVal + 1:endVal])
                        #values[writing.append( "%f %s" % (powerCounter * epochTime, line[startVal + 1:endVal]))
                        writing = (writing + 1) % (valuesPerEpoch)
                elif line.startswith('----Epoch'):
                    splitLine = splitter2.split(line)
                    epochTime = float(splitLine[1])
                    #gnuplot[0].stdin.write("set xtics %f\n" % epochTime)
                    gnuplot[0].stdin.write("set xtics autofreq\n")

                elif line.startswith('----'):
                    gnuplot[0].stdin.write("set title \"Power Consumed vs. Time\\n^{%s}\"  offset character 0, -1, 0 font \"VeraMono,14\" norotate\n" % line[4:len(line) - 5])
                elif line[1] == '+':
                    gnuplot[0].stdin.write('set output "' + basefilename + "." + extension + '\n')
                    splitLine = splitter.split(line)
                    channels = int(splitLine[2])
                    ranks = int(splitLine[6])

                    gnuplot[0].stdin.write('plot ')

                    for a in range(channels):
                        for b in range(ranks):
                           gnuplot[0].stdin.write('"-" using 1 title "%s ch[%d]rk[%d]"' % ("P_{sys}(ACT-STBY)" if b % 2 == 0 else "P_{sys}(ACT)", a, b))
                           gnuplot[0].stdin.write(",")
                        for b in range(2):
                            gnuplot[0].stdin.write('"-" using 1 title "P_{sys}ch[%d](%s)"' % (a, "RD" if b % 2 == 0 else "WR"))
                            if (a < channels - 1) or (b < 1):
                                gnuplot[0].stdin.write(",")
                        if (a == channels - 1):
                            gnuplot[0].stdin.write("\n")
                    # plus the RD and WR values
                    valuesPerEpoch = channels * (ranks + 2)
                    for i in range(valuesPerEpoch):
                        values.append([])
    except IOError, strerror:
        print "I/O error", strerror
    except OSError, strerror:
        print "OS error", strerror

    #print len(values[0])
    #gnuplot[0].stdin.write("\nset xrange [0:%d]" % len(values[0]))

    for u in values:
        for v in u:
            gnuplot[0].stdin.write("%s\n" % v)
        gnuplot[0].stdin.write("e\n")

    gnuplot[0].stdin.write("unset output\n")

    print 'unset'

    for b in gnuplot:
        b.stdin.write('exit\n')
        b.wait()

    print 'done'

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
    unset key
    set autoscale xfixmax
    set autoscale xfixmin
    set xtics nomirror out
    set boxwidth 1.00 relative
    '''

    # setup the script headers
    scripts = [terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set ytics out
    set logscale y
    set style fill  solid 1.00 border -1
    set xlabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Transactions with this Execution Time"
    set title "Transaction Latency\\n%s"  offset character 0, -1, 0 font "" norotate
        ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    unset ytics
    set style fill  solid 1.00 noborder
    set xlabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Commands with this Execution Time"
    set title "Command Execution Time\\n%s"  offset character 0, -1, 0 font "" norotate
        ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set ytics out
    set logscale y
    set style fill  solid 1.00 noborder
    set xlabel "Time From Enqueue To Execution (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Commands with this Latency"
    set title "Command Latency\\n%s"  offset character 0, -1, 0 font "" norotate
    ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    unset ytics
    set mxtics
    set style fill  solid 1.00 noborder
    set xlabel "Time"
    set ylabel "Working Set Size" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set title "Working Set Size vs Time\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "workingSet.''' + extension + '''"
    plot '-' using 1:2 with boxes
    ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set ytics out
    set style fill solid 1.00 noborder
    set style data histograms
    set style histogram rowstacked title offset 0,0,0
    set output "bandwidth.''' + extension + '''"
    set xlabel "Time (epochs)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Bandwidth (bytes per second)"
    set title "System Bandwidth\\n%s"  offset character 0, -1, 0 font "" norotate
    plot '-' using 2 title "Read Bytes", '-' using 2 title "Write Bytes", '-' using 1:2 title "Average Bandwidth" with lines linetype 1001
    ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set ytics out
    set logscale y
    set xlabel "PC Value" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Total Latency (cycles)"
    set title "Total Latency vs. PC Value\\n%s"  offset character 0, -1, 0 font "" norotate
    #set style fill solid 1.00 border 0
    set format x "0x1%%x"
    set style fill solid 1.00 noborder
    ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    set ytics out
    set xlabel "Time" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Instructions Per Cycle"
    set title "Average IPC vs. Time\\n%s"  offset character 0, -1, 0 font "" norotate
    set style fill solid 1.00 noborder
    set output "averageIPC.''' + extension + '''"
    plot '-' using 1:2 title "IPC" with impulses, '-' using 1:2 title "Average IPC" with lines, '-' using 1:2 title "Moving Average IPC" with lines
    ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    set ytics out
    set xlabel "Time" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Hit Rate"
    set title "Hit Rate of Open Rows vs. Time\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "rowHitRate.''' + extension + '''"
    plot '-' using 1:2 title "Hit Rate" with filledcurve below x1, '-' using 1:2 title "Average Hit Rate" with lines
    #, '-' using 1:2 title "Hit Rate" with filledcurve
    '''
    ]

    transCounter = 0
    commandTurnaroundCounter = 0
    cmdCounter = 0
    workingSetCounter = 0
    rwTotalCounter = 0
    latencyVsPC = 0
    ipcCounter = 0

    # what type we are writing to
    writing = 0
    # the list of files to be compressed
    fileList = []

    # the list of files to be removed
    # one instance of gnuplot per script that needs this
    gnuplot = []

    hitMissCounter = 0
    hitMissTotal = 0.0
    hitTotal = 0.0
    hitMissValues = []
    hitMissValues.append([])
    hitMissValues.append([])
    hitMissValues.append([])

    bandwidthCounter = 0
    bandwidthTotal = 0.0
    bandwidthValues = []
    bandwidthValues.append([])
    bandwidthValues.append([])
    bandwidthValues.append([])

    ipcValues = []
    ipcValues.append([])
    ipcValues.append([])
    ipcValues.append([])
    ipcBuffer = RingBuffer(IPCWindow)
    ipcTotal = 0.0

    outFileBZ2 = os.path.join(os.getcwd(),  filename.split('.gz')[0] + ".tar.bz2")

    started = False
    ipcLinesWritten = 0
    writeLines = 0
    epochTime = 0.0

    #process2.wait()

    #lineCount = max(int(process2.stdout.readline()),1)

    # counter to keep track of how many lines are written to each file
    #linesWritten = 0
    print 'Generate graphs'

    try:
        if filename.endswith("gz"):
            compressedstream = gzip.open(filename, 'rb')
        elif filename.endswith("bz2"):
            compressedstream = bz2.open(filename, 'rb')

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

                writing = 0
                line = line.strip()

                if started == False:
                    # extract the command line, set the writers up
                    if line.startswith('----Command Line:'):
                        commandLine = line.split(":")[1]

                        for i in scripts:
                            p = Popen(['gnuplot'], stdin=PIPE, shell=False, cwd=tempPath)
                            p.stdin.write(i % commandLine)
                            #% line[4:len(line) - 5]
                            gnuplot.append(p)
                        started = True
                        print commandLine
                elif line[4] == 'E':
                    epochTime = float(line.split(" ")[1])
                else:
                    if line[4] == 'T' :
                        if processPerEpochGraphs:
                            outFile = "transactionExecution" + '%05d' % transCounter + "." + extension
                            gnuplot[0].stdin.write("set output './%s'\nplot '-' using 1:2:(1) with boxes\n" % outFile)
                            transCounter += 1
                            fileList.append(os.path.join(tempPath,outFile))
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
                    elif line[4] == 'R':
                        writing = 10
            # data in this section
            else:
                if processPerEpochGraphs:
                    if writing == 1:
                        gnuplot[0].stdin.write(line)
                    elif writing == 4:
                        gnuplot[1].stdin.write(line)
                    elif writing == 6:
                        gnuplot[2].stdin.write(line)
                    elif writing == 8:
                        splitLine = line.split(" ")
                        gnuplot[5].stdin.write("%d %d\n" % (int(splitLine[0], 16) - 4294967296, (float(splitLine[1]) * float(splitLine[2]))))

                if writing == 2:
                    if len(line) > 1:
                        gnuplot[3].stdin.write("%f %s" % (workingSetCounter * epochTime,line))
                        workingSetCounter += 1
                elif writing == 7:
                    newLine = line.strip().split(" ")
                    bandwidthValues[0].append("%f %s" % (bandwidthCounter * epochTime,newLine[0]))
                    bandwidthValues[1].append("%f %s" % (bandwidthCounter * epochTime,newLine[1]))
                    bandwidthTotal += float(newLine[0]) + float(newLine[1])
                    bandwidthCounter += 1
                    bandwidthValues[2].append("%f %f" % (bandwidthCounter * epochTime,bandwidthTotal / bandwidthCounter))
                elif writing == 9:
                    if (ipcLinesWritten < 1):
                        # append the values for this time
                        ipcValues[0].append("%f %s" % (ipcCounter * epochTime,line))
                        # append the moving average
                        currentValue = float(line)
                        ipcTotal += currentValue
                        ipcCounter += 1
                        ipcValues[1].append("%f %f" % (ipcCounter * epochTime, ipcTotal / ipcCounter))
                        ipcBuffer.append(currentValue)
                        if ipcCounter > (IPCWindow / 2):
                            ipcValues[2].append("%f %f" % ((ipcCounter - IPCWindow / 2)  * epochTime, sum(ipcBuffer.get()) / IPCWindow))
                    ipcLinesWritten += 1
                elif writing == 10:
                    newLine = line.strip().split(" ")
                    hitCount = max(float(newLine[0]),1)
                    missCount = max(float(newLine[1]),1)
                    hitMissValues[0].append("%f %f" % (hitMissCounter * epochTime, missCount / (hitCount + missCount)))
                    hitMissValues[1].append("%f %f" % (hitMissCounter * epochTime, hitCount / (hitCount + missCount)))
                    hitMissCounter += 1
                    hitMissTotal += hitCount + missCount
                    hitTotal += hitCount
                    hitMissValues[2].append("%f %f" % (hitMissCounter * epochTime , hitTotal / hitMissTotal))

        compressedstream.close()

    except IOError, strerror:
        print "I/O error", strerror
    except OSError, strerror:
        print "OS error", strerror

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

        for t in bandwidthValues:
            for u in t:
                gnuplot[4].stdin.write("%s\n" % u)
            gnuplot[4].stdin.write("e\n")

        #gnuplot[4].stdin.write("\n".join(bandwidthValues[0]) + "\ne\n")
        #gnuplot[4].stdin.write("\n".join(bandwidthValues[1]) + "\ne\n")
        #gnuplot[4].stdin.write("\n".join(bandwidthValues[2]) + "\ne\n")

        for t in ipcValues:
            for u in t:
                gnuplot[6].stdin.write("%s\n" % u)
            gnuplot[6].stdin.write("e\n")

        #for u in hitMissValues:
        #gnuplot[7].stdin.write("\n".join(u) + "\ne\n")
        for t in hitMissValues[1]:
            gnuplot[7].stdin.write("%s\n" % t)

        gnuplot[7].stdin.write("e\n")

        for t in hitMissValues[2]:
            gnuplot[7].stdin.write("%s\n" % t)

        gnuplot[7].stdin.write("e\n")

        # close outputs that are written to once per epoch
        gnuplot[6].stdin.write("unset output\n")
        gnuplot[3].stdin.write("e\nunset output\n")
        gnuplot[4].stdin.write("unset output\n")
        gnuplot[7].stdin.write("unset output\n")

        fileList.append(os.path.join(tempPath,"workingSet." + extension))
        fileList.append(os.path.join(tempPath,"bandwidth." + extension))
        fileList.append(os.path.join(tempPath,"averageIPC." + extension))
        fileList.append(os.path.join(tempPath,"rowHitRate." + extension))

        for b in gnuplot:
            b.stdin.write('exit\n')
            b.wait()

    # make a big file of them all
    print 'Compress graphs'
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
        item = workQueue.get()
        if item.endswith("stats.gz") or item.endswith("stats.bz2"):
            processStats(item)
        elif item.endswith("power.gz") or item.endswith("power.bz2"):
            processPower(item)
        workQueue.task_done()

def main():
    os.environ["GDFONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"

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
    #main()
     # This is the main function for profiling
     # We've renamed our original main() above to real_main()
     import cProfile, pstats, StringIO
     prof = cProfile.Profile()
     prof = prof.runctx("main()", globals(), locals())
     stream = StringIO.StringIO()
     stats = pstats.Stats(prof, stream=stream)
     stats.sort_stats("time")  # Or cumulative
     stats.print_stats()  # 80 = how many to print
     # The rest is optional.
     stats.print_callees()
     stats.print_callers()
     print stream.getvalue()
     #logging.info("Profile data:\n%s", stream.getvalue())

