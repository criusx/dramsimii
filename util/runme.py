#!/usr/bin/python -O
#!/usr/bin/env python

from subprocess import Popen, PIPE, STDOUT
import sys, traceback
import os
from threading import Thread
import time
import gzip
from Queue import Queue
import re

class Results():
     def __init__(self, fetches, misses, cacheSize, blockSize, associativity):
          self.fetches = fetches
          self.misses = misses
          self.cacheSize = cacheSize
          self.blockSize = blockSize
          self.associativity = associativity

class L3Cache(Thread):
    def __init__(self, benchmark, file, addressMappingPolicies, workQueue):
        Thread.__init__(self)

        self.pattern = re.compile("([0-9]+) ([0-9a-fA-F]+) ([0-9]+)")
        self.freq = 5

        self.zcat = Popen(["/bin/zcat", file], shell = False, stdout = PIPE)
        dineroMissratePath = os.path.join(os.path.dirname(os.path.abspath(__file__)), "dinero/dineroIV-w-misstrace")
        self.dinero = Popen([dineroMissratePath, '-informat', 'd', '-l1-usize', '16M', '-l1-ubsize', '64', '-l1-uassoc', '16', '-l1-upfdist', '3'], shell = False, stdin = self.zcat.stdout, stdout = PIPE)

        self.AMP = []

        for addressmapping in addressMappingPolicies:
             # TODO switch these to lambda functions 
             if addressmapping == 'sdramhiperf': self.AMP.append(lambda addr: ((addr >> 6) & 0x01, (addr >> 18) & 0x03))
             elif addressmapping == 'sdrambase': self.AMP.append(ampTable[1])
             elif addressmapping == 'closepagebaseline': self.AMP.append(ampTable[2])
             elif addressmapping == 'closepagebaselineopt': self.AMP.append(ampTable[3])
             elif addressmapping == 'closepagelowlocality': self.AMP.append(ampTable[4])
             elif addressmapping == 'closepagehighlocality': self.AMP.append(ampTable[5])
             elif addressmapping == 'amp0a': self.append(ampTable[6])
             elif addressmapping == 'amp0b': self.append(ampTable[7])
             elif addressmapping == 'amp0c': self.append(ampTable[8])
             elif addressmapping == 'amp0d': self.append(ampTable[9])
             elif addressmapping == 'amp0e': self.append(ampTable[10])
             elif addressmapping == 'amp0f': self.append(ampTable[11])
             elif addressmapping == 'amp0g': self.append(ampTable[12])
             elif addressmapping == 'amp0h': self.append(ampTable[13])
             elif addressmapping == 'amp0i': self.append(ampTable[14])
             elif addressmapping == 'amp0j': self.append(ampTable[15])
             elif addressmapping == 'amp1a': self.append(ampTable[16])
             elif addressmapping == 'amp1b': self.append(ampTable[17])
             elif addressmapping == 'amp1c': self.append(ampTable[18])
             elif addressmapping == 'amp1d': self.append(ampTable[19])
             elif addressmapping == 'amp1e': self.append(ampTable[20])
             elif addressmapping == 'amp1f': self.append(ampTable[21])
             elif addressmapping == 'amp1g': self.append(ampTable[22])
             elif addressmapping == 'amp1h': self.append(ampTable[23])
             elif addressmapping == 'amp1i': self.append(ampTable[24])
             elif addressmapping == 'amp1j': self.append(ampTable[25])
             elif addressmapping == 'amp2a': self.append(ampTable[26])
             elif addressmapping == 'amp2b': self.append(ampTable[27])
             elif addressmapping == 'amp2c': self.append(ampTable[28])
             elif addressmapping == 'amp2d': self.append(ampTable[29])
             elif addressmapping == 'amp2e': self.append(ampTable[30])
             elif addressmapping == 'amp2f': self.append(ampTable[31])
             elif addressmapping == 'amp2g': self.append(ampTable[32])
             elif addressmapping == 'amp2h': self.append(ampTable[33])
             elif addressmapping == 'amp2i': self.append(ampTable[34])
             elif addressmapping == 'amp2j': self.append(ampTable[35])
             elif addressmapping == 'amp3a': self.append(ampTable[36])
             elif addressmapping == 'amp3b': self.append(ampTable[37])
             elif addressmapping == 'amp3c': self.append(ampTable[38])
             elif addressmapping == 'amp3d': self.append(ampTable[39])
             elif addressmapping == 'amp3e': self.append(ampTable[40])
             elif addressmapping == 'amp3f': self.append(ampTable[41])
             elif addressmapping == 'amp3g': self.append(ampTable[42])
             elif addressmapping == 'amp3h': self.append(ampTable[43])
             elif addressmapping == 'amp3i': self.append(ampTable[44])
             elif addressmapping == 'amp3j': self.append(ampTable[45])
             else:
                 print "Unknown address mapping policy: %s" % addressmapping
                 system.exit(-1)

    def run(self):
        while True:
            newLine = self.dinero.stdout.readline()
            if not newLine:
                # to signal the end of the file
                for a in workQueue:
                    a.put(None)
                break

            m = self.pattern.match(newLine)

            if m is not None:
                req = m.group(1)
                addr = int(m.group(2), 16)
                time = float(m.group(3)) / self.freq

                ampNumber = 0
                for a in self.AMP:
                     (channel, rank) = a(addr)

                     workQueue[ampNumber * 4 + channel * 2 + rank / 2].put("%s %s 0\n" % (req, m.group(2)))
                     ampNumber += 1

        self.dinero.wait()


