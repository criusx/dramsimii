#include "resultSet.hh"
#include "processStats.hh"

using std::string;

const string ResultSet::urlString = "<a href=\"%1/index.html\">%2</a>";

void ResultSet::parseCommandLine(const char *commandLine)
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
	perBankQueueDepth = regexMatch<unsigned>(commandLine,"PBQ\\[([0-9]+)\\]");
	tFaw = regexMatch<unsigned>(commandLine,"t_\\{FAW\\}\\[([0-9]+)\\]");
	cacheSize = regexMatch<unsigned>(commandLine,"cache\\[([0-9]+)MB\\]");
	blockSize = regexMatch<unsigned>(commandLine,"blkSz\\[([0-9]+)\\]");
	associativity = regexMatch<unsigned>(commandLine,"assoc\\[([0-9]+)\\]");
	numberOfSets = regexMatch<unsigned>(commandLine,"sets\\[([0-9]+)\\]");
	title = regexMatch<string>(commandLine,": ([0-9A-Za-z-]+)");
	//cerr << title << endl;
}

std::pair<string,string> ResultSet::generateResultLine() const
{
	string csvOutput, fileList("<tr>");
	
	csvOutput += title + ",";
	fileList += generateTd(title);

	csvOutput += channels + ",";
	fileList += generateTd(channels);

	csvOutput += dimms + ",";
	fileList += generateTd(dimms);

	csvOutput += ranks + ",";
	fileList += generateTd(ranks);

	csvOutput += banks + ",";
	fileList += generateTd(banks);

	csvOutput += rows + ",";
	fileList += generateTd(rows);

	csvOutput += columns + ",";
	fileList += generateTd(columns);

	csvOutput += "x" + lexical_cast<string>(width) + ",";
	fileList += generateTd("x" + lexical_cast<string>(width));

	csvOutput += tRas + ",";
	fileList += generateTd(tRas);

	csvOutput += tCas + ",";
	fileList += generateTd(tCas);

	csvOutput += tRcd + ",";
	fileList += generateTd(tRcd);

	csvOutput += tRc + ",";
	fileList += generateTd(tRc);

	csvOutput += (postedCas ? string("true") : string("false")) + ",";
	fileList += generateTd(postedCas ? "true" : "false");

	csvOutput += matchMap(addressMappingPolicy) + ",";
	fileList += generateTd(matchMap(addressMappingPolicy));

	csvOutput += matchMap(commandOrderingAlgorithm) + ",";
	fileList += generateTd(matchMap(commandOrderingAlgorithm));

	csvOutput += matchMap(rowBufferManagementPolicy) + ",";
	fileList += generateTd(matchMap(rowBufferManagementPolicy));

	csvOutput += datarate + ",";
	fileList += generateTd(datarate);
	
	csvOutput += perBankQueueDepth + ",";
	fileList += generateTd(perBankQueueDepth);

	csvOutput += tFaw + ",";
	fileList += generateTd(tFaw);

	csvOutput += cacheSize + ",";
	fileList += generateTd(cacheSize);

	csvOutput += blockSize + ",";
	fileList += generateTd(blockSize);

	csvOutput += associativity + ",";
	fileList += generateTd(associativity);

	csvOutput += numberOfSets + ",";
	fileList += generateTd(numberOfSets);

	csvOutput += replacementPolicy + ",";
	fileList += generateTd(replacementPolicy);
	
	csvOutput += lexical_cast<string>(runtime) + ",";
	fileList += generateTd(runtime);

	csvOutput += lexical_cast<string>(readHitRate) + ",";
	fileList += generateTd(readHitRate);

	csvOutput += lexical_cast<string>(hitRate) + ",";
	fileList += generateTd(hitRate);

	csvOutput += lexical_cast<string>(averageLatency) + ",";
	fileList += generateTd(averageLatency);

	csvOutput += lexical_cast<string>(averageTheoreticalLatency) + ",";
	fileList += generateTd(averageTheoreticalLatency);

	csvOutput += lexical_cast<string>(averageLatency - averageTheoreticalLatency) + ",";
	fileList += generateTd(averageLatency - averageTheoreticalLatency);

	csvOutput += lexical_cast<string>(energyUsed) + ",";
	fileList += generateTd(energyUsed);

	csvOutput += lexical_cast<string>(energyUsedTheoretical) + ",";
	fileList += generateTd(energyUsedTheoretical);

	csvOutput += lexical_cast<string>((energyUsedTheoretical) / energyUsed * 100) + ",";
	fileList += generateTd((energyUsedTheoretical) / energyUsed * 100);
	
	csvOutput += lexical_cast<string>(noCacheRuntime) + ",";
	fileList += generateTd(noCacheRuntime);

	csvOutput += lexical_cast<string>(cacheRuntime) + ",";
	fileList += generateTd(cacheRuntime);

	csvOutput += lexical_cast<string>(percentCacheTimeInUse * 100) + ",";
	fileList += generateTd(percentCacheTimeInUse);

	///////////////////////////////////////////////////////////////////////
	fileList += "</tr>";

	//cerr << fileList << endl;

	return pair<string,string>(csvOutput,fileList);
}

string ResultSet::generateTd(const string &str) const
{
	return "<td>" + ireplace_all_copy(ireplace_all_copy(urlString,"%2", str), "%1", title) + "</td>";
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
		readHitRate = rs.readHitRate;
		hitRate = rs.hitRate;
		averageLatency = rs.averageLatency;
		averageTheoreticalLatency = rs.averageTheoreticalLatency;
		title = rs.title;
		cacheRuntime = rs.cacheRuntime;
		replacementPolicy = rs.replacementPolicy;
		
	}
	else
	{
		energyUsed = rs.energyUsed;
		energyUsedTheoretical = rs.energyUsedTheoretical;		
		percentCacheTimeInUse = rs.percentCacheTimeInUse;
	}
}