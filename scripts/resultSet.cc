#include "resultSet.hh"
#include "processStats.hh"


using std::string;

const string ResultSet::urlString = "<a href=\"%1/index.html\">%2</a>";

const string ResultSet::csvHeader = "Benchmark,Channels,DIMMs,Ranks,Banks,Rows,Columns,DRAM Width,Posted CAS,tRAS,tCAS,tRCD,tRC,Address Mapping Policy,Command Ordering Algorithm, Row Buffer Management Policy,Datarate,Per Bank Queue Depth,tFAW,Cache Size,Block Size,Associativity,Number of Sets,Replacement Policy,Runtime,Row Reuse Rate,Average IPC,Read Hit Rate,Hit Rate,Average Bandwidth, Average ACT_STBY Power, Average Latency,Average Theoretical Latency,Average Latency Change,Energy Used,Theoretical Energy Used,Energy Used Ratio (%),No Cache Runtime,Cache Runtime,Cache In Use (%),Energy Reduced Due to Latency, Difference in Total Latency, Average DRAM Power, Average Cache Power\n";

void ResultSet::parseCommandLine(const char *commandLine, const string &filename)
{
	channels = regexMatch<unsigned>(commandLine,"ch\\[([0-9]+)\\]");
	dimms = regexMatch<unsigned>(commandLine,"dimm\\[([0-9]+)\\]");
	ranks = regexMatch<unsigned>(commandLine,"rk\\[([0-9]+)\\]");
	banks = regexMatch<unsigned>(commandLine,"bk\\[([0-9]+)\\]");
	rows = regexMatch<unsigned>(commandLine,"row\\[([0-9]+)\\]");
	columns = regexMatch<unsigned>(commandLine,"col\\[([0-9]+)\\]");
	width = regexMatch<unsigned>(commandLine,"\\[x([0-9]+)\\]");
	tRas = regexMatch<unsigned>(commandLine,"t_\\{RAS\\}\\[([0-9]+)\\]");
	tCas = regexMatch<unsigned>(commandLine,"t_\\{CAS\\}\\[([0-9]+)\\]");
	tRcd = regexMatch<unsigned>(commandLine,"t_\\{RCD\\}\\[([0-9]+)\\]");
	tRc = regexMatch<unsigned>(commandLine,"t_\\{RC\\}\\[([0-9]+)\\]");
	postedCas = regexMatch<string>(commandLine,"PC\\[(T|F)\\]") == "T" ? true : false;

	addressMappingPolicy = regexMatch<string>(commandLine,"AMP\\[([A-Z]+)\\]");
	commandOrderingAlgorithm = regexMatch<string>(commandLine,"COA\\[([A-Z]+)\\]");
	rowBufferManagementPolicy = regexMatch<string>(commandLine,"RBMP\\[([A-Z]+)\\]");
	replacementPolicy = regexMatch<string>(commandLine,"policy\\[([A-Z0-9]+)\\]");	

	datarate = regexMatch<string>(commandLine,"DR\\[([0-9]+[KMG])\\]");
	double value = regexMatch<double>(datarate.c_str(), "([0-9]+)");
	datarateVal = value * (ends_with(datarate,"G") ? 1e9 : 1e6);
	perBankQueueDepth = regexMatch<unsigned>(commandLine,"PBQ\\[([0-9]+)\\]");
	tFaw = regexMatch<unsigned>(commandLine,"t_\\{FAW\\}\\[([0-9]+)\\]");
	cacheSize = regexMatch<unsigned>(commandLine,"cache\\[([0-9]+)MB\\]");
	blockSize = regexMatch<unsigned>(commandLine,"blkSz\\[([0-9]+)\\]");
	associativity = regexMatch<unsigned>(commandLine,"assoc\\[([0-9]+)\\]");
	numberOfSets = regexMatch<unsigned>(commandLine,"sets\\[([0-9]+)\\]");
	title = regexMatch<string>(commandLine,": ([0-9A-Za-z-]+)");
	basename = regexMatch<string>(filename.c_str(),"^([0-9A-Za-z-]+)(-stats|-power)");
	//cerr << "bn: " << basename << endl;
	//cerr << title << endl;
}

