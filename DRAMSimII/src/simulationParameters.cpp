#include "simulationParameters.h"

using namespace std;
using namespace DRAMSimII;

simulationParameters::simulationParameters(const dramSettings& settings):
input_type(RANDOM),
request_count(settings.requestCount)
{}
