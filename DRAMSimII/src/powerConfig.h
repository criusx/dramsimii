#ifndef POWERCONFIG_H
#define POWERCONFIG_H
#pragma once

#include <map>
#include <string>

#include "globals.h"
#include "enumTypes.h"
#include "dramSettings.h"

namespace DRAMSimII
{	
	/// @brief stores power configuration parameters for this DRAM system necessary to calculate power consumed
	/// @author Joe Gross
	class PowerConfig
	{
	protected:
		float VDD;			///< the current voltage
		float VDDmax;		///< the maximum voltage as specified by the manufacturer's datasheets
		int IDD0;			///< Operating Current: One Bank Active-Precharge in mA
		int IDD2P;			///< Precharge Power-Down Current (CKE=0), in mA
		int IDD2N;			///< Precharge Standby Current (CKE=1), in mA
		int IDD3P;			///< Active Power-Down Current (CKE=0), in mA
		int IDD3N;			///< Active Standby Current (CKE=1), in mA
		int IDD4R;			///< Operating Burst Read Current in mA
		int IDD4W;			///< Operating Burst Write Current in mA
		int IDD5;			///< Burst Refresh Current in mA

		double PdsACT;		///< power for ACT/PRE commands
		double PdsACT_STBY;	///< background power for active standby
		double PdsRD;		///< read power
		double PdsWR;		///< write power

		// termination power, based on system design
		double PdstermW;	///< termination power for writes
		double PdqRD;		///< power per DQ, determined by system design
		double PdqWR;		///< power per DQ write
		double PdqRDoth;	///< power per other DQ, read
		double PdqWRoth;	///< power per other DQ, write

		// pins on the DRAM devices
		unsigned DQperDRAM;	///< number of DQ per DRAM
		unsigned DQSperDRAM;///< number of DQS per DRAM
		unsigned DMperDRAM;	///< number of DM per DRAM

		unsigned frequency;	///< frequency of the system
		unsigned specFrequency;	///< frequency specified in the data sheets

		unsigned tBurst;	///< burst length in cycles
		unsigned tRC;		///< tRC according to the datasheet, in cycles

		tick lastCalculation;	///< the last time at which a power calculation was made

		//tick allBanksPrecharged;
		//tick RDsch; // number of clock cycles spent sending data
		//tick WRsch; // number of clock cycles spent receiving data


	public:
		// constructors
		PowerConfig();
		PowerConfig(const Settings& settings);
		//void recordCommand(const command *, const dramChannel &channel, const dramTimingSpecification &timing);
		//void doPowerCalculation(dramChannel * const chan);

		// accessors
		float getVDD() const { return VDD; }
		float getVDDmax() const { return VDDmax; }
		unsigned getFrequency() const { return frequency; }
		unsigned getSpecFrequency() const { return specFrequency; }
		unsigned getLastCalculation() const { return lastCalculation; }
		double getPdsACT() const { return PdsACT; }
		unsigned gettRC() const { return tRC; }
		int getIDD3N() const { return IDD3N; }
		int getIDD4R() const { return IDD4R; }
		int getIDD4W() const { return IDD4W; }
		unsigned gettBurst() const { return tBurst; }

		// mutators
		void setLastCalculation(const tick lastTime) { lastCalculation = lastTime; }
		


	public:
		~PowerConfig();
	};
}
#endif
