#include "PowerParameters.hh"

#include <boost/algorithm/string/regex.hpp>
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "processStats.hh"


using std::list;
using std::pair;
using std::string;
using std::vector;
using std::cerr;
using std::cout;
using std::endl;
using boost::split;
using boost::regex;
using boost::cmatch;
using std::max;
using std::min;


void PowerParameters::setParameters(const char* commandLine, const list<pair<string,string> > &updatedPowerParams)
{
	dimms = regexMatch<unsigned>(commandLine, "dimm\\[([0-9]+)\\]");

	tRc = regexMatch<double>(commandLine, "\\{RC\\}\\[([0-9]+)\\]");

	tBurst = regexMatch<double>(commandLine, "tBurst\\[([0-9]+)\\]");

	idd1 = regexMatch<double>(commandLine,"IDD1\\[([0-9]+)\\]");

	double idd0 = regexMatch<double>(commandLine,"IDD0\\[([0-9]+)\\]");

	double idd3n = regexMatch<double>(commandLine,"IDD3N\\[([0-9]+)\\]");

	double tRas = regexMatch<double>(commandLine,"\\{RAS\\}\\[([0-9]+)\\]");

	double idd2n = regexMatch<double>(commandLine,"IDD2N\\[([0-9]+)\\]");

	vdd = regexMatch<double>(commandLine,"VDD\\[([0-9.]+)\\]");

	double vddMax = regexMatch<double>(commandLine,"VDDmax\\[([0-9.]+)\\]");

	double idd3p = regexMatch<double>(commandLine,"IDD3P\\[([0-9]+)\\]");

	double idd2p = regexMatch<double>(commandLine,"IDD2P\\[([0-9]+)\\]");

	double idd4r = regexMatch<double>(commandLine,"IDD4R\\[([0-9]+)\\]");

	double idd4w = regexMatch<double>(commandLine,"IDD4W\\[([0-9]+)\\]");

	double specFreq = regexMatch<double>(commandLine,"spedFreq\\[([0-9]+)\\]");

	std::string cache = regexMatch<std::string>(commandLine,"usingCache\\[([TF]+)\\]");

	usingCache = (cache == "T");

	freq = regexMatch<double>(commandLine,"DR\\[([0-9]+)M\\]") * 1E6;

	/// @TODO fix this to read the value from the simulation
	devicesPerRank = 8;

	int channelWidth = regexMatch<int>(commandLine,"ChannelWidth\\[([0-9]+)\\]");

	int dqPerDram = regexMatch<int>(commandLine,"DQPerDRAM\\[([0-9]+)\\]");

	ranksPerDimm = regexMatch<unsigned>(commandLine,"rk\\[([0-9]+)\\]");

	unsigned associativity = regexMatch<unsigned>(commandLine,"assoc\\[([0-9]+)\\]");

	/// @TODO get this value from the simulation
	idd = 500; // in mA

	/// @TODO get this from the simulation results
	isb1 = 340; // in mA

	if (associativity == 2)
		hitLatency = 2.0;
	else if (associativity == 4)
		hitLatency = 3.0;
	else if (associativity == 8)
		hitLatency = 4.0;
	else if (associativity == 16)
		hitLatency = 5.0;
	else if (associativity == 24)
		hitLatency = 6.0;
	else if (associativity == 32)
		hitLatency = 7.0;
	else
	{
		cerr << "error: unknown associativity, cannot set hit latency accordingly" << endl;
		exit(-1);
	}

	// read power params to see what the requested changes are
	for (list<pair<string,string> >::const_iterator currentPair = updatedPowerParams.begin(), end = updatedPowerParams.end();
		currentPair != end; ++currentPair)
	{
		string parameter(currentPair->first);
		string value(currentPair->second);
		boost::algorithm::to_lower(parameter);

		if (parameter == "idd0")
		{
#ifndef NDEBUG
			cerr << "note: idd0 updated to " << value << endl;
#endif
			idd0 = lexical_cast<double>(value);
		}
		else if (parameter == "devicesperrank")
		{
#ifndef NDEBUG
			cerr << "note: devicesPerRank updated to " << value << endl;
#endif // NDEBUG
			devicesPerRank = lexical_cast<double>(value);
		}
		else if (parameter == "idd")
		{
#ifndef NDEBUG
			cerr << "note: IDD (DIMM cache) updated to " << value << endl;
#endif
			idd = lexical_cast<double>(value);
		}
		else if (parameter == "isb1")
		{
#ifndef NDEBUG
			cerr << "note: ISB1 updated to " << value << endl;
#endif
			isb1 = lexical_cast<double>(value);
		}
		else if (parameter == "idd3n")
		{
#ifndef NDEBUG
			cerr << "note: idd3n updated to " << value << endl;
#endif
			idd3n = lexical_cast<double>(value);
		}
		else if (parameter == "tras")
		{
#ifndef NDEBUG
			cerr << "note: tras updated to " << value << endl;
#endif
			tRas = lexical_cast<double>(value);
		}
		else if (parameter == "idd2n")
		{
#ifndef NDEBUG
			cerr << "note: idd2n updated to " << value << endl;
#endif
			idd2n = lexical_cast<double>(value);
		}
		else if (parameter == "vdd")
		{
#ifndef NDEBUG
			cerr << "note: vdd updated to " << value << endl;
#endif
			vdd = lexical_cast<double>(value);
		}
		else if (parameter == "idd3n")
		{
#ifndef NDEBUG
			cerr << "note: idd3n updated to " << value << endl;
#endif
			idd3n = lexical_cast<double>(value);
		}
		else if (parameter == "vddmax")
		{
#ifndef NDEBUG
			cerr << "note: vddmax updated to " << value << endl;
#endif
			vddMax = lexical_cast<double>(value);
		}
		else if (parameter == "idd3p")
		{
#ifndef NDEBUG
			cerr << "note: idd3p updated to " << value << endl;
#endif
			idd3p = lexical_cast<double>(value);
		}
		else if (parameter == "idd2p")
		{
#ifndef NDEBUG
			cerr << "note: idd2p updated to " << value << endl;
#endif
			idd2p = lexical_cast<double>(value);
		}
		else if (parameter == "idd4r")
		{
#ifndef NDEBUG
			cerr << "note: idd4r updated to " << value << endl;
#endif
			idd4r = lexical_cast<double>(value);
		}
		else if (parameter == "idd4w")
		{
#ifndef NDEBUG
			cerr << "note: idd4w updated to " << value << endl;
#endif
			idd4w = lexical_cast<double>(value);
		}
		else if (parameter == "specfreq")
		{
#ifndef NDEBUG
			cerr << "note: specFreq updated to " << value << endl;
#endif
			specFreq = lexical_cast<double>(value);
		}
		else if (parameter == "freq")
		{
#ifndef NDEBUG
			cerr << "note: freq updated to " << value << endl;
#endif
			freq = lexical_cast<double>(value);
		}
		else if (parameter == "channelwidth")
		{
#ifndef NDEBUG
			cerr << "note: channelWidth updated to " << value << endl;
#endif
			channelWidth = lexical_cast<double>(value);
		}
		else if (parameter == "dqperdram")
		{
#ifndef NDEBUG
			cerr << "note: dqPerDram updated to " << value << endl;
#endif
			dqPerDram = lexical_cast<double>(value);
		}
		else if (parameter == "cke_lo_pre")
		{
#ifndef NDEBUG
			cerr << "note: CKE_LO_PRE updated to " << value << endl;
#endif
			CKE_LO_PRE = lexical_cast<double>(value);
		}
		else if (parameter == "cke_lo_act")
		{
#ifndef NDEBUG
			cerr << "note: CKE_LO_ACT updated to " << value << endl;
#endif
			CKE_LO_ACT = lexical_cast<double>(value);
		}
	}

	// update power values
	devicesPerRank = channelWidth / dqPerDram;
	pDsAct = (idd0 - ((idd3n * tRas + idd2n * (tRc - tRas))/tRc)) * vdd;
	pDsActStby = idd3n * vdd;
	pDsActPdn = idd3p * vdd;
	pDsPreStby = idd2n * vdd;
	pDsPrePdn = idd2p * vdd;
	frequencyScaleFactor= freq / specFreq;
	voltageScaleFactor = (vdd / vddMax) * (vdd / vddMax);
	pDsRd = (idd4r - idd3n) * vdd;
	pDsWr = (idd4w - idd3n) * vdd;

	// term power numbers
	PdsDq = .0032 * 10;
	PdsTermW = .0056 * 11;
	PdsTermRoth = .0249 * 10;
	PdsTermWoth = .0208 * 11;
}

