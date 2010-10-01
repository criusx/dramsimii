# m5 trace file to dinero trace file
# usage: python m5ToDin.py <m5tracefile> <dintracefile>

import sys
import gzip

m5trace = sys.argv[1]
dintrace = sys.argv[2]

# open m5 trace file for read
try:
    fin = gzip.open(m5trace, 'r')
except:
    print m5trace + "does not exists"

# open din trace file for write
try:
    fout = open(dintrace, 'w')
except:
    print dintrace + "does not exists"

# format translation
for line in fin:
    l3cache = line.find("l3cache")

    if l3cache != -1:
        l = len(line)

        colon = line.find(':')

        miss = line.find("miss")
        hit = line.find("hit")

        if miss != -1 or hit != -1:
            readreqifetch = line.find("ReadReq (ifetch)")
            readexreq = line.find("ReadExReq")
            readreq = line.find("ReadReq")
            writeback = line.find("Writeback")

            if readreqifetch != -1:
                req = '0'
                if miss != -1:
                    addr = line[readreqifetch+17:miss-1]
                elif hit != -1:
                    addr = line[readreqifetch+17:hit-1]

            elif readexreq != -1:
                req = '0'
                if miss != -1:
                    addr = line[readexreq+10:miss-1]
                elif hit != -1:
                    addr = line[readexreq+10:hit-1]

            elif readreq != -1:
                req = '0'
                if miss != -1:
                    addr = line[readreq+8:miss-1]
                elif hit != -1:
                    addr = line[readreq+8:hit-1]

            elif writeback != -1:
                req = '1'
                if miss != -1:
                    addr = line[writeback+9:miss-1]
                elif hit != -1:
                    addr = line[writeback+9:hit-1]

            time = line[0:colon]

            if readreqifetch != -1 or readexreq != -1 or readreq != -1 or writeback != -1:
                fout.write(req + ' ' + addr + ' ' + time + '\n')
                #print "%s %s %s" %(req, addr, time)
                #print "%s -- %s %s %s" %(line, req, addr, time)

fin.close()
fout.close()
