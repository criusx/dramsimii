// Copyright (C) 2008 University of Maryland.
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

#include <map>
#include <sstream>
#include <string>
#include <iomanip>

#include "powerConfig.h"

using namespace DRAMsimII;

PowerConfig::PowerConfig(const Settings& settings):
VDD(settings.VDD),
VDDmax(settings.maxVCC),
IDD0(settings.IDD0),
IDD1(settings.IDD1),
IDD2P(settings.IDD2P),
IDD2N(settings.IDD2N),
IDD3P(settings.IDD3P),
IDD3N(settings.IDD3N),
IDD4R(settings.IDD4R),
IDD4W(settings.IDD4W),
IDD5(settings.IDD5),
PdsACT((settings.IDD0 - ((settings.IDD3N * settings.tRAS + settings.IDD2N * (settings.tRC - settings.tRAS))/settings.tRC)) * settings.VDD),
PdsACT_STBY(settings.IDD3N * settings.VDD),
PdsPRE_STBY(settings.IDD2N * settings.VDD),
PdsACT_PDN(settings.IDD3P * settings.VDD),
PdsPRE_PDN(settings.IDD2P * settings.VDD),
PdsRD((settings.IDD4R - settings.IDD3N) * settings.VDD),
PdsWR((settings.IDD4W - settings.IDD3N) * settings.VDD),
voltageScaleFactor((settings.VDD / settings.maxVCC) * (settings.VDD / settings.maxVCC)),
frequencyScaleFactor((double)settings.dataRate / (double)settings.frequencySpec),
PdstermW(settings.PdqWR * (settings.DQperDRAM + settings.DQSperDRAM + settings.DMperDRAM)),
PdqRD(settings.PdqRD),
PdqWR(settings.PdqWR),
PdqRDoth(settings.PdqRDoth),
PdqWRoth(settings.PdqWRoth),
DQperDRAM(settings.DQperDRAM),
DQSperDRAM(settings.DQSperDRAM),
DMperDRAM(settings.DMperDRAM),
DQperRank(settings.channelWidth * 8 / DQperDRAM),
frequency(settings.dataRate),
specFrequency(settings.frequencySpec),
tBurst(settings.tBurst),
tRC(settings.tRC),
tRAS(settings.tRAS),
lastCalculation(0)
{}

PowerConfig::~PowerConfig()
{}

// no arg constructor for unit testing and deserialization
PowerConfig::PowerConfig():
VDD(-1.0F),
VDDmax(-1.0F),
IDD0(INT_MAX),
IDD1(INT_MAX),
IDD2P(INT_MAX),
IDD2N(INT_MAX),
IDD3P(INT_MAX),
IDD3N(INT_MAX),
IDD4R(INT_MAX),
IDD4W(INT_MAX),
IDD5(INT_MAX),
PdsACT(-1.0F),
PdsACT_STBY(-1.0F),
PdsPRE_STBY(-1.0F),
PdsACT_PDN(-1.0F),
PdsPRE_PDN(-1.0F),
PdsRD(-1.0F),
PdsWR(-1.0F),
voltageScaleFactor(-1.0),
frequencyScaleFactor(-1.0),
PdstermW(-1.0F),
PdqRD(-1.0F),
PdqWR(-1.0F),
PdqRDoth(-1.0F),
PdqWRoth(-1.0F),
DQperDRAM(UINT_MAX),
DQSperDRAM(UINT_MAX),
DMperDRAM(UINT_MAX),
DQperRank(UINT_MAX),
frequency(UINT_MAX),
specFrequency(UINT_MAX),
tBurst(UINT_MAX),
tRC(UINT_MAX),
tRAS(UINT_MAX),
lastCalculation(TICK_MAX)
{}

bool PowerConfig::operator==(const PowerConfig& rhs) const
{
	return (AlmostEqual<float>(VDD, rhs.VDD) && AlmostEqual<float>(VDDmax, rhs.VDDmax) && IDD0 == rhs.IDD0 &&
		IDD2P == rhs.IDD2P && IDD2N == rhs.IDD2N && IDD3P == rhs.IDD3P && IDD1 == rhs.IDD1 &&
		IDD3N == rhs.IDD3N && IDD4R == rhs.IDD4R && IDD4W == rhs.IDD4W &&
		IDD5 == rhs.IDD5 && AlmostEqual<float>(PdsACT, rhs.PdsACT) && AlmostEqual<float>(PdsACT_STBY, rhs.PdsACT_STBY) &&
		AlmostEqual<double>(PdsRD, rhs.PdsRD) &&
		AlmostEqual<double>(PdsACT_PDN, rhs.PdsACT_PDN) &&
		AlmostEqual<double>(PdsPRE_STBY, rhs.PdsPRE_STBY) &&
		AlmostEqual<double>(PdsPRE_PDN, rhs.PdsPRE_PDN) &&
		AlmostEqual<double>(PdsWR, rhs.PdsWR) &&
		AlmostEqual<double>(PdstermW, rhs.PdstermW) &&
		AlmostEqual<double>(PdqRD, rhs.PdqRD) &&
		AlmostEqual<double>(PdqWR, rhs.PdqWR) &&
		AlmostEqual<double>(PdqRDoth, rhs.PdqRDoth) &&
		AlmostEqual<double>(PdqWRoth, rhs.PdqWRoth) && DQperDRAM == rhs.DQperDRAM && DQSperDRAM == rhs.DQSperDRAM &&
		DMperDRAM == rhs.DMperDRAM && DQperRank == rhs.DQperRank && frequency == rhs.frequency &&
		specFrequency == rhs.specFrequency && tBurst == rhs.tBurst && tRC == rhs.tRC &&
		tRAS == rhs.tRAS && lastCalculation==rhs.lastCalculation &&
		AlmostEqual<double>(voltageScaleFactor, rhs.voltageScaleFactor) &&
		AlmostEqual<double>(frequencyScaleFactor, rhs.frequencyScaleFactor));
}

std::ostream& DRAMsimII::operator<<(std::ostream& in, const PowerConfig& pc)
{
	using std::endl;

	in << "PowerConfig" << endl;
	in << "VDD" << pc.VDD << endl;
	in << "VDDmax" << pc.VDDmax << endl;
	in << "IDD0" << pc.IDD0 << endl;
	in << "IDD2p" << pc.IDD2P << endl;
	return in;
}
