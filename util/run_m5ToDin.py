# ----------------------------------
# m5 to dinero trace file translator
# ----------------------------------

import os
import sys

if len(sys.argv) < 3:
    print 'usage: python run_m5ToDin.py <m5 trace> <dinero trace>'
    sys.exit()
else:
    m5trace = sys.argv[1]
    dintrace = sys.argv[2]
    commandline = 'python m5ToDin.py ' + m5trace + ' ' + dintrace 

    print 'Command line: %s' %commandline
    os.system(commandline)
    print 'm5 to dinero trace file translation is complete'
