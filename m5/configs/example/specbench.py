#specbench.py
import m5
from m5.objects import *
import os, optparse, sys
m5.AddToPath('../common')
import Simulation
from Caches import *

liveProcessDict = {}
cmdLineDict = {}
alternateMemorySize = {}

#binary_dir = '/spec/'
#data_dir = '/spec/'
binary_dir = '/home/crius/benchmarks/spec2006/benchspec/CPU2006/'
data_dir = '/home/crius/benchmarks/spec2006/benchspec/CPU2006/'
suffix = '.alpha-gcc410-glibc236'


#400.perlbench
perlbench = LiveProcess()
perlbench.executable = os.path.join(binary_dir , '400.perlbench/exe/perlbench_base' + suffix)
perlbench.cmd = [perlbench.executable] + ['-I' + os.path.join(data_dir, '/400.perlbench/data/all/input/lib'), os.path.join(binary_dir, '400.perlbench/data/ref/input/checkspam.pl'), '2500', '5', '25', '11', '150', '1', '1', '1', '1']
perlbench.output = 'attrs.out'
cmdLineDict["perlbench"] = '400.perlbench.checkspam.pl'
liveProcessDict["perlbench"] = perlbench

#401.bzip2
bzip2 = LiveProcess()
bzip2.executable = os.path.join(binary_dir, '401.bzip2/exe/bzip2_base' + suffix)
data = os.path.join(data_dir, '401.bzip2/data/all/input/input.combined')
bzip2.cmd = [bzip2.executable] + [data, '1']
#bzip2.output = 'bzip2.out'
cmdLineDict["bzip2"] = '401.bzip2.combined'
liveProcessDict["bzip2"] = bzip2

#403.gcc
gcc = LiveProcess()
gcc.executable = binary_dir + '403.gcc/exe/gcc_base' + suffix
data = data_dir + '403.gcc/data/test/input/cccp.i'
output = 'cccp.s'
gcc.cmd = [gcc.executable] + [data] + ['-o', output]
gcc.output = 'ccc.out'
cmdLineDict["gcc"] = '403.gcc.cccp.i'
liveProcessDict["gcc"] = gcc

#410.bwaves
bwaves = LiveProcess()
bwaves.executable = os.path.join(binary_dir, '410.bwaves/exe/bwaves_base' + suffix)
bwaves.cmd = [bwaves.executable]
cmdLineDict["bwaves"] = '410.bwaves'
liveProcessDict["bwaves"] = bwaves

#416.gamess
gamess = LiveProcess()
gamess.executable = os.path.join(binary_dir, '416.gamess/exe/gamess_base' + suffix)
gamess.cmd = [gamess.executable]
gamess.cwd = os.path.join(binary_dir, '416.gamess/data/test/input')
gamess.input = os.path.join(binary_dir, '416.gamess/data/test/input/exam29.config')
gamess.output = 'exam29.output'
cmdLineDict["gamess"] = '416.gamess.exam29'
liveProcessDict["gamess"] = gamess
alternateMemorySize['gamess'] = "768MB"

#429.mcf
mcf = LiveProcess()
mcf.executable = os.path.join(binary_dir, '429.mcf/exe/mcf_base' + suffix)
data = os.path.join(data_dir, '429.mcf/data/test/input/inp.in')
mcf.cmd = [mcf.executable] + [data]
#mcf.output  = 'inp.out'
cmdLineDict["mcf"] = '429.mcf.test.inp'
liveProcessDict["mcf"] = mcf

mcfref = LiveProcess()
mcfref.executable = mcf.executable
data = os.path.join(data_dir, '429.mcf/data/ref/input/inp.in')
mcfref.cmd = [mcfref.executable] + [data]
cmdLineDict["mcfref"] = '429.mcf.ref'
liveProcessDict["mcfref"] = mcfref

mcftrain = LiveProcess()
mcftrain.executable = mcf.executable
data = os.path.join(data_dir, '429.mcf/data/train/input/inp.in')
mcftrain.cmd = [mcftrain.executable] + [data]
cmdLineDict["mcftrain"] = '429.mcf.train'
liveProcessDict["mcftrain"] = mcftrain

