#!/usr/bin/python -O

import sys
import os

benchmark = sys.argv[1]

os.system('mkdir stats/' + benchmark)

# ----------------------------------
# m5 to dinero trace file translator
# ----------------------------------
m5todin_commandline = 'python m5ToDin.py ../traces/m5_format/' + benchmark + 'Stream.gz ../traces/dinero_format/' + benchmark + '.din'
print 'command line: %s' %m5todin_commandline
print 'm5 to dinero trace file translation begins...'
os.system(m5todin_commandline)
print 'done'


# ----------------------------------
# run dineroIV to get l3 cache stats
# ----------------------------------
l3stats_commandline = './dineroIV -informat d -l1-usize 16M -l1-ubsize 64 -l1-uassoc 16 -l1-upfdist 3 < ../traces/dinero_format/' + benchmark +'.din > stats/' + benchmark + '/' + benchmark + '.l3.txt'
print 'command line: %s' %l3stats_commandline
print 'getting l3 miss rate...'
os.system(l3stats_commandline)
print 'done'


# ---------------------------------------------------
# run dineroIV-w-misstrace to generate L3 miss traces
# ---------------------------------------------------
l3miss_commandline = './dineroIV-w-misstrace -informat d -l1-usize 16M -l1-ubsize 64 -l1-uassoc 16 -l1-upfdist 3 < ../traces/dinero_format/' + benchmark + '.din > ../traces/dinero_format/miss/' + benchmark + '-miss'
print 'command line: %s' %l3miss_commandline
print 'generating l3 miss traces...'
os.system(l3miss_commandline)
print 'done'


# ----------------------------------------------
# generate miss trace file for ds2 and dinero l4
# ----------------------------------------------
dintods2_commandline = 'python dinToDs2.py ../traces/dinero_format/miss/' + benchmark + '-miss ../traces/ds2_format/' + benchmark + '-miss.trace ../traces/dinero_format/' + benchmark + '.l4.din'
print 'command line: %s' %dintods2_commandline
print 'generating traces for ds2 and dinero l4...'
os.system(dintods2_commandline)
print 'done'


# ----------------------------------
# run dineroIV to get l4 cache stats
# ----------------------------------
#print 'getting l4 miss rate...'

cacheSize = []
cacheSize += ['16M']
cacheSize += ['32M']
cacheSize += ['64M']
cacheSize += ['128M']
cacheSize += ['256M']

blockSize = []
blockSize += ['64']
blockSize += ['128']
blockSize += ['256']
blockSize += ['512']
blockSize += ['1024']
blockSize += ['2048']

associativity = []
associativity += ['1']
associativity += ['2']
associativity += ['4']
associativity += ['8']
associativity += ['16']
associativity += ['32']
associativity += ['64']

result = 'stats/' + benchmark + '/' + benchmark + '.l4.txt'
fout = open(result, 'w')

# sweep cache size
blocksize = '64'
assoc = '16'
for cachesize in cacheSize:
    l4stats_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark +'.l4.din > stats/' + benchmark + '.' + cachesize + '.' + blocksize + '.' + assoc
    os.system(l4stats_commandline)

    stat = 'stats/' + benchmark + '.' + cachesize + '.' + blocksize + '.' + assoc

    f = open(stat, 'r')

    for line in f:
        if line.find('Demand Fetches\t') != -1:
            demandfetches = line[18:36]            
        if line.find('Demand miss rate') != -1:
            missrate = line[25:32]

    print >>fout, '%s %s %s %s %s' %(cachesize, blocksize, assoc, missrate, demandfetches)

    f.close()

# sweep block size
cachesize = '64M'
assoc = '16'
for blocksize in blockSize:
    l4stats_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark +'.l4.din > stats/' + benchmark + '.' + cachesize + '.' + blocksize + '.' + assoc
    os.system(l4stats_commandline)

    stat = 'stats/' + benchmark + '.' + cachesize + '.' + blocksize + '.' + assoc

    f = open(stat, 'r')

    for line in f:
        if line.find('Demand Fetches\t') != -1:
            demandfetches = line[18:36]
        if line.find('Demand miss rate') != -1:
            missrate = line[25:32]

    print >>fout, '%s %s %s %s %s' %(cachesize, blocksize, assoc, missrate, demandfetches)

    f.close()

