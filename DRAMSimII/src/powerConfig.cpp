#include <map>
#include <sstream>
#include <string>

#include "powerConfig.h"

using namespace std;

powerConfig::powerConfig(dramSettings *settings):
VDD(settings->VDD),
IDD0(settings->IDD0),
IDD2P(settings->IDD2P),
IDD2N(settings->IDD2N),
IDD3P(settings->IDD3P),
IDD3N(settings->IDD3N),
IDD4R(settings->IDD4R),
IDD4W(settings->IDD4W),
IDD5(settings->IDD5)
{}

powerConfig::~powerConfig()
{}

void powerConfig::recordCommand(const command *cmd, const dramChannel &channel, const dramTimingSpecification &timing)
{
	switch (cmd->getCommandType())
	{
	case RAS_COMMAND:

		break;
	case CAS_AND_PRECHARGE_COMMAND:
		//RDsch += timing.t_burst;
		break;
	case CAS_COMMAND:
		//RDsch += timing.t_burst;
		break;
	case CAS_WRITE_AND_PRECHARGE_COMMAND:
		//WRsch += timing.t_burst;
		break;
	case CAS_WRITE_COMMAND:
		//WRsch += timing.t_burst;
		break;
	case PRECHARGE_COMMAND:
		break;
	case REFRESH_ALL_COMMAND:
		break;
	default:
		break;
	}
}


void calculatePower(const std::vector<dramChannel> &channels)
{

}