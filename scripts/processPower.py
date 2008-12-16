#!/usr/bin/python -O

import gzip
import sys
import os
import string
import subprocess
import re
from subprocess import Popen, PIPE, STDOUT

def main():

    terminal = 'set terminal png font "VeraMono,10" transparent nointerlace truecolor  size 1600, 768 nocrop enhanced'
    os.environ["GDFONTPATH"] = "/usr/share/fonts/truetype/ttf-bitstream-vera"

    # setup the script headers
    scripts = ['''
    set terminal png font "VeraMono,10" transparent nointerlace truecolor  size 1600, 768 nocrop enhanced
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    set key center rmargin Right reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5
    #set xrange [0:*] noreverse nowriteback
    set autoscale xfixmin
    set autoscale xfixmax
    set yrange [0:*] noreverse nowriteback

    set ytics out
    set xtics out
    #set mxtics
    #set logscale y

    set ylabel "Power Dissipated (mW)" offset character .05, 0,0 font "VeraMono,12" textcolor lt -1 rotate by 90
    set xlabel "Epoch" font "VeraMono,12"

    set boxwidth 1.00 relative
    set style fill  solid 1.00 border 0
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
    outFileBZ2 = sys.argv[1].split('.gz')[0] + ".tar.bz2"
    channels = 0
    ranks = 0
    try:
        compressedstream = gzip.open(sys.argv[1], 'rb')
        for line in compressedstream:
            if line[0] == '-':
              # normal lines
                if line[1] == 'P':
                    startVal = line.find("{")
                    if startVal > 0:
                        endVal = line.find("}")
                        values[writing].append(line[startVal + 1:endVal])
                        writing = (writing + 1) % (valuesPerEpoch)
                elif line.startswith('----') and not line.startswith('----epoch'):
					gnuplot[0].stdin.write("set title \"Power Consumed vs. Time\\n^{%s}\"  offset character 0, -1, 0 font \"VeraMono,14\" norotate\n" % line[4:len(line) - 5])

                elif line[1] == '+':
                    gnuplot[0].stdin.write('set output "' + sys.argv[1].split('.gz')[0] + '.png"\n')
                    splitter = re.compile('([\[\]])')
                    splitLine = splitter.split(line)
                    #splitLine = line.split(']').split('[')
                    channels = int(splitLine[2])
                    ranks = int(splitLine[6])

                    gnuplot[0].stdin.write('plot ')
                    valuesPerEpoch = channels * (ranks + 2)
                    for a in range(channels):
                        for b in range(ranks):
                           gnuplot[0].stdin.write('"-" using 1 title "%sch[%d]rk[%d]"' % ("P_{sys}(ACT-STBY)" if b % 2 == 0 else "P_{sys}(ACT)", a, b))
                           gnuplot[0].stdin.write(",")
                        for b in range(2):
                            gnuplot[0].stdin.write('"-" using 1 title "P_{sys}ch[%d](%s)"' % (a, "RD" if b % 2 == 0 else "WR"))
                            if (a < channels - 1) or (b < 1):
                                gnuplot[0].stdin.write(",")
                        if (a == channels - 1):
                            gnuplot[0].stdin.write("\n")
                    # plus the RD and WR values
                    for i in range(valuesPerEpoch):
                        values.append([])
    except IOError, strerror:
        print "I/O error", strerror
    except OSError, strerror:
        print "OS error", strerror

    #print len(values[0])
    #gnuplot[0].stdin.write("\nset xrange [0:%d]" % len(values[0]))

    for u in values:
        gnuplot[0].stdin.write("\n".join(u) + "\ne\n")

    gnuplot[0].stdin.write("unset output\n")

    for b in gnuplot:
        b.stdin.write('exit\n')
        b.wait()

    # make a big file of them all
    #os.system("tar cjf " + outFileBZ2 + " --remove-files " + string.join(fileList, ' '))

if __name__ == "__main__":
    main()
