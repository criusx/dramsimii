import sys
import gzip

#m5trace = 'gupsStream.gz'
m5trace = "lbmStream_1"

try:
#    f = gzip.open(m5trace, 'r')
    f = open(m5trace, 'r')
except:
    print m5trace + "does not exists"

for line in f:
    l3cache = line.find("l3cache")

    if l3cache != -1:
        l = len(line)

        colon = line.find(':')

        miss = line.find("miss")
        hit = line.find("hit")
        sending = line.find("Sending")

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

            time = line[colon-13:colon]

            if readreqifetch != -1 or readexreq != -1 or readreq != -1 or writeback != -1:
                print "%s %s %s" %(req, addr, time)
                #print "%s -- %s %s %s" %(line, req, addr, time)

f.close()
