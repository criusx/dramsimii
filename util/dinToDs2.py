#!/usr/bin/python -O

import sys

freq = 4

misstrace = sys.argv[1]
ds2trace = sys.argv[2] 
dintrace = sys.argv[3]

# open miss trace file from dinero for read
try:
    fin = open(misstrace, 'r')
except:
    print misstrace + ' does not exists'

# open miss trace file for write in dramsimii format
try:
    fout0 = open(ds2trace, 'w')
except:
    print ds2trace + ' does not exists'

# open miss trace file for write in dinero l4 cache
try:
    fout1 = open(dintrace, 'w')
except:
    print ds2trace + ' does not exists'


flag = 0
for line in fin:
    if line.find("Simulation complete") != -1:
        flag = 0

    if flag == 1:
        l = len(line)
        findspace = line.find(' ', 2, l)

        req = line[0]
        addr = line[2:findspace]
        time = float(line[findspace:l-1]) / freq

        if req == '0':
            print >>fout0, "%s %s %f %s" %(addr, 'R', time, '0')
            print >>fout1, "%s %s %f" %(req, addr, time) 
        elif req == '1':
            print >>fout0, "%s %s %f %s" %(addr, 'W', time, '0')
            print >>fout1, "%s %s %f" %(req, addr, time) 

    if line.find("Simulation begins") != -1:
        flag = 1

fin.close()
fout0.close()
fout1.close()
