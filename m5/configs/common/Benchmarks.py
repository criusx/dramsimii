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

from SysPaths import *

class SysConfig:
    def __init__(self, script=None, mem=None, disk=None):
        self.scriptname = script
        self.diskname = disk
        self.memsize = mem

    def script(self):
        if self.scriptname:
            return script(self.scriptname)
        else:
            return ''

    def mem(self):
        if self.memsize:
            return self.memsize
        else:
            return '1024MB'

    def disk(self):
        if self.diskname:
            return disk(self.diskname)
        else:
            return env.get('LINUX_IMAGE', disk('linux-latest.img'))

# Benchmarks are defined as a key in a dict which is a list of SysConfigs
# The first defined machine is the test system, the others are driving systems

Benchmarks = {
    'astar_1_rivers':  [SysConfig('astar_1_rivers.rcS', '512MB')],
    'astar_4_rivers':  [SysConfig('astar_4_rivers.rcS', '1536MB')],
    'bzip2_1_combined':  [SysConfig('bzip2_1_combined.rcS', '512MB')],
    'bzip2_4_combined':  [SysConfig('bzip2_4_combined.rcS', '1536MB')],
    'mcf_1':  [SysConfig('mcf_1.rcS', '512MB')],
    'mcf_4':  [SysConfig('mcf_4.rcS', '1536MB')],
    'milc_1':  [SysConfig('milc_1.rcS', '512MB')],
    'milc_4':  [SysConfig('milc_4.rcS', '1536MB')],
    'xalancbmk_1':  [SysConfig('xalancbmk_1.rcS', '512MB')],
    'xalancbmk_4':  [SysConfig('xalancbmk_4.rcS', '1536MB')],
    'swaptions_1_small':  [SysConfig('swaptions_1c_simsmall.rcS', '512MB')],
    'swaptions_4_small':  [SysConfig('swaptions_4c_simsmall.rcS', '1536MB')],
    'ferret_1_small':  [SysConfig('ferret_1c_simsmall.rcS', '512MB')],
    'ferret_4_small':  [SysConfig('ferret_4c_simsmall.rcS', '1536MB')],
    'freqmine_1_small':  [SysConfig('freqmine_1c_simsmall.rcS', '512MB')],
    'freqmine_4_small':  [SysConfig('freqmine_4c_simsmall.rcS', '1536MB')],

    'perlbench_1':  [SysConfig('perlbench_1.rcS', '512MB')],
    'perlbench_4':  [SysConfig('perlbench_4.rcS', '1536MB')],

    'PovrayBench':  [SysConfig('povray-bench.rcS', '512MB', 'povray.img')],
    'PovrayAutumn': [SysConfig('povray-autumn.rcS', '512MB', 'povray.img')],

    'NetperfStream':    [SysConfig('netperf-stream-client.rcS'),
                         SysConfig('netperf-server.rcS')],
    'NetperfStreamUdp': [SysConfig('netperf-stream-udp-client.rcS'),
                         SysConfig('netperf-server.rcS')],
    'NetperfUdpLocal':  [SysConfig('netperf-stream-udp-local.rcS')],
    'NetperfStreamNT':  [SysConfig('netperf-stream-nt-client.rcS'),
                         SysConfig('netperf-server.rcS')],
    'NetperfMaerts':    [SysConfig('netperf-maerts-client.rcS'),
                         SysConfig('netperf-server.rcS')],
    'SurgeStandard':    [SysConfig('surge-server.rcS', '512MB'),
                         SysConfig('surge-client.rcS', '256MB')],
    'SurgeSpecweb':     [SysConfig('spec-surge-server.rcS', '512MB'),
                         SysConfig('spec-surge-client.rcS', '256MB')],
    'Nhfsstone':        [SysConfig('nfs-server-nhfsstone.rcS', '512MB'),
                         SysConfig('nfs-client-nhfsstone.rcS')],
    'Nfs':              [SysConfig('nfs-server.rcS', '900MB'),
                         SysConfig('nfs-client-dbench.rcS')],
    'NfsTcp':           [SysConfig('nfs-server.rcS', '900MB'),
                         SysConfig('nfs-client-tcp.rcS')],
    'IScsiInitiator':   [SysConfig('iscsi-client.rcS', '512MB'),
                         SysConfig('iscsi-server.rcS', '512MB')],
    'IScsiTarget':      [SysConfig('iscsi-server.rcS', '512MB'),
                         SysConfig('iscsi-client.rcS', '512MB')],
    'Validation':       [SysConfig('iscsi-server.rcS', '512MB'),
                         SysConfig('iscsi-client.rcS', '512MB')],
    'Ping':             [SysConfig('ping-server.rcS',),
                         SysConfig('ping-client.rcS')],

    'ValAccDelay':      [SysConfig('devtime.rcS', '512MB')],
    'ValAccDelay2':     [SysConfig('devtimewmr.rcS', '512MB')],
    'ValMemLat':        [SysConfig('micro_memlat.rcS', '512MB')],
    'ValMemLat2MB':     [SysConfig('micro_memlat2mb.rcS', '512MB')],
    'ValMemLat8MB':     [SysConfig('micro_memlat8mb.rcS', '512MB')],
    'ValMemLat':        [SysConfig('micro_memlat8.rcS', '512MB')],
    'ValTlbLat':        [SysConfig('micro_tlblat.rcS', '512MB')],
    'ValSysLat':        [SysConfig('micro_syscall.rcS', '512MB')],
    'ValCtxLat':        [SysConfig('micro_ctx.rcS', '512MB')],
    'ValStream':        [SysConfig('micro_stream.rcS', '512MB')],
    'ValStreamScale':   [SysConfig('micro_streamscale.rcS', '512MB')],
    'ValStreamCopy':    [SysConfig('micro_streamcopy.rcS', '512MB')],

    'MutexTest':        [SysConfig('mutex-test.rcS', '128MB')],

    'bnAn': [SysConfig('/z/saidi/work/m5.newmem.head/configs/boot/bn-app.rcS',
                       '128MB', '/z/saidi/work/bottleneck/bnimg.img')]
}

benchs = Benchmarks.keys()
benchs.sort()
DefinedBenchmarks = ", ".join(benchs)
