#!/usr/bin/python -OO

from subprocess import Popen, PIPE, STDOUT
import sys, traceback
import os
from threading import Thread
import time
import gzip
from Queue import Queue
import re
import argparse


DEBUG = False
DEBUGOUTPUT = False

archDir = ""
profileDir = "/2T/profile3"
setupValue = ""
minimumRuntime = 300

class bcolors:
	HEADER = '\033[95m'
	OKBLUE = '\033[94m'
	OKGREEN = '\033[92m'
	WARNING = '\033[93m'
	FAIL = '\033[91m'
	ENDC = '\033[0m'
	
	def disable(self):
		self.HEADER = ''
		self.OKBLUE = ''
		self.OKGREEN = ''
		self.WARNING = ''
		self.FAIL = ''
		self.ENDC = ''



class Benchmark():
	
	def __init__(self, name, cwd, executable, numberProcesses, pinProcesses, selfStart=False):
		self.name = name
		self.executable = executable
		self.cwd = cwd
		self.numberProcesses = numberProcesses
		self.pinProcesses = pinProcesses
		self.selfStart = selfStart
		
	def run(self):
		
		if self.cwd is not None:
			os.chdir(self.cwd)
			
		runningProcesses = []
		value = 0
		
		endTime = startTime = time.time()
				
		if not DEBUG:
			self.setup()
		
		print bcolors.WARNING
		print "Running %s" % self.name,
				
		while True:
			cpuNumber = 0
			for a in range(self.numberProcesses):
				redirect = "" if DEBUGOUTPUT else ">>1%s.out.%d 2>>%s.err.%d" % (self.name, value, self.name, value)
					
				if self.pinProcesses:
					executable = "taskset -c %d %s %s" % (cpuNumber, self.executable, redirect)
					cpuNumber += 1
				else:
					executable = "%s %s" % (self.executable, redirect)
									
				value += 1 
				print executable
				runningProcesses.append(Popen([executable], shell=True))
				#print self.executable
				
			for a in runningProcesses:				
				a.wait()
				
			runningProcesses = []
			
			if time.time() - startTime > minimumRuntime or DEBUG:
				break
			#else:
				#print "ran for %d" % (endTime - startTime)
			
		print ", took %d" % (time.time() - startTime)
		print bcolors.ENDC
		
		if not DEBUG:	
			self.shutdown()
		
	def setup(self):
		#print "dir %s, %s."% (profileDir, archDir)		
		path = os.path.join(os.path.join(profileDir, archDir), self.name)
		#print "sudo opcontrol --setup %s --session-dir=%s" % (setupValue, path)
		os.system("sudo opcontrol --setup %s --session-dir=%s" % (setupValue, path))
				
		#print self
		#print self.selfStart
		if not self.selfStart:
			#print "sudo opcontrol --start"
			os.system("sudo opcontrol --start")
		
	def shutdown(self):
		
		os.system("sudo opcontrol --dump")
		#print "sudo opcontrol --dump"
		os.system("sudo opcontrol --shutdown")
		#print "sudo opcontrol --shutdown"
		
			

