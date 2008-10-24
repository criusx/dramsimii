#!/usr/bin/python

import gzip
import sys
import os

print sys.argv
os.environ["GDFONTPATH"]="/usr/share/fonts/truetype/msttcorefonts"

workingSetOutfile = open('workingSet.dat', 'w')
transCounter = 0
cmdCounter = 0
workingSetCounter = 0
writing = 0

try:
    compressedstream = gzip.open(sys.argv[1], 'rb')
    for line in compressedstream:
        # end of section
        if line[0] == '-':
            if writing == 1:
                writing = 0
                transCounter += 1                
                transOutfile.close()
                os.system("gnuplot transactionExecution.plt")    
                os.rename("transactionExecution.png", "transactionExecution" + `transCounter` + ".png")    
            elif writing == 2:
                writing = 0
                pass
            elif writing == 3:
                writing = 0
                pass
            elif writing == 4:                
                writing = 0
                cmdCounter += 1
                commandOutfile.close()
                os.system("gnuplot commandExecution.plt")
                os.rename("commandExecution.png",  "commandExecution" + `cmdCounter` + ".png")
                pass
        # data in this section
        else:
            if writing == 1:
                transOutfile.write(line)
            elif writing == 2:
                if len(line) > 1:
                    workingSetCounter += 1
                    workingSetOutfile.write(`workingSetCounter` + ' ' + line)                
            elif writing == 3:
                pass
            elif writing == 4:
                commandOutfile.write(line)                
            elif writing == 0:
                pass
        if line.strip() == '----Transaction Execution Time----':
            writing = 1
            transOutfile = open('transactionExecution.dat', 'w')       
        elif line.strip() == '----Working Set----':
            writing = 2
        elif line.strip() == '----Delay----':
            writing = 3
        elif line.strip() == '----CMD Execution Time----':            
            writing = 4
            commandOutfile = open('commandExecution.dat', 'w')
except IOError, strerror:
    print "I/O error", strerror
    
workingSetOutfile.close()
os.system("gnuplot workingSet.plt")
