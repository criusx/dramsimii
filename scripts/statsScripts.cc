#include "processStats.hh"
#include "globals.hh"
#include "statsScripts.hh"

using std::tr1::unordered_map;

void StatsScripts::addressLatencyDistributionPerChannelGraph(const bf::path &outFilename, opstream &p, unsigned channelID, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
#if 0	
	p << "set multiplot layout " << channelDistribution[channelID].size() << ", 1 title \"";


	for (vector<string>::const_iterator i = commandLine.begin(), end = commandLine.end();
		i < end; ++i)
	{
		p << "{/*1.5" << i << "};";
	}

#endif
	p << subAddrDistroA;

	printTitle("", commandLine, p, channelDistribution[channelID].size());

	for (unsigned rankID = 0; rankID
		< channelDistribution[channelID].size(); rankID++)
	{
		p << "set title \"Rank " << rankID
			<< " Distribution Rate\" offset character 0, 0, 0 font \"\" norotate"
			<< endl;

		if (rankID < channelDistribution[channelID].size() - 1)
			p << "unset key" << endl << "unset label" << endl;
		else
			p << "set xlabel 'Time (s)' offset 0,0.6" << endl
			<< "set key outside center bottom horizontal reverse Left"
			<< endl;

		p << "plot ";
		for (unsigned a = 0; a
			< channelDistribution[channelID][rankID].size()
			- 1; a++)
			p << "'-' using 1 axes x2y1 t 'bank_{" << a << "}  ',";
		p
			<< "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01"
			<< endl;

		for (unsigned bankID = 0; bankID
			< channelDistribution[channelID][rankID].size()
			- 1; bankID++)
		{
			for (unsigned epoch = 0; epoch
				< channelDistribution[channelID][rankID][bankID].size(); epoch++)
			{
				p
					<< max(
					1E-5F,
					channelDistribution[channelID][rankID][bankID][epoch]
				/ ((float) channelDistribution[channelID][rankID].back()[epoch]))
					<< endl;
			}
			p << "e" << endl;
		}
		p << channelDistribution[channelID][rankID][0].size()
			* epochTime << " " << "0.2" << endl << "e" << endl;
	}
	p << "unset multiplot" << endl << "unset output" << endl;
}


void StatsScripts::addressDistributionPerChannelGraph(const bf::path &outFilename, opstream &p, unsigned channelID, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << subAddrDistroA;

	printTitle("", commandLine, p, channelDistribution[channelID].size());

	for (unsigned rankID = 0; rankID
		< channelDistribution[channelID].size(); rankID++)
	{
		p << "set title \"Rank " << rankID
			<< " Distribution Rate\" offset character 0, 0, 0 font \"\" norotate"
			<< endl;

		if (rankID < channelDistribution[channelID].size() - 1)
			p << "unset key" << endl << "unset label" << endl;
		else
			p << "set xlabel 'Time (s)' offset 0,0.6" << endl
			<< "set key outside center bottom horizontal reverse Left"
			<< endl;

		p << "plot ";
		for (unsigned a = 0; a
			< channelDistribution[channelID][rankID].size() - 1; a++)
			p << "'-' using 1 axes x2y1 t 'bank_{" << a << "}  ',";
		p
			<< "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01"
			<< endl;

		for (unsigned bankID = 0; bankID
			< channelDistribution[channelID][rankID].size() - 1; bankID++)
		{
			for (unsigned epoch = 0; epoch
				< channelDistribution[channelID][rankID][bankID].size(); epoch++)
			{
				p
					<< max(
					1E-5F,
					channelDistribution[channelID][rankID][bankID][epoch]
				/ ((float) channelDistribution[channelID][rankID].back()[epoch]))
					<< endl;
			}
			p << "e" << endl;
		}
		p << channelDistribution[channelID][rankID][0].size()
			* epochTime << " " << "0.2" << endl << "e" << endl;
	}
	p << "unset multiplot" << endl << "unset output" << endl;
}

void StatsScripts::overallAddressDistributionGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("Channel Distribution Rate", commandLine, p);

	p << addressDistroA;
	p << "plot ";
	for (unsigned i = 0; i < channelCount; ++i)
		p << "'-' using 1 axes x2y1 t 'ch[" << i << "]',";
	p << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01"
		<< endl;

	for (unsigned channelID = 0; channelID < channelDistribution.size(); channelID++)
	{
		for (unsigned epochID = 0; epochID
			< channelDistribution[0][0][0].size(); epochID++)
		{
			unsigned thisChannelTotal = 0;
			unsigned allChannelTotal = 1;
			for (unsigned rankID = 0; rankID < rankCount; rankID++)
			{
				for (unsigned bankID = 0; bankID < bankCount; bankID++)
				{
					thisChannelTotal
						+= channelDistribution[channelID][rankID][bankID][epochID];
					for (unsigned channelID2 = 0; channelID2 < channelCount; channelID2++)
						allChannelTotal
						+= channelDistribution[channelID2][rankID][bankID][epochID];
				}
			}
			p << thisChannelTotal / ((float) allChannelTotal) << endl;
		}
		p << "e" << endl;
	}

	p << channelDistribution[0][0][0].size() * epochTime << " " << "0.2"
		<< endl << "e" << endl;
	p << addressDistroB << endl << "plot ";

	for (unsigned i = 0; i < rankCount; ++i)
		p << "'-' using 1 axes x2y1 t 'rk[" << i << "]',";
	p << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01"
		<< endl;

	for (unsigned rankID = 0; rankID < rankCount; rankID++)
	{
		for (unsigned epochID = 0; epochID
			< channelDistribution[0][0][0].size(); epochID++)
		{
			unsigned thisRankTotal = 0;
			unsigned allRankTotal = 1;
			for (unsigned channelID = 0; channelID < channelCount; channelID++)
			{
				for (unsigned bankID = 0; bankID < bankCount; bankID++)
				{
					thisRankTotal
						+= channelDistribution[channelID][rankID][bankID][epochID];
					for (unsigned rankID2 = 0; rankID2 < rankCount; rankID2++)
						allRankTotal
						+= channelDistribution[channelID][rankID2][bankID][epochID];
				}
			}
			p << thisRankTotal / ((float) allRankTotal) << endl;
		}
		p << "e" << endl;
	}
	p << channelDistribution[0][0][0].size() * epochTime << " " << "0.2"
		<< endl << "e" << endl;
	p << addressDistroC << endl << "plot ";
	for (unsigned i = 0; i < bankCount; ++i)
		p << "'-' using 1 axes x2y1 t 'bk[" << i << "]',";
	p << "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01"
		<< endl;

	for (unsigned bankID = 0; bankID < bankCount; bankID++)
	{
		for (unsigned epochID = 0; epochID
			< channelDistribution[0][0][0].size(); epochID++)
		{
			unsigned thisBankTotal = 0;
			unsigned allBankTotal = 1;
			for (unsigned rankID = 0; rankID < rankCount; rankID++)
			{
				for (unsigned channelID = 0; channelID < channelCount; channelID++)
				{
					thisBankTotal
						+= channelDistribution[channelID][rankID][bankID][epochID];
					for (unsigned bankID2 = 0; bankID2 < bankCount; bankID2++)
						allBankTotal
						+= channelDistribution[channelID][rankID][bankID2][epochID];
				}
			}
			p << thisBankTotal / ((float) allBankTotal) << endl;
		}
		p << "e" << endl;
	}
	p << channelDistribution[0][0][0].size() * epochTime << " " << "0.2"
		<< endl << "e" << endl;
	p << "unset multiplot" << endl << "unset output" << endl;
}

