#!/usr/bin/python -O

# -----------------------------------
# generate trace file for dimm caches
# -----------------------------------

import sys

freqMultiplier = 4

misstrace = sys.argv[1]
dimmcachetrace = sys.argv[2]
addressmapping = sys.argv[3]

# open miss trace file from dinero for read
try:
    fin = open(misstrace, 'r')
except:
    print misstrace + ' does not exists'

# open miss trace file for write for dimm caches
fout = []
for a in range(4):
     fout.append(open("%s.%s.%d" % (dimmcachetrace, addressmapping, a), 'w'))

pattern = re.compile("([0-9]+) ([0-9a-fA-F]+) ([0-9]+)")

for line in fin:

     m = pattern.match(line)

     if m is not None:

          time = float(m.group(3)) / freqMultiplier

          # hex to bin
          address_h = int(m.group(2), 16)

          print bin(address_h)

             # set channel id and rank id
          if addressmapping == "sdramhiperf":
                 #channelid = address_bin[29]
                 channelid = (address_h >> 28) & 0x01
                 #rankid = address_bin[16:18]
                 rankid = (address_h >> 15) & 0x03
          elif addressmapping == "sdrambase":
                 #channelid = address_bin[29]
                 channelid = (address_h >> 28) & 0x01
                 rankid = address_bin[2:4]
                 rankid = (address_h >> 1) & 0x03
          elif addressmapping == "closepagebaseline":
                 #channelid = address_bin[29]
                 channelid = (address_h >> 28) & 0x01

                 rankid = address_bin[23:25]
          elif addressmapping == "closepagebaselineopt":
                 channelid = address_bin[29]
                 rankid = address_bin[20:22]
          elif addressmapping == "closepagelowlocality":
                 channelid = address_bin[32]
                 rankid = address_bin[30:32]
          elif addressmapping == "closepagehighlocality":
                 channelid = address_bin[8]
                 rankid = address_bin[2:4]
          elif addressmapping == "mutien":
              channelid = address_bin[24]
              rankid = address_bin[16:18]

	#print "%s %s %s %s %s" %(line, addr, address_bin, channelid, rankid)

        # print files
        rankid = rankid / 2
        fout[channelid * 2 + rankid].write(req + ' ' + addr + ' 0' + '\n')


fin.close()
for a in fout:
     a.close()