PowerCalculations PowerParameters::calculateSystemPower(const char* newLine, const double epochTime)
{
	string line(newLine);
	vector<string> splitLine;
	boost::split_regex(splitLine, line, regex(" rk"));

	vector<string>::const_iterator currentRank = splitLine.begin(), end = splitLine.end();
	currentRank++;

	PowerCalculations pc;

	double totalReadHits = 0.0;

	const double duration = max(regexMatch<double>(currentRank->c_str(),"duration\\{([0-9]+)\\}"), 0.000001);

	// calculate SRAM power from the DIMM caches
	string &lastRank = splitLine.back();
	vector<string> splitDimmLine;
	boost::split_regex(splitDimmLine, lastRank, regex(" dimm\\["));

	vector<string>::const_iterator currentDimm = splitDimmLine.begin(), dimmEnd = splitDimmLine.end();
	currentDimm++;

	double sramPower = 0.0;

	if (usingCache)
	{
		for (;currentDimm != dimmEnd; ++currentDimm)
		{
			unsigned accesses;

			try
			{
				unsigned readHits = regexMatch<unsigned>(currentDimm->c_str(),"dimmReadHits\\{([0-9]+)\\}");
				unsigned readMisses = regexMatch<unsigned>(currentDimm->c_str(),"dimmReadMisses\\{([0-9]+)\\}");
				unsigned writeHits = regexMatch<unsigned>(currentDimm->c_str(),"dimmWriteHits\\{([0-9]+)\\}");
				unsigned writeMisses = regexMatch<unsigned>(currentDimm->c_str(),"dimmWriteMisses\\{([0-9]+)\\}");

				// number of accesses * duration of access gives the time that this DIMM cache spent in Idd
				accesses = readHits + readMisses + writeHits + writeMisses;
			}
			catch (std::exception& e)
			{
				try
				{
					unsigned reads = regexMatch<unsigned>(currentDimm->c_str(),"read\\{([0-9]+)\\}") / 8;
					unsigned writes = regexMatch<unsigned>(currentDimm->c_str(),"write\\{([0-9]+)\\}") / 8;

					accesses = reads + writes;
				}
				catch (std::exception& e)
				{
					cerr << "nl " << newLine << endl;
					//throw new std::exception;
				}					
			}

			unsigned inUseTime = accesses * hitLatency;

			pc.inUseTime = inUseTime / duration;

			assert(inUseTime >= 0);

			// the remaining time was spent in idle mode
			double idleTime = duration - (double)inUseTime;
			assert(idleTime >= 0);

			pc.sramActivePower += (double)inUseTime / duration * idd * vdd;
			pc.sramIdlePower += (double)idleTime / duration * isb1 * vdd;
			sramPower += (inUseTime / duration * idd + idleTime / duration * isb1) * 1.8;
		}
	}
	else
	{
		pc.sramActivePower = pc.sramIdlePower = 0.0;
	}

	pc.dimmEnergy = sramPower * epochTime;

	for (;currentRank != end; ++currentRank)
	{
		try
		{			
			//cerr << *currentRank << endl;
			unsigned currentRankID = regexMatch<unsigned>(currentRank->c_str(),"^\\[([0-9]+)\\]");


			// because each rank on any dimm will report the same hit and miss counts
			//if ((currentRankID % ranksPerDimm) == 0)


			//double duration = regexMatch<float>(currentRank->c_str(),"duration\\{([0-9]+)\\}");
			double thisRankRasCount = regexMatch<double>(currentRank->c_str(),"rasCount\\{([0-9]+)\\}");
			double thisRankAdjustedRasCount = regexMatch<double>(currentRank->c_str(),"adjRasCount\\{([0-9]+)\\}");
			double readCycles = regexMatch<double>(currentRank->c_str(),"read\\{([0-9]+)\\}");
			double writeCycles = regexMatch<double>(currentRank->c_str(),"write\\{([0-9]+)\\}");
			//double readHits = regexMatch<double>(currentRank->c_str(),"readHits\\{([0-9]+)\\}");
			//totalReadHits += readHits;
			double prechargeTime = regexMatch<float>(currentRank->c_str(),"prechargeTime\\{([0-9]+)\\}");


			double percentActive = 1.0 - (prechargeTime / max((double)(duration), 0.00000001));
			//double percentActiveAdjusted = percentActive * (1 - readHits / (readCycles / tBurst));

			assert(percentActive >= 0.0 && percentActive <= 1.0);
			//assert(percentActiveAdjusted >= 0.0 && percentActiveAdjusted <= 1.0);

			// background power analysis
			// activate-standby
			double PschACT_STBY = pDsActStby * percentActive * (1 - CKE_LO_ACT);
			pc.PsysACT_STBY += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * PschACT_STBY;
			//assert(readHits <= readCycles / 8);
			//cerr << pc.PsysACT_STBYAdjusted << " " << pc.PsysACT_STBY << endl;

			// precharge-standby
			double PschPRE_STBY = pDsPreStby * (1.0 - percentActive) * (1 - CKE_LO_PRE);
			pc.PsysPRE_STBY += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschPRE_STBY;

			// precharge-powerdown
			double PschPRE_PDN = pDsPrePdn * (1.0 - percentActive) * (CKE_LO_PRE);
			pc.PsysPRE_PDN += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschPRE_PDN;

			// activate-powerdown
			double PschACT_PDN = pDsActPdn * percentActive * CKE_LO_ACT;
			pc.PsysACT_PDN += devicesPerRank * frequencyScaleFactor * voltageScaleFactor * PschACT_PDN;
			//cerr << pc.PsysACT_PDN << " " << pc.PsysACT_PDNAdjusted << endl;

			// activate power analysis
			double tRRDsch = ((double)duration) / (thisRankRasCount > 0 ? thisRankRasCount : 0.00000001);
			double PschACT = pDsAct * tRc / tRRDsch;
			pc.PsysACT += devicesPerRank * voltageScaleFactor * PschACT;

			// read power analysis
			double RDschPct = readCycles / duration;
			pc.PsysRD += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsRd * RDschPct;
			//pc.PsysDQ += devicesPerRank * PdsDq * RDschPct;
			//pc.PsysTermRoth += devicesPerRank * PdsTermRoth * RDschPct * (dimms - 1) ;

			// write power analysis
			double WRschPct = writeCycles / duration;
			pc.PsysWR += devicesPerRank * voltageScaleFactor * frequencyScaleFactor * pDsWr * WRschPct;
			//pc.PsysTermW += devicesPerRank * PdsTermW * WRschPct;
			//pc.PsysTermWoth += devicesPerRank * PdsTermWoth * WRschPct * (dimms - 1);

			//cerr << pc.PsysDQ << " " << pc.PsysTermW << " " << pc.PsysTermRoth << " " << pc.PsysTermWoth <<  " " << RDschPct << " " << WRschPct << endl;
		}
		catch (std::exception& e)
		{
			//cerr << "nl " << newLine << endl;
			//throw new std::exception;
		}		
	}	

	pc.energy = (pc.PsysACT_STBY + pc.PsysACT + pc.PsysPRE_STBY + pc.PsysRD + pc.PsysWR + pc.PsysACT_PDN + pc.PsysPRE_PDN 
		/*+ pc.PsysDQ + pc.PsysTermRoth + pc.PsysTermW + pc.PsysTermWoth*/) * epochTime;

	return pc;
}

PowerCalculations PowerParameters::calculateSystemPowerIdle(const double epochTime) const
{
	PowerCalculations pc;
	pc.sramActivePower = 0.0;
	pc.sramIdlePower = isb1 * vdd * dimms;

	pc.dimmEnergy = isb1 * epochTime;

	pc.PsysACT_STBY = 0.0;
	pc.PsysACT_PDN = 0.0;
	pc.PsysPRE_STBY = ranksPerDimm * devicesPerRank * frequencyScaleFactor * voltageScaleFactor * pDsPreStby * CKE_LO_PRE;
	pc.PsysPRE_PDN = ranksPerDimm * devicesPerRank * frequencyScaleFactor * voltageScaleFactor * pDsPrePdn * (1 - CKE_LO_PRE);
	pc.PsysWR = pc.PsysRD = 0.0;
	pc.energy = (pc.PsysACT_STBY + pc.PsysACT + pc.PsysPRE_STBY + pc.PsysRD + pc.PsysWR + pc.PsysACT_PDN + pc.PsysPRE_PDN) * epochTime;

	return pc;
}
