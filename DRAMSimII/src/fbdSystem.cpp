#include "fbdSystem.h"

using namespace DRAMSimII;

fbdSystem::fbdSystem(const dramSettings& settings):
dramSystem(settings), // inherited class
channels(systemConfig.getChannelCount(),fbdChannel(settings))
{}


