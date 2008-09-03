#ifndef ADDRESSES_H
#define ADDRESSES_H
#pragma once

#include "globals.h"
#include "Settings.h"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/is_abstract.hpp>

namespace DRAMSimII
{
	/// @brief This class logically represents several interpretations of a memory address
	/// @details A class to store various representations of an address
	/// The same address may be represented as a virtual address, physical address, 
	/// or as channel, rank, bank, column and row identifiers
	class Address
	{
	protected:
		static unsigned channelAddressDepth;
		static unsigned rankAddressDepth;
		static unsigned bankAddressDepth;
		static unsigned rowAddressDepth;
		static unsigned columnAddressDepth;
		static unsigned columnSizeDepth;
		static unsigned cacheLineSize;
		static AddressMappingScheme mappingScheme;

		unsigned virtualAddress;			///< the virtual address
		PHYSICAL_ADDRESS physicalAddress;	///< the physical address

		unsigned channel;					///< the enumerated channel id
		unsigned rank;						///< the rank id
		unsigned bank;						///< the bank id
		unsigned row;						///< the row id
		unsigned column;					///< the column id

		// functions
		bool convertFromAddress();
		bool convertToAddress();
	public:

		// functions
		PHYSICAL_ADDRESS static highestAddress();

		// accessors
		PHYSICAL_ADDRESS getPhysicalAddress() const { return physicalAddress; }
		unsigned getChannel() const { return channel; }
		unsigned getRank() const { return rank; }
		unsigned getBank() const { return bank; }
		unsigned getRow() const { return row; }
		unsigned getColumn() const { return column; }

		// mutators
		void setPhysicalAddress(PHYSICAL_ADDRESS pa) { physicalAddress = pa; convertFromAddress(); }
		void setAddress(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column);

		// constructor
		Address();						///< the no-arg constructor
		explicit Address(PHYSICAL_ADDRESS pA);	///< the constructor based on a physical address
		explicit Address(const unsigned channel, const unsigned rank, const unsigned bank, const unsigned row, const unsigned column);

		// initialize
		void static initialize(const Settings &dramSettings);
		void static initialize(const SystemConfiguration &systemConfig);

		// friend
		friend std::ostream &DRAMSimII::operator<<(std::ostream &os, const Address &this_a);
		friend std::ostream &DRAMSimII::operator<<(std::ostream &os, const Transaction &this_t);
		friend class boost::serialization::access;

		// overloads
		bool operator==(const Address& right) const;

	private:
		template<class Archive>
		void serialize(Archive & ar, const unsigned version )
		{
			ar & channelAddressDepth & rankAddressDepth & bankAddressDepth & rowAddressDepth & columnAddressDepth & columnSizeDepth & cacheLineSize;
			ar & mappingScheme & virtualAddress & physicalAddress & channel & rank & bank & row & column;
		}
	};
}
#endif
