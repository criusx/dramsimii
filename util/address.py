# SDRAM higher performance address mapping policy
# SDRAM_HIPERF_MAP

tracein = 'lbm_1-miss.trace'
traceout0 = 'lbm_1-miss0.din'
traceout1 = 'lbm_1-miss1.din'
traceout2 = 'lbm_1-miss2.din'
traceout3 = 'lbm_1-miss3.din'

fin = open(tracein, 'r')
fout0 = open(traceout0, 'w')
fout1 = open(traceout1, 'w')
fout2 = open(traceout2, 'w')
fout3 = open(traceout3, 'w')

address = "00000"
address_dec = [0, 0, 0, 0, 0]
channelid_course = 3
channelid_fine = 3
rankid_course = 0
rankid_fine = 2

for line in fin:
    space = ' '
    findspace = line.find(space)

    # request
    if line[findspace+1] == 'R':
        req = '0'
    elif line[findspace+1] == 'W':
        req = '1'

    # real address
    addr = line[0:findspace]

    # get address portion to find channel and rank ids
    if findspace == 0:
        address = "00000"
    elif findspace == 1:
        address = "0000" + line[0:findspace]
    elif findspace == 2:
        address = "000" + line[0:findspace]
    elif findspace == 3:
        address = "00" + line[0:findspace]
    elif findspace == 4:
        address = '0' + line[0:findspace]
    else:
        address = line[findspace-5:findspace]

    # hex to dec
    i = 0
    while i < 5:
        if address[i] == 'a':
            address_dec[i] = '10'
        elif address[i] == 'b':
            address_dec[i] = '11'
        elif address[i] == 'c':
            address_dec[i] = '12'
        elif address[i] == 'd':
            address_dec[i] = '13'
        elif address[i] == 'e':
            address_dec[i] = '14'
        elif address[i] == 'f':
            address_dec[i] = '15'
        else:
            address_dec[i] = address[i]
        i += 1

    # get channel and rank ids
    channelid = int(address_dec[channelid_course]) >> channelid_fine
    rankid = int(address_dec[rankid_course]) >> rankid_fine
    
    # print files
    if channelid == 0:
        if rankid == 0 or rankid == 1:
            fout0.write(req + ' ' + addr + ' 0' + '\n')
        elif rankid == 2 or rankid == 3:
            fout1.write(req + ' ' + addr + ' 0' + '\n')
    elif channelid == 1:
        if rankid == 0 or rankid == 1:
            fout2.write(req + ' ' + addr + ' 0' + '\n')
        elif rankid == 2 or rankid == 3:
            fout3.write(req + ' ' + addr + ' 0' + '\n')

fin.close()
fout0.close()
fout1.close()
fout2.close()
fout3.close()
