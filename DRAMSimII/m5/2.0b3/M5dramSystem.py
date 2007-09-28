from m5.params import *
from m5.proxy import *
from MemObject import *
from PhysicalMemory import *


class M5dramSystem(PhysicalMemory):
	type = 'M5dramSystem'
	#abstract = True
	#compressed = Param.Bool(False, "This memory stores compressed data.")
	#do_writes = Param.Bool(False, "update memory")
	#in_bus = Param.Bus("incoming bus object")
	#snarf_updates = Param.Bool(True,"update memory on cache-to-cache transfers")
	#uncacheable_latency = Param.Latency('0ns', "uncacheable latency")
	cpuRatio = Param.Int(5,"ratio of CPU speed to DRAM data bus speed")
	settingsFile = Param.String("/a/d/c/b","the settings file")
	outFilename = Param.String("","output file name")
	dramType = Param.String("ddr2","dram type")
	rowBufferManagementPolicy = Param.String("close_page","")
	autoPrecharge = Param.String("true","");
	datarate = Param.String("200","");
	refreshTime = Param.String("64000","")
	readWriteGrouping = Param.String("false","")
	refreshPolicy = Param.String("none","")
	seniorityAgeLimit = Param.String("200","")
	postedCas = Param.String("false","")
	clockGranularity = Param.String("2","")
	rowCount = Param.String("16384","")
	cachelinesPerRow = Param.String("-1","")
	colCount = Param.String("512","")
	colSize = Param.String("8","")
	rowSize = Param.String("-1","")
	cachelineSize = Param.String("64","")
	historyQueueDepth = Param.String("8","")
	completionQueueDepth = Param.String("8","")
	transactionQueueDepth = Param.String("32","")
	eventQueueDepth = Param.String("32","")
	perBankQueueDepth = Param.String("6","")
	orderingAlgorithm = Param.String("strict_order","")
	configType = Param.String("baseline","")
	chanCount = Param.String("1","")
	rankCount = Param.String("8","")
	bankCount = Param.String("8","")
	refreshQueueDepth = Param.String("131072","")
	tAL = Param.String("50","")
	tBURST = Param.String("8","")
	tCAS = Param.String("8","")
	tFAW = Param.String("25","")
	tRAS = Param.String("30","")
	tRP = Param.String("8","")
	tRCD = Param.String("8","")
	tRFC = Param.String("85","")
	tRRD = Param.String("5","")
	tRTP = Param.String("5","")
	tRTRS = Param.String("2","")
	tWR = Param.String("10","")
	tWTR = Param.String("6","")
