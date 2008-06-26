#include "simulationParameters.h"

using namespace std;
using namespace DRAMSimII;

SimulationParameters::SimulationParameters(const Settings& settings):
inputType(RANDOM),
requestCount(settings.requestCount)
{}
