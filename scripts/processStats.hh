
std::string powerScripts[] = 
{"unset border\n\
 set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
 set autoscale xfixmin\n\
 set autoscale xfixmax\n\
 set yrange [0:*] noreverse nowriteback\n\
 unset x2tics\n\
 set mxtics\n\
 set xrange [0:*]\n\
 set xlabel \"Time (s)\"\n\
 set ylabel \"Power Dissipated (mW)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
 set ytics out\n\
 set multiplot\n\
 set size 1.0, 0.66\n\
 set origin 0.0, 0.34\n\
 set boxwidth 0.95 relative\n\
 set style fill  solid 1.00 noborder\n\
 set style data histograms\n\
 #set style data filledcurves below x1\n\
 set style histogram rowstacked title offset 0,0,0\n",
 "set size 1.0, 0.35\n\
 set origin 0.0, 0.0\n\
 set title \"Power Dissipated\"\n\
 set boxwidth 0.95 relative\n\
 plot '-' u 1:2 sm csp t \"Cumulative Average\" w lines lw 2.00, '-' u 1:2 t \"Total Power\" w boxes, '-' u 1:2 sm csp t \"Running Average\" w lines lw 2.00\n",
 "unset border\n\
 set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
 set autoscale xfixmin\n\
 set autoscale xfixmax\n\
 set yrange [0:*] noreverse nowriteback\n\
 set title\n\
 set ytics out\n\
 set xtics out\n\
 set mxtics\n\
 set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
 unset x2tics\n\
 set multiplot\n\
 set size 1.0, 0.5\n\
 set origin 0.0, 0.5\n\
 set ylabel \"Energy\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
 set xlabel \"Time (s)\"\n",
 "set size 1.0, 0.5\n\
 set origin 0.0, 0.0\n\
 set title\n\
 plot '-' u 1:2 sm csp t \"Energy Delay Prod (P t^{2})\" w lines lw 2.00, '-' u 1:2 sm csp t \"IBM Energy2 (P^{2}t^{3})\" w lines lw 2.00\n"};

std::string powerTypes[] = 
{"ACT-STBY","ACT","PRE-STBY","RD","WR"};


std::string subAddrDistroA = 
"unset y2tics\n\
unset logscale y\n\
set format x\n\
set xtics\n\
set xlabel 'Time (s)'\n\
set xrange [0:*]\n\
set autoscale xfixmax\n\
set ylabel \"Access %\"\n\
set yrange [0 : 1.0] noreverse nowriteback\n\
set key outside center bottom horizontal reverse Left\n\
set style data histograms\n\
set style histogram rowstacked\n\
set multiplot\n";

std::string workingSetSetup = 
"set yrange [0 : *] noreverse nowriteback\n\
set boxwidth 0.98 relative\n\
set style fill solid 1.00 noborder\n\
set xrange [0 : *] noreverse nowriteback\n\
set xlabel 'Time (s)'\n\
#set logscale y\n\
set ylabel 'Working Set Size' offset character .05, 0, 0 font '' textcolor lt -1 rotate by 90\n\
plot '-' using 1:2 t 'Working Set Size' with boxes\n";

std::string basicSetup = 
"unset border\n\
set size 1.0, 1.0\n\
set origin 0.0, 0.0\n\
set autoscale xfixmax\n\
set autoscale xfixmin\n\
set mxtics\n\
set xtics nomirror out\n\
set key outside center bottom horizontal reverse Left\n\
set style fill solid noborder\n\
#set boxwidth 0.95 relative\n\
set boxwidth 0.95 absolute\n\
set ytics out\n";

std::string addressDistroA = 
"unset y2tics\n\
set xtics\n\
set xlabel 'Time (s)'\n\
set xrange [0:*]\n\
set autoscale xfixmax\n\
set ylabel 'Access %'\n\
set yrange [0 : 1] noreverse nowriteback\n\
set key outside center bottom horizontal reverse Left\n\
set style data histograms\n\
set style histogram rowstacked\n\
set multiplot\n\
set size 1.0, 0.333\n\
set origin 0.0, 0.666\n";

std::string addressDistroB =
"set size 1.0, 0.333\n\
set origin 0.0, 0.333\n\
set title 'Rank Distribution Rate' offset character 0, -1, 0 font '' norotate\n";

std::string addressDistroC = 
"set size 1.0, 0.333\n\
set origin 0.0, 0.0\n\
set title 'Bank Distribution Rate' offset character 0, -1, 0 font '' norotate\n";



std::string transactionGraph = 
"set xrange [1 : *] noreverse nowriteback\n\
set logscale y\n\
set format x\n\
set style fill solid 1.00 noborder\n\
#set boxwidth 0.95 relative\n\
set xlabel 'Execution Time (ns)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
set ylabel 'Number of Transactions with this Execution Time'\n\
plot '-' using 1:2 t 'Total Latency' with boxes\n";

