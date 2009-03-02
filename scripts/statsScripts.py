#!/usr/bin/python -O

import re


thumbnailResolution = "640x480"

#terminal = 'set terminal png font "Arial,10" transparent nointerlace truecolor  size 1700, 1024 nocrop enhanced\n'
#extension = 'png'

terminal = 'set terminal svg size 1920,1200 dynamic enhanced fname "Arial" fsize 10\n'
extension = 'svg'

#terminal = 'set terminal postscript eps enhanced color font "Arial, 10"'
#extension = 'eps'

decoder = { "OPBAS":"Open Page Baseline", "SDBAS": "SDRAM Baseline", "CPBAS":"Close Page Baseline", "LOLOC":"Low Locality", "HILOC":"High Locality",
           "GRD":"Greedy", "STR":"Strict", "BRR":"Bank Round Robin", "RRR":"Rank Round Robin", "CLSO":"Close Page Aggressive", "OPEN":"Open Page", "CPAG":"Close Page Aggressive",
           "CLOS":"Close Page", "OPA":"Open Page Aggressive", "CPBOPT":"Close Page Baseline Opt"}

powerRegex = re.compile('(?<={)[\d.]+')
channelRegex = re.compile('(?<=ch\[)[\d]+')
rankRegex = re.compile('(?<=rk\[)[\d]+')
bankRegex = re.compile('(?<=bk\[)[\d]+')
combinedRegex = re.compile('\(([\d.]+),([\d.]+),([\d.]+)\) (\d+)')
NaN = float('nan')

addressDistroString = '<h2>Address Distribution, Channel %d</h2><a rel="lightbox" href="addressDistribution%d.svgz"><img class="fancyzoom" src="addressDistribution%d-thumb.png" alt="" /></a>'

# setup the script headers
powerScripts = ['''
    unset border
    set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5
    set autoscale xfixmin
    set autoscale xfixmax
    set yrange [0:*] noreverse nowriteback
    unset x2tics
    set mxtics
    set xrange [0:*]
    set xlabel "Time (s)"
    set ylabel "Power Dissipated (mW)" offset character .05, 0,0 textcolor lt -1 rotate by 90

    set ytics out
    set multiplot
    set size 1.0, 0.66
    set origin 0.0, 0.34
    set boxwidth 0.95 relative
    set style fill  solid 1.00 noborder
    set style data histograms
    #set style data filledcurves below x1
    set style histogram rowstacked title offset 0,0,0
    ''', '''
    set size 1.0, 0.35
    set origin 0.0, 0.0
    set title "Power Dissipated"
    set boxwidth 0.95 relative
    plot '-' u 1:2 sm csp t "Cumulative Average" w lines lw 2.00, '-' u 1:2 t "Total Power" w boxes, '-' u 1:2 sm csp t "Running Average" w lines lw 2.00
    ''', '''
    unset border
    set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5
    set autoscale xfixmin
    set autoscale xfixmax
    set yrange [0:*] noreverse nowriteback
    set title
    set ytics out
    set xtics out
    set mxtics
    set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5
    unset x2tics
    set output "%s"
    set multiplot
    set size 1.0, 0.5
    set origin 0.0, 0.5
    set ylabel "Energy" offset character .05, 0,0 textcolor lt -1 rotate by 90
    set xlabel "Time (s)"
    ''', '''
    set size 1.0, 0.5
    set origin 0.0, 0.0
    set title
    plot '-' u 1:2 sm csp t "Energy Delay Prod (P t^{2})" w lines lw 2.00, '-' u 1:2 sm csp t "IBM Energy2 (P^{2}t^{3})" w lines lw 2.00
    ''']


basicSetup = '''
    unset border
    set size 1.0, 1.0
    set origin 0.0, 0.0
    set autoscale xfixmax
    set autoscale xfixmin
    set mxtics
    set xtics nomirror out
    set key outside center bottom horizontal reverse Left
    set style fill solid noborder
    #set boxwidth 0.95 relative
    set boxwidth 0.95 absolute
    set ytics out
    '''

    # setup the script headers
statsScripts = [terminal + basicSetup + '''
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
    set boxwidth 0.98 relative
    set style fill solid 1.00 noborder
    set xrange [0 : *] noreverse nowriteback
    set xlabel "Time (s)"
    #set logscale y
    set ylabel "Working Set Size" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set title "Working Set Size vs Time\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "workingSet.''' + extension + '''"
    plot '-' using 1:2 t "Working Set Size" with boxes
    ''', terminal + basicSetup + '''
    set yrange [0 : *] noreverse nowriteback
    set output "bandwidth.''' + extension + '''"
    set xlabel "Time (s)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set xrange [0:*]
    set multiplot
    set size 1.0, 0.66
    set origin 0.0, 0.33
    set style data histograms
    set style histogram rowstacked title offset 0,0,0
    set ylabel "Bandwidth (bytes per second)"
    set title "System Bandwidth\\n%s"  offset character 0, -1, 0 font "" norotate
    plot '-' using 1 axes x2y1 title "Read Bytes", '-' using 1 axes x2y1 title "Write Bytes", '-' using 1:2 axes x1y1 sm csp title "Average Bandwidth" with lines
    ''', terminal + basicSetup + '''
    set yrange [1 : *] noreverse nowriteback
    set logscale y
    set xlabel "PC Value" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Total Latency (cycles)"
    set title "Total Latency vs. PC Value\\n%s"  offset character 0, -1, 0 font "" norotate
    #set style fill solid 1.00 border 0
    set format x "0x1%%x"
    set xrange [*:*] noreverse nowriteback
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
    #set logscale y
    set ylabel "Reuse Rate"
    set title "Reuse Rate of Open Rows vs. Time\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "rowHitRate.''' + extension + '''"
    plot '-' using 1:2 sm csp title "Hit Rate" with filledcurve below x1, '-' using 1:2 sm csp title "Cumulative Average Hit Rate" with lines lw 1.250, '-' using 1:2 sm csp title "Moving Average" with lines lw 1.250
    '''
    ]