def amp01(addr):
    channelid = (addr >> 6) & 0x01
    rankid = (addr >> 32) & 0x03
    return (channelid, rankid)
def amp02(addr):
    channelid = (addr >> 6) & 0x01
    rankid = (addr >> 11) & 0x03
    return (channelid, rankid)
def amp03(addr):
    channelid = (addr >> 6) & 0x01
    rankid = (addr >> 14) & 0x03
    return (channelid, rankid)
def amp04(addr):
    channelid = (addr >> 3) & 0x01
    rankid = (addr >> 4) & 0x03
    return (channelid, rankid)
def amp05(addr):
    channelid = (addr >> 27) & 0x01
    rankid = (addr >> 32) & 0x03
    return (channelid, rankid)
def amp06(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 8) & 0x03
    return (channelid, rankid)
def amp07(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 12) & 0x03
    return (channelid, rankid)
def amp08(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 16) & 0x03
    return (channelid, rankid)
def amp09(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 20) & 0x03
    return (channelid, rankid)
def amp10(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 22) & 0x03
    return (channelid, rankid)
def amp11(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 24) & 0x03
    return (channelid, rankid)
def amp12(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 26) & 0x03
    return (channelid, rankid)
def amp13(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 28) & 0x03
    return (channelid, rankid)
def amp14(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 30) & 0x03
    return (channelid, rankid)
def amp15(addr):
    channelid = (addr >> 11) & 0x01
    rankid = (addr >> 32) & 0x03
    return (channelid, rankid)
def amp16(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 8) & 0x03
    return (channelid, rankid)
def amp17(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 12) & 0x03
    return (channelid, rankid)
def amp18(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 16) & 0x03
    return (channelid, rankid)
def amp19(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 20) & 0x03
    return (channelid, rankid)
def amp20(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 22) & 0x03
    return (channelid, rankid)
def amp21(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 24) & 0x03
    return (channelid, rankid)
def amp22(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 26) & 0x03
    return (channelid, rankid)
def amp23(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 28) & 0x03
    return (channelid, rankid)
def amp24(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 30) & 0x03
    return (channelid, rankid)
def amp25(addr):
    channelid = (addr >> 15) & 0x01
    rankid = (addr >> 32) & 0x03
    return (channelid, rankid)
def amp26(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 8) & 0x03
    return (channelid, rankid)
def amp27(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 12) & 0x03
    return (channelid, rankid)
def amp28(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 16) & 0x03
    return (channelid, rankid)
def amp29(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 20) & 0x03
    return (channelid, rankid)
def amp30(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 22) & 0x03
    return (channelid, rankid)
def amp31(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 24) & 0x03
    return (channelid, rankid)
def amp32(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 26) & 0x03
    return (channelid, rankid)
def amp33(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 28) & 0x03
    return (channelid, rankid)
def amp34(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 30) & 0x03
    return (channelid, rankid)
def amp35(addr):
    channelid = (addr >> 19) & 0x01
    rankid = (addr >> 32) & 0x03
    return (channelid, rankid)
def amp36(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 8) & 0x03
    return (channelid, rankid)
def amp37(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 12) & 0x03
    return (channelid, rankid)
def amp38(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 16) & 0x03
    return (channelid, rankid)
def amp39(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 18) & 0x03
    return (channelid, rankid)
def amp40(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 20) & 0x03
    return (channelid, rankid)
def amp41(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 24) & 0x03
    return (channelid, rankid)
def amp42(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 26) & 0x03
    return (channelid, rankid)
def amp43(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 28) & 0x03
    return (channelid, rankid)
def amp44(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 30) & 0x03
    return (channelid, rankid)
def amp45(addr):
    channelid = (addr >> 23) & 0x01
    rankid = (addr >> 32) & 0x03
    return (channelid, rankid)


