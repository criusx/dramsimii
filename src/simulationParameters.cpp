#include "simulationParameters.h"

using namespace DRAMSimII;

SimulationParameters::SimulationParameters(const Settings& settings):
inputType(RANDOM),
requestCount(settings.requestCount)
{}

std::ostream& DRAMSimII::operator<<(std::ostream& in, const SimulationParameters& params)
{	
	in << "ReqCount[" << params.requestCount << "] Input[" << params.inputType << "]" << std::endl;
	return in;
}

bool SimulationParameters::operator ==(const SimulationParameters& right) const
{
	return requestCount == right.requestCount && inputType == right.inputType;
}