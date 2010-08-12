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
	dramType = Param.String("ddr3","dram type")
	datarate = Param.Int(800000000,"");
	clockGranularity = Param.Int(2,"the number of beats per cycle")
	channels = Param.Int(2,"the number of channels per dram system")
	dimms = Param.Int(2,"the number of dimms in each channel")
	ranks = Param.Int(2,"the number of ranks in each dimm")
	banks = Param.Int(8,"the number of banks per rank")
	rows = Param.Int(16384,"")
	columns = Param.Int(1024,"")
	channelWidth = Param.Int(8, "")
	physicalAddressMappingPolicy = Param.String("closePageBaseline", "address mapping policy, physical to logical")
	rowBufferPolicy = Param.String("closePageAggressive","")
	rowSize = Param.Int(16,"the size of each row")
	columnSize = Param.Int(16,"the size of the column")
	postedCas = Param.Bool(True,"")
	autoRefreshPolicy = Param.String("refreshOneChanAllRankAllBank","")
	commandOrderingAlgorithm = Param.String("firstAvailableAge","")
	transactionOrderingAlgorithm = Param.String("RIFF","")
	perBankQueueDepth = Param.Int(8,"")
	transactionQueueDepth = Param.Int(16,"")
	decodeWindow = Param.Int(16,"")
	readWriteGrouping = Param.Bool(True,"")
	autoPrecharge = Param.Bool(True,"")

	# timing parameters
	tBufferDelay = Param.Int(2,"delay from Transaction to Commands")
	tBurst = Param.Int(8,"burst length, DDR2=8, DDR3=16")
	tCAS = Param.Int(10,"column address strobe latency")
	tCMD = Param.Int(2,"1T timing, 2 beats per command")
	tCWD = Param.Int(8,"write delay")
	tFAW = Param.Int(32,"four activation window")
	tRAS = Param.Int(28,"minimum activate to precharge time")
	tRC = Param.Int(38, "row cycle")
	tRCD = Param.Int(10,"row to column delay")
	tRFC = Param.Int(128, "refresh to refresh time")
	tRRD = Param.Int(6,"activate to activate different ranks")
	tRP = Param.Int(10,"row precharge time")
	tRTP = Param.Int(6,"read to precharge time")
	tRTRS = Param.Int(2,"rank to rank switching time")
	tWR = Param.Int(12,"write recovery time")
	tWTR = Param.Int(6,"write to read time")
	tAL = Param.Int(8,"additive latency")
	refreshTime = Param.Int(64000,"")
	tREFI = Param.Int(6240,"refresh interval")
	seniorityAgeLimit = Param.Int(1024,"how long a command can be deferred")

	# power settings
	PdqRD = Param.Float(1.1, "read power")
	PdqWR = Param.Float(8.2,"")
	PdqRDoth = Param.Float(0,"")
	PdqWRoth = Param.Float(0,"")
	DQperDRAM = Param.Int(8,"")
	DQSperDRAM = Param.Int(4,"")
	DMperDRAM = Param.Int(2,"")
	frequencySpec = Param.Int(800000000,"")
	maxVCC = Param.Float(1.9,"")
	systemVDD = Param.Float(1.8,"")
	IDD0 = Param.Float(90,"")
	IDD2P = Param.Float(7,"")
	IDD2N = Param.Float(60,"")
	IDD3N = Param.Float(60,"")
	IDD3P = Param.Float(50,"")
	IDD4R = Param.Float(145,"")
	IDD4W = Param.Float(145,"")
	IDD5A = Param.Float(235,"")

	cpuRatio = Param.Int(5,"ratio of CPU speed to DRAM data bus speed")
	cachelinesPerRow = Param.String("-1","")
	epoch = Param.Int(1000000, "cycles (memory) between stats reporting")
