#specbench.py
import m5
from m5.objects import *
import os, optparse, sys
m5.AddToPath('../common')
import Simulation
from Caches import *



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
cmdLine = '400.perlbench.checkspam.pl'

#401.bzip2
bzip2 = LiveProcess()
bzip2.executable = os.path.join(binary_dir, '401.bzip2/exe/bzip2_base' + suffix)
data = os.path.join(data_dir, '401.bzip2/data/all/input/input.combined')
bzip2.cmd = [bzip2.executable] + [data, '1']
bzip2.output = 'bzip2.out'
cmdLine = '401.bzip2.combined'

#403.gcc
gcc = LiveProcess()
gcc.executable = binary_dir + '403.gcc/exe/gcc_base' + suffix
data = data_dir + '403.gcc/data/test/input/cccp.i'
output = 'cccp.s'
gcc.cmd = [gcc.executable] + [data] + ['-o', output]
gcc.output = 'ccc.out'
cmdLine = '403.gcc.cccp.i'

#410.bwaves
bwaves = LiveProcess()
bwaves.executable = os.path.join(binary_dir, '410.bwaves/exe/bwaves_base' + suffix)
bwaves.cmd = [bwaves.executable]
cmdLine = '410.bwaves'

#416.gamess
gamess = LiveProcess()
gamess.executable = binary_dir + '416.gamess/exe/gamess_base' + suffix
gamess.cmd = [gamess.executable]
gamess.input = 'exam29.config'
gamess.output = 'exam29.output'
cmdLine = '416.gamess.exam29'

#429.mcf
mcf = LiveProcess()
mcf.executable = os.path.join(binary_dir, '429.mcf/exe/mcf_base' + suffix)
data = os.path.join(data_dir, '429.mcf/data/test/input/inp.in')
mcf.cmd = [mcf.executable] + [data]
#mcf.output  = 'inp.out'
cmdLine = '429.mcf.test.inp'

#433.milc
milc = LiveProcess()
milc.executable = os.path.join(binary_dir, '433.milc/exe/milc_base' + suffix)
stdin = os.path.join(data_dir, '433.milc/data/test/input/su3imp.in')
milc.cmd = [milc.executable]
milc.input = stdin
cmdLine = '433.milc.su3imp'
#milc.output = 'su3imp.out'

#434.zeusmp
zeusmp = LiveProcess()
zeusmp.executable = os.path.join(binary_dir, '434.zeusmp/exe/zeusmp_base' + suffix)
zeusmp.cmd = [zeusmp.executable]
#zeusmp.output = 'zeusmp.stdout'
cmdLine = '434.zeusmp'

#435.gromacs
gromacs = LiveProcess()
gromacs.executable = os.path.join(binary_dir, '435.gromacs/exe/gromacs_base' + suffix)
data = os.path.join(data_dir, '435.gromacs/data/test/input/gromacs.tpr')
gromacs.cmd = [gromacs.executable] + ['-silent', '-deffnm', data, '-nice', '0']
cmdLine = '435.gromacs.test.gromacs'

#436.cactusADM
cactusADM = LiveProcess()
cactusADM.executable = os.path.join(binary_dir, '436.cactusADM/exe/cactusADM_base' + suffix)
data = os.path.join(data_dir, '436.cactusADM/data/test/input/benchADM.par')
cactusADM.cmd = [cactusADM.executable] + [data]
#cactusADM.output = 'benchADM.out'
cmdLine = '436.cactusADM.benchADM'

#437.leslie3d
leslie3d = LiveProcess()
leslie3d.executable = os.path.join(binary_dir, '437.leslie3d/exe/leslie3d_base' + suffix)
stdin = os.path.join(data_dir, '437.leslie3d/data/test/input/leslie3d.in')
leslie3d.cmd = [leslie3d.executable]
leslie3d.input = stdin
#leslie3d.output = 'leslie3d.stdout'
cmdLine = '437.leslie3d.test'