runTypes = {
'i0' : '-e LLC_MISSES:10000 -e LLC_REFS:10000 --no-vmlinux -e MEM_LOAD_RETIRED:100000:0x04:0:1 -e CPU_CLK_UNHALTED:500000',
'i1' : '-e L2_RQSTS:50000:0xAA -e L2_RQSTS:50000:0xFF --no-vmlinux -e INST_RETIRED:500000:0x03:0:1 -e CPU_CLK_UNHALTED:100000',
'i2' : '-e L1D_ALL_REF:100000:0x03 -e L1D:100000:0x01 -e L1I:100000:0x01 -e L1I:100000:0x02 --no-vmlinux',
'i3' : '-e MEM_LOAD_RETIRED:500000:0x10 -e CPU_CLK_UNHALTED:500000 -e MEM_INST_RETIRED:500000:0x03 -e UOPS_RETIRED:500000:0x01 --no-vmlinux',
'i4' : '--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e INST_RETIRED:500000:0x01 -e MEM_INST_RETIRED:500000:0x01 -e MEM_INST_RETIRED:500000:0x02',
'i5' : '--no-vmlinux -e DTLB_MISSES:50000:0x01 -e DTLB_MISSES:50000:0x02 -e DTLB_MISSES:50000:0x10 -e DTLB_LOAD_MISSES:50000:0x01',
'i6' : '--no-vmlinux -e L2_HW_PREFETCH:50000:0x01 -e L2_HW_PREFETCH:50000:0x04 -e MEM_LOAD_RETIRED:50000:0x10 -e MEM_UNCORE_RETIRED:50000:0x30',
'i7' : '-e LLC_MISSES:10000 -e LLC_REFS:10000 --no-vmlinux -e INST_RETIRED:100000:0x01:0:1 -e CPU_CLK_UNHALTED:100000',
'a0' : '-e L3_EVICTIONS:50000:0x0F --no-vmlinux -e READ_REQUEST_L3_CACHE:50000:0xF7 -e RETIRED_INSTRUCTIONS:500000 -e L3_CACHE_MISSES:50000:0xF7',
'a1' : '-e RETIRED_INSTRUCTIONS:500000 -e REQUESTS_TO_L2:50000:0x3F --no-vmlinux -e L2_CACHE_MISS:50000:0x0F -e L2_CACHE_FILL_WRITEBACK:50000:0x03',
'a2' : '-e DATA_CACHE_ACCESSES:500000 -e DATA_CACHE_REFILLS_FROM_L2_OR_NORTHBRIDGE:50000:0x1E --no-vmlinux -e DATA_CACHE_REFILLS_FROM_NORTHBRIDGE:50000:0x1F -e RETIRED_INSTRUCTIONS:100000',
'a3' : '--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e OCTWORD_WRITE_TRANSFERS:500000:0x01 -e NORTHBRIDGE_READ_RESPONSES:500000:0x07 -e DRAM_ACCESSES:500000:0x3F',
'a4' : '--no-vmlinux -e RETIRED_INSTRUCTIONS:500000 -e DATA_CACHE_ACCESSES:50000 -e L1_DTLB_MISS_AND_L2_DTLB_HIT:50000:0x07 -e L1_DTLB_AND_L2_DTLB_MISS:50000:0x07',
'a5' : '--no-vmlinux -e RETIRED_INSTRUCTIONS:500000 -e INSTRUCTION_CACHE_FETCHES:500000 -e L1_ITLB_MISS_AND_L2_ITLB_HIT:50000 -e L1_ITLB_MISS_AND_L2_ITLB_MISS:50000:0x03',
'a6' : '--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e OCTWORD_WRITE_TRANSFERS:500000:0x01 -e NORTHBRIDGE_READ_RESPONSES:500000:0x07 -e DRAM_ACCESSES:500000:0x38',
'a7' : '--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e RETIRED_INSTRUCTIONS:500000',
'a8' : '--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e NORTHBRIDGE_READ_RESPONSES:500000:0x1F -e DRAM_ACCESSES:500000:0x07 -e DRAM_ACCESSES:500000:0x38"',
'a9' : '--no-vmlinux -e MEMORY_CONTROLLER_REQUESTS:500000:0x01 -e MEMORY_CONTROLLER_REQUESTS:500000:0x02 -e MEMORY_CONTROLLER_REQUESTS:500000:0x04 -e MEMORY_CONTROLLER_REQUESTS:500000:0x10',
'a10' : '--no-vmlinux -e DATA_PREFETCHES:50000:0x01 -e DATA_PREFETCHES:50000:0x02 -e INEFFECTIVE_SW_PREFETCHES:50000:0x01 -e INEFFECTIVE_SW_PREFETCHES:50000:0x08'
}


specDir = "/2T/benchmarks/spec06/benchspec/CPU2006"
parsecDir = "/2T/benchmarks/parsec-2.1"
mineBenchDir = "/2T/benchmarks/NU-MineBench-3.0"


