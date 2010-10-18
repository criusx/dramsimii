#!/usr/bin/python -OO

from subprocess import Popen, PIPE, STDOUT
import sys, traceback
import os
from threading import Thread
import time
import gzip
from Queue import Queue
import re

running = True

class Results():

     def __init__(self, fetches, misses, cacheSize, blockSize, associativity):
          self.fetches = fetches
          self.misses = misses
          self.cacheSize = cacheSize
          self.blockSize = blockSize
          self.associativity = associativity

class L3Cache(Thread):
    def __init__(self, benchmark, file, addressMappingPolicies, workQueue, dineroMissPath):
        Thread.__init__(self)

        self.workQueue = workQueue
        self.zcat = Popen(["/bin/zcat", file], shell = False, stdout = PIPE)
        self.dinero = Popen([dineroMissPath, '-informat', 'd', '-l1-usize', '16M', '-l1-ubsize', '64', '-l1-uassoc', '16', '-l1-upfdist', '3'], shell = False, stdin = self.zcat.stdout, stdout = PIPE)

        self.AMP = []

        for addressmapping in addressMappingPolicies:
             # TODO switch these to lambda functions 
             if addressmapping == 'sdramhiperf': self.AMP.append(lambda addr: ((addr >> 6) & 0x01, (addr >> 18) & 0x03))
             elif addressmapping == 'sdrambase': self.AMP.append(lambda addr: ((addr >> 6) & 0x01, (addr >> 32) & 0x03))
             elif addressmapping == 'closepagebaseline': self.AMP.append(lambda addr: ((addr >> 6) & 0x01, (addr >> 11) & 0x03))
             elif addressmapping == 'closepagebaselineopt': self.AMP.append(lambda addr: ((addr >> 6) & 0x01, (addr >> 14) & 0x03))
             elif addressmapping == 'closepagelowlocality': self.AMP.append(lambda addr: ((addr >> 3) & 0x01, (addr >> 4) & 0x03))
             elif addressmapping == 'closepagehighlocality': self.AMP.append(lambda addr: ((addr >> 27) & 0x01, (addr >> 32) & 0x03))
             elif addressmapping == 'amp0a': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 8) & 0x03))
             elif addressmapping == 'amp0b': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 12) & 0x03))
             elif addressmapping == 'amp0c': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 16) & 0x03))
             elif addressmapping == 'amp0d': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 20) & 0x03))
             elif addressmapping == 'amp0e': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 22) & 0x03))
             elif addressmapping == 'amp0f': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 24) & 0x03))
             elif addressmapping == 'amp0g': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 26) & 0x03))
             elif addressmapping == 'amp0h': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 28) & 0x03))
             elif addressmapping == 'amp0i': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 30) & 0x03))
             elif addressmapping == 'amp0j': self.append(lambda addr: ((addr >> 11) & 0x01, (addr >> 32) & 0x03))
             elif addressmapping == 'amp1a': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 8) & 0x03))
             elif addressmapping == 'amp1b': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 12) & 0x03))
             elif addressmapping == 'amp1c': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 16) & 0x03))
             elif addressmapping == 'amp1d': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 20) & 0x03))
             elif addressmapping == 'amp1e': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 22) & 0x03))
             elif addressmapping == 'amp1f': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 24) & 0x03))
             elif addressmapping == 'amp1g': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 26) & 0x03))
             elif addressmapping == 'amp1h': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 28) & 0x03))
             elif addressmapping == 'amp1i': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 30) & 0x03))
             elif addressmapping == 'amp1j': self.append(lambda addr: ((addr >> 15) & 0x01, (addr >> 32) & 0x03))
             elif addressmapping == 'amp2a': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 8) & 0x03))
             elif addressmapping == 'amp2b': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 12) & 0x03))
             elif addressmapping == 'amp2c': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 16) & 0x03))
             elif addressmapping == 'amp2d': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 20) & 0x03))
             elif addressmapping == 'amp2e': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 22) & 0x03))
             elif addressmapping == 'amp2f': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 24) & 0x03))
             elif addressmapping == 'amp2g': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 26) & 0x03))
             elif addressmapping == 'amp2h': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 28) & 0x03))
             elif addressmapping == 'amp2i': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 30) & 0x03))
             elif addressmapping == 'amp2j': self.append(lambda addr: ((addr >> 19) & 0x01, (addr >> 32) & 0x03))
             elif addressmapping == 'amp3a': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 8) & 0x03))
             elif addressmapping == 'amp3b': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 12) & 0x03))
             elif addressmapping == 'amp3c': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 16) & 0x03))
             elif addressmapping == 'amp3d': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 18) & 0x03))
             elif addressmapping == 'amp3e': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 20) & 0x03))
             elif addressmapping == 'amp3f': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 24) & 0x03))
             elif addressmapping == 'amp3g': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 26) & 0x03))
             elif addressmapping == 'amp3h': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 28) & 0x03))
             elif addressmapping == 'amp3i': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 30) & 0x03))
             elif addressmapping == 'amp3j': self.append(lambda addr: ((addr >> 23) & 0x01, (addr >> 32) & 0x03))
             else:
                 print "Unknown address mapping policy: %s" % addressmapping
                 system.exit(-1)

    def run(self):
        #pattern = re.compile("(\d) ([0-9a-f]+) ([\d.]+)")
        pattern = re.compile("\d ([0-9a-f]+) [\d.]+")
        freq = 5

        #count = 0
        global running

        while True:
            newLine = self.dinero.stdout.readline()
            if not newLine or not running:
                if not running:
                     self.zcat.kill()
                     self.dinero.kill()
                # to signal the end of the file
                for a in self.workQueue:
                    a.put(None)
                break

            m = pattern.match(newLine)

            if m is not None:
                #count += 1
                #if count % 10000 == 0:
                #    print "%d\r" % count
                req = m.group(1)
                addr = int(m.group(1), 16)
                #addr = int(m.group(2), 16)
                #time = float(m.group(3)) / freq

                ampNumber = 0
                for a in self.AMP:
                     (channel, rank) = a(addr)

                     #self.workQueue[ampNumber * 4 + channel * 2 + rank / 2].put("%s %s 0\n" % (req, m.group(2)))
                     self.workQueue[ampNumber * 4 + channel * 2 + rank / 2].put(newLine)
                     ampNumber += 1
            else:
               print newLine
               if newLine.find('Demand Fetches\t') != -1:
                   print newLine
               elif newLine.find('Demand miss rate') != -1:
                    print newLine

        if running:
             self.dinero.wait()



