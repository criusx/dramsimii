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
	chanCount = Param.String("2","")
	rankCount = Param.String("4","")
	bankCount = Param.String("16","")
	rowCount = Param.String("16384","")
	colCount = Param.String("2048","")
	physicalAddressMappingPolicy = Param.String("closePageBaseline", "address mapping policy, physical to logical")
	rowBufferManagementPolicy = Param.String("closePageOptimized","")
	rowSize = Param.String("16","")
	colSize = Param.String("16","")
	postedCas = Param.String("true","")
	autoRefreshPolicy = Param.String("refreshOneChanAllRankAllBank","")
	commandOrderingAlgorithm = Param.String("firstAvailable","")
	transactionOrderingAlgorithm = Param.String("RIFF","")
	perBankQueueDepth = Param.String("8","")
	configType = Param.String("baseline","")
	cachelineSize = Param.String("64","")
	historyQueueDepth = Param.String("16","")
	completionQueueDepth = Param.String("32","")
	transactionQueueDepth = Param.String("16","")
	eventQueueDepth = Param.String("32","")
	refreshQueueDepth = Param.String("8","")
	readWriteGrouping = Param.String("false","")
	autoPrecharge = Param.String("true","");

	tBufferDelay = Param.Int(1,"delay from Transaction to Commands")
	tBURST = Param.Int(8,"")
	tCAS = Param.Int(8,"")
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
	seniorityAgeLimit = Param.String(512,"how long a command can be deferred")

	settingsFile = Param.String("","the settings file, overrides the params")
	extraParameters = Param.String("","extra parameters to override the config file")

	cpuRatio = Param.Int(5,"ratio of CPU speed to DRAM data bus speed")
	outFilename = Param.String("","output file name")
	commandLine = Param.String("","command line used in this run")
	cachelinesPerRow = Param.String("-1","")