#433.milc
milc = LiveProcess()
milc.executable = os.path.join(binary_dir, '433.milc/exe/milc_base' + suffix)
stdin = os.path.join(data_dir, '433.milc/data/test/input/su3imp.in')
milc.cmd = [milc.executable]
milc.input = stdin
cmdLineDict["milc"] = '433.milc.su3imp'
liveProcessDict["milc"] = milc
#milc.output = 'su3imp.out'

#434.zeusmp
zeusmp = LiveProcess()
zeusmp.executable = os.path.join(binary_dir, '434.zeusmp/exe/zeusmp_base' + suffix)
zeusmp.cmd = [zeusmp.executable]
zeusmp.cwd = os.path.join(binary_dir, '434.zeusmp/data/ref/input')
#zeusmp.output = 'zeusmp.stdout'
cmdLineDict["zeusmp"] = '434.zeusmp'
liveProcessDict["zeusmp"] = zeusmp
alternateMemorySize['zeusmp'] = "1200MB"

#435.gromacs
gromacs = LiveProcess()
gromacs.executable = os.path.join(binary_dir, '435.gromacs/exe/gromacs_base' + suffix)
data = os.path.join(data_dir, '435.gromacs/data/test/input/gromacs.tpr')
gromacs.cmd = [gromacs.executable] + ['-silent', '-deffnm', data, '-nice', '0']
cmdLineDict["gromacs"] = '435.gromacs.test.gromacs'
liveProcessDict["gromacs"] = gromacs

#436.cactusADM
cactusADM = LiveProcess()
cactusADM.executable = os.path.join(binary_dir, '436.cactusADM/exe/cactusADM_base' + suffix)
data = os.path.join(data_dir, '436.cactusADM/data/test/input/benchADM.par')
cactusADM.cmd = [cactusADM.executable] + [data]
#cactusADM.output = 'benchADM.out'
cmdLineDict["cactusADM"] = '436.cactusADM.benchADM'
liveProcessDict["cactusADM"] = cactusADM

#437.leslie3d
leslie3d = LiveProcess()
leslie3d.executable = os.path.join(binary_dir, '437.leslie3d/exe/leslie3d_base' + suffix)
stdin = os.path.join(data_dir, '437.leslie3d/data/test/input/leslie3d.in')
leslie3d.cmd = [leslie3d.executable]
leslie3d.input = stdin
#leslie3d.output = 'leslie3d.stdout'
cmdLineDict["leslie3d"] = '437.leslie3d.test'
liveProcessDict["leslie3d"] = leslie3d

#444.namd
namd = LiveProcess()
namd.executable = os.path.join(binary_dir, '444.namd/exe/namd_base' + suffix)
input = os.path.join(data_dir, '444.namd/data/all/input/namd.input')
namd.cmd = [namd.executable] + ['--input', input, '--iterations', '1', '--output', 'namd.out']
#namd.output = 'namd.stdout'
cmdLineDict["namd"] = '444.namd.ref'
liveProcessDict["namd"] = namd

#445.gobmk
gobmk = LiveProcess()
gobmk.executable = os.path.join(binary_dir, '445.gobmk/exe/gobmk_base' + suffix)
stdin = os.path.join(data_dir, '445.gobmk/data/test/input/connection.tst')
gobmk.cmd = [gobmk.executable] + ['--quiet', '--mode', 'gtp']
gobmk.input = stdin
gobmk.cwd = os.path.join(data_dir, "445.gobmk/data/all/input")
#gobmk.output = 'capture.out'
cmdLineDict["gobmk"] = '445.gobmk.connection'
liveProcessDict["gobmk"] = gobmk

#447.dealII
dealII = LiveProcess()
dealII.executable = binary_dir + '447.dealII/exe/dealII_base' + suffix
dealII.cmd = [gobmk.executable] + ['8']
#dealII.output = 'dealII.log'
cmdLineDict["dealII"] = '447.dealII.8'
liveProcessDict["dealII"] = dealII