void StatsScripts::pcVsLatencyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << pcVsLatencyGraphScript << endl;
	p << "set multiplot layout 1, 2 title \"" << commandLine[0]
	<< "\\n{/*1.5Total Latency Due to Reads vs. PC Value}\"" << endl;
	p << "plot '-' using 1:2 t 'Total Latency' with boxes" << endl;

	if (latencyVsPcLow.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcLow.begin(); i != latencyVsPcLow.end(); ++i)
			p << i->first << " " << periodInNs * i->second.first << endl;
	else
		p << "4294967296 1.01" << endl;

	p << endl << "e" << endl << "set format x '0x1%x'" << endl
		<< "plot '-' using 1:2 t 'Total Latency' with boxes" << endl;
	if (latencyVsPcHigh.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcHigh.begin(); i != latencyVsPcHigh.end(); ++i)
			p << (i->first - 0x100000000) << " " << periodInNs
			* i->second.first << endl;
	else
		p << "4294967296 1.01" << endl;
	p << "e" << endl << "unset multiplot" << endl << "unset output"
		<< endl;
}

void StatsScripts::pcVsAverageLatencyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << avgPcVsLatencyGraphScript << endl;
	p << "set multiplot layout 1, 2 title \"" << commandLine[0]
	<< "\\n{/*1.5Average Latency Due to Reads vs. PC Value}\"" << endl;
	p << "plot '-' using 1:2 t 'Average Latency' with boxes" << endl;

	if (latencyVsPcLow.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcLow.begin(); i != latencyVsPcLow.end(); ++i)
			p << i->first << " " << periodInNs * (i->second.first
			/ i->second.second) << endl;
	else
		p << "4294967296 1.01" << endl;

	p << endl << "e" << endl;
	p << "set format x '0x1%x'" << endl
		<< "plot '-' using 1:2 t 'Average Latency' with boxes" << endl;
	if (latencyVsPcHigh.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcHigh.begin(); i != latencyVsPcHigh.end(); ++i)
			p << (i->first - 0x100000000) << " " << periodInNs
			* (i->second.first / i->second.second) << endl;
	else
		p << "4294967296 1.01" << endl;
	p << "e" << endl << "unset multiplot" << endl << "unset output"
		<< endl;
}

void StatsScripts::transactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("Read Transaction Latency", commandLine, p);

	p << transactionGraphScript << endl;

	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
	{
		p << i->first * periodInNs << " " << i->second << endl;
	}

	p << "e" << endl << "unset output" << endl;
}

void StatsScripts::zoomedTransactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	StdDev<float> latencyDeviation;

	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
		latencyDeviation.add(i->first * periodInNs, i->second);

	printTitle("Read Transaction Latency", commandLine, p);

	p << "set xrange [0:"
		<< latencyDeviation.getStdDev().get<1> () + 8
		* latencyDeviation.getStdDev().get<2> () << "]" << endl
		<< transactionGraphScript << endl;

	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
	{
		p << i->first * periodInNs << " " << i->second << endl;
	}
	p << "e" << endl << "unset output" << endl;
}


void StatsScripts::bandwidthGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	bool allZeros = true;
	for (vector<pair<uint64_t,uint64_t> >::const_iterator i = bandwidthValues.begin(), end = bandwidthValues.end();
		i != end; ++i)
	{
		if (i->first > 0 || i->second > 0)
			allZeros = false;
	}
	if (allZeros)
		p << "set yrange [0:1]" << endl;

	printTitle("", commandLine, p, 2);
	p << bandwidthGraphScript << endl;

	for (vector<pair<uint64_t, uint64_t> >::const_iterator i =
		bandwidthValues.begin(); i != bandwidthValues.end(); ++i)
		p << 1.0 * i->first << endl;
	p << "e" << endl;

	for (vector<pair<uint64_t, uint64_t> >::const_iterator i =
		bandwidthValues.begin(); i != bandwidthValues.end(); ++i)
		p << 1.0 * i->second << endl;
	p << "e" << endl;

	for (vector<pair<uint64_t, uint64_t> >::const_iterator i =
		cacheBandwidthValues.begin(); i != cacheBandwidthValues.end(); ++i)
		p << 1.0 * i->first << endl;
	p << "e" << endl;

	for (vector<pair<uint64_t, uint64_t> >::const_iterator i =
		cacheBandwidthValues.begin(); i != cacheBandwidthValues.end(); ++i)
		p << 1.0 * i->second << endl;
	p << "e" << endl;

	float time = 0.0F;
	PriorMovingAverage bandwidthTotal(WINDOW);
	for (vector<pair<uint64_t, uint64_t> >::const_iterator i =
		bandwidthValues.begin(), j = cacheBandwidthValues.begin(); i
		!= bandwidthValues.end() && j != cacheBandwidthValues.end(); ++i, ++j)
	{
		bandwidthTotal.append(1.0F * (float) (i->first + i->second + j->first
			+ j->second));
		p << time << " " << bandwidthTotal.getAverage() << endl;
		time += epochTime;
	}
	p << "e" << endl;

	p << smallIPCGraphScript << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i
		!= ipcValues.end(); ++i)
	{
		p << time << " " << *i << endl;
		time += epochTime;
	}
	p << "e" << endl;

	time = 0.0F;
	CumulativePriorMovingAverage ipcTotal;
	for (vector<float>::const_iterator i = ipcValues.begin(); i
		!= ipcValues.end(); ++i)
	{
		ipcTotal.add(1, *i);
		p << time << " " << ipcTotal.getAverage() << endl;
		time += epochTime;
	}
	p << "e" << endl;

	time = 0.0F;
	PriorMovingAverage ipcBuffer(WINDOW);
	for (vector<float>::const_iterator i = ipcValues.begin(); i
		!= ipcValues.end(); ++i)
	{
		ipcBuffer.append(*i);
		p << time << " " << ipcBuffer.getAverage() << endl;
		time += epochTime;
	}
	p << "e" << endl << "unset multiplot" << endl << "unset output"
		<< endl;
}

