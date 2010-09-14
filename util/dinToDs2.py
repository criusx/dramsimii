import sys

freq = 4

misstrace = sys.argv[1]
ds2trace = sys.argv[2] 

# open miss trace file from dinero for read
try:
    fin = open(misstrace, 'r')
except:
    print misstrace + ' does not exists'

# open miss trace file for write in dramsimii foramt
try:
    fout = open(ds2trace, 'w')
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
            print >>fout, "%s %s %f %s" %(addr, 'R', time, '0')
        elif req == '1':
            print >>fout, "%s %s %f %s" %(addr, 'W', time, '0')

    if line.find("Simulation begins") != -1:
        flag = 1

fin.close()
fout.close()
