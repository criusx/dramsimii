#!/bin/bash

function run4 {
	dt=0
	printf 'running %s' $1
	# run for at least 600s
	until [ $dt -gt 600 ]; do
		stime=$(date '+%s')
		eval taskset -c 0 $2 1>$1.out.0 2>$1.err.0&
		eval taskset -c 1 $2 1>$1.out.1 2>$1.err.1&
		eval taskset -c 2 $2 1>$1.out.2 2>$1.err.2&
		eval taskset -c 3 $2 1>$1.out.3 2>$1.err.3&
		wait
		etime=$(date '+%s')
		dt=$((etime - stime + dt))
	done
	printf ', took %ds\n' $dt
}

function run1 {
	printf 'running %s' $1
	dt=0
	until [ $dt -gt 600 ]; do
		stime=$(date '+%s')
		eval taskset -c 0-3 $2 1>$1.out 2>$1.err
		etime=$(date '+%s')
		dt=$((etime - stime + dt))
	done 
	printf ', took %ds\n' $dt
}

sudo opcontrol --start

run4 "401.bzip2" "/home/joe/benchmarks/spec06/benchspec/CPU2006/401.bzip2/exe/bzip2_base.amd64-m64-gcc42-nn /home/joe/benchmarks/spec06/benchspec/CPU2006/401.bzip2/data/ref/input/input.source 1024"

run4 "403.gcc" "/home/joe/benchmarks/spec06/benchspec/CPU2006/403.gcc/exe/gcc_base.amd64-m64-gcc42-nn /home/joe/benchmarks/spec06/benchspec/CPU2006/403.gcc/data/ref/input/166.i -o 166.s"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/410.bwaves/data/ref/input
run4 "410.bwaves" "../../../exe/bwaves_base.amd64-m64-gcc42-nn"

run4 "433.milc" "cat /home/joe/benchmarks/spec06/benchspec/CPU2006/433.milc/data/ref/input/su3imp.in | /home/joe/benchmarks/spec06/benchspec/CPU2006/433.milc/exe/milc_base.amd64-m64-gcc42-nn"

run4 "429.mcf" "/home/joe/benchmarks/spec06/benchspec/CPU2006/429.mcf/exe/mcf_base.amd64-m64-gcc42-nn /home/joe/benchmarks/spec06/benchspec/CPU2006/429.mcf/data/ref/input/inp.in"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/434.zeusmp/data/ref/input
run4 "434.zeusmp" "../../../exe/zeusmp_base.amd64-m64-gcc42-nn"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/435.gromacs/data/ref/input
run4 "435.gromacs" "/home/joe/benchmarks/spec06/benchspec/CPU2006/435.gromacs/exe/gromacs_base.amd64-m64-gcc42-nn -silent -deffnm gromacs -nice 0"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/444.namd/data/all/input
run4 "444.namd" "../../../exe/namd_base.amd64-m64-gcc42-nn --input namd.input --iterations 38 --output namd.out"

run4 "447.dealII" "/home/joe/benchmarks/spec06/benchspec/CPU2006/447.dealII/exe/dealII_base.amd64-m64-gcc42-nn 23"

run4 "450.soplex" "/home/joe/benchmarks/spec06/benchspec/CPU2006/450.soplex/exe/soplex_base.amd64-m64-gcc42-nn /home/joe/benchmarks/spec06/benchspec/CPU2006/450.soplex/data/ref/input/ref.mps -m3500"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/453.povray/data/ref/input
run4 "453.povray" "/home/joe/benchmarks/spec06/benchspec/CPU2006/453.povray/exe/povray_base.amd64-m64-gcc42-nn /home/joe/benchmarks/spec06/benchspec/CPU2006/453.povray/data/ref/input/SPEC-benchmark-ref.ini"

run4 "458.sjeng" "/home/joe/benchmarks/spec06/benchspec/CPU2006/458.sjeng/exe/sjeng_base.amd64-m64-gcc42-nn /home/joe/benchmarks/spec06/benchspec/CPU2006/458.sjeng/data/ref/input/ref.txt"