std::pair<string,string> ResultSet::generateResultLine() const
{
	string csvOutput, currentRow("<tr>");
	
	csvOutput += lexical_cast<string>(title) + ",";
	currentRow += generateTd(title);

	csvOutput += lexical_cast<string>(channels) + ",";
	currentRow += generateTd(channels);

	csvOutput += lexical_cast<string>(dimms) + ",";
	currentRow += generateTd(dimms);

	csvOutput += lexical_cast<string>(ranks) + ",";
	currentRow += generateTd(ranks);

	csvOutput += lexical_cast<string>(banks) + ",";
	currentRow += generateTd(banks);

	csvOutput += lexical_cast<string>(rows) + ",";
	currentRow += generateTd(rows);

	csvOutput += lexical_cast<string>(columns) + ",";
	currentRow += generateTd(columns);

	csvOutput += "x" + lexical_cast<string>(width) + ",";
	currentRow += generateTd("x" + lexical_cast<string>(width));

	csvOutput += lexical_cast<string>(tRas) + ",";
	currentRow += generateTd(tRas);

	csvOutput += lexical_cast<string>(tCas) + ",";
	currentRow += generateTd(tCas);

	csvOutput += lexical_cast<string>(tRcd) + ",";
	currentRow += generateTd(tRcd);

	csvOutput += lexical_cast<string>(tRc) + ",";
	currentRow += generateTd(tRc);

	csvOutput += (postedCas ? string("true") : string("false")) + ",";
	currentRow += generateTd(postedCas ? "true" : "false");

	csvOutput += matchMap(addressMappingPolicy) + ",";
	currentRow += generateTd(matchMap(addressMappingPolicy));

	csvOutput += matchMap(commandOrderingAlgorithm) + ",";
	currentRow += generateTd(matchMap(commandOrderingAlgorithm));

	csvOutput += matchMap(rowBufferManagementPolicy) + ",";
	currentRow += generateTd(matchMap(rowBufferManagementPolicy));

	csvOutput += datarate + ",";
	currentRow += generateTd(datarate);
	
	csvOutput += lexical_cast<string>(perBankQueueDepth) + ",";
	currentRow += generateTd(perBankQueueDepth);

	csvOutput += lexical_cast<string>(tFaw) + ",";
	currentRow += generateTd(tFaw);

	csvOutput += lexical_cast<string>(cacheSize) + ",";
	currentRow += generateTd(cacheSize);

	csvOutput += lexical_cast<string>(blockSize) + ",";
	currentRow += generateTd(blockSize);

	csvOutput += lexical_cast<string>(associativity) + ",";
	currentRow += generateTd(associativity);

	csvOutput += lexical_cast<string>(numberOfSets) + ",";
	currentRow += generateTd(numberOfSets);

	csvOutput += replacementPolicy + ",";
	currentRow += generateTd(replacementPolicy);
	
	csvOutput += lexical_cast<string>(runtime) + ",";
	currentRow += generateTd(runtime);

	csvOutput += lexical_cast<string>(reuseRate) + ",";
	currentRow += generateTd(reuseRate);

	csvOutput += lexical_cast<string>(averageIpc) + ",";
	currentRow += generateTd(averageIpc);

	csvOutput += lexical_cast<string>(readHitRate) + ",";
	currentRow += generateTd(readHitRate);

	csvOutput += lexical_cast<string>(hitRate) + ",";
	currentRow += generateTd(hitRate);

	csvOutput += lexical_cast<string>(averageBandwidth.first + averageBandwidth.second) + ",";
	currentRow += generateTd(averageBandwidth.first + averageBandwidth.second);

	csvOutput += lexical_cast<string>(averageActStbyPower) + ",";
	currentRow += generateTd(averageActStbyPower);
	
	csvOutput += lexical_cast<string>(averageLatency) + ",";
	currentRow += generateTd(averageLatency);

	csvOutput += lexical_cast<string>(averageTheoreticalLatency) + ",";
	currentRow += generateTd(averageTheoreticalLatency);

	csvOutput += lexical_cast<string>(averageLatency - averageTheoreticalLatency) + ",";
	currentRow += generateTd(averageLatency - averageTheoreticalLatency);

	csvOutput += lexical_cast<string>(energyUsed) + ",";
	currentRow += generateTd(energyUsed);

	csvOutput += lexical_cast<string>(energyUsedTheoretical) + ",";
	currentRow += generateTd(energyUsedTheoretical);

	csvOutput += lexical_cast<string>((energyUsedTheoretical) / energyUsed * 100) + ",";
	currentRow += generateTd((energyUsedTheoretical) / energyUsed * 100);
	
	csvOutput += lexical_cast<string>(noCacheRuntime) + ",";
	currentRow += generateTd(noCacheRuntime);

	csvOutput += lexical_cast<string>(cacheRuntime) + ",";
	currentRow += generateTd(cacheRuntime);

	csvOutput += lexical_cast<string>(percentCacheTimeInUse) + ",";
	currentRow += generateTd(percentCacheTimeInUse);

	csvOutput += lexical_cast<string>(energyUsedTheoretical - energyUsed) + ",";
	currentRow += generateTd(energyUsedTheoretical - energyUsed);

	csvOutput += lexical_cast<string>(noCacheRuntime - cacheRuntime) + ",";
	currentRow += generateTd((double)(noCacheRuntime - cacheRuntime));

	csvOutput += lexical_cast<string>(averageDramPower) + ",";
	currentRow += generateTd(averageDramPower);

	csvOutput += lexical_cast<string>(averageCachePower) + ",";
	currentRow += generateTd(averageCachePower);
	
	currentRow += "</tr>";

	return pair<string,string>(csvOutput,currentRow);
}

