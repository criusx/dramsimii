// Copyright (C) 2010 University of Maryland.
// This file is part of DRAMsimII.
// 
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#ifndef POWERCONFIG_H
#define POWERCONFIG_H
#pragma once

#include "globals.hh"
#include "enumTypes.hh"
#include "Settings.hh"

#include <map>
#include <string>

namespace DRAMsimII
{	
	/// @brief stores power configuration parameters for this DRAM system necessary to calculate power consumed
	/// @author Joe Gross
	class PowerConfig
	{
	protected:
		const float VDD;			///< the current voltage
		const float VDDmax;			///< the maximum voltage as specified by the manufacturer's datasheets		
		const int IDD0;				///< Operating Current: One Bank Active-Precharge in mA
		const int IDD1;				///< one bank ACT to read to Pre current, in mA
		const int IDD2P;			///< Precharge Power-Down Current (CKE=0), in mA
		const int IDD2N;			///< Precharge Standby Current (CKE=1), in mA
		const int IDD3P;			///< Active Power-Down Current (CKE=0), in mA
		const int IDD3N;			///< Active Standby Current (CKE=1), in mA
		const int IDD4R;			///< Operating Burst Read Current in mA
		const int IDD4W;			///< Operating Burst Write Current in mA
		const int IDD5;				///< Burst Refresh Current in mA

		const double PdsACT;		///< power for ACT/PRE commands
		const double PdsACT_STBY;	///< background power for active standby
		const double PdsPRE_STBY;	///< background power for precharge standby
		const double PdsACT_PDN;	///< background power for active powerdown
		const double PdsPRE_PDN;	///< background power for precharge powerdown
		const double PdsRD;			///< read power
		const double PdsWR;			///< write power

		const double voltageScaleFactor;	///< derating based on voltage
		const double frequencyScaleFactor;	///< derating based on frequency

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
		const unsigned DQperRank;	///< the number of pins per rank, useful to know how many DRAMs there are per rank

		const unsigned frequency;	///< frequency of the system
		const unsigned specFrequency;	///< frequency specified in the data sheets

		const unsigned tBurst;	///< burst length in cycles
		const unsigned tRC;		///< tRC according to the data sheet, in cycles
		const unsigned tRAS;	///< tRAS according to the data sheet

		tick lastCalculation;	///< the last time at which a power calculation was made


	public:
		// constructors
		PowerConfig(const Settings& settings);
		~PowerConfig();
		
		// accessors
		float getVDD() const { return VDD; }
		float getVDDmax() const { return VDDmax; }
		unsigned getFrequency() const { return frequency; }
		unsigned getSpecFrequency() const { return specFrequency; }
		tick getLastCalculation() const { return lastCalculation; }
		double getPdsACT_STBY() const { return PdsACT_STBY; }
		double getPdsACT_PDN() const { return PdsACT_PDN; }
		double getPdsPRE_STBY() const { return PdsPRE_STBY; }
		double getPdsPRE_PDN() const { return PdsPRE_PDN; }
		double getPdsACT() const { return PdsACT; }
		double getPdqRD() const { return PdqRD; }
		double getPdqWR() const { return PdqWR; }
		double getPdsRD() const { return PdsRD; }
		double getPdsWR() const { return PdsWR; }
		double getPdqRDoth() const { return PdqRDoth; }
		double getPdqWRoth() const { return PdqWRoth; }
		unsigned gettRC() const { return tRC; }
		unsigned getDQperDRAM() const { return DQperDRAM; }
		unsigned getDQSperDRAM() const { return DQSperDRAM; }
		unsigned getDMperDRAM() const { return DMperDRAM; }
		unsigned getDevicesPerRank() const { return DQperRank; }
		int getIDD0() const { return IDD0; }
		int getIDD1() const { return IDD1; }
		int getIDD2N() const { return IDD2N; }
		int getIDD2P() const { return IDD2P; }		
		int getIDD3N() const { return IDD3N; }
		int getIDD3P() const { return IDD3P; }
		int getIDD4R() const { return IDD4R; }
		int getIDD4W() const { return IDD4W; }
		int getIDD5() const { return IDD5; }
		unsigned gettBurst() const { return tBurst; }
		unsigned gettRAS() const { return tRAS; }
		double getVoltageScaleFactor() const { return voltageScaleFactor;}
		double getFrequencyScaleFactor() const { return frequencyScaleFactor; }
		void resetToTime(const tick newTime) { lastCalculation = newTime; }

		// mutators
		void setLastCalculation(const tick lastTime) { lastCalculation = lastTime; }

		// overloads
		bool operator==(const PowerConfig& right) const;
		PowerConfig &operator=(const PowerConfig& right);
		friend std::ostream& operator<<(std::ostream& os, const PowerConfig& pc);

	private:
		
		PowerConfig();
	};
}
#endif
