#include "processStats.hh"
#include "globals.hh"

using std::map;
using std::tr1::unordered_map;

void addressLatencyDistributionPerChannelGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
											   vector<vector<vector<vector<unsigned> > > > &channelLatencyDistribution,
											   float epochTime, unsigned channelID, bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << subAddrDistroA;
	p << "set multiplot layout "
		<< channelLatencyDistribution[channelID].size()
		<< ", 1 title \"{/=18" << commandLine << "\"" << endl;

	for (unsigned rankID = 0; rankID
		< channelLatencyDistribution[channelID].size(); rankID++)
	{
		p << "set title \"Rank " << rankID
			<< " Distribution Rate\" offset character 0, 0, 0 font \"\" norotate"
			<< endl;

		if (rankID < channelLatencyDistribution[channelID].size() - 1)
			p << "unset key" << endl << "unset label" << endl;
		else
			p << "set xlabel 'Time (s)' offset 0,0.6" << endl
			<< "set key outside center bottom horizontal reverse Left"
			<< endl;

		p << "plot ";
		for (unsigned a = 0; a
			< channelLatencyDistribution[channelID][rankID].size()
			- 1; a++)
			p << "'-' using 1 axes x2y1 t 'bank_{" << a << "}  ',";
		p
			<< "'-' using 1:2 axes x1y1 notitle with points pointsize 0.01"
			<< endl;

		for (unsigned bankID = 0; bankID
			< channelLatencyDistribution[channelID][rankID].size()
			- 1; bankID++)
		{
			for (unsigned epoch = 0; epoch
				< channelLatencyDistribution[channelID][rankID][bankID].size(); epoch++)
			{
				p
					<< max(
					1E-5F,
					channelLatencyDistribution[channelID][rankID][bankID][epoch]
				/ ((float) channelLatencyDistribution[channelID][rankID].back()[epoch]))
					<< endl;
			}
			p << "e" << endl;
		}
		p << channelLatencyDistribution[channelID][rankID][0].size()
			* epochTime << " " << "0.2" << endl << "e" << endl;
	}
	p << "unset multiplot" << endl << "unset output" << endl;
}


void addressDistributionPerChannelGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
										const vector<vector<vector<vector<unsigned> > > > &channelDistribution,
										float epochTime, unsigned channelID, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << subAddrDistroA;
	p << "set multiplot layout "
		<< channelDistribution[channelID].size()
		<< ", 1 title \"" << commandLine << "\"" << endl;

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

void overallAddressDistributionGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
									 unsigned channelCount,const vector<vector<vector<vector<unsigned> > > > &channelDistribution,
									 unsigned rankCount, unsigned bankCount,
									 float epochTime, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << commandLine << "\\nChannel Distribution Rate\""
		<< endl << addressDistroA;
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

void pcVsLatencyGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
					  const std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> > &latencyVsPcLow,
					  const std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> > &latencyVsPcHigh,
					  float period, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << pcVsLatencyGraphScript << endl;
	p << "set multiplot layout 1, 2 title \"" << commandLine
		<< "\\nTotal Latency Due to Reads vs. PC Value\"" << endl;
	p << "plot '-' using 1:2 t 'Total Latency' with boxes" << endl;

	if (latencyVsPcLow.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcLow.begin(); i != latencyVsPcLow.end(); ++i)
			p << i->first << " " << period * i->second.first << endl;
	else
		p << "4294967296 1.01" << endl;

	p << endl << "e" << endl << "set format x '0x1%x'" << endl
		<< "plot '-' using 1:2 t 'Total Latency' with boxes" << endl;
	if (latencyVsPcHigh.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcHigh.begin(); i != latencyVsPcHigh.end(); ++i)
			p << (i->first - 0x100000000) << " " << period
			* i->second.first << endl;
	else
		p << "4294967296 1.01" << endl;
	p << "e" << endl << "unset multiplot" << endl << "unset output"
		<< endl;
}

void pcVsAverageLatencyGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
							 const std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> > &latencyVsPcLow,
							 const std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> > &latencyVsPcHigh,
							 float period, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << avgPcVsLatencyGraphScript << endl;
	p << "set multiplot layout 1, 2 title \"" << commandLine
		<< "\\nAverage Latency Due to Reads vs. PC Value\"" << endl;
	p << "plot '-' using 1:2 t 'Average Latency' with boxes" << endl;

	if (latencyVsPcLow.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcLow.begin(); i != latencyVsPcLow.end(); ++i)
			p << i->first << " " << period * (i->second.first
			/ i->second.second) << endl;
	else
		p << "4294967296 1.01" << endl;

	p << endl << "e" << endl;
	p << "set format x '0x1%x'" << endl
		<< "plot '-' using 1:2 t 'Average Latency' with boxes" << endl;
	if (latencyVsPcHigh.size() > 0)
		for (std::tr1::unordered_map<uint64_t, pair<uint64_t, uint64_t> >::const_iterator
			i = latencyVsPcHigh.begin(); i != latencyVsPcHigh.end(); ++i)
			p << (i->first - 0x100000000) << " " << period
			* (i->second.first / i->second.second) << endl;
	else
		p << "4294967296 1.01" << endl;
	p << "e" << endl << "unset multiplot" << endl << "unset output"
		<< endl;
}

void transactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
										 const std::tr1::unordered_map<unsigned, unsigned> &distTransactionLatency, float period,
										 bool isThumbnail)
{
	p << endl << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << commandLine << "\\nRead Transaction Latency\""
		<< endl << transactionGraphScript << endl;
	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
	{
		p << i->first * period << " " << i->second << endl;
	}
	p << "e" << endl << "unset output" << endl;
}

void zoomedTransactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
											   const std::tr1::unordered_map<unsigned,unsigned> & distTransactionLatency, float period,
											   bool isThumbnail)
{
	StdDev<float> latencyDeviation;
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << commandLine << "\\nRead Transaction Latency\""
		<< endl << "set xrange [0:"
		<< latencyDeviation.getStdDev().get<1> () + 8
		* latencyDeviation.getStdDev().get<2> () << "]" << endl
		<< transactionGraphScript << endl;

	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
			latencyDeviation.add(i->first * period, i->second);

	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distTransactionLatency.begin(); i != distTransactionLatency.end(); ++i)
	{
		p << i->first * period << " " << i->second << endl;
	}
	p << "e" << endl << "unset output" << endl;
}

void adjustedTransactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
												 const std::tr1::unordered_map<unsigned,unsigned> & distAdjustedTransactionLatency, float period,
												 bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << commandLine
		<< "\\nAdjusted Read Transaction Latency\"" << endl
		<< transactionGraphScript << endl;
	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distAdjustedTransactionLatency.begin(), end =
		distAdjustedTransactionLatency.end(); i != end; ++i)
	{
		p << i->first * period << " " << i->second << endl;
	}
	p << "e" << endl << "unset output" << endl;
}

void zoomedAdjustedTransactionLatencyDistributionGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
													   const std::tr1::unordered_map<unsigned,unsigned> & distAdjustedTransactionLatency, float period,
													   bool isThumbnail)
{
	StdDev<float> latencyDeviation;
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << commandLine
		<< "\\nZoomed Adjusted Read Transaction Latency\"" << endl
		<< "set xrange [0:" << latencyDeviation.getStdDev().get<1> () + 8
		* latencyDeviation.getStdDev().get<2> () << "]" << endl
		<< transactionGraphScript << endl;
	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distAdjustedTransactionLatency.begin(), end =
		distAdjustedTransactionLatency.end(); i != end; ++i)
		latencyDeviation.add(i->first * period, i->second);

	for (std::tr1::unordered_map<unsigned, unsigned>::const_iterator i =
		distAdjustedTransactionLatency.begin(), end =
		distAdjustedTransactionLatency.end(); i != end; ++i)
	{
		p << i->first * period << " " << i->second << endl;
	}
#ifndef NDEBUG
	cerr << "range is " << latencyDeviation.getStdDev().get<1> () + 3
		* latencyDeviation.getStdDev().get<2> () << endl;
#endif
	p << "e" << endl << "unset output" << endl;
}

void bandwidthGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
					const vector<pair<uint64_t, uint64_t> > & bandwidthValues,const vector<float> &ipcValues,vector<pair<uint64_t, uint64_t> > &cacheBandwidthValues,
					float epochTime, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set multiplot title \"" << commandLine << "\"" << endl;
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

void cacheGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
				const vector<unsigned> & iCacheMisses,const vector<unsigned> &iCacheHits,
				const vector<unsigned> & dCacheMisses,const vector<unsigned> &dCacheHits,
				const vector<unsigned> & l2CacheMisses,const vector<unsigned> &l2CacheHits,
				float epochTime, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set multiplot layout 3, 1 title \"" << commandLine << "\""
		<< endl;
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

void averageIpcAndLatencyGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
							   vector<unsigned> & transactionCount, vector<tuple<unsigned, unsigned, double, unsigned> > &transactionLatency,
							   vector<float> &ipcValues,
							   float epochTime, float period, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set multiplot layout 2,1 title \"" << commandLine << "\"" << endl;

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
		p << time << " " << period * i->get<0> () << endl;
		time += epochTime;
	}
	p << "e" << endl;

	// mean
	time = 0.0F;
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator
		i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p << time << " " << period * i->get<1> () << endl;
		time += epochTime;
	}
	p << "e" << endl;

	time = 0.0F;
	// mean + 1 std dev
	for (vector<tuple<unsigned, unsigned, double, unsigned> >::const_iterator
		i = transactionLatency.begin(); i != transactionLatency.end(); ++i)
	{
		p << time << " " << period * (i->get<1> () + 2 * i->get<2> ()) << endl;
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

void hitMissGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
				  const vector<float> & hitMissValues,const vector<unsigned> &hitMissTotals, float epochTime, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << "Reuse Rate of Open Rows vs. Time\\n"
		<< commandLine << "\"" << endl << rowHitMissGraphScript << endl;

	float time = 0.0F;
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

	time = 0.0F;
	for (vector<unsigned>::const_iterator i = hitMissTotals.begin(), end = hitMissTotals.end(); i
		!= end; ++i)
	{
		p << time << " " << max(*i, 1U) << endl;
		time += epochTime;
	}

	p << "e" << endl << "unset output" << endl;
}

void workingSetGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
					 const vector<unsigned> & workingSetSize, float epochTime, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;

	p << "set title \"" << commandLine
		<< "\\nWorking Set Size vs Time\" offset character 0, -1, 0 font '' norotate"
		<< endl;	
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

void bigIpcGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
				 const vector<float> & ipcValues, float epochTime, bool isThumbnail)
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

void cacheHitMissGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
					   const vector<pair<unsigned,unsigned> > & cacheHitMiss, float epochTime, bool isThumbnail)
{
	p << "reset" << endl << (isThumbnail ? thumbnailTerminal : terminal) << basicSetup << "set output '"
		<< outFilename.native_directory_string() << "'" << endl;
	p << "set title \"" << "Per-DIMM Cache Hit Rate\\n" << commandLine << "\""
		<< endl << hitMissScript << endl;

	float time = 0.0F;
	for (vector<pair<unsigned, unsigned> >::const_iterator i =
		cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		p << time << " " << i->first + i->second << endl;
		//cerr << time << " " << i->first + i->second << endl;
		time += epochTime;
	}

	p << "e" << endl;

	time = 0.0F;
	for (vector<pair<unsigned, unsigned> >::const_iterator i =
		cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		p << time << " " << (double) i->first / (i->first + i->second) << endl;
		//cerr << time << " " << i->first << " " <<  i->second << endl;
		time += epochTime;
	}

	p << "e" << endl << "unset output" << endl;
}


//////////////////////////////////////////////////////////////////////////
void transactionLatencyCumulativeDistributionGraph(const bf::path &outFilename, opstream &p, const string& commandLine,
										 const unordered_map<unsigned, unsigned> &distTransactionLatency, float period,
										 bool isThumbnail)
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
	p << "set title \"" << commandLine << "\\nRead Transaction Latency Cumulative Distribution\""
		<< endl << cumulativeTransactionGraphScript << endl;
	for (map<unsigned, unsigned>::const_iterator i = orderedTransactionLatency.begin(), end = orderedTransactionLatency.end();
		i != end; ++i)
	{		
		countSoFar += i->second;
		p << i->first * period << " " << (double)countSoFar / (double)sum << endl;
		if ((double)countSoFar / (double)sum > .99)
			break;

	}
	p << "e" << endl << "unset output" << endl;
}


