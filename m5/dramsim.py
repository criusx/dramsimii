# Simple test script
#
# "m5 test.py"

import m5
from m5.objects import *
import os, optparse, sys
m5.AddToPath('../common')
import Simulation
from Caches import *

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
		  default=os.path.join(m5_root,"src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml"),
		  help="The DRAMsimII config file.")

execfile(os.path.join(config_root, "common", "Options.py"))

(options, args) = parser.parse_args()

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

process = LiveProcess()
process.executable = options.cmd
process.cmd = [options.cmd] + options.options.split()
if options.input != "":
    process.input = options.input

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

executable = options.cmd.split("/")

cmdLine = executable[len(executable) - 1] + " " + options.options + " <" + options.input

system = System(cpu = [CPUClass(cpu_id=i) for i in xrange(np)],
                #physmem = PhysicalMemory(range=AddrRange("512MB")),
  		#  physmem = M5dramSystem(settingsFile = "/home/crius/m5/src/mem/DRAMSimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml", outFilename = "seNew.gz", range=AddrRange("512MB")),	
  		physmem = M5dramSystem(settingsFile = options.DRAMsimConfig, outFilename = executable.pop(), commandLine = cmdLine, range=AddrRange("512MB")),
                membus = Bus(), mem_mode = test_mem_mode)

system.physmem.port = system.membus.port

for i in xrange(np):    
    if options.caches:
        system.cpu[i].addPrivateSplitL1Caches(L1Cache(size = '32kB'),
                                              L1Cache(size = '64kB'))
    if options.l2cache:
        system.l2 = L2Cache(size='2MB')
        system.tol2bus = Bus()
        system.l2.cpu_side = system.tol2bus.port
        system.l2.mem_side = system.membus.port
        system.cpu[i].connectMemPorts(system.tol2bus)
    else:
        system.cpu[i].connectMemPorts(system.membus)
    system.cpu[i].workload = process

root = Root(system = system)

Simulation.run(options, root, system, FutureClass)
