#!/bin/bash
#This script runs m5-DramSimII then fixes the timing output for DramSimValid then runs DramSimValid then splits its output into multiple files one file for each type of error detected

#Process command line options
outputPrefix="errors"
timeW="1000000000"
clean=0
while [ $# -gt 0 ]
do
	case "$1" in
		-h | --help)
			echo -e "List of Options \n"
			echo -e "-h --help\tDisplay this help text\n"
			echo -e "-o --outfile filename \n\tPrefix to beused for the error files created as the final output.  The default is \"errors\".\n"
			echo -e "-t --time ticks\n\tNumber seconds to let m5 run.  The default is 1billion.\n"
			echo -e "-c --clean\n\tRemove all intermediate files at end of run, including m5 generated files."
			exit 1
			;;
		-o | --outfile)
			shift
			outputPrefix="$1"
			;;
		-t | --time)
			shift
			timeW="$1"
			;;
		-c | --clean)
			clean=1
			;;
		-*)
			echo -e "$1 is not a known option please see help file option -h.\n"
			exit 1
			;;
		*)
			break
			;;
	esac
	shift
done
#End processing of command line options

#Start checking of the xml file and its timing parameters. Assumes TimingChecker.class is in ~/workspace/TimingChecker/

cd ~/workspace/TimingChecker/ && java TimingChecker ~-/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25.xml && cd ~-

#Start m5 process and wait time seconds before killing it.  Assumes the spec binary is in specs/ and is the sjeng benchmark

echo -e "./build/ALPHA_SE/m5.opt configs/example/se.py -c specs/sjeng_base.amd64-m64-gcc41-nn &\n"
./build/ALPHA_SE/m5.opt configs/example/se.py -m $timeW -c specs/sjeng_base.amd64-m64-gcc41-nn --detailed --caches
#sleep $timeW
#killall m5.opt
echo -e "m5 killed\n"

#End of m5
#Find which m5 output file to use

m5Out=$(ls | grep 'standardOutFile.*-timing.gz' | tail -n1)
echo "m5 Output file to process $m5Out"

#Run process.sh on m5Out

echo -e "./process.sh $m5Out\n"
./process.sh $m5Out

#Run DramSimValid on newly created output which according to process.sh is $m5Out.cmd.gz  Assumes DRAMSimValid.class and all its included calsses are in ~/workspace/DramSimValid

echo -e "DramSimValid Running\n"
cd ~/workspace/DramSimValid 
java DramSimTest ~-/$m5Out.cmd.gz ~-/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25.xml ~-/$outputPrefix.gz
cd ~-

#Run filter.py with the newly created errors file named $outputPrefix.gz

echo -e "python filter.py $outputPrefix.gz\n"
python filter.py $outputPrefix.gz

#Cleanup
echo -e "Cleaning Up"
rm $outputPrefix.gz #This file will be empty after processing
if [ $clean -eq 0]
	then
		echo "rm ${m5Out:0:18}*"
		rm ${m5Out:0:18}*
fi

#Done
echo "done with Super Validation!"
