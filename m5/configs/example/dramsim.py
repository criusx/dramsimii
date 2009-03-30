# Simple test script
#
# "m5 test.py"

import m5
from m5.objects import *
import os, optparse, sys
m5.AddToPath('../common')
import Simulation
from Caches import *
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
		  default=os.path.join(m5_root, "/home/crius/m5/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml"),
		  help="The DRAMsimII config file.")

parser.add_option("-b", "--benchmark",
					default=None, help="Choose the number from the following:\nperlbench\nbzip2\ngcc\nbwaves\ngamess\nmcf\nmilc\nzeusmp\ngromacs\ncactusADM\nleslie3d\nnamd\ngobmk\ndealII\nsoplex\npovray\ncalculix\nhmmer\nsjeng\nGemsFDTD\nlibquantum\nh264ref\ntonto\nlbm\nomnetpp\nastar\nwrf\nsphinx3\nxalancbmk\n998.specrand_i\n999.specrand_f")

parser.add_option("--simple", action="store_true")

parser.add_option("--mp",
                  default="", help="Override default memory parameters with this switch")
parser.add_option("--revert", default=None)


execfile(os.path.join(config_root, "common", "Options.py"))

(options, args) = parser.parse_args()

if options.simple == True:
    options.detailed = False
    options.l2cache = False
    options.caches = False
else:
    options.detailed = True
    options.l2cache = True
    options.caches = True

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

memorySize = '512MB'

if options.benchmark is not None:
    try:
        cmdLine = cmdLineDict[options.benchmark]
        process = liveProcessDict[options.benchmark]
        executable = [cmdLineDict[options.benchmark]]
        if options.benchmark in alternateMemorySize:
            memorySize = alternateMemorySize[options.benchmark]

    except KeyError:
        print "Unknown benchmark.\n"
        sys.exit()
else:
    process = LiveProcess()
    process.executable = options.cmd
    process.cmd = [options.cmd] + options.options.split()
    if options.input != "":
        process.input = options.input

    executable = options.cmd.split("/")

    if options.input != "":
        cmdInput = " <%s" % options.input
    else:
        cmdInput = ""
    cmdLine = executable[len(executable) - 1] + " " + options.options + cmdInput


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

CPUClass.clock = '3GHz'

np = options.num_cpus

if options.revert:
    print 'not using ds2'
    system = System(cpu=[CPUClass(cpu_id=i) for i in xrange(np)],
                    physmem= PhysicalMemory(range=AddrRange(memorySize)),
                           membus=Bus(), mem_mode=test_mem_mode)
else:
    system = System(cpu=[CPUClass(cpu_id=i) for i in xrange(np)],
                    physmem=M5dramSystem(extraParameters=options.mp,
                           settingsFile=options.DRAMsimConfig,
                           outFilename=executable.pop(),
                           commandLine=cmdLine,
                           range=AddrRange(memorySize)),
                           membus=Bus(), mem_mode=test_mem_mode)

system.physmem.port = system.membus.port

for i in xrange(np):
    if options.caches:
        system.cpu[i].addPrivateSplitL1Caches(L1Cache(size='64kB', assoc=2),
                                              L1Cache(size='64kB', assoc=2))
    if options.l2cache:
        system.l2 = L2Cache(size='1MB', assoc=16, latency="7ns", mshrs=32, prefetch_policy='ghb', prefetch_degree=3, prefetcher_size=256, tgts_per_mshr=24, prefetch_cache_check_push=False)
        #system.l2 = L2Cache(size='1MB', assoc=16, latency="7ns", mshrs = 32, prefetch_policy = 'none', prefetch_degree = 3, prefetcher_size = 256, tgts_per_mshr=24, prefetch_cache_check_push=False)
        system.tol2bus = Bus()
        system.l2.cpu_side = system.tol2bus.port
        system.l2.mem_side = system.membus.port
        system.cpu[i].connectMemPorts(system.tol2bus)
    else:
        system.cpu[i].connectMemPorts(system.membus)
    system.cpu[i].workload = process

root = Root(system=system)

Simulation.run(options, root, system, FutureClass)
