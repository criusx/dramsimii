#include "simulationParameters.h"

using namespace std;
using namespace DRAMSimII;

simulationParameters::simulationParameters(const Settings& settings):
input_type(RANDOM),
request_count(settings.requestCount)
{}
