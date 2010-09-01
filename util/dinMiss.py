import sys

freq = 4

misstrace = sys.argv[1]
ds2trace = sys.argv[2] 
dimmcachetrace = sys.argv[3]
addressmapping = sys.argv[4]

# open miss trace file from dinero for read
try:
    fin = open(misstrace, 'r')
except:
    print misstrace + ' does not exists'

# open miss trace file for write in dramsimii foramt
try:
    fout = open(ds2trace, 'w')
except:
    print ds2trace + ' does not exists'

# open miss trace file for write for dimm caches
fout0 = open(dimmcachetrace + '.' + addressmapping + '.0', 'w')
fout1 = open(dimmcachetrace + '.' + addressmapping + '.1', 'w')
fout2 = open(dimmcachetrace + '.' + addressmapping + '.2', 'w')
fout3 = open(dimmcachetrace + '.' + addressmapping + '.3', 'w')


flag = 0
for line in fin:
    if line.find("Simulation complete") != -1:
        flag = 0

    if flag == 1:
        l = len(line)
        findspace = line.find(' ', 2, l)

        req = line[0]
        addr = line[2:findspace]
        time = float(line[findspace:l-1]) / freq

        # -----------------------------------
        # format translation -- dinero to ds2
        # -----------------------------------
        #if req == '0':
        #    print >>fout, "%s %s %f %s" %(addr, 'R', time, '0')
        #elif req == '1':
        #    print >>fout, "%s %s %f %s" %(addr, 'W', time, '0')

        # ----------------------
        # dinero miss trace file
        # ----------------------
        #print >>fout, "%s %s %f" %(req, addr, time)

        # ---------------
        # address mapping
        # ---------------

        # format address to complete 36 bits notation
        addr_len = findspace - 2
        
        if addr_len == 1:
            address = "00000000" + addr
        elif addr_len == 2:
            address = "0000000" + addr
        elif addr_len == 3:
            address = "000000" + addr
        elif addr_len == 4:
            address = "00000" + addr
        elif addr_len == 5:
            address = "0000" + addr
        elif addr_len == 6:
            address = "000" + addr
        elif addr_len == 7:
            address = "00" + addr
        elif addr_len == 8:
            address = "0" + addr
        else:
            address = addr[addr_len-9:addr_len]

        # hex to bin
        address_b = [0, 0, 0, 0, 0, 0, 0, 0, 0]

        i = 0
        while i < 9:
            if address[i] == '0':
                address_b[i] = "0000"
            elif address[i] == '1':
                address_b[i] = "0001"
            elif address[i] == '2':
                address_b[i] = "0010"
            elif address[i] == '3':
                address_b[i] = "0011"
            elif address[i] == '4':
                address_b[i] = "0100"
            elif address[i] == '5':
                address_b[i] = "0101"
            elif address[i] == '6':
                address_b[i] = "0110"
            elif address[i] == '7':
                address_b[i] = "0111"
            elif address[i] == '8':
                address_b[i] = "1000"
            elif address[i] == '9':
                address_b[i] = "1001"
            elif address[i] == 'a':
                address_b[i] = "1010"
            elif address[i] == 'b':
                address_b[i] = "1011"
            elif address[i] == 'c':
                address_b[i] = "1100"
            elif address[i] == 'd':
                address_b[i] = "1101"
            elif address[i] == 'e':
                address_b[i] = "1110"
            elif address[i] == 'f':
                address_b[i] = "1111"
            i += 1

        # binary notation
        address_bin = ''

        i = 0
        while i < 9:
            address_bin = address_bin + address_b[i]
            i += 1

        # set channel id and rank id
	if addressmapping == "SDRAM_HIPERF_MAP":
            channelid = address_bin[29]
            rankid = address_bin[16:18]
        elif addressmapping == "SDRAM_BASE_MAP":
            channelid = address_bin[29]
            rankid = address_bin[2:4]
	elif addressmapping == "CLOSE_PAGE_BASELINE":
            channelid = address_bin[29]
            rankid = address_bin[23:25]
	elif addressmapping == "CLOSE_PAGE_BASELINE_OPT":
            channelid = address_bin[29]
            rankid = address_bin[20:22]
	elif addressmapping == "CLOSE_PAGE_LOW_LOCALITY":
            channelid = address_bin[32]
            rankid = address_bin[30:32]
	elif addressmapping == "CLOSE_PAGE_HIGH_LOCALITY":
            channelid = address_bin[8]
            rankid = address_bin[2:4]

	#print "%s %s %s %s %s" %(line, addr, address_bin, channelid, rankid)

        # print files
        if channelid == '0':
            if rankid == "00" or rankid == "01":
                fout0.write(req + ' ' + addr + ' 0' + '\n')
            elif rankid == "10" or rankid == "11":
                fout1.write(req + ' ' + addr + ' 0' + '\n')
        elif channelid == '1':
            if rankid == "00" or rankid == "01":
                fout2.write(req + ' ' + addr + ' 0' + '\n')
            elif rankid == "10" or rankid == "11":
                fout3.write(req + ' ' + addr + ' 0' + '\n')

    if line.find("Simulation begins") != -1:
        flag = 1

fin.close()
fout.close()
fout0.close()
fout1.close()
fout2.close()
fout3.close()
