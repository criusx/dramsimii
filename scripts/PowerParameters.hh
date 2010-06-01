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
		inUseTime(0.0)
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
	CKE_LO_ACT(0.01),
		CKE_LO_PRE(0.95),
		devicesPerRank(8.0)
	{}

	void setParameters(const char* commandLine, const std::list<std::pair<std::string,std::string> > &updatedPowerParams);
	PowerCalculations calculateSystemPower(const char* newLine, const double epochTime);
	PowerCalculations calculateSystemPowerIdle(const double epochTime) const;
};

#endif