class ThreadMonitor(Thread):
    def __init__(self, cacheSizes, blockSizes, associativities, queue, dineroPath):
        Thread.__init__(self)
        self.p = []
        self.workQueue = queue

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
                global running

                if not line or not running:
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

def which(program, expectedDirectory):
    import os
    def is_exe(fpath):
        return os.path.exists(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        array = os.environ["PATH"].split(os.pathsep)
        array.append(expectedDirectory)
        for path in array:
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def main():
     #check for dinero to and make sure it is executable
     dineroPath = which("dineroIV", os.path.join(os.path.dirname(os.path.abspath(__file__)), "dinero"))
     if not dineroPath:
          print "Cannot find dineroIV"
          sys.exit(-1)

     dineroMissPath = which("dineroIV-w-misstrace", os.path.join(os.path.dirname(os.path.abspath(__file__)), "dinero"))
     if not dineroMissPath:
          print "Cannot find dineroIV-w-misstrace"
          sys.exit(-1)

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
     blockSizes += ['256']
     blockSizes += ['512']
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
         tm = ThreadMonitor(cacheSizes, blockSizes, associativities, queue, dineroPath)
         dimm.append(tm)
         tm.start()

     l3cache = L3Cache(basename, benchmark, addressMappingPolicies, workQueue, dineroMissPath)

     l3cache.start()
     try:
          l3cache.join()
     except KeyboardInterrupt:
          print "Caught"
          global running
          running = False
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





# ----
# main
# ----   
if __name__ == "__main__":
    main()
    #import profile
    #profile.run('main()', 'profile.tmp')
    #import pstats
    #p = pstats.Stats('profile.tmp')
    #p.sort_stats('cumulative').print_stats(10)