string ResultSet::generateTd(const string &str) const
{
	return "<td>" + ireplace_all_copy(ireplace_all_copy(urlString,"%2", str), "%1", basename) + "</td>";
}

string ResultSet::generateTd(const unsigned value) const
{
	stringstream current;
	
	current << std::dec << std::fixed << std::setprecision(2) << value;

	return generateTd(current.str());
		
}

string ResultSet::generateTd(const double value) const
{
	stringstream current;
	
	current << std::dec << std::fixed << std::setprecision(2) << value;

	return generateTd(current.str());
}

string ResultSet::generateTd(const char *value) const
{
	return generateTd(string(value));
}

void ResultSet::setStats(const ResultSet &rs, const bool isStat)
{
	if (isStat)
	{
		channels = rs.channels;
		dimms = rs.dimms;
		ranks = rs.ranks;
		banks = rs.banks;
		rows = rs.rows;
		columns = rs.columns;
		width = rs.width;
		tRas = rs.tRas;
		tCas = rs.tCas;
		tRcd = rs.tRcd;
		tRc = rs.tRc;
		noCacheRuntime = rs.noCacheRuntime;		
		postedCas = rs.postedCas;
		datarate = rs.datarate;
		datarateVal = rs.datarateVal;
		perBankQueueDepth = rs.perBankQueueDepth;
		commandOrderingAlgorithm = rs.commandOrderingAlgorithm;
		rowBufferManagementPolicy = rs.rowBufferManagementPolicy;
		addressMappingPolicy = rs.addressMappingPolicy;
		tFaw = rs.tFaw;
		cacheSize = rs.cacheSize;
		blockSize = rs.blockSize;
		associativity = rs.associativity;
		numberOfSets = rs.numberOfSets;
		runtime = rs.runtime;
		reuseRate = rs.reuseRate;
		averageIpc = rs.averageIpc;
		readHitRate = rs.readHitRate;
		hitRate = rs.hitRate;
		averageLatency = rs.averageLatency;
		averageTheoreticalLatency = rs.averageTheoreticalLatency;
		title = rs.title;
		cacheRuntime = rs.cacheRuntime;
		replacementPolicy = rs.replacementPolicy;
		basename = rs.basename;
		withoutCacheLatency = rs.withoutCacheLatency;
		withCacheLatency = rs.withCacheLatency;
		withCacheRequestCount = rs.withCacheRequestCount;
		withoutCacheRequestCount = rs.withoutCacheRequestCount;
		averageBandwidth = rs.averageBandwidth;
	}
	else
	{
		//powerParameters = rs.getPowerParameters();
		energyUsed = rs.energyUsed;
		energyUsedTheoretical = rs.energyUsedTheoretical;		
		percentCacheTimeInUse = rs.percentCacheTimeInUse;	
		averageActStbyPower = rs.averageActStbyPower;
		totalPower = rs.totalPower;
		averageDramPower = rs.getAverageDramPower();
		averageCachePower = rs.getAverageCachePower();
	}
}