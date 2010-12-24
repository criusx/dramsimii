#!/bin/bash

case "$1" in
	i0)
		opcontrolFlags="-e LLC_MISSES:10000 -e LLC_REFS:10000 --no-vmlinux -e MEM_LOAD_RETIRED:100000:0x04:0:1 -e CPU_CLK_UNHALTED:100000"
		iteration='0-intel'
		;;
	i1)
		opcontrolFlags="-e L2_RQSTS:50000:0xAA -e L2_RQSTS:50000:0xFF --no-vmlinux -e INST_RETIRED:500000:0x03:0:1 -e CPU_CLK_UNHALTED:100000"
		iteration='0-intel'
		;;
	i2)
		opcontrolFlags="-e L1D_ALL_REF:100000:0x03 -e L1D:100000:0x01 -e L1I:100000:0x01 -e L1I:100000:0x02 --no-vmlinux"
		iteration='2-intel'
		;;
	i3)
		opcontrolFlags="-e MEM_LOAD_RETIRED:500000:0x10 -e CPU_CLK_UNHALTED:500000 -e MEM_INST_RETIRED:500000:0x03 -e UOPS_RETIRED:500000:0x01 --no-vmlinux"
		iteration='3-intel'
		;;
	i4)
                opcontrolFlags="--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e INST_RETIRED:500000:0x01 -e MEM_INST_RETIRED:500000:0x01 -e MEM_INST_RETIRED:500000:0x02"
		# -e MEM_LOAD_RETIRED:500000:0x10 -e MEM_LOAD_RETIRED:500000:0x04 -e MEM_LOAD_RETIRED:500000:0x08 -e MEM_UNCORE_RETIRED:500000:0x20 -e MEM_UNCORE_RETIRED:500000:0x10"
                iteration='4-intel'
                ;;
	i5)
                opcontrolFlags="--no-vmlinux -e DTLB_MISSES:50000:0x01 -e DTLB_MISSES:50000:0x02 -e DTLB_MISSES:50000:0x10 -e DTLB_LOAD_MISSES:50000:0x01"
                iteration='5-intel'
                ;;
        i6)
                opcontrolFlags="--no-vmlinux -e L2_HW_PREFETCH:50000:0x01 -e L2_HW_PREFETCH:50000:0x04 -e MEM_LOAD_RETIRED:50000:0x10 -e MEM_UNCORE_RETIRED:50000:0x30"
                iteration='6-intel'
                ;;

	a0)
		opcontrolFlags="-e L3_EVICTIONS:50000:0x0F --no-vmlinux -e READ_REQUEST_L3_CACHE:50000:0xF7 -e RETIRED_INSTRUCTIONS:500000 -e L3_CACHE_MISSES:50000:0xF7"
		iteration='0-amd'
		;;	
	a1)
		opcontrolFlags="-e RETIRED_INSTRUCTIONS:500000 -e REQUESTS_TO_L2:50000:0x3F --no-vmlinux -e L2_CACHE_MISS:50000:0x0F -e L2_CACHE_FILL_WRITEBACK:50000:0x03"
		iteration='1-amd'
		;;
	a2)
		opcontrolFlags="-e DATA_CACHE_ACCESSES:50000 -e DATA_CACHE_REFILLS_FROM_L2_OR_NORTHBRIDGE:50000:0x1E --no-vmlinux -e DATA_CACHE_REFILLS_FROM_NORTHBRIDGE:50000:0x1F -e RETIRED_INSTRUCTIONS:100000"
		iteration='2-amd'
		;;
	a3)
		# note that DRAM_ACCESSES of 0x3F is actually a combination of 0x07 and 0x38, for DCT0 and DCT1, respectively, so this will collect data from both channels
		opcontrolFlags="--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e OCTWORD_WRITE_TRANSFERS:500000:0x01 -e NORTHBRIDGE_READ_RESPONSES:500000:0x07 -e DRAM_ACCESSES:500000:0x3F"
		iteration='3-amd'
		;;
	a4)
		# to determine the dtlb misses and accesses
		opcontrolFlags="--no-vmlinux -e RETIRED_INSTRUCTIONS:500000 -e DATA_CACHE_ACCESSES:50000 -e L1_DTLB_MISS_AND_L2_DTLB_HIT:50000:0x07 -e L1_DTLB_AND_L2_DTLB_MISS:50000:0x07"
		iteration='4-amd'
		;;
	a5)
		# to determine the icache misses, itlb misses
		opcontrolFlags="--no-vmlinux -e RETIRED_INSTRUCTIONS:500000 -e INSTRUCTION_CACHE_FETCHES:50000 -e L1_ITLB_MISS_AND_L2_ITLB_HIT:50000 -e L1_ITLB_MISS_AND_L2_ITLB_MISS:50000:0x03"
		iteration='5-amd'
		;;
	a6)
		# to figure out the number of writes, reads and accesses to DCT1
		opcontrolFlags="--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e OCTWORD_WRITE_TRANSFERS:500000:0x01 -e NORTHBRIDGE_READ_RESPONSES:500000:0x07 -e DRAM_ACCESSES:500000:0x38"
		iteration='6-amd'
		;;
	a7)
		# to determine the number of retired instructions for the run
		opcontrolFlags="--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e RETIRED_INSTRUCTIONS:500000"
		iteration='7-amd'
		;;
	a8)
		# to determine the number of reads and accesses to DCT0/1
		opcontrolFlags="--no-vmlinux -e CPU_CLK_UNHALTED:500000 -e NORTHBRIDGE_READ_RESPONSES:500000:0x1F -e DRAM_ACCESSES:500000:0x07 -e DRAM_ACCESSES:500000:0x38"
		iteration='8-amd'
		;;
	a9)
		# to determine the number of reads and accesses to DCT0/1
		opcontrolFlags="--no-vmlinux -e MEMORY_CONTROLLER_REQUESTS:500000:0x01 -e MEMORY_CONTROLLER_REQUESTS:500000:0x02 -e MEMORY_CONTROLLER_REQUESTS:500000:0x04 -e MEMORY_CONTROLLER_REQUESTS:500000:0x10"
		iteration='9-amd'
		;;
	a10)
		# to determine the number of reads and accesses to DCT0/1
		opcontrolFlags="--no-vmlinux -e DATA_PREFETCHES:50000:0x01 -e DATA_PREFETCHES:50000:0x02 -e INEFFECTIVE_SW_PREFETCHES:50000:0x01 -e INEFFECTIVE_SW_PREFETCHES:50000:0x08"
		iteration='10-amd'
		;;


	*)
		echo "No valid processor selected, choose (i|a)([0..2])"
		exit -1	