#450.soplex
soplex = LiveProcess()
soplex.executable = binary_dir + '450.soplex/exe/soplex_base' + suffix
data = os.path.join(data_dir, '450.soplex/data/ref/input/ref.mps')
soplex.cmd = [soplex.executable] + ['-m10000', data]
#soplex.output = 'test.out'
cmdLineDict["soplex"] = '450.soplex.ref'
liveProcessDict["soplex"] = soplex

#453.povray
povray = LiveProcess()
povray.executable = binary_dir + '453.povray/exe/povray_base' + suffix
data = data_dir + '453.povray/data/ref/input/SPEC-benchmark-ref.ini'
povray.cwd = os.path.join(data_dir, '453.povray/data/all/input/')
#povray.cmd = [povray.executable]+['SPEC-benchmark-test.ini']
povray.cmd = [povray.executable] + [data]
#povray.output = 'SPEC-benchmark-test.stdout'
cmdLineDict["povray"] = '453.povray.ref'
liveProcessDict["povray"] = povray

#454.calculix
calculix = LiveProcess()
calculix.executable = binary_dir + '454.calculix/exe/calculix_base' + suffix
data = data_dir + '454.calculix/data/test/input/beampic'
calculix.cmd = [calculix.executable] + ['-i', data]
#calculix.output = 'beampic.log'
cmdLineDict["calculix"] = '454.calculix.beampic'
liveProcessDict["calculix"] = calculix

#456.hmmer
hmmer = LiveProcess()
hmmer.executable = binary_dir + '456.hmmer/exe/hmmer_base' + suffix
data = data_dir + '456.hmmer/data/ref/input/nph3.hmm'
hmmer.cmd = [hmmer.executable] + ['--fixed', '0', '--mean', '425', '--num', '85000', '--sd', '300', '--seed', '0', data]
#hmmer.output = 'bombesin.out'
cmdLineDict["hmmer"] = '456.hmmer.nph3'
liveProcessDict["hmmer"] = hmmer

#458.sjeng
sjeng = LiveProcess()
sjeng.executable = binary_dir + '458.sjeng/exe/sjeng_base' + suffix
data = data_dir + '458.sjeng/data/test/input/test.txt'
sjeng.cmd = [sjeng.executable] + [data]
#sjeng.output = 'test.out'
cmdLineDict["sjeng"] = '458.sjeng.test'
liveProcessDict["sjeng"] = sjeng

#459.GemsFDTD
GemsFDTD = LiveProcess()
GemsFDTD.executable = binary_dir + '459.GemsFDTD/exe/GemsFDTD_base' + suffix
GemsFDTD.cmd = [GemsFDTD.executable]
#GemsFDTD.output = 'test.log'
GemsFDTD.cwd = data_dir + '459.GemsFDTD/data/test/input/'
cmdLineDict["GemsFDTD"] = '459.GemsFDTD.test'
liveProcessDict["GemsFDTD"] = GemsFDTD

#462.libquantum
libquantum = LiveProcess()
libquantum.executable = binary_dir + '462.libquantum/exe/libquantum_base' + suffix
libquantum.cmd = [libquantum.executable], '129', '5'
#libquantum.output = 'test.out'
cmdLineDict["libquantum"] = '462.libquantum.129.5'
liveProcessDict["libquantum"] = libquantum

#464.h264ref
h264ref = LiveProcess()
h264ref.executable = binary_dir + '464.h264ref/exe/h264ref_base' + suffix
data = data_dir + '464.h264ref/data/test/input/foreman_test_encoder_baseline.cfg'
h264ref.cmd = [h264ref.executable] + ['-d', data]
#h264ref.output = 'foreman_test_encoder_baseline.out'
cmdLineDict["h264ref"] = '464.h264ref.foreman_test_encoder_baseline'
liveProcessDict["h264ref"] = h264ref

