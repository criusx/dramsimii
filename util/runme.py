#!/usr/bin/python -O

from subprocess import Popen, PIPE, STDOUT
import sys
import os

benchmark = sys.argv[1]

#os.system('mkdir stats/' + benchmark)


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
    l3_in = 'zcat ../traces/dinero_format/' + benchmark + '.din.gz'
    l3_config = './dineroIV-w-misstrace -informat d -l1-usize 16M -l1-ubsize 64 -l1-uassoc 16 -l1-upfdist 3'
    l3_miss = './dinMiss.py ' + benchmark + ' ' + addressmapping
    l3_commandline = '%s | %s | %s' %(l3_in, l3_config, l3_miss)

    print 'command line: %s' %l3_commandline

    l3_out = Popen([l3_commandline], shell = True, executable = "/bin/bash", stdout = PIPE)

    # === sweep block size ===
    csize = '8M'
    assoc = '16'
    for bsize in blockSize:
        dimmcache_config = './dineroIV -informat d -l1-usize %s -l1-ubsize %s -l1-uassoc %s' %(csize, bsize, assoc)

        dimm = []
        for a in range(4):
            dimm.append(Popen([dimmcache_config], shell = True, executable = '/bin/bash', stdin = PIPE))

        while True:
            line = l3_out.stdout.readline()
            if not line: break

            dimm[int(line[0])].stdin.write(line[2:])




#        if line0.find('Demand Fetches\t') != -1: fetch0 = line0[18:36]
#        if line1.find('Demand Fetches\t') != -1: fetch1 = line1[18:36]
#        if line2.find('Demand Fetches\t') != -1: fetch2 = line2[18:36]
#        if line3.find('Demand Fetches\t') != -1: fetch3 = line3[18:36]
#        if line0.find('Demand miss rate') != -1: miss0 = line0[25:32]
#        if line1.find('Demand miss rate') != -1: miss1 = line1[25:32]
#        if line2.find('Demand miss rate') != -1: miss2 = line2[25:32]
#        if line3.find('Demand miss rate') != -1: miss3 = line3[25:32]
        
#    print '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, miss0, miss1, miss2, miss3, fetch0, fetch1, fetch2, fetch3)

    
    print 'done'