esac

runit=''

archDir=$1

function run4 {
	dt=0
	printf 'running %s' $1
	# run for at least 300s
	until [ $dt -gt 300 ]; do
		stime=$(date '+%s')
		eval taskset -c 0 $runit 1>>$1.out.0 2>>$1.err.0&
		eval taskset -c 1 $runit 1>>$1.out.1 2>>$1.err.1&
		eval taskset -c 2 $runit 1>>$1.out.2 2>>$1.err.2&
		eval taskset -c 3 $runit 1>>$1.out.3 2>>$1.err.3&
		wait
		etime=$(date '+%s')
		dt=$((etime - stime + dt))
	done
	printf ', took %ds\n' $dt
}

function run1 {
	printf 'running %s' $1
	dt=0
	until [ $dt -gt 300 ]; do
		stime=$(date '+%s')
		eval taskset -c 0 $runit 1>>$1.out 2>>$1.err
		etime=$(date '+%s')
		dt=$((etime - stime + dt))
	done 
	printf ', took %ds\n' $dt
}

function run1multi {
	printf 'running %s' $1
	dt=0
	until [ $dt -gt 300 ]; do
		stime=$(date '+%s')
		eval taskset -c 0-3 $runit 1>>$1.out 2>>$1.err
		etime=$(date '+%s')
		dt=$((etime - stime + dt))
	done 
	printf ', took %ds\n' $dt
}