#444.namd
namd = LiveProcess()
namd.executable = os.path.join(binary_dir, '444.namd/exe/namd_base' + suffix)
input = os.path.join(data_dir, '444.namd/data/all/input/namd.input')
namd.cmd = [namd.executable] + ['--input', input, '--iterations', '1', '--output', 'namd.out']
#namd.output = 'namd.stdout'
cmdLine = '444.namd.ref'

#445.gobmk
gobmk = LiveProcess()
gobmk.executable = os.path.join(binary_dir, '445.gobmk/exe/gobmk_base' + suffix)
stdin = os.path.join(data_dir, '445.gobmk/data/test/input/connection.tst')
gobmk.cmd = [gobmk.executable] + ['--quiet', '--mode', 'gtp']
gobmk.input = stdin
gobmk.cwd = os.path.join(data_dir, "445.gobmk/data/all/input")
#gobmk.output = 'capture.out'
cmdLine = '445.gobmk.connection'

#447.dealII
dealII = LiveProcess()
dealII.executable = binary_dir + '447.dealII/exe/dealII_base' + suffix
dealII.cmd = [gobmk.executable] + ['8']
#dealII.output = 'dealII.log'
cmdLine = '447.dealII.8'

#450.soplex
soplex = LiveProcess()
soplex.executable = binary_dir + '450.soplex/exe/soplex_base' + suffix
data = os.path.join(data_dir, '450.soplex/data/ref/input/ref.mps')
soplex.cmd = [soplex.executable] + ['-m10000', data]
#soplex.output = 'test.out'
cmdLine = '450.soplex.ref'

#453.povray
povray = LiveProcess()
povray.executable = binary_dir + '453.povray/exe/povray_base' + suffix
data = data_dir + '453.povray/data/ref/input/SPEC-benchmark-ref.ini'
povray.cwd = os.path.join(data_dir, '453.povray/data/all/input/')
#povray.cmd = [povray.executable]+['SPEC-benchmark-test.ini']
povray.cmd = [povray.executable] + [data]
#povray.output = 'SPEC-benchmark-test.stdout'
cmdLine = '453.povray.ref'

#454.calculix
calculix = LiveProcess()
calculix.executable = binary_dir + '454.calculix/exe/calculix_base' + suffix
data = data_dir + '454.calculix/data/test/input/beampic'
calculix.cmd = [calculix.executable] + ['-i', data]
#calculix.output = 'beampic.log'
cmdLine = '454.calculix.beampic'

#456.hmmer
hmmer = LiveProcess()
hmmer.executable = binary_dir + '456.hmmer/exe/hmmer_base' + suffix
data = data_dir + '456.hmmer/data/ref/input/nph3.hmm'
hmmer.cmd = [hmmer.executable] + ['--fixed', '0', '--mean', '425', '--num', '85000', '--sd', '300', '--seed', '0', data]
#hmmer.output = 'bombesin.out'
cmdLine = '456.hmmer.nph3'

#458.sjeng
sjeng = LiveProcess()
sjeng.executable = binary_dir + '458.sjeng/exe/sjeng_base' + suffix
data = data_dir + '458.sjeng/data/test/input/test.txt'
sjeng.cmd = [sjeng.executable] + [data]
#sjeng.output = 'test.out'
cmdLine = '458.sjeng.test'

#459.GemsFDTD
GemsFDTD = LiveProcess()
GemsFDTD.executable = binary_dir + '459.GemsFDTD/exe/GemsFDTD_base' + suffix
GemsFDTD.cmd = [GemsFDTD.executable]
#GemsFDTD.output = 'test.log'
GemsFDTD.cwd = data_dir + '459.GemsFDTD/data/test/input/'
cmdLine = '459.GemsFDTD.test'