benchmarks = {
"bzip2" : Benchmark("401.bzip2", None, "%s/401.bzip2/exe/bzip2_base.amd64-m64-gcc42-nn %s/401.bzip2/data/ref/input/input.source 1024" % (specDir, specDir), 4, True),
"gcc" : Benchmark("403.gcc", None, "%s/403.gcc/exe/gcc_base.amd64-m64-gcc42-nn %s/403.gcc/data/ref/input/166.i -o 166.s" % (specDir, specDir), 4, True),
"bwaves" : Benchmark("410.bwaves", "%s/410.bwaves/data/ref/input" % specDir, "../../../exe/bwaves_base.amd64-m64-gcc42-nn", 4, True),
"milc" : Benchmark("433.milc", None, "cat %s/433.milc/data/ref/input/su3imp.in | %s/433.milc/exe/milc_base.amd64-m64-gcc42-nn" % (specDir, specDir), 4, True),
"mcf" : Benchmark("429.mcf", None, "%s/429.mcf/exe/mcf_base.amd64-m64-gcc42-nn %s/429.mcf/data/ref/input/inp.in" % (specDir, specDir), 4, True),
"zeusmp" : Benchmark("434.zeusmp", "%s/434.zeusmp/data/ref/input" % specDir, "../../../exe/zeusmp_base.amd64-m64-gcc42-nn", 4, True),
"gromacs" : Benchmark("435.gromacs", "%s/435.gromacs/data/ref/input" % specDir, "%s/435.gromacs/exe/gromacs_base.amd64-m64-gcc42-nn -silent -deffnm gromacs -nice 0" % specDir, 4, True),
"namd" : Benchmark("444.namd", "%s/444.namd/data/all/input" % specDir, "../../../exe/namd_base.amd64-m64-gcc42-nn --input namd.input --iterations 38 --output namd.out", 4, True),
"dealII" : Benchmark("447.dealII", None, "%s/447.dealII/exe/dealII_base.amd64-m64-gcc42-nn 23" % specDir, 4, True),
"soplex" : Benchmark("450.soplex", None, "%s/450.soplex/exe/soplex_base.amd64-m64-gcc42-nn %s/450.soplex/data/ref/input/ref.mps -m3500" % (specDir, specDir), 4, True),
"povray" : Benchmark("453.povray", "%s/453.povray/data/ref/input" % specDir, "%s/453.povray/exe/povray_base.amd64-m64-gcc42-nn %s/453.povray/data/ref/input/SPEC-benchmark-ref.ini" % (specDir, specDir), 4, True),
"sjeng" : Benchmark("458.sjeng", None, "%s/458.sjeng/exe/sjeng_base.amd64-m64-gcc42-nn %s/458.sjeng/data/ref/input/ref.txt" % (specDir, specDir), 4, True),
"hmmer" : Benchmark("456.hmmer", None, "%s/456.hmmer/exe/hmmer_base.amd64-m64-gcc42-nn --fixed 0 --mean 500 --num 500000 --sd 350 --seed 0 %s/456.hmmer/data/ref/input/retro.hmm" % (specDir, specDir), 4, True),
"libquantum" : Benchmark("462.libquantum", None, "%s/462.libquantum/exe/libquantum_base.amd64-m64-gcc42-nn 1397 8" % specDir, 4, True),
"h264ref" : Benchmark("464.h264ref", "%s/464.h264ref/data/ref/input" % specDir, "%s/464.h264ref/exe/h264ref_base.amd64-m64-gcc42-nn -d %s/464.h264ref/data/ref/input/sss_encoder_main.cfg" % (specDir, specDir), 4, True),
"lbm" : Benchmark("470.lbm", None, "%s/470.lbm/exe/lbm_base.amd64-m64-gcc42-nn 3000 reference.dat 0 0 %s/470.lbm/data/ref/input/100_100_130_ldc.of" % (specDir, specDir), 4, True),
"omnetpp" : Benchmark("471.omnetpp", "%s/471.omnetpp/data/ref/input" % specDir, "../../../exe/omnetpp_base.amd64-m64-gcc42-nn omnetpp.ini", 4, True),
"astar" : Benchmark("473.astar", "%s/473.astar/data/ref/input" % specDir, "../../../exe/astar_base.amd64-m64-gcc42-nn rivers.cfg", 4, True),
"xalancbmk" : Benchmark("483.xalancbmk", None, "%s/483.xalancbmk/exe/Xalan_base.amd64-m64-gcc42-nn -v %s/483.xalancbmk/data/ref/input/t5.xml %s/483.xalancbmk/data/ref/xalanc.xsl" % (specDir, specDir, specDir), 4, True),
"gamess" : Benchmark("416.gamess", "%s/416.gamess/data/ref/input" % specDir, "cat %s/416.gamess/data/ref/input/triazolium.config|%s/416.gamess/exe/gamess_base.amd64-m64-gcc42-nn" % (specDir, specDir), 4, True),
"cactusADM" : Benchmark("436.cactusADM", None, "%s/436.cactusADM/exe/cactusADM_base.amd64-m64-gcc42-nn %s/436.cactusADM/data/ref/input/benchADM.par" % (specDir, specDir), 4, True),
"leslie3d" : Benchmark("437.leslie3d", None, "cat %s/437.leslie3d/data/ref/input/leslie3d.in|%s/437.leslie3d/exe/leslie3d_base.amd64-m64-gcc42-nn" % (specDir, specDir), 4, True),
"calculix" : Benchmark("454.calculix", "%s/454.calculix/data/ref/input" % specDir, "../../../exe/calculix_base.amd64-m64-gcc42-nn -i hyperviscoplastic", 4, True),
"GemsFDTD" : Benchmark("459.GemsFDTD", "%s/459.GemsFDTD/data/ref/input" % specDir, "../../../exe/GemsFDTD_base.amd64-m64-gcc42-nn", 4, True),
"tonto" : Benchmark("465.tonto", "%s/465.tonto/data/ref/input" % specDir, "../../../exe/tonto_base.amd64-m64-gcc42-nn" , 4, True),
"sphinx3" : Benchmark("482.sphinx3", "%s/482.sphinx3/run/run_base_ref_amd64-m64-gcc42-nn.0000" % specDir, "../../exe/sphinx_livepretend_base.amd64-m64-gcc42-nn ctlfile . args.an4", 4, True),
"ECLAT" : Benchmark("eclat", None, "/home/joe/benchmarks/NU-MineBench-3.0/src/ECLAT/eclat -i %s/datasets/ECLAT/ntrans_2000.tlen_20.nitems_1.npats_2000.patlen_6 -e 30 -s 0.0075" % mineBenchDir, 4, True),
"APRIORI" : Benchmark("apriori", None, "%s/src/APR/no_output_apriori -i %s/datasets/APR/data.ntrans_10000.tlen_20.nitems_1.npats_2000.patlen_6 -f %s/datasets/APR/offset_file_10000_P4.txt -s 0.0075 -n 4" % (mineBenchDir, mineBenchDir, mineBenchDir), 1, False),
"utility_mine" : Benchmark("utility_mine", None, "%s/src/utility_mine/tran_utility/utility_mine %s/datasets/utility_mine/RealData/real_data_aa_binary  %s/datasets/utility_mine/RealData/product_price_binary 0.01" % (mineBenchDir, mineBenchDir, mineBenchDir), 4, True),
"gups" : Benchmark("gups", None, "/2T/benchmarks/gups/gups.x86 1024", 4, True),
"gups.parallel" : Benchmark("gups.parallel", None, "/2T/benchmarks/gups/gupsParallel 1024 4 64", 1, False),
"gups.order.parallel" : Benchmark("gups.order.parallel", None, "/2T/benchmarks/gups/gupsParallelOrder 1024 4 128", 1, False),
"stream" : Benchmark("stream", None, "/2T/benchmarks/stream/stream.x86", 1, False),
"stream.parallel" : Benchmark("stream.parallel", None, "/2T/benchmarks/stream/stream.parallel.x86", 1, False),
"stream.short.parallel" : Benchmark("stream.short.parallel", None, "/2T/benchmarks/stream/stream.short.parallel.x86", 1, False),
"genome" : Benchmark("stamp.genome", None, "/2T/benchmarks/stamp-0.9.10/genome/genome -g65536 -s64 -n16777216 -t4", 1, False),
"intruder" : Benchmark("stamp.intruder", None, "/2T/benchmarks/stamp-0.9.10/intruder/intruder -a10 -l128 -n522144 -s1 -t4", 1, False),
"kmeans" : Benchmark("stamp.kmeans", None, "/2T/benchmarks/stamp-0.9.10/kmeans/kmeans -m80 -n30 -t0.00001 -i /2T/benchmarks/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p4", 1, False),
"ssca2" : Benchmark("stamp.ssca2", None, "/2T/benchmarks/stamp-0.9.10/ssca2/ssca2 -s21 -i1.0 -u1.0 -l3 -p3 -t4", 1, False),
"vacation" : Benchmark("stamp.vacation", None, "/2T/benchmarks/stamp-0.9.10/vacation/vacation -n4 -q60 -u90 -r1048576 -t4194304", 1, False),
"yada" : Benchmark("stamp.yada", None, "/2T/benchmarks/stamp-0.9.10/yada/yada -a15 -i /2T/benchmarks/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t4", 1, False),
"bayes" : Benchmark("stamp.bayes", None, "/2T/benchmarks/stamp-0.9.10/bayes/bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4", 1, False),
"labyrinth" : Benchmark("stamp.labyrinth", None, "/2T/benchmarks/stamp-0.9.10/labyrinth/labyrinth -i /2T/benchmarks/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt", 1, False),
"blackscholes" : Benchmark("blackscholes", None, "%s/bin/parsecmgmt -a run -i native -p blackscholes -n 4" % parsecDir, 1, False, True),
"bodytrack" : Benchmark("bodytrack", None, "%s/bin/parsecmgmt -a run -i native -p bodytrack -n 4" % parsecDir, 1, False, True),
"facesim" : Benchmark("facesim", None, "%s/bin/parsecmgmt -a run -i native -p facesim -n 4" % parsecDir, 1, False, True),
"ferret" : Benchmark("ferret", None, "%s/bin/parsecmgmt -a run -i native -p ferret -n 4" % parsecDir, 1, False, True),
"freqmine" : Benchmark("freqmine", None, "%s/bin/parsecmgmt -a run -i native -p freqmine -n 4" % parsecDir, 1, False, True),
"raytrace" : Benchmark("raytrace", None, "%s/bin/parsecmgmt -a run -i native -p raytrace -n 4" % parsecDir, 1, False, True),
"swaptions" : Benchmark("swaptions", None, "%s/bin/parsecmgmt -a run -i native -p swaptions -n 4" % parsecDir, 1, False, True),
"fluidanimate" : Benchmark("fluidanimate", None, "%s/bin/parsecmgmt -a run -i native -p fluidanimate -n 4" % parsecDir, 1, False, True),
"vips" : Benchmark("vips", None, "%s/bin/parsecmgmt -a run -i native -p vips -n 4" % parsecDir, 1, False, True),
"x264" : Benchmark("x264", None, "%s/bin/parsecmgmt -a run -i native -p x264 -n 4" % parsecDir, 1, False, True),
"canneal" : Benchmark("canneal", None, "%s/bin/parsecmgmt -a run -i native -p canneal -n 4" % parsecDir, 1, False, True),
"streamcluster" : Benchmark("streamcluster", None, "%s/bin/parsecmgmt -a run -i native -p streamcluster -n 4" % parsecDir, 1, False, True)		
}