function setup {
	sudo opcontrol --setup $opcontrolFlags --session-dir=$profileDir/$archDir/$1-ref-4
	sudo opcontrol --start
}

function shutdown {
	sudo opcontrol --dump
	sudo opcontrol --shutdown
}

profileDir=/2T/profile2/
if [ ]; then

	# will not take this path
	echo "Nothing"

fi
parsecDir=/2T/benchmarks/parsec-2.1
for a in blackscholes bodytrack facesim ferret freqmine raytrace swaptions fluidanimate vips x264 canneal streamcluster
do
	sudo opcontrol --setup $opcontrolFlags --session-dir=$profileDir/$archDir/$a-native-4-$iteration
	
	runit="$parsecDir/bin/parsecmgmt -a run -i native -p $a -n 4"
	run1multi $a

	sudo opcontrol --shutdown
done 

specDir=/2T/benchmarks/spec06/benchspec/CPU2006

setup "401.bzip2"
runit="$specDir/401.bzip2/exe/bzip2_base.amd64-m64-gcc42-nn $specDir/401.bzip2/data/ref/input/input.source 1024"
run4 "401.bzip2" 
shutdown 

setup "403.gcc"
runit="$specDir/403.gcc/exe/gcc_base.amd64-m64-gcc42-nn $specDir/403.gcc/data/ref/input/166.i -o 166.s"
run4 "403.gcc" 
shutdown 

setup "410.bwaves"
cd $specDir/410.bwaves/data/ref/input
runit= "../../../exe/bwaves_base.amd64-m64-gcc42-nn"
run4 "410.bwaves"
shutdown 

setup "433.milc"
runit="cat $specDir/433.milc/data/ref/input/su3imp.in | $specDir/433.milc/exe/milc_base.amd64-m64-gcc42-nn"
run4 "433.milc" 
sudo opcontrol --shutdown

setup "429.mcf"
runit="$specDir/429.mcf/exe/mcf_base.amd64-m64-gcc42-nn $specDir/429.mcf/data/ref/input/inp.in"
run4 "429.mcf" 
shutdown

setup "434.zeusmp"
cd $specDir/434.zeusmp/data/ref/input
runit="../../../exe/zeusmp_base.amd64-m64-gcc42-nn"
run4 "434.zeusmp"
shutdown

setup "435.gromacs"
cd $specDir/435.gromacs/data/ref/input
runit="$specDir/435.gromacs/exe/gromacs_base.amd64-m64-gcc42-nn -silent -deffnm gromacs -nice 0"
run4 "435.gromacs" 
shutdown

setup "444.namd"
cd $specDir/444.namd/data/all/input
runit="../../../exe/namd_base.amd64-m64-gcc42-nn --input namd.input --iterations 38 --output namd.out"
run4 "444.namd" 
shutdown

setup "447.dealII"
runit="$specDir/447.dealII/exe/dealII_base.amd64-m64-gcc42-nn 23"
run4 "447.dealII" 
shutdown

setup "450.soplex"
runit="$specDir/450.soplex/exe/soplex_base.amd64-m64-gcc42-nn $specDir/450.soplex/data/ref/input/ref.mps -m3500"
run4 "450.soplex" 
shutdown

setup "453.povray"
cd $specDir/453.povray/data/ref/input
runit="$specDir/453.povray/exe/povray_base.amd64-m64-gcc42-nn $specDir/453.povray/data/ref/input/SPEC-benchmark-ref.ini"
run4 "453.povray" 
shutdown

