#!/usr/bin/env python
# #!/usr/bin/env python -O

from subprocess import Popen, PIPE, STDOUT
import sys, traceback
import os
from threading import Thread
import time
import gzip
from Queue import Queue
import re


class L3Cache(Thread):
    def __init__(self, benchmark, file, addressmapping, workQueue):
        Thread.__init__(self)
        
        self.pattern = re.compile("([0-9]+) ([0-9a-fA-F]+) ([0-9]+)")
        self.freq = 5
        
        l3_config = 'zcat %s | ./dineroIV-w-misstrace -informat d -l1-usize 16M -l1-ubsize 64 -l1-uassoc 16 -l1-upfdist 3' % file
        self.p = Popen([l3_config], shell=True, executable="/bin/bash", stdout=PIPE)
        
        if addressmapping == 'sdramhiperf': self.AMP = 0
        elif addressmapping == 'sdrambase': self.AMP = 1
        elif addressmapping == 'closepagebaseline': self.AMP = 2
        elif addressmapping == 'closepagebaselineopt': self.AMP = 3
        elif addressmapping == 'closepagelowlocality': self.AMP = 4
        elif addressmapping == 'closepagehighlocality': self.AMP = 5
        elif addressmapping == 'amp0a': self.AMP = 6
        elif addressmapping == 'amp0b': self.AMP = 7
        elif addressmapping == 'amp0c': self.AMP = 8
        elif addressmapping == 'amp0d': self.AMP = 9
        elif addressmapping == 'amp0e': self.AMP = 10
        elif addressmapping == 'amp0f': self.AMP = 11
        elif addressmapping == 'amp0g': self.AMP = 12
        elif addressmapping == 'amp0h': self.AMP = 13
        elif addressmapping == 'amp0i': self.AMP = 14
        elif addressmapping == 'amp0j': self.AMP = 15
        elif addressmapping == 'amp1a': self.AMP = 16
        elif addressmapping == 'amp1b': self.AMP = 17
        elif addressmapping == 'amp1c': self.AMP = 18
        elif addressmapping == 'amp1d': self.AMP = 19
        elif addressmapping == 'amp1e': self.AMP = 20
        elif addressmapping == 'amp1f': self.AMP = 21
        elif addressmapping == 'amp1g': self.AMP = 22
        elif addressmapping == 'amp1h': self.AMP = 23
        elif addressmapping == 'amp1i': self.AMP = 24
        elif addressmapping == 'amp1j': self.AMP = 25
        elif addressmapping == 'amp2a': self.AMP = 26
        elif addressmapping == 'amp2b': self.AMP = 27
        elif addressmapping == 'amp2c': self.AMP = 28
        elif addressmapping == 'amp2d': self.AMP = 29
        elif addressmapping == 'amp2e': self.AMP = 30
        elif addressmapping == 'amp2f': self.AMP = 31
        elif addressmapping == 'amp2g': self.AMP = 32
        elif addressmapping == 'amp2h': self.AMP = 33
        elif addressmapping == 'amp2i': self.AMP = 34
        elif addressmapping == 'amp2j': self.AMP = 35
        elif addressmapping == 'amp3a': self.AMP = 36
        elif addressmapping == 'amp3b': self.AMP = 37
        elif addressmapping == 'amp3c': self.AMP = 38
        elif addressmapping == 'amp3d': self.AMP = 39
        elif addressmapping == 'amp3e': self.AMP = 40
        elif addressmapping == 'amp3f': self.AMP = 41
        elif addressmapping == 'amp3g': self.AMP = 42
        elif addressmapping == 'amp3h': self.AMP = 43
        elif addressmapping == 'amp3i': self.AMP = 44
        elif addressmapping == 'amp3j': self.AMP = 45
        else:
            print "Unknown address mapping policy: %s" % addressmapping
            system.exit(-1)
            
    def run(self):
        while True:                
            newLine = self.p.stdout.readline()
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
                (channel, rank) = self.addressDecode(addr)
                
                workQueue[channel * 2 + rank / 2].put("%s %s 0\n" % (req, m.group(2)))
                
                #print "+%d %s %s 0" % (channel  * 2 + rank / 2, req, m.group(2))
                           
        
    def addressDecode(self, addr):
        if self.AMP == 0:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 18) & 0x03
        elif self.AMP == 1:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 32) & 0x03
        elif self.AMP == 2:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 11) & 0x03
        elif self.AMP == 3:
            channelid = (addr >> 6) & 0x01
            rankid = (addr >> 14) & 0x03
        elif self.AMP == 4:
            channelid = (addr >> 3) & 0x01
            rankid = (addr >> 4) & 0x03
        elif self.AMP == 5:
            channelid = (addr >> 27) & 0x01
            rankid = (addr >> 32) & 0x03
        # self.AMP0*
        elif self.AMP == 6:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 8) & 0x03
        elif self.AMP == 7:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 12) & 0x03
        elif self.AMP == 8:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 16) & 0x03
        elif self.AMP == 9:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 20) & 0x03
        elif self.AMP == 10:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 22) & 0x03
        elif self.AMP == 11:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 24) & 0x03
        elif self.AMP == 12:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 26) & 0x03
        elif self.AMP == 13:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 28) & 0x03
        elif self.AMP == 14:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 30) & 0x03
        elif self.AMP == 15:
            channelid = (addr >> 11) & 0x01
            rankid = (addr >> 32) & 0x03
        # self.AMP1*
        elif self.AMP == 16:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 8) & 0x03
        elif self.AMP == 17:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 12) & 0x03
        elif self.AMP == 18:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 16) & 0x03
        elif self.AMP == 19:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 20) & 0x03
        elif self.AMP == 20:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 22) & 0x03
        elif self.AMP == 21:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 24) & 0x03
        elif self.AMP == 22:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 26) & 0x03
        elif self.AMP == 23:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 28) & 0x03
        elif self.AMP == 24:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 30) & 0x03
        elif self.AMP == 25:
            channelid = (addr >> 15) & 0x01
            rankid = (addr >> 32) & 0x03
        # self.AMP2*
        elif self.AMP == 26:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 8) & 0x03
        elif self.AMP == 27:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 12) & 0x03
        elif self.AMP == 28:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 16) & 0x03
        elif self.AMP == 29:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 20) & 0x03
        elif self.AMP == 30:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 22) & 0x03
        elif self.AMP == 31:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 24) & 0x03
        elif self.AMP == 32:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 26) & 0x03
        elif self.AMP == 33:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 28) & 0x03
        elif self.AMP == 34:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 30) & 0x03
        elif self.AMP == 35:
            channelid = (addr >> 19) & 0x01
            rankid = (addr >> 32) & 0x03
        # self.AMP3*
        elif self.AMP == 36:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 8) & 0x03
        elif self.AMP == 37:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 12) & 0x03
        elif self.AMP == 38:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 16) & 0x03
        elif self.AMP == 39:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 18) & 0x03
        elif self.AMP == 40:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 20) & 0x03
        elif self.AMP == 41:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 24) & 0x03
        elif self.AMP == 42:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 26) & 0x03
        elif self.AMP == 43:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 28) & 0x03
        elif self.AMP == 44:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 30) & 0x03
        elif self.AMP == 45:
            channelid = (addr >> 23) & 0x01
            rankid = (addr >> 32) & 0x03
            
        return (channelid, rankid)