#470.lbm
lbm = LiveProcess()
lbm.executable = binary_dir + '470.lbm/exe/lbm_base' + suffix
data = data_dir + '470.lbm/data/ref/input/100_100_130_ldc.of'
lbm.cmd = [lbm.executable] + ['20', 'reference.dat', '0', '1' , data]
#lbm.output = 'lbm.out'
cmdLineDict["lbm"] = '470.lbm.100_100_130_ldc'
liveProcessDict["lbm"] = lbm

#471.omnetpp
omnetpp = LiveProcess()
omnetpp.executable = os.path.join(binary_dir, '471.omnetpp/exe/omnetpp_base' + suffix)
data = os.path.join(data_dir, '471.omnetpp/data/ref/input/omnetpp.ini')
omnetpp.cwd = os.path.join(data_dir, '471.omnetpp/data/ref/input/')
omnetpp.cmd = [omnetpp.executable] + [data]
#omnetpp.output = 'omnetpp.log'
cmdLineDict["omnetpp"] = '471.omnetpp.ref'
liveProcessDict["omnetpp"] = omnetpp

#473.astar
astar = LiveProcess()
astar.executable = binary_dir + '473.astar/exe/astar_base' + suffix
astar.cmd = [astar.executable] + [os.path.join(binary_dir, '473.astar/data/test/input/lake.cfg')]
#astar.output = 'lake.out'
cmdLineDict["astar"] = '473.astar.lake'
liveProcessDict["astar"] = astar

#481.wrf
wrf = LiveProcess()
wrf.executable = os.path.join(binary_dir,'481.wrf/exe/wrf_base' + suffix)
#wrf.cmd = [wrf.executable] + [os.path.join(data_dir, '481.wrf/data/ref/input/namelist.input')]
wrf.cmd = [wrf.executable] + ['namelist.input']
#print wrf.cmd
#wrf.output = 'rsl.out.0000'
wrf.cwd = os.path.join(data_dir, '481.wrf/data/ref/input')
#print wrf.cwd
cmdLineDict["wrf"] = '481.wrf.ref'
liveProcessDict["wrf"] = wrf

#482.sphinx
sphinx3 = LiveProcess()
sphinx3.executable = binary_dir + '482.sphinx3/exe/sphinx_livepretend_base' + suffix
sphinx3.cmd = [sphinx3.executable] + ['beams.dat', '.', os.path.join(binary_dir, '482.sphinx3/data/ref/input/args.an4')]
#sphinx3.output = 'an4.out'
sphinx3.cwd = os.path.join(data_dir, '482.sphinx3/data/ref/input/')
cmdLineDict["sphinx3"] = '482.sphinx.args.an4'
liveProcessDict["sphinx3"] = sphinx3

#483.xalancbmk
xalancbmk = LiveProcess()
xalancbmk.executable = binary_dir + '483.xalancbmk/exe/Xalan_base' + suffix
xalancbmk.cmd = [xalancbmk.executable] + ['-v', 'test.xml', 'xalanc.xsl']
xalancbmk.cwd = os.path.join(data_dir, '483.xalancbmk/data/test/input/')
#xalancbmk.output = 'test.out'
cmdLineDict["xalancbmk"] = '483.xalancbmk.test.xml'
liveProcessDict["xalancbmk"] = xalancbmk

#998.specrand
specrand_i = LiveProcess()
specrand_i.executable = binary_dir + '998.specrand/exe/specrand_base' + suffix
specrand_i.cmd = [specrand_i.executable] + ['324342', '24239']
#specrand_i.output = 'rand.24239.out'
cmdLineDict["specrandi"] = '998.specrandI'
liveProcessDict["specrandi"] = specrand_i

#999.specrand
specrand_f = LiveProcess()
specrand_f.executable = binary_dir + '999.specrand/exe/specrand_base' + suffix
specrand_f.cmd = [specrand_i.executable] + ['324342', '24239']
#specrand_f.output = 'rand.24239.out'
cmdLineDict["specrandf"] = '999.specRandF'
liveProcessDict["specrandf"] = specrand_f
