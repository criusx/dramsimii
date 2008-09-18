#!/bin/bash
#This script processes the output timing files from m5 to be usable by the DRAMsim validator

if [ $1 = ""]; then
	echo "Please provide a file name"
	exit
else
	filename=$1
fi

#Create temp ungzipped file for speed

zcat $filename | grep '^C ' > $filename.tmp
echo $filename

#Remove all lines that aren't commands

#grep '^C ' $filename.tmp > $filename.tmp

#Deteremine the number of refresh cycles 

refCycles=$(grep -o 'chan\[f' $filename.tmp | wc -w)
echo $refCycles
echo $filename

#loop through input changing channels for refresh commands to numbers in order

i=0
while [ $i -lt $refCycles ]; do
	sed -f sedprocess -i $filename.tmp
	i=$((i+4))
done

#regzip and rename the file then remove tmp file

gzip -c $filename.tmp > $filename.cmd.gz

rm $filename.tmp

