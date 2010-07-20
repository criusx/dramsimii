#ifndef GLOBALS_HH
#define GLOBALS_HH

#ifdef _MSC_VER

typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef unsigned __int8 uint8_t;
#include <unordered_map>
#include <limits.h>
#define TICK_MAX _I64_MAX
#define TICK_MIN _I64_MIN
#define PHYSICAL_ADDRESS_MAX _UI64_MAX
#include <unordered_map>

#else

#include <tr1/unordered_map>

#endif

#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/filesystem.hpp>

//std::string terminal = "set terminal svg size 1920,1200 dynamic enhanced fname \"Arial\" fsize 16\n";
//std::string terminal = "set terminal svg size 2048,1152 dynamic enhanced font \"Arial\" fsize 18\n";
const std::string terminal = "set terminal svg size 1920,1200 dynamic enhanced font \"Trebuchet_MS\" fsize 14\n";

const std::string thumbnailTerminal = "set terminal png truecolor font \"Trebuchet_MS\" size 1400,900 enhanced \n";

const std::string extension = "svg";

const std::string processedExtension = "svgz";

const std::string thumbnailExtension = "png";

// sudo apt-get install msttcorefonts
// add export GDFONTPATH=/usr/share/fonts/truetype/msttcorefonts to .shrc
const std::string basicSetup = "unset border\n\
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

// 0

const std::string hitMissScript = "unset border\n\
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

const std::string hitMissPowerScript = "unset border\n\
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


//const std::string powerTypes[] = {"ACT-STBY","ACT","PRE-STBY","RD","WR","ACT-PDN","PRE-PDN", "IO","Term", "DIMM Cache"};
const std::string powerTypes[] = {"ACT-STBY","ACT","PRE-STBY","RD","WR","ACT-PDN","PRE-PDN", "DIMM Cache"};

const std::string subAddrDistroA = "unset y2tics\n\
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

const std::string workingSetSetup = "set yrange [0 : *] noreverse nowriteback\n\
									set boxwidth 0.98 relative\n\
									set style fill solid 1.00 noborder\n\
									set xrange [0 : *] noreverse nowriteback\n\
									set xlabel 'Time (s)'\n\
									#set logscale y\n\
									set ylabel 'Working Set Size' offset character .05, 0, 0 font '' textcolor lt -1 rotate by 90\n\
									plot '-' using 1:2 t 'Working Set Size' with boxes\n";


const std::string addressDistroA = "unset y2tics\n\
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

const std::string addressDistroB ="set title 'Rank Distribution Rate' offset character 0, -1, 0 font '' norotate\n";

const std::string addressDistroC = "set title 'Bank Distribution Rate' offset character 0, -1, 0 font '' norotate\n";

const std::string transactionGraphScript = "set logscale y\n\
										   set format x\n\
										   set style fill solid 1.00 noborder\n\
										   #set autoscale xfixmax\n\
										   set xlabel 'Execution Time (ns)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
										   set ylabel 'Number of Transactions with this Execution Time'\n\
										   plot '-' using 1:2 t 'Total Latency' with boxes\n";

const std::string cumulativeTransactionGraphScript = "set yrange [0:1]\n\
													 #set logscale y\n\
													 set format x\n\
													 set style fill solid 1.00 noborder\n\
													 #set autoscale xfixmax\n\
													 set xlabel 'Execution Time (ns)' offset character .05, 0,0 font '' textcolor lt -1 rotate by 90\n\
													 set ylabel 'Percent of Transactions with At Most this Latency'\n\
													 plot '-' using 1:2 t 'Cumulative Latency Distribution' with lines lw 2.00\n";


const std::string bandwidthGraphScript = "set yrange [0 : *] noreverse nowriteback\n\
										 set xlabel 'Time (s)' offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
										 set xrange [0:*]\n\
										 set size 1.0, 0.66\n\
										 set origin 0.0, 0.33\n\
										 set style data histograms\n\
										 set style histogram rowstacked title offset 0,0,0\n\
										 set ylabel 'Bandwidth (MB/s)'\n\
										 plot \
										 '-' using 1 axes x2y1 title 'Read Bytes', \
										 '-' using 1 axes x2y1 title 'Write Bytes', \
										 '-' using 1 axes x2y1 title 'Cache Read Bytes', \
										 '-' using 1 axes x2y1 title 'Cache Write Bytes',\
										 '-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n";
#if 0
										 '-' using 1:2 axes x1y1 title 'Average Bandwidth' with lines\n";
#endif

const std::string pcVsLatencyGraphScript = "set logscale y \n\
										   set yrange [1 : *] noreverse nowriteback\n\
										   set xlabel 'PC Value' offset character .05, 0,0 font 'Arial, 14' textcolor lt -1 rotate by -45\n\
										   set ylabel 'Total Latency (ns)'\n\
										   set style fill solid 1.00 noborder\n\
										   set format x '0x0%x'\n";