std::string bandwidthGraph = 
"set yrange [0 : *] noreverse nowriteback\n\
set xlabel 'Time (s)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
set xrange [0:*]\n\
set multiplot\n\
set size 1.0, 0.66\n\
set origin 0.0, 0.33\n\
set style data histograms\n\
set style histogram rowstacked title offset 0,0,0\n\
set ylabel 'Bandwidth (bytes per second)'\n\
plot '-' using 1 axes x2y1 title 'Read Bytes', '-' using 1 axes x2y1 title 'Write Bytes', '-' using 1:2 axes x1y1 sm csp title 'Average Bandwidth' with lines\n";

std::string pcVsLatencyGraph0 =
"set logscale y \n\
set yrange [1 : *] noreverse nowriteback\n\
set xlabel 'PC Value' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
set ylabel 'Total Latency (ns)'\n\
set style fill solid 1.00 noborder\n\
set format x '0x0%x'\n\
set multiplot\n\
set size 0.5, 1.0\n\
set origin 0.0, 0.0\n\
plot '-' using 1:2 t 'Total Latency' with boxes\n";

std::string pcVsLatencyGraph1 =
"set size 0.5, 1.0\n\
set origin 0.5, 0.0\n\
set format x '0x1%x'\n\
plot '-' using 1:2 t 'Total Latency' with boxes\n";

std::string smallIPCGraph = 
"set size 1.0, 0.35\n\
set origin 0.0, 0.0\n\
set ylabel 'IPC'\n\
set title 'IPC vs. Time'  offset character 0, -1, 0 font '' norotate\n\
set style fill solid 1.00 noborder\n\
plot '-' using 1:2 title 'IPC' with impulses, '-' using 1:2 sm csp title 'Cumulative Average IPC' with lines, '-' using 1:2 sm csp title 'Moving Average IPC' with lines\n";

std::string cacheGraphA = 
"set y2tics\n\
set format x\n\
set yrange [0 : 1] noreverse nowriteback\n\
set multiplot\n\
set size 1.0, 0.333\n\
set origin 0.0, 0.666\n\
unset xlabel\n\
set ylabel 'Miss Rate'\n\
set y2label 'Access Count'\n\
plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

std::string cacheGraphB =
"set size 1.0, 0.333\n\
set origin 0.0, 0.333\n\
set yrange [0 : *] noreverse nowriteback\n\
unset xlabel\n\
set ylabel 'Miss Rate'\n\
set y2label 'Access Count'\n\
set title 'Miss Rate of L1 DCache'  offset character 0, -1, 0 font '' norotate\n\
plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

std::string cacheGraphC = 
"set size 1.0, 0.333\n\
set origin 0.0, 0.0\n\
set yrange [0 : *] noreverse nowriteback\n\
set xlabel 'Time (s)' offset character .05, 0,0 font "" lt -1 rotate by 90\n\
set ylabel 'Miss Rate'\n\
set y2label 'Access Count'\n\
set title 'Miss Rate of L2 Cache' offset character 0, -1, 0 font '' norotate\n\
plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

std::string otherIPCGraph = 
"set multiplot\n\
set size 1.0, 0.5\n\
set origin 0.0, 0.0\n\
set yrange [0 : *] noreverse nowriteback\n\
set xlabel 'Time (s)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
set ylabel 'Instructions Per Cycle'\n\
set style fill solid 1.00 noborder\n\
set xrange [0 : *]\n\
plot '-' using 1:2 title 'IPC' with impulses, '-' using 1:2 sm csp title 'Cumulative Average IPC' with lines, '-' using 1:2 sm csp title 'Moving Average IPC' with lines\n";

std::string averageTransactionLatencyScript =
"set size 1.0, 0.5\n\
set origin 0.0, 0.5\n\
set yrange [0 : *] noreverse nowriteback\n\
set xlabel 'Time (s)' offset character .05, 0,0 font ' textcolor lt -1 rotate by 90\n\
set ylabel 'Latency (ns)'\n\
set y2label 'Access Count'\n\
set y2tics\n\
set style fill solid 1.00 noborder\n\
plot '-' using 1:2 title 'Latency' with impulses, '-' using 1:2 title 'Cumulative Average Latency' with lines lw 1.25, '-' using 1:2 title 'Moving Average' with lines lw 1.25, '-' using 1:2 t 'Accesses' axes x2y2 with lines lw 1.25\n";

std::string rowHitMissGraph = 
"set yrange [0 : *] noreverse nowriteback\n\
set xlabel 'Time (s)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
set ylabel 'Reuse Rate'\n\
plot '-' using 1:2 sm csp title 'Hit Rate' with filledcurve below x1, '-' using 1:2 sm csp title 'Cumulative Average Hit Rate' with lines lw 1.250, '-' using 1:2 sm csp title 'Moving Average' with lines lw 1.250\n";