class ThreadMonitor(Thread):
    def __init__(self, commandline, number):
        Thread.__init__(self)
        self.commandline = commandline
        self.p = None
        self.number = number
    def run(self):
        #self.p = Popen(self.commandline, stdin=PIPE, stdout=PIPE, bufsize=1048576)
        self.p = Popen(self.commandline, stdin=PIPE, stdout=open("%d.out" % self.number, 'w'))
        #self.p = Popen(self.commandline, stdin=PIPE)
        #output = self.p.communicate(None)[0]
        #self.p.communicate(None)[0]
        while True:
            try:
                line = workQueue[self.number].get()
                workQueue[self.number].task_done()
                
                if not line:
                    outstream = self.p.communicate("")[0]
                    #self.p.stdin.close()
                    #self.p.stdin.flush()
                    
                    #print "%d: %s" % (self.number, outstream)
                    #sys.stdout.flush()
                    #print self.number +self.p.poll()
                    #sys.stdout.flush()
                    #print "%d finishing" % self.number
                    #sys.stdout.flush()
                    #fetch1 = '0'
                    #miss1 = '0'
                    #while True:
                    #    line1 = self.p.stdout.readline()
                    #    print line1
                    #    sys.stdout.flush()
                    #    if not line1: break
                    #print '+',
                    #print output
                    #for line1 in outstream:
                    #    if line1.find('Demand Fetches\t') != -1:
                            #print line1
                    #        fetch1 = line1[18:36].strip()
                    #    if line1.find('Demand miss rate') != -1:
                            #print line1
                   #         miss1 = line1[25:32].strip()
                    
                    #print "%d: %s fetches, %s miss rate" % (self.number, fetch1, miss1)
                    #sys.stdout.flush()
                    print "%d waiting" % self.number
                    sys.stdout.flush()
                    self.p.wait()
                    #print output
                    break
                
                self.p.stdin.write(line)
                #output = self.p.communicate(line)[0]
                #self.p.communicate(line)
            except Exception as errno:
                #traceback.print_exc(file=sys.stdout)

                print "."
                print "error({0})".format(errno)
                #print ".",
                #print Exception.msg
                pass
      