const std::string avgPcVsLatencyGraphScript = "set logscale y \n\
											  set yrange [1 : *] noreverse nowriteback\n\
											  set xlabel 'PC Value' offset character .05, 0,0 font 'Arial, 14' textcolor lt -1 rotate by -45\n\
											  set ylabel 'Average Latency (ns)'\n\
											  set style fill solid 1.00 noborder\n\
											  set format x '0x0%x'\n";

const std::string smallIPCGraphScript = "set size 1.0, 0.345\n\
										set origin 0.0, 0.0\n\
										set ylabel 'IPC'\n\
										set y2label\n\
										set title 'IPC vs. Time'  offset character 0, -1, 0 font '' norotate\n\
										set style fill solid 1.00 noborder\n\
										plot '-' using 1:2 title 'IPC' with impulses, '-' using 1:2 title 'Cumulative Average IPC' with lines, '-' using 1:2 title 'Moving Average IPC' with lines\n";

const std::string bigIPCGraphScript = "set origin 0.0, 0.0\n\
									  set ylabel 'IPC'\n\
									  set y2label\n\
									  set title 'IPC vs. Time'  offset character 0, -1, 0 font '' norotate\n\
									  set style fill solid 1.00 noborder\n\
									  set boxwidth 0.95 relative\n\
									  plot '-' using 1:2 title 'IPC' with boxes lt rgb \"#007872\", '-' using 1:2 title 'Cumulative Average IPC' with lines lw 6.00 lt rgb \"#57072B\", '-' u 1:2 axes x1y1 notitle with points pointsize 0.01\n";

const std::string cacheGraph0 = "set y2tics\n\
								set format x\n";

const std::string cacheGraph1 = "unset xlabel\n\
								set ylabel 'Miss Rate'\n\
								set y2label 'Access Count'\n\
								set yrange [0:1] noreverse nowriteback\n\
								set title 'L1 ICache'\n\
								plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

const std::string cacheGraph2 = "set yrange [0 : *] noreverse nowriteback\n\
								unset xlabel\n\
								set ylabel 'Miss Rate'\n\
								set yrange [0:1] noreverse nowriteback\n\
								set y2label 'Access Count'\n\
								set title 'L1 DCache'  offset character 0, -1, 0 font '' norotate\n\
								plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

const std::string cacheGraph3 = "set yrange [0 : *] noreverse nowriteback\n\
								set xlabel 'Time (s)' offset character .05, 0, 0 textcolor lt -1 rotate by 90\n\
								set ylabel 'Miss Rate'\n\
								set y2label 'Access Count'\n\
								set yrange [0:1] noreverse nowriteback\n\
								set title 'L2 Cache' offset character 0, -1, 0 norotate\n\
								plot  '-' using 1:2 title 'Access Count' axes x2y2 with impulses, '-' using 1:2 title 'Miss Rate' with lines lw 1.0\n";

const std::string otherIPCGraphScript = "set yrange [0 : *] noreverse nowriteback\n\
										set xlabel 'Time (s)' offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
										set ylabel 'Instructions Per Cycle'\n\
										set y2label\n\
										unset logscale y2\n\
										set style fill solid 1.00 noborder\n\
										set xrange [0 : *]\n\
										plot '-' using 1:2 title 'IPC' with impulses,\
										'-' using 1:2 title 'Moving Average IPC' with lines\n";

const std::string averageTransactionLatencyScript = "set yrange [1 : *] noreverse nowriteback\n\
													set xlabel 'Time (s)' offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
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


const std::string rowReuseRateGraphScript = "set yrange [0 : *] noreverse nowriteback\n\
											set y2range [1 : *] noreverse nowriteback\n\
											set xlabel 'Time (s)' offset character .05, 0,0 textcolor lt -1 rotate by 90\n\
											set ylabel 'Reuse Rate'\n\
											set y2label 'Accesses'\n\
											set y2tics\n\
											set my2tics\n\
											set logscale y2\n\
											plot '-' using 1:2 axes x1y2 t 'Accesses' with impulses lt rgb \"#28B95A\",\
											'-' using 1:2 axes x1y1 title 'Hit Rate' with lines lw 1.75 lt rgb \"#5B28B8\"\n";




//extern const std::string csvHeader;
extern bool doneEntering;
extern std::vector<std::string> fileList;
extern boost::mutex fileListMutex;
extern boost::filesystem::path executableDirectory;
extern bool userStop;
extern bool cypressResults;
extern bool separateOutputDir;

extern std::tr1::unordered_map<std::string, std::string> decoder;

#endif