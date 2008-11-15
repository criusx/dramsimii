#!/usr/bin/python -O

import gzip
import sys
import os
import string
import subprocess
from subprocess import Popen,  PIPE, STDOUT

def main():
    os.environ["GDFONTPATH"]="/usr/share/fonts/truetype/msttcorefonts"
    
    # setup the script headers
    scripts = ['''
    set terminal png font "Arial_Black" 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    unset key
    set autoscale xfixmax
    set yrange [1 : *] noreverse nowriteback
    
    set ytics out
    set xtics out
    #set mxtics
    set logscale y
    set style fill  solid 1.00 border -1
    set xlabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Transactions with this Execution Time"
    set title "Transaction Latency"  offset character 0, -1, 0 font "" norotate 
    set boxwidth 1.00 relative
        ''','''
        set terminal png font "Arial_Black" 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    unset key
    set autoscale xfixmax
    set yrange [0 : *] noreverse nowriteback
    unset ytics
    #set mxtics
    set style fill  solid 1.00 border -1
    set xlabel "Execution Time (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Commands with this Execution Time"
    set boxwidth 1.00 relative
    set title "Command Execution Time"  offset character 0, -1, 0 font "" norotate 
        ''','''
        set terminal png font "Arial_Black" 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    unset key
    set autoscale xfixmax
    set yrange [1 : *] noreverse nowriteback
    
    set ytics out
    set xtics out
    #set mxtics
    set logscale y
    set style fill  solid 1.00 border -1
    set xlabel "Time From Enqueue To Execution (cycles)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Commands with this Turnaround Time"
    set title "Command Turnaround Time"  offset character 0, -1, 0 font "" norotate 
    set boxwidth 1.00 relative
    ''', '''
    set terminal png font "Arial_Black" 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    unset key
    set autoscale xfixmax
    set yrange [0 : *] noreverse nowriteback
    unset ytics
    set mxtics
    set style fill  solid 1.00 border -1
    set ylabel "Working Set Size" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set xlabel "Epoch"
    set title "Working Set Size vs Time"  offset character 0, -1, 0 font "" norotate 
    ''', '''
    set terminal png font "Arial_Black" 11 transparent nointerlace truecolor  size 1024, 768 nocrop enhanced
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    unset key
    set autoscale xfixmax
    set yrange [1 : *] noreverse nowriteback    
    set ytics out
    set xtics out
    #set mxtics
    set logscale y
    set style fill  solid 1.00 border -1
    set xlabel "Time (epochs)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Bandwidth (bytes transferred)"
    set title "System Bandwidth"  offset character 0, -1, 0 font "" norotate 
    set boxwidth 1.00 relative
    set output "bandwidth.png"
    plot '-' using 1:2:(1)  with boxes
    '''
    ]
    
    workingSetOutfile = []
    transCounter = 0
    commandTurnaroundCounter = 0
    cmdCounter = 0    
    workingSetCounter = 0
    rwTotalCounter = 0
    bandwidthCounter = 0
    rwTotalOutfile = []
    # what type we are writing to
    writing = 0
    # the list of files to be compressed
    fileList = []
    # the list of files to be removed
    # one instance of gnuplot per script that needs this
    gnuplot = []
    #counter = 0
    for i in scripts:
        p = Popen(['gnuplot'], stdin=PIPE, stdout=PIPE, shell=False)
        p.stdin.write(i)
        gnuplot.append(p)
    
    outFileBZ2 = sys.argv[1].split('.gz')[0] + ".tar.bz2"
    
    try:
        compressedstream = gzip.open(sys.argv[1], 'rb')
        for line in compressedstream:
            #if counter > 256:
             #   break
            # end/beginning of section
            if line[0] == '-':  
              #  counter += 1
                if writing == 1:
                    gnuplot[0].stdin.write("e\nunset output\n")
                    pass
                elif writing == 4:                
                    gnuplot[1].stdin.write("e\nunset output\n")
                    pass
                elif writing == 6:
                    gnuplot[2].stdin.write("e\nunset output\n")
                    pass
                    
                writing = 0
                line = line.strip() 
    
                if line == '----Transaction Latency----':
                    outFile = "transactionExecution" + '%04d' % transCounter + ".png"
                    gnuplot[0].stdin.write("set output './%s'\nplot '-' using 1:2:(1) with boxes\n" % outFile)
                    transCounter +=1
                    fileList.append(outFile)  
                    writing = 1           
                elif line == '----Working Set----':
                    writing = 2
                elif line == '----Delay----':
                    writing = 3
                elif line == '----CMD Execution Time----':    
                    outFile = "commandExecution" + '%04d' % cmdCounter + ".png" 
                    gnuplot[1].stdin.write("set output './%s'\nplot '-' using 1:2:(1)  with boxes\n" % outFile)
                    cmdCounter += 1
                    fileList.append(outFile)        
                    writing = 4
                elif line == '----R W Total----':
                    writing = 5
                elif line == '----Command Turnaround----':
                    outFile = "commandTurnaround" + '%04d' % commandTurnaroundCounter + ".png" 
                    gnuplot[2].stdin.write("set output './%s'\nplot '-' using 1:2:(1)   with boxes\n" % outFile)
                    commandTurnaroundCounter += 1
                    fileList.append(outFile)
                    writing = 6
                elif line == "----Bandwidth----":
					writing = 7
                    
            # data in this section
            else:
                if writing == 1:
                    gnuplot[0].stdin.write(line)
                    pass
                elif writing == 2:
                    if len(line) > 1:
                        workingSetCounter += 1
                        workingSetOutfile.append(`workingSetCounter` + ' ' + line)
                elif writing == 4:
                    gnuplot[1].stdin.write(line)
                elif writing == 5:
                    rwTotalOutfile.append(`rwTotalCounter` + '' + line)
                elif writing == 6:
                    gnuplot[2].stdin.write(line)
                elif writing == 7:
					gnuplot[4].stdin.write(`bandwidthCounter` + " " + line)
					bandwidthCounter += 1
                    
    
    except IOError, strerror:
        print "I/O error", strerror
    except OSError,  strerror:
        print "OS error",  strerror
     
    outFile = "workingSet.png"
    gnuplot[0].stdin.write("set output './%s'\nplot '-' using 1:2:(1) with boxes\n" % outFile)
    gnuplot[0].stdin.write(string.join(workingSetOutfile,  "\n") + "e\n")
    
    gnuplot[4].stdin.write("e\n")
    
    fileList.append("workingSet.png")
    fileList.append("bandwidth.png")
    
    for b in gnuplot:
        b.stdin.write('exit\n')
        b.wait()
        
    # make a big file of them all
    os.system("tar cjf " + outFileBZ2 + " --remove-files " + " ".join(fileList))
    
if __name__ == "__main__":
    main()