averageTransactionLatencyScript = basicSetup + '''
    set size 1.0, 0.5
    set origin 0.0, 0.5
    set yrange [0 : *] noreverse nowriteback
    set xlabel "Time (s)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Latency (ns)"
    set y2label "Access Count"
    set y2tics
    set title "Transaction Latency\\n%s"  offset character 0, -1, 0 font "" norotate
    set style fill solid 1.00 noborder
    plot '-' using 1:2 title "Latency" with impulses, '-' using 1:2 title "Cumulative Average Latency" with lines lw 1.25, '-' using 1:2 title "Moving Average" with lines lw 1.25, '-' using 1:2 t "Accesses" axes x2y2 with lines lw 1.25
    '''
smallIPCGraph = basicSetup + '''
    set size 1.0, 0.35
    set origin 0.0, 0.0
    set ylabel "IPC"
    set title "IPC vs. Time"  offset character 0, -1, 0 font "" norotate
    set style fill solid 1.00 noborder
    plot '-' using 1:2 title "IPC" with impulses, '-' using 1:2 sm csp title "Cumulative Average IPC" with lines, '-' using 1:2 sm csp title "Moving Average IPC" with lines
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
pcVsLatencyGraph0 = terminal + basicSetup + '''
    set logscale y
    set yrange [1 : *] noreverse nowriteback
    set xlabel "PC Value" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Total Latency (ns)"
    set title "Total Latency Due to Reads vs. PC Value\\n%s"  offset character 0, -1, 0 font "" norotate
    #set style fill solid 1.00 border 0
    set style fill solid 1.00 noborder
    #set boxwidth 1.00 absolute
    set format x "0x%%x"
    set output "latencyVsPc.%s"
    set multiplot

    set size 0.5, 1.0
    set origin 0.0, 0.0
    plot '-' using 1:2 t "Total Latency" with boxes
    '''
pcVsLatencyGraph1 = '''
    set size 0.5, 1.0
    set origin 0.5, 0.0
    set format x "0x1%x"
    plot '-' using 1:2 t "Total Latency" with boxes
    '''
transactionGraph = terminal + basicSetup + '''
    set xrange [1 : *] noreverse nowriteback
    set logscale y
    set format x
    set style fill solid 1.00 noborder
    #set boxwidth 0.95 relative
    set xlabel "Execution Time (ns)" offset character .05, 0,0 font "" textcolor lt -1 rotate by 90
    set ylabel "Number of Transactions with this Execution Time"
    set title "Read Transaction Latency\\n%s"  offset character 0, -1, 0 font "" norotate
    set output "transactionLatencyDistribution.%s"
    plot '-' using 1:2 t "Total Latency" with boxes
    '''
addressDistroA = '''
    set output "addressDistribution.%s"
    unset y2tics
    set xtics
    set xlabel 'Time (s)'
    set xrange [0:*]
    set autoscale xfixmax
    set ylabel "Access %%%%"
    set yrange [0 : 1] noreverse nowriteback
    set key outside center bottom horizontal reverse Left
    set style data histograms
    set style histogram rowstacked
    set multiplot
    set size 1.0, 0.333
    set origin 0.0, 0.666
    set title "%%s\\nChannel Distribution Rate" offset character 0, -1, 0 font "" norotate
    ''' % extension
addressDistroB = '''
    set size 1.0, 0.333
    set origin 0.0, 0.333
    set title "Rank Distribution Rate" offset character 0, -1, 0 font "" norotate
    '''
addressDistroC = '''
    set size 1.0, 0.333
    set origin 0.0, 0.0
    set title "Bank Distribution Rate" offset character 0, -1, 0 font "" norotate
    '''

subAddrDistroA = '''
    set output "addressDistribution%d.''' + extension + '''"
    unset y2tics
    unset logscale y
    set format x
    set xtics
    set xlabel 'Time (s)'
    set xrange [0:*]
    set autoscale xfixmax
    set ylabel "Access %%"
    set yrange [0 : 1.0] noreverse nowriteback
    set key outside center bottom horizontal reverse Left
    set style data histograms
    set style histogram rowstacked
    set multiplot
    '''
subAddrDistroB = '''
    set size 1.0, %f
    set origin 0.0, %f
    set title "Rank %d Distribution Rate%s" offset character 0, 0, 0 font "" norotate
    '''

