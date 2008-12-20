# Simple test script
#
# "m5 test.py"

import m5
from m5.objects import *
import os, optparse, sys
m5.AddToPath('../common')
import Simulation
from Caches import *
import specbench
from specbench import *
import string

# Get paths we might need.  It's expected this file is in m5/configs/example.
config_path = os.path.dirname(os.path.abspath(__file__))
config_root = os.path.dirname(config_path)
m5_root = os.path.dirname(config_root)

parser = optparse.OptionParser()

# Benchmark options
parser.add_option("-c", "--cmd",
                  default=os.path.join(m5_root, "tests/test-progs/hello/bin/alpha/linux/hello"),
                  help="The binary to run in syscall emulation mode.")
parser.add_option("-o", "--options", default="",
                  help="The options to pass to the binary, use \" \" around the entire\
                        string.")
parser.add_option("-i", "--input", default="",
                  help="A file of input to give to the binary.")
parser.add_option("-f", "--DRAMsimConfig",
		  default=os.path.join(m5_root, "src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml"),
		  help="The DRAMsimII config file.")
parser.add_option("-b", "--benchmark",
					default=0, help="Choose the number from the following:\nperlbench\nbzip2\ngcc\nbwaves\ngamess\nmcf\nmilc\nzeusmp\ngromacs\ncactusADM\nleslie3d\nnamd\ngobmk\ndealII\nsoplex\npovray\ncalculix\nhmmer\nsjeng\nGemsFDTD\nlibquantum\nh264ref\ntonto\nlbm\nomnetpp\nastar\nwrf\nsphinx3\nxalancbmk\n998.specrand_i\n999.specrand_f")


execfile(os.path.join(config_root, "common", "Options.py"))

(options, args) = parser.parse_args()

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

#print options

if options.benchmark != 0:

    if options.benchmark == 'perlbench':
        process = specbench.perlbench
        cmdLine = '400.perlbench'
    elif options.benchmark == 'bzip2':
        process = specbench.bzip2
        cmdLine = '401.bzip2'
    elif options.benchmark == 'gcc':
        process = specbench.gcc
        cmdLine = '403.gcc'
    elif options.benchmark == 'bwaves':
        process = specbench.bwaves
        cmdLine = '410.bwaves'
    elif options.benchmark == 'gamess':
        process = specbench.gamess
        cmdLine = '416.gamess'
    elif options.benchmark == 'mcf':
        process = specbench.mcf
        cmdLine = '429.mcf'
    elif options.benchmark == 'milc':
        process = specbench.milc
        cmdLine = '433.milc'
    elif options.benchmark == 'zeusmp':
        process = specbench.zeusmp
        cmdLine = '434.zeusmp'
    elif options.benchmark == 'gromacs':
        process = specbench.gromacs
        cmdLine = '435.gromacs'
    elif options.benchmark == 'cactusADM':
        process = specbench.cactusADM
        cmdLine = '436.cactusADM'
    elif options.benchmark == 'leslie3d':
        process = specbench.leslie3d
        cmdLine = '437.leslie3d'
    elif options.benchmark == 'namd':
        process = specbench.namd
        cmdLine = '444.namd'
    elif options.benchmark == 'gobmk':
        process = specbench.gobmk;
        cmdLine = '445.gobmk'
    elif options.benchmark == 'dealII':
        process = specbench.dealII
        cmdLine = '447.dealII'
    elif options.benchmark == 'soplex':
        process = specbench.soplex
        cmdLine = '450.soplex'
    elif options.benchmark == 'povray':
        process = specbench.povray
        cmdLine = '453.povray'
    elif options.benchmark == 'calculix':
        process = specbench.calculix
        cmdLine = '454.calculix'
    elif options.benchmark == 'hmmer':
        process = specbench.hmmer
        cmdLine = '456.hmmer'
    elif options.benchmark == 'sjeng':
        process = specbench.sjeng
        cmdLine = '458.sjeng'
    elif options.benchmark == 'GemsFDTD':
        process = specbench.GemsFDTD
        cmdLine = '459.GemsFDTD'
    elif options.benchmark == 'libquantum':
        process = specbench.libquantum
        cmdLine = '462.libquantum'
    elif options.benchmark == 'h264ref':
        process = specbench.h264ref
        cmdLine = '464.h264ref'
    elif options.benchmark == 'tonto':
        process = specbench.tonto
        cmdLine = '465.tonto'
    elif options.benchmark == 'lbm':
        process = specbench.lbm
        cmdLine = '470.lbm'
    elif options.benchmark == 'omnetpp':
        process = specbench.omnetpp
        cmdLine = '471.omnetpp'
    elif options.benchmark == 'astar':
        process = specbench.astar
        cmdLine = '473.astar'
    elif options.benchmark == 'wrf':
        process = specbench.wrf
        cmdLine = '481.wrf'
    elif options.benchmark == 'sphinx3':
        process = specbench.sphinx3
        cmdLine = '482.sphinx'
    elif options.benchmark == 'xalancbmk':
        process = specbench.xalancbmk
        cmdLine = '483.xalancbmk'
    elif options.benchmark == 'specrand_i':
        process = specbench.specrand_i
        cmdLine = '998.specrandI'
    elif options.benchmark == 'specrand_f':
        process = specbench.specrand_f
        cmdLine = '999.specRandF'
    else:
        print "Unknown benchmark.\n"
        sys.exit()

    executable = process.executable.split("/")
	#cmdLine = executable[len(executable) - 1] + " " + string.join(process.cmd[1:]," ") +" <" + process.input
