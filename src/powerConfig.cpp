#include <map>
#include <sstream>
#include <string>
#include <iomanip>

#include "powerConfig.h"

using namespace DRAMSimII;

PowerConfig::PowerConfig(const Settings& settings):
VDD(settings.VDD),
VDDmax(settings.maxVCC),
IDD0(settings.IDD0),
IDD2P(settings.IDD2P),
IDD2N(settings.IDD2N),
IDD3P(settings.IDD3P),
IDD3N(settings.IDD3N),
IDD4R(settings.IDD4R),
IDD4W(settings.IDD4W),
IDD5(settings.IDD5),
PdsACT((IDD0 - ((IDD3N * settings.tRAS + IDD2N * (settings.tRC - settings.tRAS))/settings.tRC))*settings.VDD),
PdsACT_STBY(IDD3N * VDD),
PdsRD((IDD4R - IDD3N) * VDD),
PdsWR((IDD4W - IDD3N) * VDD),
PdstermW(settings.PdqWR * (settings.DQperDRAM + settings.DQSperDRAM + settings.DMperDRAM)),
PdqRD(settings.PdqRD),
PdqWR(settings.PdqWR),
PdqRDoth(settings.PdqRDoth),
PdqWRoth(settings.PdqWRoth),
DQperDRAM(settings.DQperDRAM),
DQSperDRAM(settings.DQSperDRAM),
DMperDRAM(settings.DMperDRAM),
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
IDD2P(INT_MAX),
IDD2N(INT_MAX),
IDD3P(INT_MAX),
IDD3N(INT_MAX),
IDD4R(INT_MAX),
IDD4W(INT_MAX),
IDD5(INT_MAX),
PdsACT(-1.0F),
PdsACT_STBY(-1.0F),
PdsRD(-1.0F),
PdsWR(-1.0F),
PdstermW(-1.0F),
PdqRD(-1.0F),
PdqWR(-1.0F),
PdqRDoth(-1.0F),
PdqWRoth(-1.0F),
DQperDRAM(UINT_MAX),
DQSperDRAM(UINT_MAX),
DMperDRAM(UINT_MAX),
frequency(UINT_MAX),
specFrequency(UINT_MAX),
tBurst(UINT_MAX),
tRC(UINT_MAX),
tRAS(UINT_MAX),
lastCalculation(TICK_MAX)
{}

bool PowerConfig::operator==(const PowerConfig& rhs) const
{
	return (VDD == rhs.VDD &&
		VDDmax == rhs.VDDmax &&
		IDD0 == IDD0 &&
		IDD2P == rhs.IDD2P &&
		IDD2N == rhs.IDD2N &&
		IDD3P == rhs.IDD3P &&
		IDD3N == rhs.IDD3N &&
		IDD4R == rhs.IDD4R &&
		IDD4W == rhs.IDD4W &&
		IDD5 == rhs.IDD5 &&
		PdsACT == rhs.PdsACT &&
		PdsACT_STBY == rhs.PdsACT_STBY &&
		PdsRD == rhs.PdsRD &&
		PdsWR == rhs.PdsWR &&
		PdstermW == rhs.PdstermW &&
		PdqRD == rhs.PdqRD &&
		PdqWR == rhs.PdqWR &&
		PdqRDoth == rhs.PdqRDoth &&
		PdqWRoth == rhs.PdqWRoth &&
		DQperDRAM == rhs.DQperDRAM &&
		DQSperDRAM == rhs.DQSperDRAM &&
		DMperDRAM == rhs.DMperDRAM &&
		frequency == rhs.frequency &&
		specFrequency == rhs.specFrequency &&
		tBurst == rhs.tBurst &&
		tRC == rhs.tRC &&
		tRAS == rhs.tRAS &&
		lastCalculation==rhs.lastCalculation);
}

using std::endl;

std::ostream& DRAMSimII::operator<<(std::ostream& in, const PowerConfig& pc)
{
	in << "PowerConfig" << endl;
	in << "VDD" << pc.VDD << endl;
	in << "VDDmax" << pc.VDDmax << endl;
	in << "IDD0" << pc.IDD0 << endl;
	in << "IDD2p" << pc.IDD2P << endl;
	return in;
}