#size = "simsmall"
#size = "simmedium"
size = "simlarge"

variableSizeBenchmarks = {
						"blackscholes" : Benchmark("blackscholes", None, "%s/bin/parsecmgmt -a run -i %s -p blackscholes -n 4" % (parsecDir, size), 1, False, True),
						"bodytrack" : Benchmark("bodytrack", None, "%s/bin/parsecmgmt -a run -i %s -p bodytrack -n 4" % (parsecDir, size), 1, False, True),
						"canneal" : Benchmark("canneal", None, "%s/bin/parsecmgmt -a run -i %s -p canneal -n 4" % (parsecDir, size), 1, False, True),
						"dedup" : Benchmark("dedup", None, "%s/bin/parsecmgmt -a run -i %s -p dedup -n 4" % (parsecDir, size), 1, False, True),
						"facesim" : Benchmark("facesim", None, "%s/bin/parsecmgmt -a run -i %s -p facesim -n 4" % (parsecDir, size), 1, False, True),
						"ferret" : Benchmark("ferret", None, "%s/bin/parsecmgmt -a run -i %s -p ferret -n 4" % (parsecDir, size), 1, False, True),
						"fluidanimate" : Benchmark("fluidanimate", None, "%s/bin/parsecmgmt -a run -i %s -p fluidanimate -n 4" % (parsecDir, size), 1, False, True),
						"freqmine" : Benchmark("freqmine", None, "%s/bin/parsecmgmt -a run -i %s -p freqmine -n 4" % (parsecDir, size), 1, False, True),
						"raytrace" : Benchmark("raytrace", None, "%s/bin/parsecmgmt -a run -i %s -p raytrace -n 4" % (parsecDir, size), 1, False, True),
						"streamcluster" : Benchmark("streamcluster", None, "%s/bin/parsecmgmt -a run -i %s -p streamcluster -n 4" % (parsecDir, size), 1, False, True), 		
						"swaptions" : Benchmark("swaptions", None, "%s/bin/parsecmgmt -a run -i %s -p swaptions -n 4" % (parsecDir, size), 1, False, True),
						"vips" : Benchmark("vips", None, "%s/bin/parsecmgmt -a run -i %s -p vips -n 4" % (parsecDir, size), 1, False, True),
						"x264" : Benchmark("x264", None, "%s/bin/parsecmgmt -a run -i %s -p x264 -n 4" % (parsecDir, size), 1, False, True),
						"gups" : Benchmark("gups", None, "/2T/benchmarks/gups/gups.x86 512", 1, True),
						"ECLAT" : Benchmark("eclat", None, "/home/joe/benchmarks/NU-MineBench-3.0/src/ECLAT/eclat -i %s/datasets/ECLAT/ntrans_2000.tlen_20.nitems_1.npats_2000.patlen_6 -e 30 -s 0.0075" % mineBenchDir, 4, True)
						}




