#ifndef POWERPARAMETERS_HH
#define POWERPARAMETERS_HH

#include <list>
#include <string>

class PowerCalculations
{
public:
	double PsysACT_STBY, PsysPRE_STBY, PsysPRE_PDN, PsysACT_PDN, PsysACT, PsysRD, PsysWR;
	//double PsysDQ, PsysTermW, PsysTermRoth, PsysTermWoth;
	double sramActivePower, sramIdlePower;
	double energy, dimmEnergy;
	double inUseTime;
	int rasCount, readCount;

	PowerCalculations():
	PsysACT_STBY(0.0),
		PsysPRE_STBY(0.0),
		PsysPRE_PDN(0.0),
		PsysACT_PDN(0.0),
		PsysACT(0.0),
		PsysRD(0.0),
		PsysWR(0.0), 
		sramActivePower(0.0),
		sramIdlePower(0.0),
		energy(0.0),
		dimmEnergy(0.0),
		inUseTime(0.0),
		rasCount(0U),
		readCount(0U)
	{};
};

class PowerParameters
{
public:
	// power values
	double pDsAct;
	double pDsActStby;
	double pDsActPdn;
	double pDsPreStby;
	double pDsPrePdn;
	double pDsRd;
	double pDsWr;
	double voltageScaleFactor;
	double frequencyScaleFactor;
	double tRc;
	double tBurst;
	double CKE_LO_PRE;
	double CKE_LO_ACT;
	double freq;
	double idd1;
	double vdd;
	double devicesPerRank;
	double hitLatency;
	double idd;
	double isb1;
	double PdsDq;
	double PdsTermW;
	double PdsTermRoth;
	double PdsTermWoth;

	// system information
	unsigned ranksPerDimm;
	unsigned dimms;
	bool usingCache;

	PowerParameters():
	pDsAct(0.0),
		pDsActStby(0.0),
		pDsActPdn(0.0),
		pDsPreStby(0.0),
		pDsPrePdn(0.0),
		pDsRd(0.0),
		pDsWr(0.0),
		voltageScaleFactor(0.0),
		frequencyScaleFactor(0.0),
		tRc(0.0),
		tBurst(0.0),
		CKE_LO_ACT(0.01),
		CKE_LO_PRE(0.95),
		freq(0.0),
		idd1(0.0),
		vdd(0.0),
		devicesPerRank(8.0),
		hitLatency(0.0),
		idd(0.0),
		isb1(0.0),
		PdsDq(0.0),
		PdsTermW(0.0),
		PdsTermRoth(0.0),
		PdsTermWoth(0.0),
		ranksPerDimm(0),
		dimms(0),
		usingCache(false)
	{}

	void setParameters(const char* commandLine, const std::list<std::pair<std::string,std::string> > &updatedPowerParams);
	PowerCalculations calculateSystemPower(const char* newLine, const double epochTime);
	PowerCalculations calculateSystemPowerIdle(const double epochTime) const;
};

#endif