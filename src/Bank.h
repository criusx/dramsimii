#ifndef BANK_C_H
#define BANK_C_H
#pragma once

#include "globals.h"
#include "command.h"
#include "Settings.h"
#include "transaction.h"
#include "TimingSpecification.h"
#include "SystemConfiguration.h"
#include "queue.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>

namespace DRAMSimII
{
	/// @brief this class logically represents a bank
	/// @details contains per bank queues as well as stats about when events happened
	class Bank
	{
	private:
		const TimingSpecification &timing;		///< a reference to the timing specification
		const SystemConfiguration &systemConfig;///< reference to the system config to obtain specs
	protected:	

		// members
		Queue<Command> perBankQueue;	///< the command priority queue, stores the commands to be executed
		tick lastRASTime;				///< when did last RAS command start?
		tick lastCASTime;				///< when did last CAS command start?
		tick lastCASWTime;				///< when did last CASW command start?
		tick lastPrechargeTime;			///< when did last Precharge command start?
		tick lastRefreshAllTime;		///< must respect t_rfc. concurrent refresh takes time
		unsigned lastCASLength;			///< the length of the last CAS command issued
		unsigned lastCASWLength;		///< the length of the last CASW command issued
		unsigned openRowID;				///< if the bank is open, what is the row id?
		bool activated;					///< if the bank is activated, else precharged

		// stats
		unsigned RASCount;				///< the total number of RAS commands in this epoch
		unsigned totalRASCount;			///< the number of RAS commands 
		unsigned CASCount;				///< the total number of CAS commands in this epoch
		unsigned totalCASCount;			///< the number of CAS commands
		unsigned CASWCount;				///< the total number of CAS+W commands in this epoch
		unsigned totalCASWCount;		///< the number of CASW commands

	public:
		// functions
		void issueRAS(const tick currentTime, const Command *currentCommand);
		void issuePRE(const tick currentTime, const Command *currentCommand);
		void issueCAS(const tick currentTime, const Command *currentCommand);
		void issueCASW(const tick currentTime, const Command *currentCommand);
		void issueREF(const tick currentTime, const Command *currentCommand);
		void accumulateAndResetCounts() { totalRASCount += RASCount; totalCASCount += CASCount; totalCASWCount += CASWCount; RASCount = CASWCount = CASCount = 0; }

		// accessors
		tick getLastRASTime() const { return lastRASTime; }
		tick getLastCASTime() const { return lastCASTime; }
		tick getLastCASWTime() const {return lastCASWTime; }

		tick getLastPrechargeTime() const { return lastPrechargeTime; }
		tick getLastRefreshAllTime() const { return lastRefreshAllTime; }

		unsigned getLastCASLength() const { return lastCASLength; }
		unsigned getLastCASWLength() const { return lastCASWLength; }

		unsigned getOpenRowID() const { return openRowID; }
		bool isActivated() const { return activated; }

		unsigned getRASCount() const { return RASCount; }
		unsigned getCASCount() const { return CASCount; }
		unsigned getCASWCount() const { return CASWCount; }

		unsigned getTotalRASCount() const { return totalRASCount; }
		unsigned getTotalCASCount() const { return totalCASCount; }
		unsigned getTotalCASWCount() const { return totalCASWCount; }

		Command *pop() { return perBankQueue.pop(); }
		bool push(Command *value) { return perBankQueue.push(value); }
		bool insert(Command *value, const int index) { return perBankQueue.insert(value, index); }
		const Command *read(const unsigned value) const { return perBankQueue.read(value); }
		const Command *front() const { return perBankQueue.front(); }
		const Command *back() const { return perBankQueue.back(); }
		unsigned size() const { return perBankQueue.size(); }
		CommandType nextCommandType() const { return perBankQueue.front() ? perBankQueue.front()->getCommandType() : NO_COMMAND; }
		unsigned freeCommandSlots() const { return perBankQueue.freecount(); }
		bool openPageInsert(Transaction *value, const tick time);
		bool openPageInsertCheck(const Transaction *value, const tick time) const;
		bool isFull() const { return perBankQueue.isFull(); }

		// mutators
		void setLastRASTime(const tick value) { lastRASTime = value; }
		void setLastCASTime(const tick value) { lastCASTime = value; }
		void setLastCASWTime(const tick value) { lastCASWTime = value; }
		void setLastPrechargeTime(const tick value) { lastPrechargeTime = value; }
		void setLastRefreshAllTime(const tick value) { lastRefreshAllTime = value; }
		void setLastCASLength(const unsigned value) { lastCASLength = value; }
		void setLastCASWLength(const unsigned value) { lastCASWLength = value; }
		void setActivated(const bool value) { activated = value; }
		void incRASCount() { RASCount++; }
		void clrRASCount() { RASCount = 0; }
		void incCASCount() { CASCount++; }
		void clrCASCount() { CASCount = 0; }
		void incCASWCount() { CASWCount++; }
		void clrCASWCount() { CASWCount = 0; }

		// constructors
		explicit Bank(const Settings& settings, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal);
		Bank(const Bank&, const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal);
		Bank(const Bank&);

		// overloads
		bool operator==(const Bank& rhs) const;
		Bank &operator=(const Bank& rhs);

		friend std::ostream& operator<<(std::ostream& , const Bank& );

	private:

		// serialization
		// friends
		friend class boost::serialization::access;

		explicit Bank(const TimingSpecification &timingVal, const SystemConfiguration &systemConfigVal);

		template<class Archive>
		void serialize( Archive & ar, const unsigned version)
		{
			ar & perBankQueue;
			ar & lastRASTime & lastCASTime & lastCASWTime & lastPrechargeTime & lastRefreshAllTime & lastCASLength & 
				lastCASWLength & openRowID & activated & RASCount & totalRASCount & CASCount & totalCASCount & CASWCount & totalCASWCount;
		}

		template <class Archive>
		friend inline void save_construct_data(Archive& ar, const DRAMSimII::Bank* t, const unsigned int file_version)
		{
			const DRAMSimII::TimingSpecification* const timing = &(t->timing);
			ar << timing;
			const DRAMSimII::SystemConfiguration* const systemConfig = &(t->systemConfig);
			ar << systemConfig;			
		}

		template <class Archive>
		friend inline void load_construct_data(Archive & ar, DRAMSimII::Bank *t, const unsigned version)
		{
			DRAMSimII::TimingSpecification* timing;		///< a reference to the timing specification
			ar >> timing;
			DRAMSimII::SystemConfiguration* systemConfig;	///< reference to the system config to obtain specs
			ar >> systemConfig;

			::new(t)DRAMSimII::Bank(*timing,*systemConfig);
		}
	};

}
#endif
