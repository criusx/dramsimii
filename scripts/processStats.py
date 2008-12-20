#!/usr/bin/python -O

import gzip
import sys
import os
import string
import subprocess
from subprocess import Popen, PIPE, STDOUT

def main():
    os.environ["GDFONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"

    #terminal = 'set terminal png font "VeraMono,10" transparent nointerlace truecolor  size 1600, 768 nocrop enhanced\n'
    #extension = 'png'
    terminal = 'set terminal svg size 1600,768 dynamic enhanced fname "VeraMono" fsize 10\n'
    extension = 'svg'
    #terminal = 'set terminal postscript eps enhanced color font "VeraMono, 10"'
    #extension = 'eps'
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
    set xlabel "Epoch"
    set ylabel "Working Set Size" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set title "Working Set Size vs Time\\n%s"  offset character 0, -1, 0 font "" norotate
    ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set ytics out
    set style fill  solid 1.00 noborder
    set style data histograms
    set style histogram rowstacked title offset 0,0,0
    set output "bandwidth.''' + extension + '''"
    set xlabel "Time (epochs)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Bandwidth (bytes per second)"
    set title "System Bandwidth\\n%s"  offset character 0, -1, 0 font "" norotate
    plot '-' using 1 title "Read Bytes", '-' using 1 title "Write Bytes"
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
    plot '-' using 1 title "Average IPC" with boxes
    '''
    ]

    workingSetOutfile = []
    transCounter = 0
    commandTurnaroundCounter = 0
    cmdCounter = 0
    workingSetCounter = 0
    rwTotalCounter = 0
    latencyVsPC = 0
    rwTotalOutfile = []
    # what type we are writing to
    writing = 0
    # the list of files to be compressed
    fileList = []
    # the list of files to be removed
    # one instance of gnuplot per script that needs this
    gnuplot = []
    counter = 0


    bandwidthValues = []
    bandwidthValues.append([])
    bandwidthValues.append([])
    outFileBZ2 = sys.argv[1].split('.gz')[0] + ".tar.bz2"

    process = Popen(["zcat " + sys.argv[1] ], stdout=PIPE, shell=True)
    process2 = Popen(["wc -l"], stdin=process.stdout, stdout=PIPE, shell=True)
    process2.wait()

    lineCount = int(process2.stdout.readline())
    started = False
    printString = ""
    writeLines = 0

    # counter to keep track of how many lines are written to each file
    linesWritten = 0

    try:
        compressedstream = gzip.open(sys.argv[1], 'rb')

        for line in compressedstream:
            counter += 1
            if (counter % 5000 == 0):
                for a in printString:
                    sys.stdout.write("\b \b")
                printString = "%3.2f%%" % ((float(counter) / lineCount) * 100)
                sys.stdout.write(printString)
                sys.stdout.flush()

            if line[0] == '-':
                if writing == 1:
                    if (linesWritten == 0):
                        gnuplot[0].stdin.write("0 1.01\n");
                    linesWritten = 0
                    gnuplot[0].stdin.write("e\nunset output\n")
                elif writing == 4:
                    if (linesWritten == 0):
                        gnuplot[1].stdin.write("0 1.01\n");
                    linesWritten = 0
                    gnuplot[1].stdin.write("e\nunset output\n")
                elif writing == 6:
                    if (linesWritten == 0):
                        gnuplot[2].stdin.write("0 1.01\n");
                    linesWritten = 0
                    gnuplot[2].stdin.write("e\nunset output\n")
                elif writing == 8:
                    if (linesWritten == 0):
                        gnuplot[5].stdin.write("0 1.01\n");
                    linesWritten = 0
                    gnuplot[5].stdin.write("e\nunset output\n")
                elif writing == 9:
                    linesWritten = 0

                writing = 0
                line = line.strip()

                if started == False:
                    # extract the command line, set the writers up
                    if line.startswith('----Command Line:'):
                        commandLine = line.split(":")[1]

                        for i in scripts:
                            p = Popen(['gnuplot'], stdin=PIPE, shell=False)
                            p.stdin.write(i % commandLine)
                            #% line[4:len(line) - 5]
                            gnuplot.append(p)
                        started = True
                        print commandLine
                else:
                    if line == '----Transaction Latency----':
                        outFile = "transactionExecution" + '%04d' % transCounter + "." + extension
                        gnuplot[0].stdin.write("set output './%s'\nplot '-' using 1:2:(1) with boxes\n" % outFile)
                        transCounter += 1
                        fileList.append(outFile)
                        writing = 1
                    elif line == '----Working Set----':
                        writing = 2
                    elif line == '----Delay----':
                        writing = 3
                    elif line == '----CMD Execution Time----':
                        outFile = "commandExecution" + '%04d' % cmdCounter + "." + extension
                        gnuplot[1].stdin.write("set output './%s'\nplot '-' using 1:2:(1)  with boxes\n" % outFile)
                        cmdCounter += 1
                        fileList.append(outFile)
                        writing = 4
                    elif line == '----R W Total----':
                        writing = 5
                    elif line == '----Command Turnaround----':
                        outFile = "commandTurnaround" + '%04d' % commandTurnaroundCounter + "." + extension
                        gnuplot[2].stdin.write("set output './%s'\nplot '-' using 1:2:(1)   with boxes\n" % outFile)
                        commandTurnaroundCounter += 1
                        fileList.append(outFile)
                        writing = 6
                    elif line == "----Bandwidth----":
                        writing = 7
                    elif line == "----Average Transaction Latency Per PC Value----":
                        outFile = "latencyVsPC" + '%04d' % latencyVsPC + "." + extension
                        gnuplot[5].stdin.write("set output './%s'\nplot '-' using 1:2:(1)   with boxes\n" % outFile)
                        latencyVsPC += 1
                        fileList.append(outFile)
                        writing = 8
                    elif line == "----IPC----":
                        writing = 9
            # data in this section
            else:
                if writing == 1:
                    gnuplot[0].stdin.write(line)
                    linesWritten += 1
                elif writing == 2:
                    if len(line) > 1:
                        workingSetCounter += 1
                        workingSetOutfile.append(`workingSetCounter` + ' ' + line)
                elif writing == 4:
                    gnuplot[1].stdin.write(line)
                    linesWritten += 1
                elif writing == 5:
                    rwTotalOutfile.append(`rwTotalCounter` + ' ' + line)
                elif writing == 6:
                    gnuplot[2].stdin.write(line)
                    linesWritten += 1
                elif writing == 7:
                    newLine = line.strip().split(" ")
                    bandwidthValues[0].append(newLine[0])
                    bandwidthValues[1].append(newLine[1])
                    #bandwidthCounter += 1
                elif writing == 8:
                    splitLine = line.split(" ")
                    gnuplot[5].stdin.write("%d %d\n" % (int(splitLine[0], 16) - 4294967296, (float(splitLine[1]) * float(splitLine[2]))))
                    linesWritten += 1
                elif writing == 9:
                    if (linesWritten < 1):
                        gnuplot[6].stdin.write(line)
                    linesWritten += 1

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

        gnuplot[6].stdin.write("e\nunset output\n")

        outFile = "workingSet" + "." + extension
        gnuplot[3].stdin.write("set output './%s'\nplot '-' using 1:2:(1) with boxes\n" % outFile)
        gnuplot[3].stdin.write(string.join(workingSetOutfile, "\n") + "e\n")

        for u in bandwidthValues:
            gnuplot[4].stdin.write("\n".join(u) + "\ne\n")

        fileList.append("workingSet" + "." + extension)
        fileList.append("bandwidth" + "." + extension)
        fileList.append("averageIPC" + "." + extension)

        for b in gnuplot:
            b.stdin.write('exit\n')
            b.wait()

    # make a big file of them all
    os.system("tar cjf " + outFileBZ2 + " --remove-files --totals " + " ".join(fileList))

if __name__ == "__main__":
    main()
