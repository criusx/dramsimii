#!/usr/bin/python 

from subprocess import Popen, PIPE, STDOUT
import sys, traceback
import os
from threading import Thread
import time
import gzip
from Queue import Queue
import re
import argparse
import string

class bcolors:
	HEADER = '\033[95m'
	OKBLUE = '\033[94m'
	OKGREEN = '\033[92m'
	WARNING = '\033[93m'
	FAIL = '\033[91m'
	ENDC = '\033[0m'
	
	def disable(self):
		self.HEADER = ''
		self.OKBLUE = ''
		self.OKGREEN = ''
		self.WARNING = ''
		self.FAIL = ''
		self.ENDC = ''


def main():

 	#print "---------------------"
 	file = open('/home/joe/dramsimii/output.txt', 'a')
 	excluded = ["no-vmlinux", "python2.6", "bash", "ophelp", "[vdso]", "screen"]
 	values = []
 	values.append(0)
 	values.append(0)
 	values.append(0)
 	values.append(0)
 	name = ""
 	
 	for line in sys.stdin.readlines():
 		lines = string.split(line.strip())
 		if lines[0].isdigit() and lines[8] not in excluded:
	 		values[0] += string.atoi(lines[0])
	 		values[1] += string.atoi(lines[2])
	 		values[2] += string.atoi(lines[4])
	 		values[3] += string.atoi(lines[6])
	 		print lines
	 		name += ", " + lines[8]
	 		#print lines
	 		#print "." + lines[0] + "." + line,
	 	elif lines[0].isdigit():
	 		print "-",
	 		print lines
	print bcolors.WARNING,    
	print "%d\t%d\t%d\t%d\t%s" % (values[0], values[1], values[2], values[3], name)
	file.write("%d\t%d\t%d\t%d\t%s\n" % (values[0], values[1], values[2], values[3], name))
	print bcolors.ENDC
	print "---------------------"
      

# ----
# main
# ----   
if __name__ == "__main__":
    main()
    #import profile
    #profile.run('main()', 'profile.tmp')
    #import pstats
    #p = pstats.Stats('profile.tmp')
    #p.sort_stats('cumulative').print_stats(10)
