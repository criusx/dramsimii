#!/usr/bin/python -O

import sys
import re

benchmark = sys.argv[1]
addressmapping = sys.argv[2]

freq = 5

# write to files
fl3 = open('./stats/' + benchmark + '/' + benchmark + '.l3.txt', 'w')

# find trace pattern
pattern = re.compile("([0-9]+) ([0-9a-fA-F]+) ([0-9]+)")


# ------------------------
# address mapping policies
# ------------------------
if addressmapping == 'sdramhiperf': AMP = 0
elif addressmapping == 'sdrambase': AMP = 1
elif addressmapping == 'closepagebaseline': AMP = 2
elif addressmapping == 'closepagebaselineopt': AMP = 3
elif addressmapping == 'closepagelowlocality': AMP = 4
elif addressmapping == 'closepagehighlocality': AMP = 5
elif addressmapping == 'amp0a': AMP = 6
elif addressmapping == 'amp0b': AMP = 7
elif addressmapping == 'amp0c': AMP = 8
elif addressmapping == 'amp0d': AMP = 9
elif addressmapping == 'amp0e': AMP = 10
elif addressmapping == 'amp0f': AMP = 11
elif addressmapping == 'amp0g': AMP = 12
elif addressmapping == 'amp0h': AMP = 13
elif addressmapping == 'amp0i': AMP = 14
elif addressmapping == 'amp0j': AMP = 15
elif addressmapping == 'amp1a': AMP = 16
elif addressmapping == 'amp1b': AMP = 17
elif addressmapping == 'amp1c': AMP = 18
elif addressmapping == 'amp1d': AMP = 19
elif addressmapping == 'amp1e': AMP = 20
elif addressmapping == 'amp1f': AMP = 21
elif addressmapping == 'amp1g': AMP = 22
elif addressmapping == 'amp1h': AMP = 23
elif addressmapping == 'amp1i': AMP = 24
elif addressmapping == 'amp1j': AMP = 25
elif addressmapping == 'amp2a': AMP = 26
elif addressmapping == 'amp2b': AMP = 27
elif addressmapping == 'amp2c': AMP = 28
elif addressmapping == 'amp2d': AMP = 29
elif addressmapping == 'amp2e': AMP = 30
elif addressmapping == 'amp2f': AMP = 31
elif addressmapping == 'amp2g': AMP = 32
elif addressmapping == 'amp2h': AMP = 33
elif addressmapping == 'amp2i': AMP = 34
elif addressmapping == 'amp2j': AMP = 35
elif addressmapping == 'amp3a': AMP = 36
elif addressmapping == 'amp3b': AMP = 37
elif addressmapping == 'amp3c': AMP = 38
elif addressmapping == 'amp3d': AMP = 39
elif addressmapping == 'amp3e': AMP = 40
elif addressmapping == 'amp3f': AMP = 41
elif addressmapping == 'amp3g': AMP = 42
elif addressmapping == 'amp3h': AMP = 43
elif addressmapping == 'amp3i': AMP = 44
elif addressmapping == 'amp3j': AMP = 45
else:
    print "Unknown address mapping policy: %s" % addressmapping
    system.exit(-1)
    


# ----
# main
# ----
for line in sys.stdin:
    # print l3 miss rate stats
    l = len(line)

    if line.find('Metrics\t') != -1: print >> fl3, line[:l - 1]
    if line.find('Demand Fetches\t') != -1: print >> fl3, line[:l - 1]
    if line.find('Fraction of total\t') != -1: print >> fl3, line[:l - 1]
    if line.find('Demand Misses\t') != -1: print >> fl3, line[:l - 1]
    if line.find('Demand miss rate\t') != -1: print >> fl3, line[:l - 1]

    # get traces for each dimm cache
    m = pattern.match(line)

    if m is not None:
        req = m.group(1)
        address = m.group(2)
        time = float(m.group(3)) / freq

        addr = int(m.group(2), 16)

        #print bin(addr)

        # set channel id and rank id
        # original address mapping policies
        
        if AMP == 0:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 18) & 0x03
        elif AMP == 1:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 32) & 0x03
        elif AMP == 2:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 11) & 0x03
        elif AMP == 3:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 14) & 0x03
        elif AMP == 4:
            channelid = (addr >> 3) & 0x01
            rankid = (addr >> 4) & 0x03
        elif AMP == 5:
            channelid = (addr >> 27) & 0x01
            rankid = (addr >> 32) & 0x03
        # amp0*
        elif AMP == 6:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 8) & 0x03
        elif AMP == 7:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 12) & 0x03
        elif AMP == 8:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 16) & 0x03
        elif AMP == 9:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 20) & 0x03
        elif AMP == 10:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 22) & 0x03
        elif AMP == 11:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 24) & 0x03
        elif AMP == 12:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 26) & 0x03
        elif AMP == 13:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 28) & 0x03
        elif AMP == 14:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 30) & 0x03
        elif AMP == 15:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 32) & 0x03
        # amp1*
        elif AMP == 16:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 8) & 0x03
        elif AMP == 17:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 12) & 0x03
        elif AMP == 18:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 16) & 0x03
        elif AMP == 19:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 20) & 0x03
        elif AMP == 20:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 22) & 0x03
        elif AMP == 21:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 24) & 0x03
        elif AMP == 22:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 26) & 0x03
        elif AMP == 23:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 28) & 0x03
        elif AMP == 24:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 30) & 0x03
        elif AMP == 25:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 32) & 0x03
        # amp2*
        elif AMP == 26:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 8) & 0x03
        elif AMP == 27:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 12) & 0x03
        elif AMP == 28:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 16) & 0x03
        elif AMP == 29:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 20) & 0x03
        elif AMP == 30:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 22) & 0x03
        elif AMP == 31:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 24) & 0x03
        elif AMP == 32:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 26) & 0x03
        elif AMP == 33:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 28) & 0x03
        elif AMP == 34:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 30) & 0x03
        elif AMP == 35:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 32) & 0x03
        # amp3*
        elif AMP == 36:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 8) & 0x03
        elif AMP == 37:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 12) & 0x03
        elif AMP == 38:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 16) & 0x03
        elif AMP == 39:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 18) & 0x03
        elif AMP == 40:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 20) & 0x03
        elif AMP == 41:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 24) & 0x03
        elif AMP == 42:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 26) & 0x03
        elif AMP == 43:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 28) & 0x03
        elif AMP == 44:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 30) & 0x03
        elif AMP == 45:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 32) & 0x03

        # print traces for each dimm cache
        print '%d %s %s 0' % (channelid * 2 + rankid / 2, req, address)
        #print >>fout[channelid*2 + rankid/2], '%s %s 0' %(req, address) 

#for a in fout:
#     a.close()