# sweep associativity
cachesize = '64M'
blocksize = '16'
for assoc in associativity:
    l4stats_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark +'.l4.din > stats/' + benchmark + '.' + cachesize + '.' + blocksize + '.' + assoc
    os.system(l4stats_commandline)

    stat = 'stats/' + benchmark + '.' + cachesize + '.' + blocksize + '.' + assoc

    f = open(stat, 'r')

    for line in f:
        if line.find('Demand Fetches\t') != -1:
            demandfetches = line[18:36]
        if line.find('Demand miss rate') != -1:
            missrate = line[25:32]

    print >>fout, '%s %s %s %s %s' %(cachesize, blocksize, assoc, missrate, demandfetches)

    f.close()

fout.close()

print 'done'


# ----------------------------------------
# generate miss trace file for dimm caches
# address mapping policy options:
# SDRAM_HIPERF_MAP
# SDRAM_BASE_MAP
# CLOSE_PAGE_BASELINE
# CLOSE_PAGE_BASELINE_OPT
# CLOSE_PAGE_LOW_LOCALITY
# CLOSE_PAGE_HIGH_LOCALITY
# ----------------------------------------
addressMappingPolicy = []
addressMappingPolicy += ['sdramhiperf']
addressMappingPolicy += ['sdrambase']
addressMappingPolicy += ['closepagebaseline']
addressMappingPolicy += ['closepagebaselineopt']
addressMappingPolicy += ['closepagelowlocality']
addressMappingPolicy += ['closepagehighlocality']
addressMappingPolicy += ['mutien']

for addressmapping in addressMappingPolicy:
    dinmiss_commandline = 'python dinMiss.py ../traces/dinero_format/miss/' + benchmark + '-miss ../traces/dinero_format/' + benchmark + '.dimm.din ' + addressmapping
    print 'command line: %s' %dinmiss_commandline
    print 'generating miss traces for dimm caches...'
    os.system(dinmiss_commandline)
    print 'done'


# -------------------------------
# run dineroIV to sim dimm caches
# -------------------------------
cacheSize = []
cacheSize += ['4M']
cacheSize += ['8M']
cacheSize += ['16M']
cacheSize += ['32M']
cacheSize += ['64M']

blockSize = []
blockSize += ['64']
blockSize += ['128']
blockSize += ['256']
blockSize += ['512']
blockSize += ['1024']
blockSize += ['2048']

associativity = []
associativity += ['1']
associativity += ['2']
associativity += ['4']
associativity += ['8']
associativity += ['16']
associativity += ['32']
associativity += ['64']