run4 "456.hmmer" "/home/joe/benchmarks/spec06/benchspec/CPU2006/456.hmmer/exe/hmmer_base.amd64-m64-gcc42-nn --fixed 0 --mean 500 --num 500000 --sd 350 --seed 0 /home/joe/benchmarks/spec06/benchspec/CPU2006/456.hmmer/data/ref/input/retro.hmm"

run4 "462.libquantum" "/home/joe/benchmarks/spec06/benchspec/CPU2006/462.libquantum/exe/libquantum_base.amd64-m64-gcc42-nn 1397 8"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/464.h264ref/data/ref/input
run4 "464.h264ref" "/home/joe/benchmarks/spec06/benchspec/CPU2006/464.h264ref/exe/h264ref_base.amd64-m64-gcc42-nn -d /home/joe/benchmarks/spec06/benchspec/CPU2006/464.h264ref/data/ref/input/sss_encoder_main.cfg"

run4 "470.lbm" "/home/joe/benchmarks/spec06/benchspec/CPU2006/470.lbm/exe/lbm_base.amd64-m64-gcc42-nn 3000 reference.dat 0 0 /home/joe/benchmarks/spec06/benchspec/CPU2006/470.lbm/data/ref/input/100_100_130_ldc.of"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/471.omnetpp/data/ref/input
run4 "471.omnetpp"  "../../../exe/omnetpp_base.amd64-m64-gcc42-nn omnetpp.ini"

cd /home/joe//benchmarks/spec06/benchspec/CPU2006/473.astar/data/ref/input
run4 "473.astar" "../../../exe/astar_base.amd64-m64-gcc42-nn rivers.cfg"

#run4 "481.wrf"

run4 "483.xalancbmk" "/home/joe/benchmarks/spec06/benchspec/CPU2006/483.xalancbmk/exe/Xalan_base.amd64-m64-gcc42-nn -v /home/joe/benchmarks/spec06/benchspec/CPU2006/483.xalancbmk/data/ref/input/t5.xml /home/joe/benchmarks/spec06/benchspec/CPU2006/483.xalancbmk/data/ref/xalanc.xsl"

cd  /home/joe/benchmarks/spec06/benchspec/CPU2006/416.gamess/data/ref/input
run4 "416.gamess" "cat /home/joe/benchmarks/spec06/benchspec/CPU2006/416.gamess/data/ref/input/triazolium.config|/home/joe/benchmarks/spec06/benchspec/CPU2006/416.gamess/exe/gamess_base.amd64-m64-gcc42-nn"

run4 "436.cactusADM" "/home/joe/benchmarks/spec06/benchspec/CPU2006/436.cactusADM/exe/cactusADM_base.amd64-m64-gcc42-nn /home/joe/benchmarks/spec06/benchspec/CPU2006/436.cactusADM/data/ref/input/benchADM.par"

run4 "437.leslie3d" "cat /home/joe/benchmarks/spec06/benchspec/CPU2006/437.leslie3d/data/ref/input/leslie3d.in|/home/joe/benchmarks/spec06/benchspec/CPU2006/437.leslie3d/exe/leslie3d_base.amd64-m64-gcc42-nn"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/454.calculix/data/ref/input
run4 "454.calculix" "../../../exe/calculix_base.amd64-m64-gcc42-nn -i hyperviscoplastic"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/459.GemsFDTD/data/ref/input
run4 "459.GemsFDTD" "../../../exe/GemsFDTD_base.amd64-m64-gcc42-nn"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/465.tonto/data/ref/input
run4 "465.tonto" "../../../exe/tonto_base.amd64-m64-gcc42-nn"

cd /home/joe/benchmarks/spec06/benchspec/CPU2006/482.sphinx3/run/run_base_ref_amd64-m64-gcc42-nn.0000
run4 "482.sphinx3" "../../exe/sphinx_livepretend_base.amd64-m64-gcc42-nn ctlfile . args.an4"

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/blackscholes/run
run1 'blackscholes' '/home/joe/benchmarks/parsec-2.1/pkgs/apps/blackscholes/inst/amd64-linux.gcc/bin/blackscholes 4 in_10M.txt prices.txt'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/bodytrack/run
run1 'bodytrack' '/home/joe/benchmarks/parsec-2.1/pkgs/apps/bodytrack/inst/amd64-linux.gcc/bin/bodytrack sequenceB_261 4 261 4000 5 0 1'

