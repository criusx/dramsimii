#include "fbdSystem.h"

using namespace DRAMSimII;

fbdSystem::fbdSystem(const Settings& settings):
System(settings), // inherited class
channel(systemConfig.getChannelCount(),fbdChannel(settings, systemConfig,statistics))
{}
