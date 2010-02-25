
std::string powerScripts[] = {"unset border\n\
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
							  plot '-' u 1:2 t \"Total Power\" w boxes,\
							  '-' u 1:2 t \"Cumulative Average\" w lines lw 2.00,\
							  '-' u 1:2 t \"Running Average\" w lines lw 2.00\n",
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
							  plot '-' u 1:2 t \"Energy Delay Prod (P t^{2})\" w lines lw 2.00,\
							  '-' u 1:2 t \"IBM Energy2 (P^{2}t^{3})\" w lines lw 2.00\n"};

std::string cumulPowerScript = "unset border\n\
							   set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
							   set autoscale xfixmin\n\
							   set autoscale xfixmax\n\
							   set yrange [0:*] noreverse nowriteback\n\
							   unset x2tics\n\
							   set mxtics\n\
							   set xrange [0:*]\n\
							   set xlabel \"Time (s)\"\n\
							   set ylabel \"Energy (mJ)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
							   set ytics out\n\
							   set origin 0.0, 0.0\n\
							   plot '-' u 1:2 t \"Cumulative Energy\" w lines lw 2.00,\
							   '-' u 1:2 t \"Reduced Cumulative Energy\" w lines lw 2.00\n";

std::string hitMissScript = "unset border\n\
							set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
							set yrange [0:*] noreverse nowriteback\n\
							set y2range [0:*] noreverse nowriteback\n\
							set xrange [0:*]\n\
							set autoscale xfixmin\n\
							set autoscale xfixmax\n\
							set ytics out\n\
							set y2tics out\n\
							set xtics out\n\
							set mxtics\n\
							unset x2tics\n\
							set style fill solid 1.00 noborder\n\
							set ylabel \"Hit Rate\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
							set y2label \"Access Count\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
							set xlabel \"Time (s)\"\n\
							plot '-' u 1:2 t \"Access Count\" axes x1y2 with impulses lt rgb \"#dabCbC\",\
							'-' u 1:2 t \"Hit Rate\" axes x1y1 w lines lw 2.00\n";

std::string hitMissPowerScript = "unset border\n\
								 set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
								 set yrange [0:*] noreverse nowriteback\n\
								 set xrange [0:*]\n\
								 set autoscale xfixmin\n\
								 set autoscale xfixmax\n\
								 set ytics out\n\
								 set xtics out\n\
								 set mxtics\n\
								 unset x2tics\n\
								 set style fill solid 1.00 noborder\n\
								 set ylabel \"Hit Rate\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
								 set xlabel \"Time (s)\"\n\
								 plot '-' u 1:2 t \"SRAM Power\" axes x1y2 with lines lt rgb \"#5a3C3C\",\
								 '-' u 1:2 t \"DRAM Power\" axes x1y1 w lines lw 2.00\n";


std::string bigPowerScript = "set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
							 set yrange [0:*] noreverse nowriteback\n\
							 unset x2tics\n\
							 set mxtics\n\
							 set xrange [0:*]\n\
							 set yrange [0:*]\n\
							 set xlabel \"Time (s)\"\n\
							 set ylabel \"Power Dissipated (mW)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
							 set ytics out\n\
							 set boxwidth 1.00 absolute\n\
							 set style fill  solid 1.00 noborder\n\
							 set style data histograms\n\
							 #set style data filledcurves below x1\n\
							 set style histogram rowstacked title offset 0,0,0\n";

std::string bigEnergyScript = "set key outside center bottom horizontal Left reverse invert enhanced samplen 4 autotitles columnhead box linetype -2 linewidth 0.5\n\
							  set autoscale xfixmin\n\
							  set autoscale xfixmax\n\
							  set yrange [0:*] noreverse nowriteback\n\
							  set ytics out\n\
							  set xtics out\n\
							  set mxtics\n\
							  set yrange [0:*]\n\
							  set y2range [0:*]\n\
							  set ytics nomirror\n\
							  set y2tics\n\
							  unset x2tics\n\
							  set boxwidth 1.00 relative\n\
							  #set logscale y2\n\
							  set ylabel \"Energy (mJ)\" offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
							  set y2label \"Cumulative Energy (mJ)\"\n\
							  set xlabel \"Time (s)\"\n";