# sweep address mapping policies
for addressmapping in addressMappingPolicy:
    result = 'stats/' + benchmark + '/' + benchmark + '.dimm.' + addressmapping + '.txt'
    fout = open(result, 'w')

    # sweep cache size
    blocksize = '64'
    assoc = '16'
    for cachesize in cacheSize:
        dimmcache0_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.0  > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.0'
        dimmcache1_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.1 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.1'
        dimmcache2_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.2 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.2'
        dimmcache3_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.3 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.3'

        os.system(dimmcache0_commandline)
        os.system(dimmcache1_commandline)
        os.system(dimmcache2_commandline)
        os.system(dimmcache3_commandline)

        stat0 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.0'
        stat1 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.1'
        stat2 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.2'
        stat3 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.3'

        f0 = open(stat0, 'r')
        f1 = open(stat1, 'r')
        f2 = open(stat2, 'r')
        f3 = open(stat3, 'r')

        for line in f0:
            if line.find('Demand Fetches\t') != -1:
                demandfetches0 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate0 = line[25:32]

        for line in f1:
            if line.find('Demand Fetches\t') != -1:
                demandfetches1 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate1 = line[25:32]

        for line in f2:
            if line.find('Demand Fetches\t') != -1:
                demandfetches2 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate2 = line[25:32]

        for line in f3:
            if line.find('Demand Fetches\t') != -1:
                demandfetches3 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate3 = line[25:32]

        print >>fout, '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, missrate0, missrate1, missrate2, missrate3, demandfetches0, demandfetches1, demandfetches2, demandfetches3)
        print '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, missrate0, missrate1, missrate2, missrate3, demandfetches0, demandfetches1, demandfetches2, demandfetches3)

        f0.close()
        f1.close()
        f2.close()
        f3.close()

    # sweep block size
    cachesize = '8M'
    assoc = '16'
    for blocksize in blockSize:
        dimmcache0_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.0  > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.0'
        dimmcache1_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.1 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.1'
        dimmcache2_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.2 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.2'
        dimmcache3_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.3 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.3'

        os.system(dimmcache0_commandline)
        os.system(dimmcache1_commandline)
        os.system(dimmcache2_commandline)
        os.system(dimmcache3_commandline)

        stat0 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.0'
        stat1 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.1'
        stat2 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.2'
        stat3 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.3'

        f0 = open(stat0, 'r')
        f1 = open(stat1, 'r')
        f2 = open(stat2, 'r')
        f3 = open(stat3, 'r')

        for line in f0:
            if line.find('Demand Fetches\t') != -1:
                demandfetches0 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate0 = line[25:32]

        for line in f1:
            if line.find('Demand Fetches\t') != -1:
                demandfetches1 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate1 = line[25:32]

        for line in f2:
            if line.find('Demand Fetches\t') != -1:
                demandfetches2 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate2 = line[25:32]

        for line in f3:
            if line.find('Demand Fetches\t') != -1:
                demandfetches3 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate3 = line[25:32]

        print >>fout, '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, missrate0, missrate1, missrate2, missrate3, demandfetches0, demandfetches1, demandfetches2, demandfetches3)
        print '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, missrate0, missrate1, missrate2, missrate3, demandfetches0, demandfetches1, demandfetches2, demandfetches3)

        f0.close()
        f1.close()
        f2.close()
        f3.close()

    # sweep associativity
    cachesize = '8M'
    blocksize = '64'
    for assoc in associativity:
        dimmcache0_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.0  > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.0'
        dimmcache1_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.1 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.1'
        dimmcache2_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.2 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.2'
        dimmcache3_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < ../traces/dinero_format/' + benchmark + '.dimm.din.' + addressmapping + '.3 > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.3'

        os.system(dimmcache0_commandline)
        os.system(dimmcache1_commandline)
        os.system(dimmcache2_commandline)
        os.system(dimmcache3_commandline)

        stat0 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.0'
        stat1 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.1'
        stat2 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.2'
        stat3 = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.3'

        f0 = open(stat0, 'r')
        f1 = open(stat1, 'r')
        f2 = open(stat2, 'r')
        f3 = open(stat3, 'r')

        for line in f0:
            if line.find('Demand Fetches\t') != -1:
                demandfetches0 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate0 = line[25:32]

        for line in f1:
            if line.find('Demand Fetches\t') != -1:
                demandfetches1 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate1 = line[25:32]

        for line in f2:
            if line.find('Demand Fetches\t') != -1:
                demandfetches2 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate2 = line[25:32]

        for line in f3:
            if line.find('Demand Fetches\t') != -1:
                demandfetches3 = line[18:36]

            if line.find('Demand miss rate') != -1:
                missrate3 = line[25:32]

        print >>fout, '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, missrate0, missrate1, missrate2, missrate3, demandfetches0, demandfetches1, demandfetches2, demandfetches3)
        print '%s %s %s %s %s %s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, missrate0, missrate1, missrate2, missrate3, demandfetches0, demandfetches1, demandfetches2, demandfetches3)

        f0.close()
        f1.close()
        f2.close()
        f3.close()

    fout.close()
