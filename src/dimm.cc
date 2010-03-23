#include "dimm.hh"

using DRAMsimII::DIMM;
using DRAMsimII::Settings;
using DRAMsimII::TimingSpecification;
using DRAMsimII::SystemConfiguration;
using DRAMsimII::Statistics;
using DRAMsimII::Rank;
using DRAMsimII::tick;
using DRAMsimII::Command;
using std::vector;

DIMM::DIMM(const Settings&_settings, const TimingSpecification &_timing, const SystemConfiguration &_systemConfig, Statistics& _stats):
timing(_timing),
systemConfig(_systemConfig),
statistics(_stats),
cache(_settings),
rank(_settings.rankCount,Rank(_settings,_timing,_systemConfig,_stats))
{
	assert(rank.size() >= 1);
}

DIMM::DIMM(const DIMM &rhs):
timing(rhs.timing),
systemConfig(rhs.systemConfig),
statistics(rhs.statistics),
cache(rhs.cache),
rank(rhs.rank)
{
	assert(rank.size() >= 1);
}

DIMM::DIMM(const DIMM &rhs, const TimingSpecification &_timing, const SystemConfiguration &_systemConfig, Statistics &_stats):
timing(_timing),
systemConfig(_systemConfig),
statistics(_stats),
cache(rhs.cache),
rank(rhs.rank)
{
	assert(rank.size() >= 1);
}

//////////////////////////////////////////////////////////////////////////
/// @brief this logically issues a RAS command and updates all variables to reflect this
//////////////////////////////////////////////////////////////////////////
void DIMM::issueRAS(const tick currentTime, const Command *currentCommand)
{
	//Rank &currentRank = rank[currentCommand->getAddress().getRank()];

	//currentRank.setLastBankID(currentCommand->getAddress().getBank());
	
	assert(currentCommand->getAddress().getDimm() == dimmId);
	
	(rank.begin() + (currentCommand->getAddress().getRank() - dimmId * systemConfig.getRankCount()))->issueRAS(currentTime, currentCommand);
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a precharge command to this rank
//////////////////////////////////////////////////////////////////////////
void DIMM::issuePRE(const tick currentTime, const Command *currentCommand)
{
	//assert((rank.begin() + (currentCommand->getAddress().getRank() - dimmId * systemConfig.getRankCount()))->isActivated());

	(rank.begin() + (currentCommand->getAddress().getRank() - dimmId * systemConfig.getRankCount()))->issuePRE(currentTime, currentCommand);
}

//////////////////////////////////////////////////////////////////////////
// @brief issue a CAS command to this rank
//////////////////////////////////////////////////////////////////////////
void DIMM::issueCAS(const tick currentTime, const Command *currentCommand)
{
	Rank &currentRank = rank[currentCommand->getAddress().getRank()];

	Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	assert(currentBank.getOpenRowID() == currentCommand->getAddress().getRow());

	//////////////////////////////////////////////////////////////////////////
	bool satisfied = cache.timingAccess(currentCommand, currentCommand->getStartTime());

	if (!satisfied)
		bank[currentCommand->getAddress().getBank()].setAllHits(false);
	if (bank[currentCommand->getAddress().getBank()].isAllHits() && currentCommand->isPrecharge())
	{
		statistics.reportRasReduction(currentCommand);
	}
	//std::cout << (satisfied ? "|" : ".");
	//////////////////////////////////////////////////////////////////////////
	
	bool thisDimm = currentCommand->getAddress().getDimm() == dimmId;

	unsigned rankId = currentCommand->getAddress().getRank();

	for (vector<Rank>::iterator i = rank.begin(), end = rank.end();
		i != end; ++i)
	{
			i->issueCAS(time, currentCommand);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a CASW command to this rank
//////////////////////////////////////////////////////////////////////////
void DIMM::issueCASW(const tick currentTime, const Command *currentCommand)
{
	Rank &currentRank = rank[currentCommand->getAddress().getRank()];

	Bank &currentBank = currentRank.bank[currentCommand->getAddress().getBank()];

	assert(currentBank.getOpenRowID() == currentCommand->getAddress().getRow());

	//////////////////////////////////////////////////////////////////////////
	bool satisfied = cache.timingAccess(currentCommand, currentCommand->getStartTime());
	bank[currentCommand->getAddress().getBank()].setAllHits(false);
	//std::cout << (satisfied ? "|" : ".");
	//////////////////////////////////////////////////////////////////////////

	bool thisDimm = currentCommand->getAddress().getDimm() == dimmId;

	unsigned rankId = currentCommand->getAddress().getRank();

	for (vector<Rank>::iterator i = rank.begin(), end = rank.end();
		i != end; ++i)
	{
			i->issueCASW(time, currentCommand);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief issue a refresh command to this rank
//////////////////////////////////////////////////////////////////////////
void DIMM::issueREF(const tick currentTime)
{
	(rank.begin() + (currentCommand->getAddress().getRank() - dimmId * systemConfig.getRankCount()))->issueREF(currentTime);
}

//////////////////////////////////////////////////////////////////////////
/// @brief set the dimm id and the id of all the ranks on it
//////////////////////////////////////////////////////////////////////////
void DIMM::setDimmId(const unsigned channelId, const unsigned _dimmId)
{
	dimmId = _dimmId;
	unsigned rankID = systemConfig.getRankCount() * dimmId;
	for (vector<Rank>::iterator i = rank.begin(), end = rank.end();
		i != end; ++i)
	{
		i->setRankID(channelId, rankID++);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief reset the time of the ranks on this dimm to a certain point
//////////////////////////////////////////////////////////////////////////
void DIMM::resetToTime(const tick time)
{
	for (vector<Rank>::iterator i = rank.begin(), end = rank.end(); 
		i != end; ++i)
	{
		i->resetToTime(time);
	}
}

//////////////////////////////////////////////////////////////////////////
/// @brief assignment operator, useful for vectory copy operations
//////////////////////////////////////////////////////////////////////////
DIMM &DIMM::operator =(const DIMM &rhs)
{
	rank = rhs.rank;
	cache = rhs.cache;
	dimmId = rhs.dimmId;

	return *this;
}