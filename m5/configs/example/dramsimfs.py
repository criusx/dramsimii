# Copyright (c) 2006-2007 The Regents of The University of Michigan
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Ali Saidi

import optparse, os, sys

import m5

os.environ['M5_PATH'] = '/home/crius/m5_system_2.0b3'

if not m5.build_env['FULL_SYSTEM']:
    m5.panic("This script requires full-system mode (*_FS).")

from m5.objects import *
m5.AddToPath('../common')
from FSConfig import *
from SysPaths import *
from Benchmarks import *
import Simulation
from Caches import *

#update Benchmarks to support our benchmarks
Benchmarks['lbm'] = [SysConfig('lbm.rcS', '1024MB')]
Benchmarks['stream'] = [SysConfig('stream.rcS', '512MB')]
Benchmarks['streamLong'] = [SysConfig('streamLong.rcS', '768MB')]
Benchmarks['mcf'] = [SysConfig('mcf.rcS', '2000MB')]
Benchmarks['soplex'] = [SysConfig('soplex.rcS', '768MB')]
Benchmarks['bzip2'] = [SysConfig('bzip2.rcS', '512MB')]
Benchmarks['milc'] = [SysConfig('milc.rcS', '512MB')]
Benchmarks['cactusADM'] = [SysConfig('cactusADM.rcS', '512MB')]
Benchmarks['namd'] = [SysConfig('namd.rcS', '512MB')]
Benchmarks['gobmk'] = [SysConfig('gobmk.rcS', '512MB')]
Benchmarks['dealII'] = [SysConfig('dealII.rcS', '512MB')]
Benchmarks['povray'] = [SysConfig('povray.rcS', '512MB')]
Benchmarks['calculix'] = [SysConfig('calculix.rcS', '512MB')]
Benchmarks['hmmer'] = [SysConfig('hmmer.rcS', '512MB')]
Benchmarks['sjeng'] = [SysConfig('sjeng.rcS', '512MB')]
Benchmarks['GemsFDTD'] = [SysConfig('GemsFDTD.rcS', '512MB')]
Benchmarks['libquantum'] = [SysConfig('libquantum.rcS', '512MB')]
Benchmarks['omnetpp'] = [SysConfig('omnetpp.rcS', '512MB')]
Benchmarks['xalancbmk'] = [SysConfig('xalancbmk.rcS', '512MB')]

benchs = Benchmarks.keys()
benchs.sort()
DefinedBenchmarks = ", ".join(benchs)


def makeDramSimLinuxAlphaSystem(mem_mode, mdesc=None, extraParameters="", settingsFilename="", revert=None):
    class BaseTsunami(Tsunami):
        ethernet = NSGigE(pci_bus=0, pci_dev=1, pci_func=0)
        ide = IdeController(disks=[Parent.disk0, Parent.disk2],
                            pci_func=0, pci_dev=0, pci_bus=0)
    self = LinuxAlphaSystem()
    if not mdesc:
        # generic system
        mdesc = SysConfig()
    self.readfile = mdesc.script()
    self.iobus = Bus(bus_id=0)
    self.membus = MemBus(bus_id=1, clock='2600MHz')
    self.bridge = Bridge(delay='2ns', nack_delay='1ns')
    # use the memory size established by the benchmark definition in Benchmarks.py
    outFile = '' if mdesc.scriptname == None else mdesc.scriptname.split('.')[0]
    if revert is not None:
        print "reverting to use PhysicalMemory"
        self.physmem = PhysicalMemory(range = AddrRange(mdesc.mem()))
    else:
        print "using DRAMsimII"
        self.physmem = M5dramSystem(extraParameters=extraParameters,
                                    settingsFile=settingsFilename,
                                    outFilename=outFile,
                                    commandLine=outFile,
                                    range=AddrRange(mdesc.mem()))
    self.bridge.side_a = self.iobus.port
    self.bridge.side_b = self.membus.port
    self.physmem.port = self.membus.port
    self.disk0 = CowIdeDisk(driveID='master')
    self.disk2 = CowIdeDisk(driveID='master')
    self.disk0.childImage(mdesc.disk())
    self.disk2.childImage(disk('linux-bigswap2.img'))
    self.tsunami = BaseTsunami()
    self.tsunami.attachIO(self.iobus)
    self.tsunami.ide.pio = self.iobus.port
    self.tsunami.ethernet.pio = self.iobus.port
    self.simple_disk = SimpleDisk(disk=RawDiskImage(image_file = mdesc.disk(),
                                                     read_only = True))
    self.intrctrl = IntrControl()
    self.mem_mode = mem_mode
    self.terminal = Terminal()
    self.kernel = binary('vmlinux')
    self.pal = binary('ts_osfpal')
    self.console = binary('console')
    self.boot_osflags = 'root=/dev/hda1 console=ttyS0'

    return self

