# Copyright (c) 2010 ARM Limited
# All rights reserved.
#
# The license below extends only to copyright in the software and shall
# not be construed as granting a license to any other intellectual
# property including but not limited to intellectual property relating
# to a hardware implementation of the functionality of the software
# licensed hereunder.  You may use the software subject to the license
# terms below provided that you ensure that this notice is replicated
# unmodified and in its entirety in all distributions of the software,
# modified or unmodified, in source code or in binary form.
#
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

import optparse
import os
import sys

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal

if not buildEnv['FULL_SYSTEM']:
    fatal("This script requires full-system mode (*_FS).")

addToPath('../common')

from FSConfig import *
from SysPaths import *
from Benchmarks import *
import Simulation
import CacheConfig
from Caches import *

# Get paths we might need.  It's expected this file is in m5/configs/example.
config_path = os.path.dirname(os.path.abspath(__file__))
config_root = os.path.dirname(config_path)

parser = optparse.OptionParser()

# System options
parser.add_option("--kernel", action="store", type="string")
parser.add_option("--script", action="store", type="string")
if buildEnv['TARGET_ISA'] == "arm":
    parser.add_option("--bare-metal", action="store_true",
               help="Provide the raw system without the linux specific bits")
    parser.add_option("--machine-type", action="store", type="choice",
            choices=ArmMachineType.map.keys(), default="RealView_PBX")
# Benchmark options
parser.add_option("--dual", action="store_true",
                  help="Simulate two systems attached with an ethernet link")
parser.add_option("-b", "--benchmark", action="store", type="string",
                  dest="benchmark",
                  help="Specify the benchmark to run. Available benchmarks: %s"\
                  % DefinedBenchmarks)

# Metafile options
parser.add_option("--etherdump", action="store", type="string", dest="etherdump",
                  help="Specify the filename to dump a pcap capture of the" \
                  "ethernet traffic")

# DRAMsimII specific options
parser.add_option("-f", "--DRAMsimConfig",
          default="",
          help="The DRAMsimII config file.")

parser.add_option("--mp",
                  default="", help="Override default memory parameters with this switch")

parser.add_option("--revert", action="store_true")

parser.add_option("--nopre", action="store_true")

parser.add_option("--benchmarkName", default="")

parser.add_option("--memsize", default=None)

# more options
execfile(os.path.join(config_root, "common", "Options.py"))

(options, args) = parser.parse_args()

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

# driver system CPU is always simple... note this is an assignment of
# a class, not an instance.
DriveCPUClass = AtomicSimpleCPU
drive_mem_mode = 'atomic'

# system under test can be any CPU
(TestCPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)

TestCPUClass.clock = '5GHz'
DriveCPUClass.clock = '5GHz'

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
	if options.memsize is not None:
            bm = [SysConfig(mem=options.memsize)]
        else:
            bm = [SysConfig(mem="1024MB")]
        #bm = [SysConfig()]

np = options.num_cpus

if buildEnv['TARGET_ISA'] == "alpha":
    if options.revert:
        print "info: using PhysicalMemory"
        test_sys = makeLinuxAlphaSystem(test_mem_mode, bm[0])
    else:
        print "info: using DRAMsimII"        
        test_sys = makeDramSimLinuxAlphaSystem(test_mem_mode, bm[0], options.mp, options.DRAMsimConfig, options.benchmarkName)

elif buildEnv['TARGET_ISA'] == "mips":
    test_sys = makeLinuxMipsSystem(test_mem_mode, bm[0])
elif buildEnv['TARGET_ISA'] == "sparc":
    test_sys = makeSparcSystem(test_mem_mode, bm[0])
elif buildEnv['TARGET_ISA'] == "x86":
    test_sys = makeLinuxX86System(test_mem_mode, np, bm[0])
elif buildEnv['TARGET_ISA'] == "arm":
    test_sys = makeLinuxArmSystem(test_mem_mode, bm[0],
            bare_metal=options.bare_metal, machine_type=options.machine_type)
else:
    fatal("incapable of building non-alpha or non-sparc full system!")

if options.kernel is not None:
    test_sys.kernel = binary(options.kernel)

if options.script is not None:
    test_sys.readfile = options.script

test_sys.cpu = [TestCPUClass(cpu_id=i) for i in xrange(np)]

CacheConfig.config_cache(options, test_sys)

if options.caches or options.l2cache:
    if bm[0]:
        mem_size = bm[0].mem()
    else:
        mem_size = SysConfig().mem()
    test_sys.bridge.filter_ranges_a=[AddrRange(0, Addr.max)]
    test_sys.bridge.filter_ranges_b=[AddrRange(mem_size)]
    test_sys.iocache = IOCache(addr_range=mem_size)
    test_sys.iocache.cpu_side = test_sys.iobus.port
    test_sys.iocache.mem_side = test_sys.membus.port

for i in xrange(np):
    if options.fastmem:
        test_sys.cpu[i].physmem_port = test_sys.physmem.port

if buildEnv['TARGET_ISA'] == 'mips':
    setMipsOptions(TestCPUClass)

if len(bm) == 2:
    if buildEnv['TARGET_ISA'] == 'alpha':
        drive_sys = makeLinuxAlphaSystem(drive_mem_mode, bm[1])
    elif buildEnv['TARGET_ISA'] == 'mips':
        drive_sys = makeLinuxMipsSystem(drive_mem_mode, bm[1])
    elif buildEnv['TARGET_ISA'] == 'sparc':
        drive_sys = makeSparcSystem(drive_mem_mode, bm[1])
    elif buildEnv['TARGET_ISA'] == 'x86':
        drive_sys = makeX86System(drive_mem_mode, np, bm[1])
    elif buildEnv['TARGET_ISA'] == 'arm':
        drive_sys = makeLinuxArmSystem(drive_mem_mode, bm[1])
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