//////////////////////////////////////////////////////////////////////////
/// @brief process the stats file
//////////////////////////////////////////////////////////////////////////
void processStats(const bf::path &outputDir, const string &filename)
{
	if (!fileExists(filename))
	{
		cerr << "cannot find " << filename << endl;
		return;
	}

	if (!ensureDirectoryExists(outputDir))
		exit(-1);

	std::list<string> filesGenerated;

	unsigned scaleIndex = 0;
	unsigned scaleFactor = 1;
	bool throughOnce = false;

	// vectors to keep track of various stats
	vector<vector<vector<vector<unsigned> > > > channelDistribution;
	vector<vector<vector<uint64_t> > > channelDistributionBuffer;
	vector<vector<vector<vector<unsigned> > > > channelLatencyDistribution;
	vector<vector<vector<uint64_t> > > channelLatencyDistributionBuffer;

	std::tr1::unordered_map<boost::uint64_t, pair<uint64_t, uint64_t> >
		latencyVsPcLow;
	std::tr1::unordered_map<boost::uint64_t, pair<uint64_t, uint64_t> >
		latencyVsPcHigh;

	vector<tuple<unsigned, unsigned, double, unsigned> > transactionLatency;
	StdDev<float> averageTransactionLatency;
	StdDev<float> averageAdjustedTransactionLatency;

	vector<unsigned> transactionCount;
	transactionCount.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t transactionCountBuffer = 0ULL;

	vector<unsigned> adjustedTransactionCount;
	adjustedTransactionCount.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t adjustedTransactionCountBuffer = 0ULL;

	unordered_map<unsigned, unsigned> distTransactionLatency;

	unordered_map<unsigned, unsigned> distAdjustedTransactionLatency;

	vector<float> hitMissValues;
	hitMissValues.reserve(MAXIMUM_VECTOR_SIZE);
	double hitMissValueBuffer = 0.0;

	vector<unsigned> hitMissTotals;
	hitMissTotals.reserve(MAXIMUM_VECTOR_SIZE);
	unsigned hitMissTotalBuffer = 0U;

	vector<unsigned> iCacheHits;
	iCacheHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t iCacheHitBuffer = 0ULL;

	vector<unsigned> iCacheMisses;
	iCacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
	unsigned iCacheMissBuffer = 0U;

	vector<long> iCacheMissLatency;
	iCacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t iCacheMissLatencyBuffer = 0ULL;

	vector<unsigned> dCacheHits;
	dCacheHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t dCacheHitBuffer = 0ULL;

	vector<unsigned> dCacheMisses;
	dCacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t dCacheMissBuffer = 0ULL;

	vector<long> dCacheMissLatency;
	dCacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t dCacheMissLatencyBuffer = 0ULL;

	vector<unsigned> l2CacheHits;
	l2CacheHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2CacheHitBuffer = 0ULL;

	vector<unsigned> l2CacheMisses;
	l2CacheMisses.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2CacheMissBuffer = 0ULL;

	vector<long> l2CacheMissLatency;
	l2CacheMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2CacheMissLatencyBuffer = 0ULL;

	vector<unsigned> l2MshrHits;
	l2MshrHits.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2MshrHitBuffer = 0ULL;

	vector<unsigned> l2MshrMisses;
	l2MshrMisses.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2MshrMissBuffer = 0ULL;

	vector<long> l2MshrMissLatency;
	l2MshrMissLatency.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t l2MshrMissLatencyBuffer = 0ULL;

	vector<pair<uint64_t, uint64_t> > bandwidthValues;
	bandwidthValues.reserve(MAXIMUM_VECTOR_SIZE);
	pair<uint64_t, uint64_t> bandwidthValuesBuffer(0ULL, 0ULL);

	vector<pair<uint64_t, uint64_t> > cacheBandwidthValues;
	cacheBandwidthValues.reserve(MAXIMUM_VECTOR_SIZE);
	pair<uint64_t, uint64_t> cacheBandwidthValuesBuffer(0ULL, 0ULL);

	vector<pair<unsigned, unsigned> > cacheHitMiss;
	cacheHitMiss.reserve(MAXIMUM_VECTOR_SIZE);
	pair<unsigned, unsigned> cacheHitMissBuffer(0, 0);

	vector<float> ipcValues;
	ipcValues.reserve(MAXIMUM_VECTOR_SIZE);
	double ipcValueBuffer = 0.0;

	vector<unsigned> workingSetSize;
	workingSetSize.reserve(MAXIMUM_VECTOR_SIZE);
	uint64_t workingSetSizeBuffer;

	bool started = false;
	unsigned ipcLinesWritten = 0;
	float epochTime = 0.0F;
	float period = 0.0F;

	unsigned tRC = 0;
	unsigned tRAS = 0;

	unsigned channelCount = 0;
	unsigned rankCount = 0;
	unsigned bankCount = 0;

	string commandLine;

	filtering_istream inputStream;
	if (ends_with(filename, "gz"))
		inputStream.push(gzip_decompressor());
	else if (ends_with(filename, "bz2"))
		inputStream.push(bzip2_decompressor());

	inputStream.push(file_source(filename));

	char newLine[1024 * 1024];

	inputStream.getline(newLine, NEWLINE_LENGTH);

	while ((newLine[0] != 0) && (!userStop))
	{
		if (!started)
		{
			if (starts_with(newLine, "----Command Line"))
			{
				string line(newLine);
				trim(line);
				vector<string> splitLine;
				split(splitLine, line, is_any_of(":"));
				commandLine = splitLine[1];
				trim(commandLine);
				cerr << commandLine << endl;

				started = true;

				// get the number of channels
				regex channelSearch("ch\\[([0-9]+)\\]");
				cmatch what;

				if (regex_search(newLine, what, channelSearch))
				{
					string value(what[1].first, what[1].second);
					channelCount = lexical_cast<unsigned> (value);
				}
				else
					exit(-1);

				// get the number of ranks
				regex rankSearch("rk\\[([0-9]+)\\]");
				if (regex_search(newLine, what, rankSearch))
				{
					string value(what[1].first, what[1].second);
					rankCount = lexical_cast<unsigned> (value);
				}
				else
					exit(-1);

				// get the number of banks
				regex bankSearch("bk\\[([0-9]+)\\]");
				if (regex_search(newLine, what, bankSearch))
				{
					string value(what[1].first, what[1].second);
					bankCount = lexical_cast<unsigned> (value);
				}
				else
					exit(-1);

				// get the value of tRC
				regex trcSearch("t_\\{RC\\}\\[([0-9]+)\\]");
				if (regex_search(newLine, what, trcSearch))
				{
					string value(what[1].first, what[1].second);
					tRC = lexical_cast<unsigned> (value);
#ifndef NDEBUG
					cerr << "got tRC as " << tRC << endl;
#endif
				}
				else
					exit(-1);

				// get the value of tRC
				regex trasSearch("t_\\{RAS\\}\\[([0-9]+)\\]");
				if (regex_search(newLine, what, trasSearch))
				{
					string value(what[1].first, what[1].second);
					tRAS = lexical_cast<unsigned> (value);
#ifndef NDEBUG
					cerr << "got tRAS as " << tRAS << endl;
#endif
				}
				else
					exit(-1);

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
			char *position = strchr(newLine, ' ');
			position = strchr(position + 1, ' ');
			if (position == NULL)
				break;
			*position++ = 0;
			char* splitline2 = position;
			position = strchr(splitline2, ' ');
			if (position == NULL)
				break;
			*position++ = 0;
			char *position2 = strchr(position, ' ');
			if (position2 == NULL)
				break;
			*position2 = 0;

			if (strcmp(splitline2, "system.cpu.dcache.overall_hits") == 0)
				dCacheHitBuffer = atoi(position);
			else if (strcmp(splitline2,
				"system.cpu.dcache.overall_miss_latency") == 0)
				dCacheMissLatencyBuffer = atoi(position);
			else if (strcmp(splitline2, "system.cpu.dcache.overall_misses")
				== 0)
				dCacheMissBuffer = atoi(position);
			else if (strcmp(splitline2, "system.cpu.icache.overall_hits") == 0)
				iCacheHitBuffer = atoi(position);
			else if (strcmp(splitline2,
				"system.cpu.icache.overall_miss_latency") == 0)
				iCacheMissLatencyBuffer = atof(position);
			else if (strcmp(splitline2, "system.cpu.icache.overall_misses")
				== 0)
				iCacheMissBuffer = atoi(position);
			else if (strcmp(splitline2, "system.l2.overall_hits") == 0)
				l2CacheHitBuffer = atoi(position);
			else if (strcmp(splitline2, "system.l2.overall_miss_latency") == 0)
				l2CacheMissLatencyBuffer = atof(position);
			else if (strcmp(splitline2, "system.l2.overall_misses") == 0)
				l2CacheMissBuffer = atoi(position);
			else if (strcmp(splitline2, "system.l2.overall_mshr_hits") == 0)
				l2MshrHitBuffer = atoi(position);
			else if (strcmp(splitline2, "system.l2.overall_mshr_miss_latency")
				== 0)
				l2MshrMissLatencyBuffer = atof(position);
			else if (strcmp(splitline2, "system.l2.overall_mshr_misses") == 0)
				l2MshrMissBuffer = atoi(position);
			else
				cerr << "missed something: " << newLine << endl;
		}
		else if (starts_with(newLine, "----Epoch"))
		{
			epochTime = lexical_cast<float> (strchr(newLine, ' ') + 1);
		}
		else if (starts_with(newLine, "----Datarate"))
		{
			period = 1 / lexical_cast<float> (strchr(newLine, ' ') + 1)
				/ 0.000000001F;
		}
		else
		{
			if (starts_with(newLine, "----Transaction Latency"))
			{
				// only if this is at least the second time around
				if (throughOnce)
				{
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
							unsigned> ) averageTransactionLatency.getStdDev()));
						averageTransactionLatency.clear();

						transactionCount.push_back(transactionCountBuffer
							/ scaleFactor);
						transactionCountBuffer = 0;

						adjustedTransactionCount.push_back(
							adjustedTransactionCountBuffer / scaleFactor);
						adjustedTransactionCountBuffer = 0;

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

								adjustedTransactionCount[epoch]
								= (adjustedTransactionCount[2 * epoch]
								+ adjustedTransactionCount[2
									* epoch + 1]) / 2;

								hitMissValues[epoch]
								= (hitMissValues[2 * epoch]
								+ hitMissValues[2 * epoch + 1])
									/ 2;

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

							adjustedTransactionCount.resize(MAXIMUM_VECTOR_SIZE
								/ 2);

							hitMissValues.resize(MAXIMUM_VECTOR_SIZE / 2);

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
					}
				}
				else
				{
					throughOnce = true;
				}

				averageTransactionLatency.clear();
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

					*comma = *secondBracket = NULL;

					unsigned latency = atoi(firstBracket + 1);
					unsigned count = atoi(comma + 1);

					averageTransactionLatency.add(latency, count);
					transactionCountBuffer += count;
					distTransactionLatency[latency] += count;

					position = secondBracket + 1;
				}
			}
			else if (starts_with(newLine, "----Adjusted Transaction Latency"))
			{
				averageAdjustedTransactionLatency.clear();

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

					*comma = *secondBracket = NULL;

					unsigned latency = atoi(firstBracket + 1);
					unsigned count = atoi(comma + 1);

					averageAdjustedTransactionLatency.add(latency, count);
					//transactionCountBuffer += count;
					distAdjustedTransactionLatency[latency] += count;

					position = secondBracket + 1;
				}
			}
			else if (starts_with(newLine, "----Working Set"))
			{
				char *firstBracket = strchr(newLine, '{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine, '}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;
				workingSetSizeBuffer = atoi(firstBracket + 1);
			}
			else if (starts_with(newLine, "----Bandwidth"))
			{
				char *firstBracket = strchr(newLine, '{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine, '}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				// read bandwidth
				bandwidthValuesBuffer.first = atol(firstBracket + 1);

				firstBracket = strchr(secondBracket + 1, '{');
				if (firstBracket == NULL)
					break;

				secondBracket = strchr(secondBracket + 1, '}');
				if (secondBracket == NULL)
					break;
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
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine, '}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				// read bandwidth
				cacheBandwidthValuesBuffer.first = atol(firstBracket + 1);

				firstBracket = strchr(secondBracket + 1, '{');
				if (firstBracket == NULL)
					break;

				secondBracket = strchr(secondBracket + 1, '}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				// write bandwidth
				cacheBandwidthValuesBuffer.second = atol(firstBracket + 1);
			}
			else if (starts_with(newLine, "----IPC"))
			{
				if (ipcLinesWritten < 1)
				{
					float currentValue = starts_with(newLine, "nan") ? 0.0F
						: atof(newLine);
					if (currentValue != currentValue)
						currentValue = 0.0F;

					ipcValueBuffer += currentValue;
				}
				ipcLinesWritten++;
				ipcLinesWritten = 0;
			}
			else if (starts_with(newLine, "----Row Hit/Miss Counts"))
			{
				char *firstBracket = strchr(newLine, '{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine, '}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;
				unsigned hitCount = max(atoi(firstBracket + 1), 1);

				firstBracket = strchr(secondBracket + 1, '{');
				if (firstBracket == NULL)
					break;

				secondBracket = strchr(secondBracket + 1, '}');
				if (secondBracket == NULL)
					break;
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
					if (leftParen == NULL)
						break;
					char *firstComma = strchr(position, ',');
					if (firstComma == NULL)
						break;
					char *secondComma = strchr(firstComma + 1, ',');
					if (secondComma == NULL)
						break;
					char *rightParen = strchr(position, ')');
					if (rightParen == NULL)
						break;

					*secondComma = NULL;
					*firstComma = NULL;
					*rightParen = NULL;

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
					if (leftParen == NULL)
						break;
					char *firstComma = strchr(position, ',');
					if (firstComma == NULL)
						break;
					char *secondComma = strchr(firstComma + 1, ',');
					if (secondComma == NULL)
						break;
					char *rightParen = strchr(position, ')');
					if (rightParen == NULL)
						break;

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

					channelLatencyDistributionBuffer[channel][rank][bank]
					+= value;
					channelLatencyDistributionBuffer[channel][rank].back()
						+= value;

					position = rightBracket + 1;
				}
			}
			else if (starts_with(newLine, "----Cache Hit/Miss Counts"))
			{
				char *firstBracket = strchr(newLine, '{');
				if (firstBracket == NULL)
					break;

				char *secondBracket = strchr(newLine, '}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				cacheHitMissBuffer.first = atoi(firstBracket + 1);

				firstBracket = strchr(secondBracket + 1, '{');
				if (firstBracket == NULL)
					break;

				secondBracket = strchr(secondBracket + 1, '}');
				if (secondBracket == NULL)
					break;
				*secondBracket = NULL;

				cacheHitMissBuffer.second = atol(firstBracket + 1);

			}
			else
			{
				// 				newLine[32] = NULL;
				// 				cerr << "Not matched: " << newLine << endl;
			}
		}

		inputStream.getline(newLine, NEWLINE_LENGTH);
	} // end going through lines

	userStop = false;

	if (channelLatencyDistribution.size() < 1)
		return;

	opstream p0("gnuplot");
	p0 << terminal << basicSetup;
	opstream p1("gnuplot");
	p1 << terminal << basicSetup;
	opstream p2("gnuplot");
	p2 << terminal << basicSetup;
	opstream p3("gnuplot");
	p3 << terminal << basicSetup;

	bf::path outFilename;

	list<pair<string, string> > graphs;

	if (!cypressResults)
	{
		// make the address latency distribution per channel graphs
		for (unsigned channelID = 0; channelID
			< channelLatencyDistribution.size(); channelID++)
		{
			outFilename = outputDir / ("addressLatencyDistribution" + lexical_cast<string> (channelID) + "." + extension);
			addressLatencyDistributionPerChannelGraph(outFilename,p3,commandLine,channelDistribution,epochTime,channelID, false);
			graphs.push_back(pair<string, string> (
				"addressLatencyDistribution" + lexical_cast<string> (channelID),
				"Address Latency Distribution, Channel " + lexical_cast<
				string> (channelID)));
			filesGenerated.push_back(outFilename.native_directory_string());
			outFilename = outputDir / ("addressLatencyDistribution" + lexical_cast<string> (channelID) + "-thumb." + thumbnailExtension);
			addressLatencyDistributionPerChannelGraph(outFilename,p3,commandLine,channelDistribution,epochTime,channelID, true);

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
			addressDistributionPerChannelGraph(outFilename,p3,commandLine,channelDistribution,epochTime,channelID, false);
			graphs.push_back(pair<string, string> ("addressDistribution" + lexical_cast<string> (channelID),
				"Address Distribution, Channel " + lexical_cast<string> (
				channelID)));
			filesGenerated.push_back(outFilename.native_directory_string());
			outFilename = outputDir / ("addressDistribution" + lexical_cast<string> (channelID) + "-thumb." + thumbnailExtension);
			addressDistributionPerChannelGraph(outFilename,p3,commandLine,channelDistribution,epochTime,channelID, true);
		}
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the overall address distribution graphs
		outFilename = outputDir / ("addressDistribution." + extension);
		overallAddressDistributionGraph(outFilename,p3,commandLine,channelCount,channelDistribution,rankCount,bankCount,epochTime,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("addressDistribution",
			"Overall Address Distribution"));
		outFilename = outputDir / ("addressDistribution-thumb." + thumbnailExtension);
		overallAddressDistributionGraph(outFilename,p3,commandLine,channelCount,channelDistribution,rankCount,bankCount,epochTime, true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the PC vs latency graph
		outFilename = outputDir / ("latencyVsPc." + extension);
		pcVsLatencyGraph(outFilename,p0,commandLine,latencyVsPcLow,latencyVsPcHigh,period,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("latencyVsPc", "PC vs. Latency"));
		outFilename = outputDir / ("latencyVsPc-thumb." + thumbnailExtension);
		pcVsLatencyGraph(outFilename,p0,commandLine,latencyVsPcLow,latencyVsPcHigh,period,true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the PC vs average latency graph
		outFilename = outputDir / ("avgLatencyVsPc." + extension);
		pcVsAverageLatencyGraph(outFilename,p1,commandLine,latencyVsPcLow,latencyVsPcHigh,period,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("avgLatencyVsPc",
			"PC vs. Average Latency"));
		outFilename = outputDir / ("avgLatencyVsPc-thumb." + thumbnailExtension);
		pcVsAverageLatencyGraph(outFilename,p1,commandLine,latencyVsPcLow,latencyVsPcHigh,period,true);
		//////////////////////////////////////////////////////////////////////////
	}

	{
		//////////////////////////////////////////////////////////////////////////
		// make the transaction latency distribution graph
		//StdDev<float> latencyDeviation, latencyDeviation2;
		outFilename = outputDir / ("transactionLatencyDistribution." + extension);
		transactionLatencyDistributionGraph(outFilename, p2, commandLine, distTransactionLatency, period,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("transactionLatencyDistribution-thumb." + thumbnailExtension);
		transactionLatencyDistributionGraph(outFilename, p2, commandLine, distTransactionLatency, period,true);
		graphs.push_back(pair<string, string> ("transactionLatencyDistribution",
			"Transaction Latency Distribution"));
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// make the zoomed transaction latency distribution graph
		outFilename = outputDir / ("zoomedTransactionLatencyDistribution." + extension);
		zoomedTransactionLatencyDistributionGraph(outFilename, p3, commandLine, distTransactionLatency,period,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("zoomedTransactionLatencyDistribution-thumb." + thumbnailExtension);
		zoomedTransactionLatencyDistributionGraph(outFilename, p0, commandLine, distTransactionLatency, period,true);	
		graphs.push_back(pair<string, string> (
			"zoomedTransactionLatencyDistribution",
			"Zoomed Transaction Latency"));
		//////////////////////////////////////////////////////////////////////////
	}

	{
		//////////////////////////////////////////////////////////////////////////
		// make the adjusted transaction latency distribution graph
		//StdDev<float> latencyDeviation, latencyDeviation2;
		outFilename = outputDir / ("adjustedTransactionLatencyDistribution." + extension);
		adjustedTransactionLatencyDistributionGraph(outFilename,p0,commandLine, distAdjustedTransactionLatency,period,false);
		graphs.push_back(pair<string, string> (
			"adjustedTransactionLatencyDistribution",
			"Adjusted Transaction Latency"));
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("adjustedTransactionLatencyDistribution-thumb." + thumbnailExtension);
		adjustedTransactionLatencyDistributionGraph(outFilename, p1, commandLine, distAdjustedTransactionLatency, period,true);		
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// make the zoomed adjusted transaction latency distribution graph
		outFilename = outputDir / ("zoomedAdjustedTransactionLatencyDistribution." + extension);
		zoomedAdjustedTransactionLatencyDistributionGraph(outFilename, p2, commandLine, distAdjustedTransactionLatency, period,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("zoomedAdjustedTransactionLatencyDistribution-thumb." + thumbnailExtension);
		zoomedAdjustedTransactionLatencyDistributionGraph(outFilename, p3, commandLine, distAdjustedTransactionLatency, period,true);	
		graphs.push_back(pair<string, string> (
			"zoomedAdjustedTransactionLatencyDistribution",
			"Zoomed Adjusted Transaction Latency"));	
		//////////////////////////////////////////////////////////////////////////
	}

	{
		//////////////////////////////////////////////////////////////////////////
		// make the transaction latency cumulative distribution graph
		outFilename = outputDir / ("transactionLatencyCumulativeDistribution." + extension);
		transactionLatencyCumulativeDistributionGraph(outFilename, p2, commandLine, distTransactionLatency, period,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("transactionLatencyCumulativeDistribution-thumb." + thumbnailExtension);
		transactionLatencyCumulativeDistributionGraph(outFilename, p2, commandLine, distTransactionLatency, period,true);
		graphs.push_back(pair<string, string> ("transactionLatencyCumulativeDistribution",
			"Transaction Latency Cumulative Distribution"));
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the bandwidth graph
	outFilename = outputDir / ("bandwidth." + extension);
	bandwidthGraph(outFilename, p2, commandLine, bandwidthValues, ipcValues,cacheBandwidthValues, epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("bandwidth", "Bandwidth"));
	outFilename = outputDir / ("bandwidth-thumb." + thumbnailExtension);
	bandwidthGraph(outFilename, p2, commandLine, bandwidthValues, ipcValues,cacheBandwidthValues, epochTime,true);
	//////////////////////////////////////////////////////////////////////////

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the cache graph
		outFilename = outputDir / ("cacheData." + extension);
		cacheGraph(outFilename,p0,commandLine,iCacheMisses,iCacheHits,dCacheMisses,dCacheHits,l2CacheMisses,l2CacheHits, epochTime,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> (
			"cacheData", "Cache Data"));
		outFilename = outputDir / ("cacheData-thumb." + thumbnailExtension);
		cacheGraph(outFilename,p1,commandLine,iCacheMisses,iCacheHits,dCacheMisses,dCacheHits,l2CacheMisses,l2CacheHits, epochTime,true);
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the other IPC graph
	outFilename = outputDir / ("averageIPCandLatency." + extension);
	averageIpcAndLatencyGraph(outFilename, p0, commandLine, transactionCount, transactionLatency,ipcValues, epochTime, period,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("averageIPCandLatency",
		"IPC and Latency"));
	outFilename = outputDir / ("averageIPCandLatency-thumb." + thumbnailExtension);
	averageIpcAndLatencyGraph(outFilename, p0, commandLine, transactionCount, transactionLatency,ipcValues, epochTime, period,true);
	//////////////////////////////////////////////////////////////////////////

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the hit-miss graph
		outFilename = outputDir / ("rowHitRate." + extension);
		hitMissGraph(outFilename,p0,commandLine,hitMissValues,hitMissTotals,epochTime,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("rowHitRate", "Row Reuse"));
		outFilename = outputDir / ("rowHitRate-thumb." + thumbnailExtension);
		hitMissGraph(outFilename,p0,commandLine,hitMissValues,hitMissTotals,epochTime,true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make the working set		
		outFilename = outputDir / ("workingSet." + extension);
		workingSetGraph(outFilename,p1,commandLine,workingSetSize,epochTime,false);
		filesGenerated.push_back(outFilename.native_directory_string());
		graphs.push_back(pair<string, string> ("workingSet", "Working Set"));
		outFilename = outputDir / ("workingSet-thumb." + thumbnailExtension);
		workingSetGraph(outFilename,p2,commandLine,workingSetSize,epochTime,true);
		//////////////////////////////////////////////////////////////////////////
	}

	if (!cypressResults)
	{
		//////////////////////////////////////////////////////////////////////////
		// make special IPC graph
		outFilename = outputDir / ("bigIpcGraph." + extension);
		bigIpcGraph(outFilename,p3,commandLine,ipcValues,epochTime,false);
		graphs.push_back(pair<string, string> ("bigIpcGraph", "Big IPC"));
		filesGenerated.push_back(outFilename.native_directory_string());
		outFilename = outputDir / ("bigIpcGraph-thumb." + thumbnailExtension);
		bigIpcGraph(outFilename,p3,commandLine,ipcValues,epochTime,true);
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// make the cache hit-miss graph
	outFilename = outputDir / ("cacheHitRate." + extension);
	cacheHitMissGraph(outFilename,p1,commandLine,cacheHitMiss,epochTime,false);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string, string> ("cacheHitRate",
		"DIMM Cache Hit/Miss"));
	outFilename = outputDir / ("cacheHitRate-thumb." + thumbnailExtension);
	cacheHitMissGraph(outFilename,p1,commandLine,cacheHitMiss,epochTime,true);	
	//////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
	cerr << "Period: " << period << endl;
	cerr << "tRC: " << tRC * period << "ns" << endl;
	cerr << "Epoch Time: " << epochTime;
#endif

	//////////////////////////////////////////////////////////////////////////
#if 0
	//////////////////////////////////////////////////////////////////////////
	// make the cache hit-miss graph
	outFilename = outputDir / ("cachePower." + extension);
	filesGenerated.push_back(outFilename.native_directory_string());
	graphs.push_back(pair<string,string>(outFilename.native_directory_string(),"Cache Power"));

	p2 << "reset" << endl << terminal << basicSetup << "set output '" << outFilename.native_directory_string() << "'" << endl;
	p2 << "set title \"" << "Power Usage of SRAM vs. DRAM\\n" << commandLine << "\"" << endl << hitMissPowerScript << endl;

	time = 0.0F;
	float voltageScaleFactor = 1.8F / 1.9F;
	float frequencyScaleFactor = 800000000 / 800000000;
	float devicesPerRank = 8;
	float IDD0 = 90.0F;
	float IDD3N = 60.0F;
	float IDD2N = 60.0F;
	float vddMax = 1.9F;
	float vdd = 1.8F;
	float IDD2P = 7.0F;
	float IDD3P = 50.0F;
	double CKE_LO_PRE = 0.95F;
	double CKE_LO_ACT = 0.01F;
	float PdsACT = IDD0 - ((IDD3N * tRAS + IDD2N * (tRC - tRAS)/tRC)) * vdd;

	for (vector<pair<unsigned,unsigned> >::const_iterator i = cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		double percentActive = ((i->first + i->second) * tRC * period) / epochTime;
		cerr << percentActive << endl;

		// calculate PsysACT-STBY
		float PsysACT_STBY = devicesPerRank * voltageScaleFactor * frequencyScaleFactor *
			IDD3N * vddMax * percentActive * (1.0F - CKE_LO_ACT);

		// calculate PsysPRE-STBY
		float PsysPRE_STBY = devicesPerRank * voltageScaleFactor * frequencyScaleFactor *
			IDD2N * vddMax * (1 - percentActive) * (1 - CKE_LO_PRE);

		// calculate PsysPRE-PDN
		float PsysPRE_PDN = voltageScaleFactor * IDD2P * vddMax * (1 - percentActive) * CKE_LO_PRE;

		// calculate PsysACT-PDN
		/// @todo: account for CKE
		float PsysACT_PDN = voltageScaleFactor * IDD3P * vddMax * percentActive * (1 - CKE_LO_ACT);

		// calculate PsysACT
		double tRRDsch = (double)(epochTime) / (i->first + i->second);

		double PsysACT = devicesPerRank * ((double)tRC / (double)tRRDsch) * voltageScaleFactor * PdsACT;

		double PsysACTTotal = ((double)tRC / (double)tRRDsch) * voltageScaleFactor * PdsACT;

		// calculate PdsRD
		double RDschPct = / (double)(epochTime);

		PsysRD += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsRD() * RDschPct;

		// calculate PdsWR
		double WRschPct = k->getWriteCycles() / (double)(time - powerModel.getLastCalculation());

		PsysWR += powerModel.getDevicesPerRank() * powerModel.getVoltageScaleFactor() * powerModel.getFrequencyScaleFactor() * powerModel.getPdsWR() * WRschPct;

		p2 << time << " " << i->first + i->second << endl;
		//cerr << time << " " << i->first + i->second << endl;
		time += epochTime;
	}

	p2 << "e" << endl;

	time = 0.0F;
	for (vector<pair<unsigned,unsigned> >::const_iterator i = cacheHitMiss.begin(); i != cacheHitMiss.end(); ++i)
	{
		p2 << time << " " << (double)i->first / (i->first + i->second) << endl;
		//cerr << time << " " << (double)i->first / (i->first + i->second) << endl;
		time += epochTime;
	}

	p2 << "e" << endl << "unset output" << endl;

	//////////////////////////////////////////////////////////////////////////
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

	bf::path givenfilename(filename);
	prepareOutputDir(outputDir, givenfilename.leaf(), commandLine,
		graphs);
	return;
}
