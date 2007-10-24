#include <map>
#include <sstream>
#include <string>
#include <iomanip>

#include "powerConfig.h"

using namespace std;
using namespace DRAMSimII;

powerConfig::powerConfig(const dramSettings *settings):
VDD(settings->VDD),
VDDmax(settings->maxVCC),
IDD0(settings->IDD0),
IDD2P(settings->IDD2P),
IDD2N(settings->IDD2N),
IDD3P(settings->IDD3P),
IDD3N(settings->IDD3N),
IDD4R(settings->IDD4R),
IDD4W(settings->IDD4W),
IDD5(settings->IDD5),
PdsACT(IDD0 - ((IDD3N * settings->tRAS + IDD2N * (settings->tRC - settings->tRAS))/settings->tRC)*settings->VDD),
PdsACT_STBY(IDD3N * settings->VDD),
PdsRD((IDD4R - IDD3N) * VDD),
PdsWR((IDD4W - IDD3N) * VDD),
PdstermW(settings->PdqWR * (settings->DQperDRAM + settings->DQSperDRAM + settings->DMperDRAM)),
PdqRD(settings->PdqRD),
PdqWR(settings->PdqWR),
PdqRDoth(settings->PdqRDoth),
PdqWRoth(settings->PdqWRoth),
DQperDRAM(settings->DQperDRAM),
DQSperDRAM(settings->DQSperDRAM),
DMperDRAM(settings->DMperDRAM),
frequency(settings->dataRate),
frequencySpec(settings->frequencySpec),
tBurst(settings->tBurst),
tRC(settings->tRC),
lastCalculation(0)
{}

powerConfig::~powerConfig()
{}




