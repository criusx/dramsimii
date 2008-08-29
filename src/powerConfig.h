#ifndef POWERCONFIG_H
#define POWERCONFIG_H
#pragma once

#include "globals.h"
#include "enumTypes.h"
#include "Settings.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <map>
#include <string>

namespace DRAMSimII
{	
	/// @brief stores power configuration parameters for this DRAM system necessary to calculate power consumed
	/// @author Joe Gross
	class PowerConfig
	{
	protected:
		const float VDD;			///< the current voltage
		const float VDDmax;		///< the maximum voltage as specified by the manufacturer's datasheets
		const int IDD0;			///< Operating Current: One Bank Active-Precharge in mA
		const int IDD2P;			///< Precharge Power-Down Current (CKE=0), in mA
		const int IDD2N;			///< Precharge Standby Current (CKE=1), in mA
		const int IDD3P;			///< Active Power-Down Current (CKE=0), in mA
		const int IDD3N;			///< Active Standby Current (CKE=1), in mA
		const int IDD4R;			///< Operating Burst Read Current in mA
		const int IDD4W;			///< Operating Burst Write Current in mA
		const int IDD5;			///< Burst Refresh Current in mA

		const double PdsACT;		///< power for ACT/PRE commands
		const double PdsACT_STBY;	///< background power for active standby
		const double PdsRD;		///< read power
		const double PdsWR;		///< write power

		// termination power, based on system design
		const double PdstermW;	///< termination power for writes
		const double PdqRD;		///< power per DQ, determined by system design
		const double PdqWR;		///< power per DQ write
		const double PdqRDoth;	///< power per other DQ, read
		const double PdqWRoth;	///< power per other DQ, write

		// pins on the DRAM devices
		const unsigned DQperDRAM;	///< number of DQ per DRAM
		const unsigned DQSperDRAM;///< number of DQS per DRAM
		const unsigned DMperDRAM;	///< number of DM per DRAM

		const unsigned frequency;	///< frequency of the system
		const unsigned specFrequency;	///< frequency specified in the data sheets

		const unsigned tBurst;	///< burst length in cycles
		const unsigned tRC;		///< tRC according to the data sheet, in cycles
		const unsigned tRAS;	///< tRAS according to the data sheet

		tick lastCalculation;	///< the last time at which a power calculation was made

		//tick allBanksPrecharged;
		//tick RDsch; // number of clock cycles spent sending data
		//tick WRsch; // number of clock cycles spent receiving data


	public:
		// constructors
		PowerConfig(const Settings& settings);
		~PowerConfig();
		//void recordCommand(const command *, const dramChannel &channel, const dramTimingSpecification &timing);
		//void doPowerCalculation(dramChannel * const chan);

		// accessors
		const float getVDD() const { return VDD; }
		const float getVDDmax() const { return VDDmax; }
		const unsigned getFrequency() const { return frequency; }
		const unsigned getSpecFrequency() const { return specFrequency; }
		unsigned getLastCalculation() const { return lastCalculation; }
		double getPdsACT() const { return PdsACT; }
		const double getPdqRD() const { return PdqRD; }
		const double getPdqWR() const { return PdqWR; }
		const double getPdqRDoth() const { return PdqRDoth; }
		const double getPdqWRoth() const { return PdqWRoth; }
		const unsigned gettRC() const { return tRC; }
		const unsigned getDQperDRAM() const { return DQperDRAM; }
		const unsigned getDQSperDRAM() const { return DQSperDRAM; }
		const unsigned getDMperDRAM() const { return DMperDRAM; }
		const int getIDD0() const { return IDD0; }
		const int getIDD2N() const { return IDD2N; }
		const int getIDD2P() const { return IDD2P; }		
		const int getIDD3N() const { return IDD3N; }
		const int getIDD3P() const { return IDD3P; }
		const int getIDD4R() const { return IDD4R; }
		const int getIDD4W() const { return IDD4W; }
		const int getIDD5() const { return IDD5; }
		const unsigned gettBurst() const { return tBurst; }
		const unsigned gettRAS() const { return tRAS; }

		// mutators
		void setLastCalculation(const tick lastTime) { lastCalculation = lastTime; }

		// overloads
		bool operator==(const PowerConfig& right) const;
		friend std::ostream& operator<<(std::ostream& in, const PowerConfig& pc);

	private:
		//serialization
		friend class boost::serialization::access;

		PowerConfig();

		template <class Archive>
		void serialize( Archive & ar, const unsigned version)
		{
			ar & const_cast<float&>(VDD) & const_cast<float&>(VDDmax) &
				const_cast<int&>(IDD0) & const_cast<int&>(IDD2P) & const_cast<int&>(IDD2N) & const_cast<int&>(IDD3P) & const_cast<int&>(IDD3N) &
				const_cast<int&>(IDD4R) & const_cast<int&>(IDD4W) & const_cast<int&>(IDD5) & 
				const_cast<double&>(PdsACT) & const_cast<double&>(PdsACT_STBY) & const_cast<double&>(PdsRD) & const_cast<double&>(PdsWR)&
				const_cast<double&>(PdstermW) & const_cast<double&>(PdqRD) & const_cast<double&>(PdqWR) & const_cast<double&>(PdqRDoth) & const_cast<double&>(PdqWRoth) & 
				const_cast<unsigned&>(DQperDRAM) & const_cast<unsigned&>(DQSperDRAM) & const_cast<unsigned&>(DMperDRAM) & const_cast<unsigned&>(frequency) & const_cast<unsigned&>(specFrequency) &
				const_cast<unsigned&>(tBurst) & const_cast<unsigned&>(tRC) & const_cast<unsigned&>(tRAS) & lastCalculation;
		}

	};
}
#endif
