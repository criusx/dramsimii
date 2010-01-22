import os
import sys
from os.path import isdir, isfile, join as joinpath
import SCons

ROOT = Dir('.').abspath
SRCDIR = joinpath(ROOT,'src')
env = Environment(ENV = os.environ,  # inherit user's environment vars
                  ROOT = ROOT,
                  SRCDIR = SRCDIR)
                  
for key in os.environ.iterkeys():
	env[key] = os.environ[key]

Export('env')

# Find default configuration & binary.
#Default(os.environ.get('build', 'opt'))

SConscript('src/SConscript.standalone', build_dir = 'build', duplicate = 0)