std::string powerTypes[] = {"ACT-STBY","ACT","PRE-STBY","RD","WR"};

std::string subAddrDistroA = "unset y2tics\n\
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
							 set style histogram rowstacked\n";

std::string workingSetSetup = "set yrange [0 : *] noreverse nowriteback\n\
							  set boxwidth 0.98 relative\n\
							  set style fill solid 1.00 noborder\n\
							  set xrange [0 : *] noreverse nowriteback\n\
							  set xlabel 'Time (s)'\n\
							  #set logscale y\n\
							  set ylabel 'Working Set Size' offset character .05, 0, 0 font '' textcolor lt -1 rotate by 90\n\
							  plot '-' using 1:2 t 'Working Set Size' with boxes\n";

std::string basicSetup = "unset border\n\
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

std::string addressDistroA = "unset y2tics\n\
							 set xtics\n\
							 set xlabel 'Time (s)'\n\
							 set xrange [0:*]\n\
							 set autoscale xfixmax\n\
							 set ylabel 'Access %'\n\
							 set yrange [0 : 1] noreverse nowriteback\n\
							 set key outside center bottom horizontal reverse Left\n\
							 set style data histograms\n\
							 set style histogram rowstacked\n\
							 set multiplot layout 3,1\n";

std::string addressDistroB ="set title 'Rank Distribution Rate' offset character 0, -1, 0 font '' norotate\n";

std::string addressDistroC = "set title 'Bank Distribution Rate' offset character 0, -1, 0 font '' norotate\n";

std::string transactionGraph = "set logscale y\n\
							   set format x\n\
							   set style fill solid 1.00 noborder\n\
							   #set autoscale xfixmax\n\
							   set xlabel 'Execution Time (ns)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
							   set ylabel 'Number of Transactions with this Execution Time'\n\
							   plot '-' using 1:2 t 'Total Latency' with boxes\n";

std::string bandwidthGraph = "set yrange [0 : *] noreverse nowriteback\n\
							 set xlabel 'Time (s)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
							 set xrange [0:*]\n\
							 set title 'System Bandwidth'\n\
							 set size 1.0, 0.66\n\
							 set origin 0.0, 0.33\n\
							 set style data histograms\n\
							 set style histogram rowstacked title offset 0,0,0\n\
							 set ylabel 'Bandwidth (bytes per second)'\n\
							 plot \
							 '-' using 1 axes x2y1 title 'Read Bytes', \
							 '-' using 1 axes x2y1 title 'Write Bytes', \
							 '-' using 1 axes x2y1 title 'Cache Read Bytes', \
							 '-' using 1 axes x2y1 title 'Cache Write Bytes', \
							 '-' using 1:2 axes x1y1 title 'Average Bandwidth' with lines\n";

std::string pcVsLatencyGraph = "set logscale y \n\
							   set yrange [1 : *] noreverse nowriteback\n\
							   set xlabel 'PC Value' offset character .05, 0,0 font 'Arial, 14' textcolor lt -1 rotate by -45\n\
							   set ylabel 'Total Latency (ns)'\n\
							   set style fill solid 1.00 noborder\n\
							   set format x '0x0%x'\n";

std::string avgPcVsLatencyGraph = "set logscale y \n\
								  set yrange [1 : *] noreverse nowriteback\n\
								  set xlabel 'PC Value' offset character .05, 0,0 font 'Arial, 14' textcolor lt -1 rotate by -45\n\
								  set ylabel 'Average Latency (ns)'\n\
								  set style fill solid 1.00 noborder\n\
								  set format x '0x0%x'\n";

std::string smallIPCGraph = "set size 1.0, 0.345\n\
							set origin 0.0, 0.0\n\
							set ylabel 'IPC'\n\
							set y2label\n\
							set title 'IPC vs. Time'  offset character 0, -1, 0 font '' norotate\n\
							set style fill solid 1.00 noborder\n\
							plot '-' using 1:2 title 'IPC' with impulses, '-' using 1:2 title 'Cumulative Average IPC' with lines, '-' using 1:2 title 'Moving Average IPC' with lines\n";