setup "458.sjeng"
runit="$specDir/458.sjeng/exe/sjeng_base.amd64-m64-gcc42-nn $specDir/458.sjeng/data/ref/input/ref.txt"
run4 "458.sjeng" 
shutdown

setup "456.hmmer"
runit="$specDir/456.hmmer/exe/hmmer_base.amd64-m64-gcc42-nn --fixed 0 --mean 500 --num 500000 --sd 350 --seed 0 $specDir/456.hmmer/data/ref/input/retro.hmm"
run4 "456.hmmer" 
shutdown

setup "462.libquantum"
runit="$specDir/462.libquantum/exe/libquantum_base.amd64-m64-gcc42-nn 1397 8"
run4 "462.libquantum" 
shutdown

setup "464.h264ref"
cd $specDir/464.h264ref/data/ref/input
runit="$specDir/464.h264ref/exe/h264ref_base.amd64-m64-gcc42-nn -d $specDir/464.h264ref/data/ref/input/sss_encoder_main.cfg"
run4 "464.h264ref" 
shutdown

setup "470.lbm"
runit="$specDir/470.lbm/exe/lbm_base.amd64-m64-gcc42-nn 3000 reference.dat 0 0 $specDir/470.lbm/data/ref/input/100_100_130_ldc.of"
run4 "470.lbm" 
shutdown

setup "471.omnetpp"
cd $specDir/471.omnetpp/data/ref/input
runit="../../../exe/omnetpp_base.amd64-m64-gcc42-nn omnetpp.ini"
run4 "471.omnetpp"  
shutdown

setup "473.astar"
cd $specDir/473.astar/data/ref/input
runit="../../../exe/astar_base.amd64-m64-gcc42-nn rivers.cfg"
run4 "473.astar" 
shutdown

#run4 "481.wrf"

setup "483.xalancbmk"
runit="$specDir/483.xalancbmk/exe/Xalan_base.amd64-m64-gcc42-nn -v $specDir/483.xalancbmk/data/ref/input/t5.xml $specDir/483.xalancbmk/data/ref/xalanc.xsl"
run4 "483.xalancbmk" 
shutdown

setup "416.gamess"
cd  $specDir/416.gamess/data/ref/input
runit="cat $specDir/416.gamess/data/ref/input/triazolium.config|$specDir/416.gamess/exe/gamess_base.amd64-m64-gcc42-nn"
run4 "416.gamess" 
shutdown

setup "436.cactusADM"
runit="$specDir/436.cactusADM/exe/cactusADM_base.amd64-m64-gcc42-nn $specDir/436.cactusADM/data/ref/input/benchADM.par"
run4 "436.cactusADM" 
shutdown

setup "437.leslie3d"
runit="cat $specDir/437.leslie3d/data/ref/input/leslie3d.in|$specDir/437.leslie3d/exe/leslie3d_base.amd64-m64-gcc42-nn"
run4 "437.leslie3d" 
shutdown

setup "454.calculix"
cd $specDir/454.calculix/data/ref/input
runit="../../../exe/calculix_base.amd64-m64-gcc42-nn -i hyperviscoplastic"
run4 "454.calculix" 
shutdown

setup "459.GemsFDTD"
cd $specDir/459.GemsFDTD/data/ref/input
runit="../../../exe/GemsFDTD_base.amd64-m64-gcc42-nn"
run4 "459.GemsFDTD" 
shutdown

setup "465.tonto"
cd $specDir/465.tonto/data/ref/input
runit="../../../exe/tonto_base.amd64-m64-gcc42-nn"
run4 "465.tonto" 
shutdown

setup "482.sphinx3"
cd $specDir/482.sphinx3/run/run_base_ref_amd64-m64-gcc42-nn.0000
runit="../../exe/sphinx_livepretend_base.amd64-m64-gcc42-nn ctlfile . args.an4"
run4 "482.sphinx3" 
shutdown

