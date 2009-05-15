# Copyright (C) 2008 University of Maryland.
# This file is part of DRAMsimII.
#
# DRAMsimII is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DRAMsimII is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

from m5.params import *
from m5.proxy import *
from MemObject import *
from PhysicalMemory import *


class M5dramSystem(PhysicalMemory):
	type = 'M5dramSystem'
	dramType = Param.String("ddr2","dram type")
	datarate = Param.String("800000000","");
	clockGranularity = Param.String("2","")
	channels = Param.String("2","")
	ranks = Param.String("4","")
	banks = Param.String("16","")
	rows = Param.String("16384","")
	columns = Param.String("2048","")
	physicalAddressMappingPolicy = Param.String("closePageBaseline", "address mapping policy, physical to logical")
	rowBufferPolicy = Param.String("closePageOptimized","")
	rowSize = Param.String("16","")
	columnSize = Param.String("16","")
	postedCas = Param.String("true","")
	autoRefreshPolicy = Param.String("refreshOneChanAllRankAllBank","")
	commandOrderingAlgorithm = Param.String("firstAvailable","")
	transactionOrderingAlgorithm = Param.String("RIFF","")
	perBankQueueDepth = Param.String("8","")
	systemConfigurationType = Param.String("baseline","")
	cacheLineSize = Param.String("64","")
	historyQueueDepth = Param.String("16","")
	completionQueueDepth = Param.String("32","")
	transactionQueueDepth = Param.String("16","")
	decodeWindow = Param.String("16","")
	eventQueueDepth = Param.String("32","")
	refreshQueueDepth = Param.String("8","")
	readWriteGrouping = Param.String("true","")
	autoPrecharge = Param.String("true","");
	# timing parameters
	tBufferDelay = Param.Int(1,"delay from Transaction to Commands")
	tBurst = Param.Int(8,"burst length, DDR2=8, DDR3=16")
	tCAS = Param.Int(5,"")
	tCMD = Param.Int(2,"")
	tCWD = Param.Int(3,"write delay")
	tFAW = Param.Int(28,"")
	tRAS = Param.Int(32,"")
	tRC = Param.Int(44, "row cycle")
	tRCD = Param.Int(10,"")
	tRFC = Param.Int(84, "refresh to refresh time")
	tRRD = Param.Int(6,"activate to activate different ranks")
	tRP = Param.Int(10,"")
	tRTP = Param.Int(6,"")
	tRTRS = Param.Int(2,"")
	tWR = Param.Int(12,"")
	tWTR = Param.Int(6,"")
	tAL = Param.Int(8,"")
	refreshTime = Param.Int(64000,"")
	tREFI = Param.Int(6240,"refresh interval")
	seniorityAgeLimit = Param.String(1024,"how long a command can be deferred")

	settingsFile = Param.String("","the settings file, overrides the params")
	extraParameters = Param.String("","extra parameters to override the config file")

	cpuRatio = Param.Int(5,"ratio of CPU speed to DRAM data bus speed")
	outFilename = Param.String("","output file name")
	commandLine = Param.String("","command line used in this run")
	cachelinesPerRow = Param.String("-1","")
