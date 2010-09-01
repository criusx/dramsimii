miss = 'lbm_1.din.miss'

try:
    f = open(miss, 'r')
except:
    print miss + 'does not exists'

freq = 4

flag = 0
for line in f:
    begins = "begins"
    complete = "complete"
    findbegins = line.find(begins)
    findcomplete = line.find(complete)

    if findcomplete != -1:
        flag = 0

    if flag == 1:
        l = len(line)
        space = " "
        findspace = line.find(space, 2, l)
        
        if line[0] == '0':
            req = 'R'
        else:
            req = 'W'

        addr = line[2:findspace]
        time = float(line[findspace:l-1]) / freq 

        print "%s %s %f %s" %(addr, req, time, '0')

    if findbegins != -1:
        flag = 1