std::string bigIPCGraph = "set origin 0.0, 0.0\n\
						  set ylabel 'IPC'\n\
						  set y2label\n\
						  set title 'IPC vs. Time'  offset character 0, -1, 0 font '' norotate\n\
						  set style fill solid 1.00 noborder\n\
						  set boxwidth 0.95 relative\n\
						  plot '-' using 1:2 title 'IPC' with boxes lt rgb \"#007872\", '-' using 1:2 title 'Cumulative Average IPC' with lines lw 6.00 lt rgb \"#57072B\", '-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n";

std::string cacheGraph0 = "set y2tics\n\
						  set format x\n";

std::string cacheGraph1 = "unset xlabel\n\
						  set ylabel 'Miss Rate'\n\
						  set y2label 'Access Count'\n\
						  set yrange [0:1] noreverse nowriteback\n\
						  set title 'L1 ICache'\n\
						  plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

std::string cacheGraph2 = "set yrange [0 : *] noreverse nowriteback\n\
						  unset xlabel\n\
						  set ylabel 'Miss Rate'\n\
						  set yrange [0:1] noreverse nowriteback\n\
						  set y2label 'Access Count'\n\
						  set title 'L1 DCache'  offset character 0, -1, 0 font '' norotate\n\
						  plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

std::string cacheGraph3 = "set yrange [0 : *] noreverse nowriteback\n\
						  set xlabel 'Time (s)' offset character .05, 0, 0 font \"\" textcolor lt -1 rotate by 90\n\
						  set ylabel 'Miss Rate'\n\
						  set y2label 'Access Count'\n\
						  set yrange [0:1] noreverse nowriteback\n\
						  set title 'L2 Cache' offset character 0, -1, 0 font '' norotate\n\
						  plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

std::string otherIPCGraph = "set yrange [0 : *] noreverse nowriteback\n\
							set xlabel 'Time (s)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
							set ylabel 'Instructions Per Cycle'\n\
							set y2label\n\
							unset logscale y2\n\
							set style fill solid 1.00 noborder\n\
							set xrange [0 : *]\n\
							plot '-' using 1:2 title 'IPC' with impulses,\
							'-' using 1:2 title 'Moving Average IPC' with lines\n";

std::string averageTransactionLatencyScript = "set yrange [1 : *] noreverse nowriteback\n\
											  set xlabel 'Time (s)' offset character .05, 0,0 font ' textcolor lt -1 rotate by 90\n\
											  set ylabel 'Latency (ns)'\n\
											  #set logscale y\n\
											  set logscale y2\n\
											  set y2label 'Access Count'\n\
											  set y2tics\n\
											  set style fill solid 1.00 noborder\n\
											  plot \
											  '-' using 1:2 t 'Accesses' axes x1y2 with impulses,\
											  '-' using 1:2 title 'Minimum Latency' with lines lw 1.25, \
											  '-' using 1:2 title 'Average Latency' with lines lw 1.25, \
											  '-' using 1:2 title \"Average + 2 std. dev.\" with lines lw 1.25\n";


std::string rowHitMissGraph = "set yrange [0 : *] noreverse nowriteback\n\
							  set y2range [1 : *] noreverse nowriteback\n\
							  set xlabel 'Time (s)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
							  set ylabel 'Reuse Rate'\n\
							  set y2label 'Accesses'\n\
							  set y2tics\n\
							  set my2tics\n\
							  set logscale y2\n\
							  plot '-' using 1:2 axes x1y1 title 'Hit Rate' with filledcurve below x1 lt rgb \"#28B95A\", '-' using 1:2 axes x1y1 title 'Cumulative Average Hit Rate' with lines lw 1.250 lt rgb \"#B8283E\", '-' using 1:2 axes x1y2 t 'Accesses' with lines lw 1.250 lt rgb \"#5B28B8\"\n";

const std::string urlString("<a href=\"%1/index.html\">%2</a>");

const std::string csvHeader("Benchmark,Channels,Ranks,Banks,Rows,Columns,DRAM Width,tRAS,tCAS,tRCD,tRC,Address Mapping Policy,Command Ordering Algorithm, Row Buffer Management Policy,Datarate,Per Bank Queue Depth,tFAW,Cache Size,Block Size,Associativity,Number of Sets,Runtime,Read Hit Rate,Hit Rate,Average Latency,Average Reduced Latency\n");