void StatsScripts::cacheGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("", commandLine, p, 3);

	p << cacheGraph1 << endl;

	for (vector<unsigned>::size_type i = 0; i < iCacheMisses.size(); ++i)
		p << (float) i * epochTime << " " << iCacheMisses[i]
	+ iCacheHits[i] << endl;
	p << "e" << endl;
	for (vector<unsigned>::size_type i = 0; i < iCacheMisses.size(); ++i)
		p << (float) i * epochTime << " " << iCacheMisses[i]
	/ ((float) max(iCacheMisses[i] + iCacheHits[i], 1U))
		<< endl;
	p << "e" << endl;

	p << cacheGraph2 << endl;
	for (vector<unsigned>::size_type i = 0; i < dCacheMisses.size(); ++i)
		p << (float) i * epochTime << " " << dCacheMisses[i]
	+ dCacheHits[i] << endl;
	p << "e" << endl;
	for (vector<unsigned>::size_type i = 0; i < dCacheMisses.size(); ++i)
		p << (float) i * epochTime << " " << dCacheMisses[i]
	/ ((float) max(dCacheMisses[i] + dCacheHits[i], 1U))
		<< endl;
	p << "e" << endl;

	p << cacheGraph3 << endl;
	for (vector<unsigned>::size_type i = 0; i < l2CacheMisses.size(); ++i)
		p << (float) i * epochTime << " " << l2CacheMisses[i]
	+ l2CacheHits[i] << endl;
	p << "e" << endl;
	for (vector<unsigned>::size_type i = 0; i < l2CacheMisses.size(); ++i)
		p << (float) i * epochTime << " " << l2CacheMisses[i]
	/ ((float) max(l2CacheMisses[i] + l2CacheHits[i], 1U))
		<< endl;
	p << "e" << endl << "unset multiplot" << endl << "unset output"
		<< endl;
}

void StatsScripts::averageIpcAndLatencyGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("", commandLine, p, 2);

	// make the transaction latency graph
	p << "set title 'Transaction Latency'" << endl
		<< averageTransactionLatencyScript << endl;

	float time = 0.0F;
	// access count
	for (vector<unsigned>::const_iterator i = transactionCount.begin(); i
		!= transactionCount.end(); ++i)
	{
		p << time << " " << *i << endl;
		time += epochTime;
	}
	p << "e" << endl;

	time = 0.0F;
	// minimum
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator
		i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p << time << " " << periodInNs * i->get<0> () << endl;
		time += epochTime;
	}
	p << "e" << endl;

	// mean
	time = 0.0F;
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator
		i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p << time << " " << periodInNs * i->get<1> () << endl;
		time += epochTime;
	}
	p << "e" << endl;

	time = 0.0F;
	// mean + 1 std dev
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator
		i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p << time << " " << periodInNs * (i->get<1> () + 2 * i->get<2> ()) << endl;
		time += epochTime;
	}
	p << "e" << endl;

	// ipc part
	p << "set title 'Average IPC vs. Time'" << endl << otherIPCGraphScript << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i
		!= ipcValues.end(); ++i)
	{
		p << time << " " << *i << endl;
		time += epochTime;
	}
	p << "e" << endl;


#if 0
	ipcTotal.clear();
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i != ipcValues.end(); ++i)
	{
		ipcTotal.add(1, *i);
		p1 << time << " " << ipcTotal.getAverage() << endl;
		time += epochTime;
	}
	p1 << "e" << endl;
#endif

	PriorMovingAverage ipcBuffer(WINDOW);
	time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i
		!= ipcValues.end(); ++i)
	{
		ipcBuffer.append(*i);
		p << time << " " << ipcBuffer.getAverage() << endl;
		time += epochTime;
	}
	p << "e" << endl;

	p << "unset multiplot" << endl << "unset output" << endl;
}

void StatsScripts::hitMissGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("Reuse Rate of Open Rows vs. Time", commandLine, p);
	p << rowHitMissGraphScript << endl;

	float time = 0.0F;
	for (vector<unsigned>::const_iterator i = hitMissTotals.begin(), end = hitMissTotals.end(); i
		!= end; ++i)
	{
		p << time << " " << max(*i, 1U) << endl;
		time += epochTime;
	}

	p << "e" << endl;

	time = 0.0F;
	for (vector<float>::const_iterator i = hitMissValues.begin(); i
		!= hitMissValues.end(); ++i)
	{
		p << time << " " << *i << endl;
		time += epochTime;
	}
	p << "e" << endl;

	CumulativePriorMovingAverage hitMissTotal;
	time = 0.0F;
	for (vector<float>::const_iterator i = hitMissValues.begin(); i
		!= hitMissValues.end(); ++i)
	{
		hitMissTotal.add(1.0, *i);
		p << time << " " << hitMissTotal.getAverage() << endl;
		time += epochTime;
	}
	p << "e" << endl;

	p << "unset output" << endl;
}

void StatsScripts::workingSetGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("Working Set Size vs. Time", commandLine, p);

	p << workingSetSetup << endl;
	float time = 0.0F;
	for (vector<unsigned>::const_iterator i = workingSetSize.begin(); i
		!= workingSetSize.end(); ++i)
	{
		p << time << " " << *i << endl;
		time += epochTime;
	}
	p << "e" << endl << "unset output" << endl;
}

void StatsScripts::bigIpcGraph(const bf::path &outFilename, opstream &p, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << bigIPCGraphScript << endl;
	float time = 0.0F;
	for (vector<float>::const_iterator i = ipcValues.begin(); i
		!= ipcValues.end(); ++i)
	{
		p << time << " " << *i << endl;
		time += epochTime;
	}
	p << "e" << endl;

	time = 0.0F;
	CumulativePriorMovingAverage ipcTotal;
	for (vector<float>::const_iterator i = ipcValues.begin(); i
		!= ipcValues.end(); ++i)
	{
		ipcTotal.add(1, *i);
		p << time << " " << ipcTotal.getAverage() << endl;
		time += epochTime;
	}
	p << "e" << endl << "0 0" << endl << "3.31 1E-5" << endl << "e"
		<< endl << "unset output" << endl;
}

void StatsScripts::cacheHitMissGraph(const bf::path &outFilename, opstream &p, const vector<pair<unsigned,unsigned> > &alternateCacheHitMiss,  bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle("Per-DIMM Cache Hit Rate",commandLine, p);

	p << hitMissScript << endl;

	float time = 0.0F;
	for (vector<pair<unsigned, unsigned> >::const_iterator i =
		alternateCacheHitMiss.begin(); i != alternateCacheHitMiss.end(); ++i)
	{
		p << time << " " << i->first + i->second << endl;
		//cerr << time << " " << i->first + i->second << endl;
		time += epochTime;
	}

	p << "e" << endl;

	time = 0.0F;
	for (vector<pair<unsigned, unsigned> >::const_iterator i =
		alternateCacheHitMiss.begin(); i != alternateCacheHitMiss.end(); ++i)
	{
		p << time << " " << (double) i->first / (i->first + i->second) << endl;
		//cerr << time << " " << i->first << " " <<  i->second << endl;
		time += epochTime;
	}

	p << "e" << endl << "unset output" << endl;
}