#    cmdLine = options.benchmark
else:
	process = LiveProcess()
	process.executable = options.cmd
	process.cmd = [options.cmd] + options.options.split()
	if options.input != "":
		process.input = options.input

	executable = options.cmd.split("/")

	cmdLine = executable[len(executable) - 1] + " " + options.options + " <" + options.input

options.detailed = True
options.l2cache = True

 #options.stats-file = cmdLine
print options

if options.detailed:
    #check for SMT workload
    workloads = options.cmd.split(';')
    if len(workloads) > 1:
        process = []
        smt_idx = 0
        inputs = []

        if options.input != "":
            inputs = options.input.split(';')

        for wrkld in workloads:
            smt_process = LiveProcess()
            smt_process.executable = wrkld
            smt_process.cmd = wrkld + " " + options.options
            if inputs and inputs[smt_idx]:
                smt_process.input = inputs[smt_idx]
            process += [smt_process, ]
            smt_idx += 1

(CPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)

CPUClass.clock = '2GHz'

np = options.num_cpus


system = System(cpu=[CPUClass(cpu_id=i) for i in xrange(np)],
                #physmem = PhysicalMemory(range=AddrRange("512MB")),
  		#  physmem = M5dramSystem(settingsFile = "/home/crius/m5/src/mem/DRAMSimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml", outFilename = "seNew.gz", range=AddrRange("512MB")),
  		physmem=M5dramSystem(settingsFile=options.DRAMsimConfig, outFilename=executable.pop(), commandLine=cmdLine, range=AddrRange("1024MB")),
                membus=Bus(), mem_mode=test_mem_mode)

system.physmem.port = system.membus.port

for i in xrange(np):
    if options.caches:
        system.cpu[i].addPrivateSplitL1Caches(L1Cache(size='32kB'),
                                              L1Cache(size='64kB'))
    if options.l2cache:
        system.l2 = L2Cache(size='2MB')
        system.tol2bus = Bus()
        system.l2.cpu_side = system.tol2bus.port
        system.l2.mem_side = system.membus.port
        system.cpu[i].connectMemPorts(system.tol2bus)
    else:
        system.cpu[i].connectMemPorts(system.membus)
    system.cpu[i].workload = process

root = Root(system=system)

Simulation.run(options, root, system, FutureClass)
