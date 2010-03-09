// Copyright (C) 2008 University of Maryland.
// This file is part of DRAMsimII.
//
// DRAMsimII is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DRAMsimII is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DRAMsimII.  If not, see <http://www.gnu.org/licenses/>.

#include "simulationParameters.h"

using namespace DRAMsimII;

SimulationParameters::SimulationParameters(const Settings& settings):
requestCount(settings.requestCount),
inputType(InputStream::RANDOM)
{}

std::ostream& DRAMsimII::operator<<(std::ostream& in, const SimulationParameters& params)
{	
	in << "ReqCount[" << params.requestCount << "] Input[" << params.inputType << "]" << std::endl;
	return in;
}

bool SimulationParameters::operator ==(const SimulationParameters& right) const
{
	return requestCount == right.requestCount && inputType == right.inputType;
}