//////////////////////////////////////////////////////////////////////////
void StatsScripts::transactionLatencyCumulativeDistributionGraph(const bf::path &outFilename, opstream &p, const char* title,bool isThumbnail)
{
	map<unsigned,unsigned> orderedTransactionLatency;
	uint64_t sum = 0;

	for (unordered_map<unsigned,unsigned>::const_iterator i = distTransactionLatency.begin(), end = distTransactionLatency.end();
		i != end; ++i)
	{
		sum += i->second;
		orderedTransactionLatency[i->first] = i->second;
	}

	uint64_t countSoFar = 0;

	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	printTitle(title, commandLine, p);
	//printTitle(title, commandLine, cerr);

	p << cumulativeTransactionGraphScript << endl;
	for (map<unsigned, unsigned>::const_iterator i = orderedTransactionLatency.begin(), end = orderedTransactionLatency.end();
		i != end; ++i)
	{		
		countSoFar += i->second;
		p << i->first * periodInNs << " " << (double)countSoFar / (double)sum << endl;
		if ((double)countSoFar / (double)sum > .99)
			break;

	}
	p << "e" << endl << "unset output" << endl;
}

void StatsScripts::generateJointGraphs(const bf::path &outputDir, const StatsScripts &alternateStats)
{
	opstream p0("gnuplot");
	p0 << terminal << basicSetup;

	list<pair<string, string> > graphs;
	bf::path outFilename;

	//////////////////////////////////////////////////////////////////////////
	// make the cache hit-miss graph
	outFilename = outputDir / ("cacheHitRate." + extension);
	cacheHitMissGraph(outFilename,p0,alternateStats.getCacheHitMiss(), false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("cacheHitRate",
		"DIMM Cache Hit/Miss"));
	outFilename = outputDir / ("cacheHitRate-thumb." + thumbnailExtension);
	cacheHitMissGraph(outFilename,p0,alternateStats.getCacheHitMiss(), true);	
	//////////////////////////////////////////////////////////////////////////

	p0 << endl << "exit" << endl;


	p0.close();
	

#pragma omp critical
	{
		boost::mutex::scoped_lock lock(fileListMutex);
		for (list<string>::const_iterator i = filesGenerated.begin(); i
			!= filesGenerated.end(); ++i)
			fileList.push_back(*i);
	}

	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, graphs);	
}