def main():

     parser = argparse.ArgumentParser(description="Run benchmarks and profile them")
     parser.add_argument('-a', help="Use the alternate benchmarks", action="store_true")
     parser.add_argument('-d', help="Enable debugging, don't profile apps", action="store_true")
     parser.add_argument('-p', help="Enable debugging printing", action="store_true")
     parser.add_argument('architecture', metavar='architecture', nargs=1, help='the selected architecture and counters')
     #parser.add_argument('input files', metavar = 'F', type = string, nargs = "+", help = "The files to process")
     parser.add_argument('benchmarks', metavar='benchmarks', nargs="+", help='the benchmarks to run')

     try:
          options = parser.parse_args()
          
          global DEBUG
          if options.d:
          	print "Debugging enabled"
          	DEBUG = True
          	
          if options.p:
          	print "Printing output"
          	global DEBUGOUTPUT
          	DEBUGOUTPUT = True
          
          if options.architecture[0] in runTypes:
            global setupValue
            setupValue = runTypes[options.architecture[0]]
          else:
          	print "%s is not a valid architecture, choose i|a{0..10}" % options.architecture
          	return
          
          global archDir	
          archDir = options.architecture[0]
                              
          benchmarkList = {}
          
          for a in options.benchmarks:
          	if a == "all":
          		benchmarkList = benchmarks if not options.a else variableSizeBenchmarks
		  		
          	elif a in (benchmarks if not options.a else variableSizeBenchmarks):
          		benchmarkList[a] = benchmarks[a] if not options.a else variableSizeBenchmarks[a]
          
          print "Will run: " + ' '.join(benchmarkList.keys())
                    
          for k, v in benchmarkList.iteritems():
          		#print "running " + a
          		v.run()
          		
     except IOError, msg:
          parser.error(str(msg))
      

# ----
# main
# ----   
if __name__ == "__main__":
    main()
    #import profile
    #profile.run('main()', 'profile.tmp')
    #import pstats
    #p = pstats.Stats('profile.tmp')
    #p.sort_stats('cumulative').print_stats(10)