#462.libquantum
libquantum = LiveProcess()
libquantum.executable = binary_dir + '462.libquantum/exe/libquantum_base' + suffix
libquantum.cmd = [libquantum.executable], '129', '5'
#libquantum.output = 'test.out'
cmdLine = '462.libquantum.129.5'

#464.h264ref
h264ref = LiveProcess()
h264ref.executable = binary_dir + '464.h264ref/exe/h264ref_base' + suffix
data = data_dir + '464.h264ref/data/test/input/foreman_test_encoder_baseline.cfg'
h264ref.cmd = [h264ref.executable] + ['-d', data]
#h264ref.output = 'foreman_test_encoder_baseline.out'
cmdLine = '464.h264ref.foreman_test_encoder_baseline'

#470.lbm
lbm = LiveProcess()
lbm.executable = binary_dir + '470.lbm/exe/lbm_base' + suffix
data = data_dir + '470.lbm/data/ref/input/100_100_130_ldc.of'
lbm.cmd = [lbm.executable] + ['20', 'reference.dat', '0', '1' , data]
#lbm.output = 'lbm.out'
cmdLine = '470.lbm.100_100_130_ldc'


#471.omnetpp
omnetpp = LiveProcess()
omnetpp.executable = os.path.join(binary_dir, '471.omnetpp/exe/omnetpp_base' + suffix)
data = os.path.join(data_dir, '471.omnetpp/data/ref/input/omnetpp.ini')
omnetpp.cwd = os.path.join(data_dir,'471.omnetpp/data/ref/input/')
omnetpp.cmd = [omnetpp.executable] + [data]
#omnetpp.output = 'omnetpp.log'
cmdLine = '471.omnetpp.ref'

#473.astar
astar = LiveProcess()
astar.executable = binary_dir + '473.astar/exe/astar_base' + suffix
astar.cmd = [astar.executable] + ['lake.cfg']
#astar.output = 'lake.out'
cmdLine = '473.astar.lake'

#481.wrf
wrf = LiveProcess()
wrf.executable = binary_dir + '481.wrf/exe/wrf_base' + suffix
wrf.cmd = [wrf.executable] + [data_dir + '481.wrf/data/ref/input/namelist.input']
#wrf.output = 'rsl.out.0000'
wrf.cwd = os.path.join(data_dir, '481.wrf/data/ref/input/')
cmdLine = '481.wrf.namelist'

#482.sphinx
sphinx3 = LiveProcess()
sphinx3.executable = binary_dir + '482.sphinx3/exe/sphinx_livepretend_base' + suffix
sphinx3.cmd = [sphinx3.executable] + ['ctlfile', '.', 'args.an4']
#sphinx3.output = 'an4.out'
sphinx3.cwd = os.path.join(data_dir, '482.sphinx3/data/ref/input/')
cmdLine = '482.sphinx.args.an4'

#483.xalancbmk
xalancbmk = LiveProcess()
xalancbmk.executable = binary_dir + '483.xalancbmk/exe/Xalan_base' + suffix
xalancbmk.cmd = [xalancbmk.executable] + ['-v', 'test.xml', 'xalanc.xsl']
xalancbmk.cwd = os.path.join(data_dir, '483.xalancbmk/data/test/input/')
#xalancbmk.output = 'test.out'
cmdLine = '483.xalancbmk.test.xml'

#998.specrand
specrand_i = LiveProcess()
specrand_i.executable = binary_dir + '998.specrand/exe/specrand_base' + suffix
specrand_i.cmd = [specrand_i.executable] + ['324342', '24239']
#specrand_i.output = 'rand.24239.out'
cmdLine = '998.specrandI'

#999.specrand
specrand_f = LiveProcess()
specrand_f.executable = binary_dir + '999.specrand/exe/specrand_base' + suffix
specrand_f.cmd = [specrand_i.executable] + ['324342', '24239']
#specrand_f.output = 'rand.24239.out'
cmdLine = '999.specRandF'
