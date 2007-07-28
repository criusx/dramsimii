#include <map>
#include <sstream>
#include <string>
#include <iomanip>

#include "powerConfig.h"

using namespace std;

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

//void powerConfig::recordCommand(const command *cmd, const dramChannel &channel, const dramTimingSpecification &timing)
//{
//	switch (cmd->getCommandType())
//	{
//	case RAS_COMMAND:
//
//		break;
//	case CAS_AND_PRECHARGE_COMMAND:
//		//RDsch += timing.t_burst;
//		break;
//	case CAS_COMMAND:
//		//RDsch += timing.t_burst;
//		break;
//	case CAS_WRITE_AND_PRECHARGE_COMMAND:
//		//WRsch += timing.t_burst;
//		break;
//	case CAS_WRITE_COMMAND:
//		//WRsch += timing.t_burst;
//		break;
//	case PRECHARGE_COMMAND:
//		break;
//	case REFRESH_ALL_COMMAND:
//		break;
//	default:
//		break;
//	}
//}

// calculate the power consumed by this channel during the last epoch
void powerConfig::doPowerCalculation() 
{
	for (std::vector<rank_c>::const_iterator k = j->getRank().begin(); k != j->getRank().end(); k++)
	{
		tick_t totalRAS = 1;
		for (std::vector<bank_c>::const_iterator l = k->bank.begin(); l != k->bank.end(); l++)
		{
			// Psys(ACT)
			totalRAS += l->RASCount;
		}
		tick_t tRRDsch = (j->get_time() - lastCalculation) / totalRAS * tBurst / 2;
		cerr << "Psys(ACT) " << setprecision(3) << PdsACT * tRC / tRRDsch * (VDD / VDDmax) * (VDD / VDDmax) << endl;
		lastCalculation = time;
	}
}