workQueue = []

if __name__ == "__main__":
    
    benchmark = sys.argv[1]
    
    addressMappingPolicy = []
    addressMappingPolicy += ['sdramhiperf']
    #addressMappingPolicy += ['sdrambase']
    #addressMappingPolicy += ['closepagebaseline']
    #addressMappingPolicy += ['closepagebaselineopt']
    #addressMappingPolicy += ['closepagelowlocality']
    #addressMappingPolicy += ['closepagehighlocality']

    cacheSize = []
    # cacheSize += ['4M']
    cacheSize += ['8M']
    # cacheSize += ['16M']
    # cacheSize += ['32M']
    # cacheSize += ['64M']

    blockSize = []
    blockSize += ['64']
    #blockSize += ['128']
    #blockSize += ['256']
    #blockSize += ['512']
    #blockSize += ['1024']
    #blockSize += ['2048']

    associativity = []
    # associativity += ['1']
    # associativity += ['2']
    # associativity += ['4']
    # associativity += ['8']
    associativity += ['16']
    # associativity += ['32']
    # associativity += ['64']
    
   
    # ----
    # main
    # ----
    # === sweep address mapping policies ===
    for addressmapping in addressMappingPolicy:
         basename = os.path.basename(benchmark)
         basename = basename[0:basename.find('.')]
         
        
         
         # === sweep block size ===
         csize = '8M'
         assoc = '16'
         for bsize in blockSize:
              for a in range(4):
                  workQueue.append(Queue())

              dineroPath = os.path.join(os.path.abspath("./dinero"), "dineroIV")
              dimmcache_config = [dineroPath, '-informat', 'd', '-l1-usize', csize, '-l1-ubsize', bsize, '-l1-uassoc', assoc]
              #print dimmcache_config
              l3cache = L3Cache(basename, benchmark, addressmapping, workQueue)
              l3cache.start()
             
              dimm = []

              for number in range(4):
                  tm = ThreadMonitor(dimmcache_config, number)
                  dimm.append(tm)
                  tm.start()
              l3cache.join()

              for a in dimm:
                   print "waiting"
                   a.join()
    # 

    #    print '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, miss0, miss1, miss2, miss3, fetch0, fetch1, fetch2, fetch3)

       
         print 'done'