mineBenchDir=/2T/benchmarks/NU-MineBench-3.0

setup "ECLAT"
runit='/home/joe/benchmarks/NU-MineBench-3.0/src/ECLAT/eclat -i $mineBenchDir/datasets/ECLAT/ntrans_2000.tlen_20.nitems_1.npats_2000.patlen_6 -e 30 -s 0.0075'
run4 'ECLAT' 
shutdown

setup "APRIORI"
runit='$mineBenchDir/src/APR/no_output_apriori -i $mineBenchDir/datasets/APR/data.ntrans_10000.tlen_20.nitems_1.npats_2000.patlen_6 -f $mineBenchDir/datasets/APR/offset_file_10000_P4.txt -s 0.0075 -n 4'
run1 'APRIORI' 
shutdown

setup "utility_mine"
runit='$mineBenchDir/src/utility_mine/tran_utility/utility_mine $mineBenchDir/datasets/utility_mine/RealData/real_data_aa_binary  $mineBenchDir/datasets/utility_mine/RealData/product_price_binary 0.01'
run4 'utility_mine' 
shutdown

setup "gups"
runit='/2T/benchmarks/gups/gups.x86 1024'
run4 'gups'
shutdown

setup "parallelGups"
runit='/2T/benchmarks/gups/gupsParallel 1024 4 64'
run1multi 'parallelGUPS'
shutdown

setup "parallelGupsOrder"
runit='/2T/benchmarks/gups/gupsParallelOrder 1024 4 128'
run1multi 'parallelGUPSOrder'
shutdown

setup "stream"
runit='/2T/benchmarks/stream/stream.x86'
run1 'stream'
shutdown

setup "stream.parallel"
runit='/2T/benchmarks/stream/stream.parallel.x86'
run1multi 'stream.parallel'
shutdown

setup "stream.short.parallel"
runit='/2T/benchmarks/stream/stream.short.parallel.x86'
run1multi 'stream.short.parallel'
shutdown

setup "stamp.genome"
runit='/2T/benchmarks/stamp-0.9.10/genome/genome -g65536 -s64 -n16777216 -t4'
run1multi "stamp.genome"
shutdown

setup "stamp.intruder"
runit='/2T/benchmarks/stamp-0.9.10/intruder/intruder -a10 -l128 -n522144 -s1 -t4'
run1multi "stamp.intruder"
shutdown

setup "stamp.kmeans"
runit='/2T/benchmarks/stamp-0.9.10/kmeans/kmeans -m250 -n35 -t0.00001 -i /2T/benchmarks/stamp-0.9.10/kmeans/inputs/random-n65536-d32-c16.txt -p4'
run1multi "stamp.kmeans"
shutdown

setup "stamp.ssca2"
runit='/2T/benchmarks/stamp-0.9.10/ssca2/ssca2 -s21 -i1.0 -u1.0 -l3 -p3 -t4'
run1multi "stamp.ssca2"
shutdown

setup "stamp.vacation"
runit='/2T/benchmarks/stamp-0.9.10/vacation/vacation -n4 -q60 -u90 -r1048576 -t4194304'
run1multi "stamp.vacation"
shutdown

setup "stamp.yada"
runit='/2T/benchmarks/stamp-0.9.10/yada/yada -a15 -i /2T/benchmarks/stamp-0.9.10/yada/inputs/ttimeu1000000.2 -t4'
run1multi "stamp.yada"
shutdown

setup "stamp.bayes"
runit='/2T/benchmarks/stamp-0.9.10/bayes/bayes -v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t4'
run1multi "stamp.bayes"
shutdown

setup "stamp.labyrinth"
runit='/2T/benchmarks/stamp-0.9.10/labyrinth/labyrinth -i /2T/benchmarks/stamp-0.9.10/labyrinth/inputs/random-x512-y512-z7-n512.txt'
run1multi "stamp.labyrinth"
shutdown