# Get paths we might need.  It's expected this file is in m5/configs/example.
config_path = os.path.dirname(os.path.abspath(__file__))
config_root = os.path.dirname(config_path)

parser = optparse.OptionParser()

# System options
parser.add_option("--kernel", action="store", type="string")
parser.add_option("--script", action="store", type="string")

# Benchmark options
parser.add_option("--dual", action="store_true",
                  help="Simulate two systems attached with an ethernet link")

parser.add_option("--simple", action="store_true", help="Run in simple mode")

parser.add_option("-b", "--benchmark", action="store", type="string",
                  dest="benchmark",
                  help="Specify the benchmark to run. Available benchmarks: %s"\
                  % DefinedBenchmarks)

# DRAMsimII specific options
parser.add_option("--addmem", default=None, type="string", help="larger than normal memory size")

parser.add_option("-f", "--DRAMsimConfig",
		  default="/home/crius/m5/src/mem/DRAMsimII/memoryDefinitions/DDR2-800-4-4-4-25E.xml",
		  help="The DRAMsimII config file.")

parser.add_option("--mp",
                  default="", help="Override default memory parameters with this switch")

parser.add_option("--revert", action="store_true")

parser.add_option("--nopre", action="store_true")

# Metafile options
parser.add_option("--etherdump", action="store", type="string", dest="etherdump",
                  help="Specify the filename to dump a pcap capture of the" \
                  "ethernet traffic")


execfile(os.path.join(config_root, "common", "Options.py"))

(options, args) = parser.parse_args()

if options.simple:
    options.l2cache = False
    options.caches = False
    options.detailed = False
else:
    options.l2cache = True
    options.caches = True
    options.detailed = True
    #if options.fast-forward is None:
    #    options.fast-forward = "10000000000"

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

# driver system CPU is always simple... note this is an assignment of
# a class, not an instance.
DriveCPUClass = AtomicSimpleCPU
drive_mem_mode = 'atomic'

# system under test can be any CPU
(TestCPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)

TestCPUClass.clock = '3GHz'
DriveCPUClass.clock = '3GHz'

if options.benchmark:
    try:
        bm = Benchmarks[options.benchmark]
    except KeyError:
        print "Error benchmark %s has not been defined." % options.benchmark
        print "Valid benchmarks are: %s" % DefinedBenchmarks
        sys.exit(1)
else:
    if options.dual:
        bm = [SysConfig(), SysConfig()]
    else:
        bm = [SysConfig()]

np = options.num_cpus

if options.addmem:
    print 'set mem = ' + options.addmem
    bm[0].memsize = options.addmem


if m5.build_env['TARGET_ISA'] == "alpha":
    test_sys = makeDramSimLinuxAlphaSystem(test_mem_mode, bm[0], options.mp, options.DRAMsimConfig, options.revert)
elif m5.build_env['TARGET_ISA'] == "mips":
    test_sys = makeLinuxMipsSystem(test_mem_mode, bm[0])