void StatsScripts::generateGraphs(const bf::path &outputDir)
{
	if (!ensureDirectoryExists(outputDir))
		exit(-1);

	if (channelLatencyDistribution.size() < 1)
		return;

	opstream p0("gnuplot"), p1("gnuplot"), p2("gnuplot"), p3("gnuplot");
	
	list<pair<string, string> > graphs;
	bf::path outFilename;

	if (!cypressResults)
	{
		// make the address latency distribution per channel graphs
		for (unsigned channelID = 0; channelID
			< channelLatencyDistribution.size(); channelID++)
		{
			outFilename = outputDir / ("addressLatencyDistribution" + lexical_cast<string> (channelID) + "." + extension);
			addressLatencyDistributionPerChannelGraph(outFilename,p3,channelID, false);
			graphs.push_back(pair<string, string> (
				"addressLatencyDistribution" + lexical_cast<string> (channelID),
				"Address Latency Distribution, Channel " + lexical_cast<
				string> (channelID)));
			filesGenerated.push_back(outFilename.native_directory_string());
			outFilename = outputDir / ("addressLatencyDistribution" + lexical_cast<string> (channelID) + "-thumb." + thumbnailExtension);
			addressLatencyDistributionPerChannelGraph(outFilename,p3,channelID, true);

		}
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the address distribution per channel graphs
		for (unsigned channelID = 0; channelID < channelDistribution.size(); channelID++)
		{
			outFilename = outputDir / ("addressDistribution" + lexical_cast<string> (channelID) + "." + extension);
			addressDistributionPerChannelGraph(outFilename,p3,channelID, false);
			graphs.push_back(pair<string, string> ("addressDistribution" + lexical_cast<string> (channelID),
				"Address Distribution, Channel " + lexical_cast<string> (
				channelID)));
			filesGenerated.push_back(outFilename.native_directory_string());
			outFilename = outputDir / ("addressDistribution" + lexical_cast<string> (channelID) + "-thumb." + thumbnailExtension);
			addressDistributionPerChannelGraph(outFilename,p3,channelID, true);
		}
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the overall address distribution graphs
		outFilename = outputDir / ("addressDistribution." + extension);
		overallAddressDistributionGraph(outFilename,p3,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("addressDistribution",
			"Overall Address Distribution"));
		outFilename = outputDir / ("addressDistribution-thumb." + thumbnailExtension);
		overallAddressDistributionGraph(outFilename,p3, true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the PC vs latency graph
		outFilename = outputDir / ("latencyVsPc." + extension);
		pcVsLatencyGraph(outFilename,p0,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("latencyVsPc", "PC vs. Latency"));
		outFilename = outputDir / ("latencyVsPc-thumb." + thumbnailExtension);
		pcVsLatencyGraph(outFilename,p0,true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the PC vs average latency graph
		outFilename = outputDir / ("avgLatencyVsPc." + extension);
		pcVsAverageLatencyGraph(outFilename,p1,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("avgLatencyVsPc",
			"PC vs. Average Latency"));
		outFilename = outputDir / ("avgLatencyVsPc-thumb." + thumbnailExtension);
		pcVsAverageLatencyGraph(outFilename,p1,true);
		//////////////////////////////////////////////////////////////////////////
	}

	{
		//////////////////////////////////////////////////////////////////////////
		// make the transaction latency distribution graph
		//StdDev<float> latencyDeviation, latencyDeviation2;
		outFilename = outputDir / ("transactionLatencyDistribution." + extension);
		transactionLatencyDistributionGraph(outFilename, p2, false);
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("transactionLatencyDistribution-thumb." + thumbnailExtension);
		transactionLatencyDistributionGraph(outFilename, p2, true);
		graphs.push_back(pair<string, string> ("transactionLatencyDistribution",
			"Transaction Latency Distribution"));
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// make the zoomed transaction latency distribution graph
		outFilename = outputDir / ("zoomedTransactionLatencyDistribution." + extension);
		zoomedTransactionLatencyDistributionGraph(outFilename, p3, false);
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("zoomedTransactionLatencyDistribution-thumb." + thumbnailExtension);
		zoomedTransactionLatencyDistributionGraph(outFilename, p0, true);	
		graphs.push_back(pair<string, string> (
			"zoomedTransactionLatencyDistribution",
			"Zoomed Transaction Latency"));
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the bandwidth graph
	outFilename = outputDir / ("bandwidth." + extension);
	bandwidthGraph(outFilename, p2, false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bandwidth", "Bandwidth"));
	outFilename = outputDir / ("bandwidth-thumb." + thumbnailExtension);
	bandwidthGraph(outFilename, p2, true);
	//////////////////////////////////////////////////////////////////////////

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the cache graph
		outFilename = outputDir / ("cacheData." + extension);
		cacheGraph(outFilename,p0,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> (
			"cacheData", "Cache Data"));
		outFilename = outputDir / ("cacheData-thumb." + thumbnailExtension);
		cacheGraph(outFilename,p1,true);
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the other IPC graph
	outFilename = outputDir / ("averageIPCandLatency." + extension);
	averageIpcAndLatencyGraph(outFilename, p0, false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("averageIPCandLatency",
		"IPC and Latency"));
	outFilename = outputDir / ("averageIPCandLatency-thumb." + thumbnailExtension);
	averageIpcAndLatencyGraph(outFilename, p0, true);
	//////////////////////////////////////////////////////////////////////////

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the hit-miss graph
		outFilename = outputDir / ("rowHitRate." + extension);
		hitMissGraph(outFilename,p0,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("rowHitRate", "Row Reuse"));
		outFilename = outputDir / ("rowHitRate-thumb." + thumbnailExtension);
		hitMissGraph(outFilename,p0,true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the working set		
		outFilename = outputDir / ("workingSet." + extension);
		workingSetGraph(outFilename,p1,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("workingSet", "Working Set"));
		outFilename = outputDir / ("workingSet-thumb." + thumbnailExtension);
		workingSetGraph(outFilename,p2,true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make special IPC graph
		outFilename = outputDir / ("bigIpcGraph." + extension);
		bigIpcGraph(outFilename,p3,false);
		graphs.push_back(pair<string, string> ("bigIpcGraph", "Big IPC"));
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("bigIpcGraph-thumb." + thumbnailExtension);
		bigIpcGraph(outFilename,p3,true);
		//////////////////////////////////////////////////////////////////////////
	}

	

#ifndef NDEBUG
	cerr << "Period: " << periodInNs << endl;
	cerr << "tRC: " << tRC * periodInNs << "ns" << endl;
	cerr << "Epoch Time: " << epochTime;
#endif


	p0 << endl << "exit" << endl;
	p1 << endl << "exit" << endl;
	p2 << endl << "exit" << endl;
	p3 << endl << "exit" << endl;

	p0.close();
	p1.close();
	p2.close();
	p3.close();


#pragma omp critical
	{
		boost::mutex::scoped_lock lock(fileListMutex);
		for (list<string>::const_iterator i = filesGenerated.begin(); i
			!= filesGenerated.end(); ++i)
			fileList.push_back(*i);
	}

	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine, graphs);	
}

void StatsScripts::processLine(char *newLine)
{
	if (!foundCommandline)
	{
		if (starts_with(newLine, "----Command Line"))
		{
			char *position = strchr(newLine, ':');
			commandLine = getCommandLine(string(position + 2));	
			rawCommandLine = newLine;
			foundCommandline = true;

			// get the number of channels
			channelCount = regexMatch<unsigned>(newLine, "ch\\[([0-9]+)\\]");

			// get the number of ranks
			rankCount = regexMatch<unsigned>(newLine, "rk\\[([0-9]+)\\]");

			// get the number of banks
			bankCount = regexMatch<unsigned>(newLine, "bk\\[([0-9]+)\\]");

			// get the value of tRC
			tRC = regexMatch<unsigned>(newLine, "t_\\{RC\\}\\[([0-9]+)\\]");

			// get the value of tRAS
			tRAS = regexMatch<unsigned>(newLine, "t_\\{RAS\\}\\[([0-9]+)\\]");

			// determine the period from the datarate
			double freq = regexMatch<double>(newLine,"DR\\[([0-9]+)M\\]") * 1.0E6;
			periodInNs = 1 / freq / 1.0E-9;

			std::string cache = regexMatch<std::string>(newLine,"usingCache\\[([TF]+)\\]");

			usingCache = (cache == "T");

			channelLatencyDistribution.reserve(channelCount);

			for (unsigned i = channelCount; i > 0; --i)
			{
				channelLatencyDistribution.push_back(vector<vector<vector<
					unsigned> > > ());
				channelLatencyDistribution.back().reserve(rankCount);
				channelLatencyDistributionBuffer.push_back(vector<vector<
					uint64_t> > ());
				channelLatencyDistributionBuffer.back().reserve(rankCount);

				channelDistribution.push_back(vector<vector<
					vector<unsigned> > > ());
				channelDistribution.back().reserve(rankCount);
				channelDistributionBuffer.push_back(
					vector<vector<uint64_t> > ());
				channelDistributionBuffer.back().reserve(rankCount);

				for (unsigned j = rankCount; j > 0; --j)
				{
					channelLatencyDistribution.back().push_back(vector<
						vector<unsigned> > ());
					channelLatencyDistribution.back().back().reserve(
						bankCount + 1);
					channelLatencyDistributionBuffer.back().push_back(
						vector<uint64_t> ());
					channelLatencyDistributionBuffer.back().back().reserve(
						bankCount + 1);

					channelDistribution.back().push_back(vector<vector<
						unsigned> > ());
					channelDistribution.back().back().reserve(bankCount + 1);
					channelDistributionBuffer.back().push_back(vector<
						uint64_t> ());
					channelDistributionBuffer.back().back().reserve(
						bankCount + 1);

					for (unsigned k = bankCount + 1; k > 0; --k)
					{
						channelLatencyDistribution.back().back().push_back(
							vector<unsigned> ());
						channelLatencyDistribution.back().back().back().reserve(
							MAXIMUM_VECTOR_SIZE);
						channelLatencyDistributionBuffer.back().back().push_back(
							0ULL);

						channelDistribution.back().back().push_back(vector<
							unsigned> ());
						channelDistribution.back().back().back().reserve(
							MAXIMUM_VECTOR_SIZE);
						channelDistributionBuffer.back().back().push_back(
							0ULL);
					}
				}
			}
		}
	}
	else if (starts_with(newLine, "----M5 Stat:"))
	{
		char *firstOpeningBrace = strchr(newLine, '{');
		if (firstOpeningBrace == NULL) return;
		firstOpeningBrace++;
		char *firstClosingBrace = strchr(newLine, '}');
		if (firstClosingBrace == NULL) return;
		char *secondOpeningBrace = strchr(firstClosingBrace + 1, '{');
		if (secondOpeningBrace == NULL) return;
		secondOpeningBrace++;
		char *secondClosingBrace = strchr(firstClosingBrace + 1, '}');
		if (secondClosingBrace == NULL) return;
		*secondClosingBrace = *firstClosingBrace = NULL;

		if (strcmp(firstOpeningBrace, "system.cpu.dcache.overall_hits") == 0)
			dCacheHitBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.cpu.dcache.overall_miss_latency") == 0)
			dCacheMissLatencyBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.cpu.dcache.overall_misses") == 0)
			dCacheMissBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.cpu.icache.overall_hits") == 0)
			iCacheHitBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.cpu.icache.overall_miss_latency") == 0)
			iCacheMissLatencyBuffer = atof(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.cpu.icache.overall_misses") == 0)
			iCacheMissBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.l2.overall_hits") == 0)
			l2CacheHitBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.l2.overall_miss_latency") == 0)
			l2CacheMissLatencyBuffer = atof(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.l2.overall_misses") == 0)
			l2CacheMissBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.l2.overall_mshr_hits") == 0)
			l2MshrHitBuffer = atoi(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.l2.overall_mshr_miss_latency") == 0)
			l2MshrMissLatencyBuffer = atof(secondOpeningBrace);
		else if (strcmp(firstOpeningBrace, "system.l2.overall_mshr_misses") == 0)
			l2MshrMissBuffer = atoi(secondOpeningBrace);
		else
			cerr << "missed something: " << newLine << endl;
	}
	else if (starts_with(newLine, "----Epoch"))
	{
		epochTime = lexical_cast<float> (strchr(newLine, ' ') + 1);
		foundEpoch = true;
	}	
	else
	{
		if (starts_with(newLine, "----Transaction Latency"))
		{			
			// only if this is at least the second time around
			if (throughOnce)
			{
				pushStats();
			}
			else
			{
				throughOnce = true;
			}

			// aggregate all the transaction latencies
			char *position = newLine;
			
			while (position != NULL)
			{
				char *firstBracket = strchr(position, '{');
				if (firstBracket == NULL) break;

				char *comma = strchr(firstBracket + 1, ',');
				if (comma == NULL) break;

				char *secondBracket = strchr(comma + 1, '}');
				if (secondBracket == NULL) break;			

				*comma = *secondBracket = NULL;

				unsigned latency = atoi(firstBracket + 1);
				unsigned count = atoi(comma + 1);

				transactionCountBuffer += count;
				distTransactionLatency[latency] += count;
				
				totalLatency += (double)(latency * count);
				//cerr << latency * count << endl;
				totalCount += count;

				position = secondBracket + 1;
				if (*position == NULL) break;
			}
		}
		else if (starts_with(newLine,"----Cumulative DIMM Cache Read Hits/Misses"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				return;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = (char) NULL;
			readHitsMisses.first = max(atoi(firstBracket + 1), 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL)
				return;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = (char) NULL;
			readHitsMisses.second = max(atoi(firstBracket + 1), 1);
		}
		else if (starts_with(newLine, "----Cumulative DIMM Cache Hits/Misses"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				return;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = (char) NULL;
			hitsMisses.first = max(atoi(firstBracket + 1), 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL)
				return;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = (char) NULL;
			hitsMisses.second = max(atoi(firstBracket + 1), 1);
		}
		else if (starts_with(newLine, "----Working Set"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				return;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = NULL;
			workingSetSizeBuffer = atoi(firstBracket + 1);
		}
		else if (starts_with(newLine, "----Bandwidth"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				return;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = NULL;

			// read bandwidth
			bandwidthValuesBuffer.first = atol(firstBracket + 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL)
				return;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = NULL;

			// write bandwidth
			bandwidthValuesBuffer.second = atol(firstBracket + 1);
		}
		else if (starts_with(newLine,
			"----Average Transaction Latency Per PC Value"))
		{
			char *position = newLine;

			while (position != NULL)
			{
				char *firstBracket = strchr(position, '{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(position, '}');
				if (secondBracket == NULL)
					break;

				char *comma = strchr(position, ',');
				if (comma == NULL)
					break;

				char *secondComma = strchr(comma + 1, ',');
				if (secondComma == NULL)
					break;

				*comma = NULL;
				*secondComma = NULL;
				*secondBracket = NULL;

				uint64_t PC = strtol(firstBracket + 1, 0, 16);

				float averageAccessTime = atof(comma + 1);

				unsigned numberAccesses = atoi(secondComma + 1);

				if (PC < 0x100000000)
				{
					latencyVsPcLow[PC].first += numeric_cast<uint64_t> (
						averageAccessTime * (float) numberAccesses);
					latencyVsPcLow[PC].second += numberAccesses;
				}
				else
				{
					latencyVsPcHigh[PC].first += numeric_cast<uint64_t> (
						averageAccessTime * (float) numberAccesses);
					latencyVsPcHigh[PC].second += numberAccesses;
				}

				position = secondBracket + 1;
			}
		}
		else if (starts_with(newLine, "----DIMM Cache Bandwidth"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL) return;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL) return;

			*secondBracket = NULL;

			// read bandwidth
			cacheBandwidthValuesBuffer.first = atol(firstBracket + 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL)
				return;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = NULL;

			// write bandwidth
			cacheBandwidthValuesBuffer.second = atol(firstBracket + 1);
		}
		else if (starts_with(newLine, "----IPC"))
		{
			if (ipcLinesWritten < 1)
			{
				char *firstBracket = strchr(newLine,'{');
				char *secondBracket = strchr(newLine,'}');
				*secondBracket = NULL;
				float currentValue = starts_with(firstBracket + 1, "nan") ? 0.0F
					: atof(firstBracket + 1);
				if (currentValue != currentValue)
					currentValue = 0.0F;

				ipcValueBuffer += currentValue;
				cerr << "IPC added: " << currentValue << " " << endl;
			}
			ipcLinesWritten++;
			ipcLinesWritten = 0;
		}
		else if (starts_with(newLine, "----Row Hit/Miss Counts"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL) return;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL) return;
			*secondBracket = NULL;
			unsigned hitCount = max(atoi(firstBracket + 1), 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL) return;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL) return;
			*secondBracket = NULL;
			unsigned missCount = max(atoi(firstBracket + 1), 1);

			hitMissValueBuffer += hitCount
				/ ((double) missCount + hitCount);
			hitMissTotalBuffer += (hitCount + missCount);
		}
		else if (starts_with(newLine, "----Utilization"))
		{
			char *position = newLine;
			while (position != NULL)
			{
				char *leftParen = strchr(position, '(');
				if (leftParen == NULL) break;
				char *firstComma = strchr(position, ',');
				if (firstComma == NULL) break;
				char *secondComma = strchr(firstComma + 1, ',');
				if (secondComma == NULL) break;
				char *rightParen = strchr(position, ')');
				if (rightParen == NULL) break;

				*secondComma = *firstComma = *rightParen = NULL;

				unsigned channel = atoi(leftParen + 1);
				unsigned rank = atoi(firstComma + 1);
				unsigned bank = atoi(secondComma + 1);

				char *leftBracket = strchr(rightParen + 1, '{');
				if (leftBracket == NULL)
					break;
				char *rightBracket = strchr(rightParen + 1, '}');
				if (rightBracket == NULL)
					break;
				*rightBracket = NULL;
				unsigned value = atoi(leftBracket + 1);

				channelDistributionBuffer[channel][rank][bank] += value;
				channelDistributionBuffer[channel][rank].back() += value;

				position = rightBracket + 1;
			}
		}
		else if (starts_with(newLine, "----Latency Breakdown"))
		{
			char *position = newLine;
			while (position != NULL)
			{
				char *leftParen = strchr(position, '(');
				if (leftParen == NULL) break;
				char *firstComma = strchr(position, ',');
				if (firstComma == NULL) break;
				char *secondComma = strchr(firstComma + 1, ',');
				if (secondComma == NULL) break;
				char *rightParen = strchr(position, ')');
				if (rightParen == NULL) break;

				*rightParen = NULL;
				*firstComma = NULL;
				*secondComma = NULL;

				unsigned channel = atoi(leftParen + 1);
				unsigned rank = atoi(firstComma + 1);
				unsigned bank = atoi(secondComma + 1);

				char *leftBracket = strchr(rightParen + 1, '{');
				if (leftBracket == NULL)
					break;
				char *rightBracket = strchr(rightParen + 1, '}');
				if (rightBracket == NULL)
					break;
				*rightBracket = NULL;
				unsigned value = atoi(leftBracket + 1);

				channelLatencyDistributionBuffer[channel][rank][bank] += value;
				channelLatencyDistributionBuffer[channel][rank].back() += value;

				position = rightBracket + 1;
			}
		}
		else if (starts_with(newLine, "----Cache Hit/Miss Counts"))
		{
			char *firstBracket = strchr(newLine, '{');
			if (firstBracket == NULL)
				return;

			char *secondBracket = strchr(newLine, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = NULL;

			cacheHitMissBuffer.first = atoi(firstBracket + 1);

			firstBracket = strchr(secondBracket + 1, '{');
			if (firstBracket == NULL)
				return;

			secondBracket = strchr(secondBracket + 1, '}');
			if (secondBracket == NULL)
				return;
			*secondBracket = NULL;

			cacheHitMissBuffer.second = atol(firstBracket + 1);

		}
		else
		{
			// 				newLine[32] = NULL;
			// 				cerr << "Not matched: " << newLine << endl;
		}
	}
}

void StatsScripts::pushStats()
{
	epochCounter++;
	// look to dump the buffers into arrays
	scaleIndex = (scaleIndex + 1) % scaleFactor;

	// when the scale buffer is full
	if (scaleIndex == 0)
	{
		// dump all the buffers into arrays, scaled correctly
		for (unsigned i = 0; i < channelCount; ++i)
		{
			for (unsigned j = 0; j < rankCount; ++j)
			{
				for (unsigned k = 0; k < bankCount + 1; k++)
				{
					channelDistribution[i][j][k].push_back(
						channelDistributionBuffer[i][j][k]
					/ scaleFactor);
					channelDistributionBuffer[i][j][k] = 0;
					channelLatencyDistribution[i][j][k].push_back(
						channelLatencyDistributionBuffer[i][j][k]
					/ scaleFactor);
					channelLatencyDistributionBuffer[i][j][k]
					= 0;
				}
			}
		}

		transactionLatency.push_back(
			tuple<unsigned, unsigned, double, unsigned> (
			(tuple<unsigned, unsigned, double,
			unsigned> ) getAverageLatency()));
		
		transactionCount.push_back(transactionCountBuffer
			/ scaleFactor);
		transactionCountBuffer = 0;

		hitMissValues.push_back(hitMissValueBuffer
			/ scaleFactor);
		hitMissValueBuffer = 0;

		hitMissTotals.push_back(hitMissTotalBuffer
			/ scaleFactor);
		hitMissTotalBuffer = 0;

		iCacheHits.push_back(iCacheHitBuffer / scaleFactor);
		iCacheHitBuffer = 0;

		iCacheMisses.push_back(iCacheMissBuffer / scaleFactor);
		iCacheMissBuffer = 0;

		iCacheMissLatency.push_back(iCacheMissLatencyBuffer
			/ scaleFactor);
		iCacheMissLatencyBuffer = 0;

		dCacheHits.push_back(dCacheHitBuffer / scaleFactor);
		dCacheHitBuffer = 0;

		dCacheMisses.push_back(dCacheMissBuffer / scaleFactor);
		dCacheMissBuffer = 0;

		dCacheMissLatency.push_back(dCacheMissLatencyBuffer
			/ scaleFactor);
		dCacheMissLatencyBuffer = 0;

		l2CacheHits.push_back(l2CacheHitBuffer / scaleFactor);
		l2CacheHitBuffer = 0;

		l2CacheMisses.push_back(l2CacheMissBuffer / scaleFactor);
		l2CacheMissBuffer = 0;

		l2CacheMissLatency.push_back(l2CacheMissLatencyBuffer
			/ scaleFactor);
		l2CacheMissLatencyBuffer = 0;

		l2MshrHits.push_back(l2MshrHitBuffer / scaleFactor);
		l2MshrHitBuffer = 0;

		l2MshrMisses.push_back(l2MshrMissBuffer / scaleFactor);
		l2CacheMissBuffer = 0;

		l2MshrMissLatency.push_back(l2MshrMissLatencyBuffer
			/ scaleFactor);
		l2MshrMissLatencyBuffer = 0;

		bandwidthValues.push_back(pair<uint64_t, uint64_t> (
			bandwidthValuesBuffer.first / scaleFactor,
			bandwidthValuesBuffer.second / scaleFactor));
		bandwidthValuesBuffer.first
			= bandwidthValuesBuffer.second = 0;

		cacheBandwidthValues.push_back(
			pair<uint64_t, uint64_t> (
			cacheBandwidthValuesBuffer.first
			/ scaleFactor,
			cacheBandwidthValuesBuffer.second
			/ scaleFactor));
		cacheBandwidthValuesBuffer.first
			= cacheBandwidthValuesBuffer.second = 0;

		cacheHitMiss.push_back(pair<unsigned, unsigned> (
			cacheHitMissBuffer.first,
			cacheHitMissBuffer.second));
		cacheHitMissBuffer.first = cacheHitMissBuffer.second
			= 0;

		ipcValues.push_back(ipcValueBuffer / scaleFactor);
		ipcValueBuffer = 0;

		workingSetSize.push_back(workingSetSizeBuffer
			/ scaleFactor);
		workingSetSizeBuffer = 0;

		if (ipcValues.size() >= MAXIMUM_VECTOR_SIZE)
		{
			compressStats();
		}
	}
}

void StatsScripts::compressStats()
{
	// adjust the epoch time to account for the fact that each amount counts for more
	epochTime *= 2;
	// double the scale factor
	scaleFactor *= 2;

	cerr << "scaleFactor at " << scaleFactor << endl;

	// scale all the arrays by half
	for (unsigned epoch = 0; epoch
		< MAXIMUM_VECTOR_SIZE / 2; epoch++)
	{
		for (unsigned i = 0; i < channelCount; ++i)
		{
			for (unsigned j = 0; j < rankCount; ++j)
			{
				for (unsigned k = 0; k < bankCount + 1; k++)
				{
					channelDistribution[i][j][k][epoch]
					= (channelDistribution[i][j][k][2
						* epoch]
					+ channelDistribution[i][j][k][2
						* epoch + 1])
						/ 2;
					channelLatencyDistribution[i][j][k][epoch]
					= (channelLatencyDistribution[i][j][k][2
						* epoch]
					+ channelLatencyDistribution[i][j][k][2
						* epoch + 1])
						/ 2;
				}
			}
		}
		//fixit
		//transactionLatency[epoch] = (transactionLatency[2 * epoch] + transactionLatency[2 * epoch + 1]) / 2;
		transactionLatency[epoch].get<0> ()
			= (transactionLatency[2 * epoch].get<0> ()
			+ transactionLatency[2 * epoch
			+ 1].get<0> ()) / 2;
		transactionLatency[epoch].get<1> ()
			= (transactionLatency[2 * epoch].get<1> ()
			+ transactionLatency[2 * epoch
			+ 1].get<1> ()) / 2;
		transactionLatency[epoch].get<2> ()
			= (transactionLatency[2 * epoch].get<2> ()
			+ transactionLatency[2 * epoch
			+ 1].get<2> ()) / 2;
		transactionLatency[epoch].get<3> ()
			= (transactionLatency[2 * epoch].get<3> ()
			+ transactionLatency[2 * epoch
			+ 1].get<3> ()) / 2;
		//transactionLatency[epoch].get<4>() = (transactionLatency[2 * epoch].get<4>() + transactionLatency[2 * epoch + 1].get<4>()) / 2;
		//transactionLatency[epoch].get<5>() = (transactionLatency[2 * epoch].get<5>() + transactionLatency[2 * epoch + 1].get<5>()) / 2;

		transactionCount[epoch] = (transactionCount[2
			* epoch] + transactionCount[2 * epoch
			+ 1]) / 2;

		hitMissValues[epoch]
		= (hitMissValues[2 * epoch]
		+ hitMissValues[2 * epoch + 1])
			/ 2;

		hitMissTotals[epoch] = (hitMissTotals[2 * epoch] + hitMissTotals[2 * epoch + 1]) / 2;

		iCacheHits[epoch] = (iCacheHits[2 * epoch]
		+ iCacheHits[2 * epoch + 1]) / 2;

		iCacheMisses[epoch] = (iCacheMisses[2 * epoch]
		+ iCacheMisses[2 * epoch + 1]) / 2;

		iCacheMissLatency[epoch] = (iCacheMissLatency[2
			* epoch] + iCacheMissLatency[2 * epoch
			+ 1]) / 2;

		dCacheHits[epoch] = (dCacheHits[2 * epoch]
		+ dCacheHits[2 * epoch + 1]) / 2;

		dCacheMisses[epoch] = (dCacheMisses[2 * epoch]
		+ dCacheMisses[2 * epoch + 1]) / 2;

		dCacheMissLatency[epoch] = (dCacheMissLatency[2
			* epoch] + dCacheMissLatency[2 * epoch
			+ 1]) / 2;

		l2CacheHits[epoch] = (l2CacheHits[2 * epoch]
		+ l2CacheHits[2 * epoch + 1]) / 2;

		l2CacheMisses[epoch]
		= (l2CacheMisses[2 * epoch]
		+ l2CacheMisses[2 * epoch + 1])
			/ 2;

		l2CacheMissLatency[epoch]
		= (l2CacheMissLatency[2 * epoch]
		+ l2CacheMissLatency[2 * epoch
			+ 1]) / 2;

		l2MshrHits[epoch] = (l2MshrHits[2 * epoch]
		+ l2MshrHits[2 * epoch + 1]) / 2;

		l2MshrMisses[epoch] = (l2MshrMisses[2 * epoch]
		+ l2MshrMisses[2 * epoch + 1]) / 2;

		l2MshrMissLatency[epoch] = (l2MshrMissLatency[2
			* epoch] + l2MshrMissLatency[2 * epoch
			+ 1]) / 2;

		bandwidthValues[epoch].first
			= (bandwidthValues[2 * epoch].first
			+ bandwidthValues[2 * epoch + 1].first)
			/ 2;
		bandwidthValues[epoch].second
			= (bandwidthValues[2 * epoch].second
			+ bandwidthValues[2 * epoch + 1].second)
			/ 2;

		cacheBandwidthValues[epoch].first
			= (cacheBandwidthValues[2 * epoch].first
			+ cacheBandwidthValues[2
			* epoch + 1].first) / 2;
		cacheBandwidthValues[epoch].second
			= (cacheBandwidthValues[2 * epoch].second
			+ cacheBandwidthValues[2
			* epoch + 1].second)
			/ 2;

		cacheHitMiss[epoch].first = (cacheHitMiss[2
			* epoch].first + cacheHitMiss[2 * epoch
			+ 1].first) / 2;
		cacheHitMiss[epoch].second = (cacheHitMiss[2
			* epoch].second + cacheHitMiss[2
			* epoch + 1].second) / 2;

		ipcValues[epoch] = (ipcValues[2 * epoch]
		+ ipcValues[2 * epoch + 1]) / 2;

		workingSetSize[epoch] = (workingSetSize[2
			* epoch]
		+ workingSetSize[2 * epoch + 1]) / 2;
	}

	// resize all the vectors
	for (unsigned i = 0; i < channelCount; ++i)
	{
		for (unsigned j = 0; j < rankCount; ++j)
		{
			for (unsigned k = 0; k < bankCount + 1; k++)
			{
				channelDistribution[i][j][k].resize(
					MAXIMUM_VECTOR_SIZE / 2);
				channelLatencyDistribution[i][j][k].resize(
					MAXIMUM_VECTOR_SIZE / 2);
			}
		}
	}

	transactionLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

	transactionCount.resize(MAXIMUM_VECTOR_SIZE / 2);

	hitMissValues.resize(MAXIMUM_VECTOR_SIZE / 2);

	hitMissTotals.resize(MAXIMUM_VECTOR_SIZE / 2);

	iCacheHits.resize(MAXIMUM_VECTOR_SIZE / 2);

	iCacheMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

	iCacheMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

	dCacheHits.resize(MAXIMUM_VECTOR_SIZE / 2);

	dCacheMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

	dCacheMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

	l2CacheHits.resize(MAXIMUM_VECTOR_SIZE / 2);

	l2CacheMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

	l2CacheMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

	l2MshrHits.resize(MAXIMUM_VECTOR_SIZE / 2);

	l2MshrMisses.resize(MAXIMUM_VECTOR_SIZE / 2);

	l2MshrMissLatency.resize(MAXIMUM_VECTOR_SIZE / 2);

	bandwidthValues.resize(MAXIMUM_VECTOR_SIZE / 2);

	cacheBandwidthValues.resize(MAXIMUM_VECTOR_SIZE / 2);

	cacheHitMiss.resize(MAXIMUM_VECTOR_SIZE / 2);

	ipcValues.resize(MAXIMUM_VECTOR_SIZE / 2);

	workingSetSize.resize(MAXIMUM_VECTOR_SIZE / 2);
}


bool StatsScripts::processStatsForFile(const string &file)
{
	givenfilename = file;
	filtering_istream inputStream;

	if (ends_with(file, ".gz"))
		inputStream.push(boost::iostreams::gzip_decompressor());
	else if (ends_with(file, ".bz2"))
		inputStream.push(boost::iostreams::bzip2_decompressor());

	inputStream.push(file_source(file));

	char newLine[NEWLINE_LENGTH];

	if (!inputStream.is_complete())
		return false;

	for (inputStream.getline(newLine, NEWLINE_LENGTH);
		(newLine[0] != NULL) && (!userStop);
		inputStream.getline(newLine, NEWLINE_LENGTH))
	{				
		processLine(newLine);
	}

	return working();
}
