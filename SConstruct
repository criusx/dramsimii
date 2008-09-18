import os
import sys
from os.path import isdir, isfile, join as joinpath
import SCons

ROOT = Dir('.').abspath
SRCDIR = joinpath(ROOT,'src')
env = Environment(ENV = os.environ,  # inherit user's environment vars
                  ROOT = ROOT,
                  SRCDIR = SRCDIR)

Export('env')

SConscript('src/SConscript.standalone', build_dir = 'build', duplicate = 0)