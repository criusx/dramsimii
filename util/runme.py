import sys
import os

benchmark = sys.argv[1]

# ----------------------------------
# m5 to dinero trace file translator
# ----------------------------------
#m5todin_commandline = 'python m5ToDin.py trace/' + benchmark + 'Stream trace/' + benchmark + '.din'
#print 'command line: %s' %m5todin_commandline
#print 'm5 to dinero trace file translation begins...'
#os.system(m5todin_commandline)
#print 'done'


# ----------------------------------
# run dineroIV to get l3 cache stats
# ----------------------------------
#l3stats_commandline = './dineroIV -informat d -l1-usize 16M -l1-ubsize 64 -l1-uassoc 16 -l1-upfdist 3 < trace/' + benchmark +'.din > stats/' + benchmark + '.l3.txt'
#print 'command line: %s' %l3stats_commandline
#print 'getting l3 miss rate...'
#os.system(l3stats_commandline)
#print 'done'


# ---------------------------------------------------
# run dineroIV-w-misstrace to generate L3 miss traces
# ---------------------------------------------------
#l3miss_commandline = './dineroIV-w-misstrace -informat d -l1-usize 16M -l1-ubsize 64 -l1-uassoc 16 -l1-upfdist 3 < trace/' + benchmark + '.din > trace/' + benchmark + '-miss'
#print 'command line: %s' %l3miss_commandline
#print 'generating l3 miss traces...'
#os.system(l3miss_commandline)
#print 'done'


# --------------------------------
# generate miss trace file for ds2
# --------------------------------
#dintods2_commandline = 'python dinToDs2.py trace/' + benchmark + '-miss trace/' + benchmark + '-miss.trace' 
#print 'command line: %s' %dintods2_commandline
#print 'generating traces for ds2...'
#os.system(dintods2_commandline)
#print 'done'


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

#for addressmapping in addressMappingPolicy:
#    dinmiss_commandline = 'python dinMiss.py trace/' + benchmark + '-miss trace/' + benchmark + '-miss.din ' + addressmapping
#    print 'command line: %s' %dinmiss_commandline
#    print 'generating miss traces for dimm caches...'
#    os.system(dinmiss_commandline)
#    print 'done'


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

dimmID = ['0', '1', '2', '3']

result = 'stats/result.txt'
fout = open(result, 'w')

for addressmapping in addressMappingPolicy:
    for cachesize in cacheSize:
        for blocksize in blockSize:
            for assoc in associativity:
                for dimmid in dimmID:
                    dimmcache_commandline = './dineroIV -informat d -l1-usize ' + cachesize + ' -l1-ubsize ' + blocksize + ' -l1-uassoc ' + assoc + ' < trace/' + benchmark + '-miss.din.' + addressmapping + '.' + dimmid + ' > stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.' + dimmid
                    os.system(dimmcache_commandline)

                    stat = 'stats/' + benchmark + '.' + addressmapping + '.' + cachesize + '.' + blocksize + '.' + assoc + '.' + dimmid
                    f = open(stat, 'r')

                    for line in f:
                        if line.find('Demand Fetches\t') != -1:
                            demandfetches = line[18:36]

                        if line.find('Demand miss rate') != -1:
                            missrate = line[25:32]

                            print >>fout, '%s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, dimmid, demandfetches, missrate)
                            print '%s %s %s %s %s %s %s' %(addressmapping, cachesize, blocksize, assoc, dimmid, demandfetches, missrate)
                    f.close()
fout.close()



# run dineroIV to get L4 cache stats
#./dineroIV -informat d -l1-usize 64M -l1-ubsize 256 -l1-uassoc 16 < trace/gups-miss.din > stats/l4-cache.txt

