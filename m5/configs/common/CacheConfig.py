# Copyright (c) 2010 Advanced Micro Devices, Inc.
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
# Authors: Lisa Hsu

# Configure the M5 cache hierarchy config in one place
#

import m5
from m5.objects import *
from Caches import *

def config_cache(options, system):
    if options.l2cache:
        system.l2cache = L2Cache(size='2MB')
        system.tol2bus = Bus()
        system.l2cache.cpu_side = system.tol2bus.port
        system.l2cache.mem_side = system.membus.port
        system.l2cache.num_cpus = options.num_cpus

    elif options.l3cache:
        system.l3cache = L3Cache()
        system.tol3bus = Bus()
        system.l3cache.cpu_side = system.tol3bus.port
        system.l3cache.mem_side = system.membus.port
        system.l3cache.num_cpus = options.num_cpus

    for i in xrange(options.num_cpus):
        if options.l1cache:
            system.cpu[i].addPrivateSplitL1Caches(L1Cache(), L1Cache())
            system.cpu[i].connectMemPorts(system.membus)

        elif options.l2cache:
            system.cpu[i].addPrivateSplitL1Caches(L1Cache(), L1Cache())
            system.cpu[i].connectMemPorts(system.tol2bus)

        elif options.l3cache:
            system.cpu[i].addTwoLevelCacheHierarchy(L1Cache(), L1Cache(), L2Cache())
            system.cpu[i].connectMemPorts(system.tol3bus)


    return system