elif m5.build_env['TARGET_ISA'] == "sparc":
    test_sys = makeSparcSystem(test_mem_mode, bm[0])
elif m5.build_env['TARGET_ISA'] == "x86":
    test_sys = makeLinuxX86System(test_mem_mode, bm[0])
else:
    m5.panic("incapable of building non-alpha or non-sparc full system!")

if options.kernel is not None:
    test_sys.kernel = binary(options.kernel)

if options.script is not None:
    test_sys.readfile = options.script



if options.l2cache:
    if options.nopre is True:
        print "no prefetcher"
        test_sys.l2 = L2Cache(size='1MB', assoc=16, latency="7ns", mshrs = 32)
    else:
        #test_sys.l2 = L2Cache(size='1MB', assoc=16, latency="7ns", mshrs=32, prefetch_policy='ghb', prefetch_degree=3, prefetcher_size=256, tgts_per_mshr=24, prefetch_cache_check_push=False)
        #test_sys.l2 = L2Cache(size='1MB', assoc=16, latency="7ns", mshrs=32, prefetch_policy='stride', prefetch_degree=2, prefetcher_size=64, prefetch_cache_check_push=True)
        test_sys.l2 = L2Cache(size='1MB', assoc=16, latency="7ns", mshrs=32, prefetch_policy='ghb', prefetch_degree=2, prefetcher_size=16)


    #test_sys.l2 = L2Cache(size = '2MB')
    test_sys.tol2bus = Bus()
    test_sys.l2.cpu_side = test_sys.tol2bus.port
    test_sys.l2.mem_side = test_sys.membus.port

test_sys.cpu = [TestCPUClass(cpu_id=i) for i in xrange(np)]

if options.caches:
    test_sys.bridge.filter_ranges_a=[AddrRange(0, Addr.max)]
    test_sys.bridge.filter_ranges_b=[AddrRange(0, size='8GB')]
    test_sys.iocache = IOCache(addr_range=AddrRange(0, size='8GB'))
    test_sys.iocache.cpu_side = test_sys.iobus.port
    test_sys.iocache.mem_side = test_sys.membus.port

for i in xrange(np):
    if options.caches:
        test_sys.cpu[i].addPrivateSplitL1Caches(L1Cache(size = '64kB'),
                                                L1Cache(size = '64kB'))
    if options.l2cache:
        test_sys.cpu[i].connectMemPorts(test_sys.tol2bus)
    else:
        test_sys.cpu[i].connectMemPorts(test_sys.membus)

    if options.fastmem:
        test_sys.cpu[i].physmem_port = test_sys.physmem.port

if m5.build_env['TARGET_ISA'] == 'mips':
    setMipsOptions(TestCPUClass)

if len(bm) == 2:
    if m5.build_env['TARGET_ISA'] == 'alpha':
        drive_sys = makeLinuxAlphaSystem(drive_mem_mode, bm[1])
    elif m5.build_env['TARGET_ISA'] == 'mips':
        drive_sys = makeLinuxMipsSystem(drive_mem_mode, bm[1])
    elif m5.build_env['TARGET_ISA'] == 'sparc':
        drive_sys = makeSparcSystem(drive_mem_mode, bm[1])
    elif m5.build.env['TARGET_ISA'] == 'x86':
        drive_sys = makeX86System(drive_mem_mode, bm[1])
    drive_sys.cpu = DriveCPUClass(cpu_id=0)
    drive_sys.cpu.connectMemPorts(drive_sys.membus)
    if options.fastmem:
        drive_sys.cpu.physmem_port = drive_sys.physmem.port
    if options.kernel is not None:
        drive_sys.kernel = binary(options.kernel)

    root = makeDualRoot(test_sys, drive_sys, options.etherdump)
elif len(bm) == 1:
    root = Root(system=test_sys)
else:
    print "Error I don't know how to create more than 2 systems."
    sys.exit(1)

Simulation.run(options, root, test_sys, FutureClass)