cd /home/joe/benchmarks/parsec-2.1/pkgs/kernels/canneal/run
run1 'canneal' '/home/joe/benchmarks/parsec-2.1/pkgs/kernels/canneal/inst/amd64-linux.gcc/bin/canneal 4 15000 2000 2500000.nets 6000'

#run1 'dedup' 

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/facesim/run
run1 'facesim' '/home/joe/benchmarks/parsec-2.1/pkgs/apps/facesim/inst/amd64-linux.gcc/bin/facesim -timing -threads 4 -lastframe 100'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/ferret/run
run1 'ferret' '/home/joe/./benchmarks/parsec-2.1/pkgs/apps/ferret/inst/amd64-linux.gcc/bin/ferret corel lsh queries 50 20 4 output.txt'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/fluidanimate/run
run1 'fluidanimate' '/home/joe/./benchmarks/parsec-2.1/pkgs/apps/fluidanimate/inst/amd64-linux.gcc/bin/fluidanimate 4 500 in_500K.fluid out.fluid'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/freqmine/run
run1 'freqmine' '/home/joe/./benchmarks/parsec-2.1/pkgs/apps/freqmine/inst/amd64-linux.gcc/bin/freqmine webdocs_250k.dat 11000'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/raytrace/run
run1 'raytrace' '/home/joe/./benchmarks/parsec-2.1/pkgs/apps/raytrace/inst/amd64-linux.gcc/bin/rtview thai_statue.obj -nodisplay -automove -nthreads 4 -frames 200 -res 1920 1080'

cd /home/joe/benchmarks/parsec-2.1/pkgs/kernels/streamcluster/run
run1 'streamcluster' '/home/joe/benchmarks/parsec-2.1/pkgs/kernels/streamcluster/inst/amd64-linux.gcc/bin/streamcluster 10 20 128 1000000 200000 5000 none output.txt 4'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/swaptions/run
run1 'swaptions' '/home/joe/./benchmarks/parsec-2.1/pkgs/apps/swaptions/inst/amd64-linux.gcc/bin/swaptions -ns 128 -sm 1000000 -nt 4'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/vips/run
run1 'vips' '/home/joe/benchmarks/parsec-2.1/pkgs/apps/vips/inst/amd64-linux.gcc/bin/vips im_benchmark orion_18000x18000.v output.v'

cd /home/joe/benchmarks/parsec-2.1/pkgs/apps/x264/run
run1 'x264' '/home/joe/benchmarks/parsec-2.1/pkgs/apps/x264/inst/amd64-linux.gcc/bin/x264 --quiet --qp 20 --partitions b8x8,i4x4 --ref 5 --direct auto --b-pyramid --weightb --mixed-refs --no-fast-pskip --me umh --subme 7 --analyse b8x8,i4x4 --threads 4 -o eledream.264 eledream_1920x1080_512.y4m'

run4 'ECLAT' '/home/joe/benchmarks/NU-MineBench-3.0/src/ECLAT/eclat -i /home/joe/benchmarks/NU-MineBench-3.0/datasets/ECLAT/ntrans_2000.tlen_20.nitems_1.npats_2000.patlen_6 -e 30 -s 0.0075'

run1 'APRIORI' '/home/joe/benchmarks/NU-MineBench-3.0/src/APR/no_output_apriori -i /home/joe/benchmarks/NU-MineBench-3.0/datasets/APR/data.ntrans_10000.tlen_20.nitems_1.npats_2000.patlen_6 -f /home/joe/benchmarks/NU-MineBench-3.0/datasets/APR/offset_file_10000_P4.txt -s 0.0075 -n 4'

run4 'utility_mine' '/home/joe/benchmarks/NU-MineBench-3.0/src/utility_mine/tran_utility/utility_mine /home/joe/benchmarks/NU-MineBench-3.0/datasets/utility_mine/RealData/real_data_aa_binary  /home/joe/benchmarks/NU-MineBench-3.0/datasets/utility_mine/RealData/product_price_binary 0.01'

sudo opcontrol --dump
sudo opcontrol --stop