class ThreadMonitor(Thread):
    def __init__(self, cacheSizes, blockSizes, associativities, queue):
        Thread.__init__(self)
        self.p = []
        self.workQueue = queue

        dineroPath = os.path.join(os.path.dirname(os.path.abspath(__file__)), "dinero/dineroIV")

        for a in cacheSizes:
             for b in blockSizes:
                  for c in associativities:
                       dineroConfig = [dineroPath, '-informat', 'd', '-l1-usize', a, '-l1-ubsize', b, '-l1-uassoc', c]
                       self.p.append([Popen(dineroConfig, stdin = PIPE, stdout = PIPE, bufsize = 131072), a, b, c])

    def run(self):
        while True:
            try:
                line = self.workQueue.get()
                self.workQueue.task_done()

                if not line:
                     multipleResult = []

                     for p in self.p:
                          outstream = p[0].communicate("\x00")[0]
                          fetch1 = '0'
                          miss1 = '0'
                          for currentLine in outstream.split("\n"):
                               if currentLine.find('Demand Fetches\t') != -1:
                                    fetch1 = currentLine[18:36].strip()
                               if currentLine.find('Demand miss rate') != -1:
                                    miss1 = currentLine[25:32].strip()

                          p[0].wait()
                          multipleResult.append(Results(fetch1, miss1, p[1], p[2], p[3]))

                     self.workQueue.put(multipleResult)
                     break

                # write this line to each of the simulated caches
                for p in self.p:
                     p[0].stdin.write(line)

            except Exception as errno:
                print "error({0})".format(errno)
                pass


ampTable = {1 : amp01,
            2 : amp02,
            3 : amp03,
            4 : amp04,
            5 : amp05,
            6 : amp06,
            7 : amp07,
            8 : amp08,
            9 : amp09,
            10 : amp10,
            11 : amp11,
            12 : amp12,
            13 : amp13,
            14 : amp14,
            15 : amp15,
            16 : amp16,
            17 : amp17,
            18 : amp18,
            19 : amp19,
            20 : amp20,
            21 : amp21,
            22 : amp22,
            23 : amp23,
            24 : amp24,
            25 : amp25,
            26 : amp26,
            27 : amp27,
            28 : amp28,
            29 : amp29,
            30 : amp30,
            31 : amp31,
            32 : amp32,
            33 : amp33,
            34 : amp34,
            35 : amp35,
            36 : amp36,
            37 : amp37,
            38 : amp38,
            39 : amp39,
            40 : amp40,
            41 : amp41,
            42 : amp42,
            43 : amp43,
            44 : amp44,
            45 : amp45
            }
# ----
# main
# ----   
if __name__ == "__main__":

     benchmark = sys.argv[1]

     addressMappingPolicies = []
     addressMappingPolicies += ['sdramhiperf']
     addressMappingPolicies += ['sdrambase']
     #addressMappingPolicies += ['closepagebaseline']
     #addressMappingPolicies += ['closepagebaselineopt']
     #addressMappingPolicies += ['closepagelowlocality']
     #addressMappingPolicies += ['closepagehighlocality']

     cacheSizes = []
     # cacheSizes += ['4M']
     cacheSizes += ['8M']
     cacheSizes += ['16M']
     # cacheSizes += ['32M']
     # cacheSizes += ['64M']

     blockSizes = []
     blockSizes += ['64']
     blockSizes += ['128']
     #blockSizes += ['256']
     #blockSizes += ['512']
     #blockSizes += ['1024']
     #blockSizes += ['2048']

     associativities = []
     # associativities += ['1']
     # associativities += ['2']
     # associativities += ['4']
     associativities += ['8']
     associativities += ['16']
     # associativities += ['32']
     # associativities += ['64']

     basename = os.path.basename(benchmark)
     basename = basename[0:basename.find('.')]

     workQueue = []
     dimm = []

     for a in range(4 * len(addressMappingPolicies)):
         queue = Queue()
         workQueue.append(queue)
         tm = ThreadMonitor(cacheSizes, blockSizes, associativities, queue)
         dimm.append(tm)
         tm.start()

     l3cache = L3Cache(basename, benchmark, addressMappingPolicies, workQueue)

     l3cache.start()

     l3cache.join()

     for a in dimm:
          a.join()

     resultsArray = []
     for a in workQueue:
          array = a.get()
          a.task_done()
          # append the list of results for this dimm/amp
          resultsArray.append(array)

     for b in range(len(resultsArray[0])):
          val = 0

          for a in range(len(resultsArray)):
              currentResult = resultsArray[a][b]
              print "%s: %s, %s B block, %s-way, dimm #%d: %s fetches, %s miss rate" % (addressMappingPolicies[val / 4], currentResult.cacheSize, currentResult.blockSize, currentResult.associativity, val, currentResult.fetches, currentResult.misses)
              val += 1
