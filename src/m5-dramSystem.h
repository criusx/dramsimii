#ifndef M5_DRAMSYSTEM
#define M5_DRAMSYSTEM
#pragma once

#include "mem/physical.hh"
#include "mem/tport.hh"
#include "base/random.hh"
#include "params/M5dramSystem.hh" // this gets autogenerated and creates the params struct
#include "sim/async.hh"
#include "base/statistics.hh"
#include "base/stats/types.hh"

#ifdef WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#ifdef TRACE_GENERATE
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>


using boost::iostreams::bzip2_compressor;
using boost::iostreams::gzip_compressor;
using boost::iostreams::gzip_params;
using boost::iostreams::file_sink;
using boost::filesystem::exists;
using boost::filesystem::path;
using boost::filesystem::create_directory;
using boost::filesystem::is_directory;
using std::cout;
using std::stringstream;
using std::setw;
using std::setfill;

#endif

#include <queue>

#include "System.h"
#include "fbdSystem.h"
#include "Settings.h"
#include "globals.h"

using DRAMsimII::tick;


#if defined(DEBUG) && defined(M5DEBUG) && !defined(NDEBUG) // compiler should declare this
#define M5_TIMING(X) ds->getTimingStream() << X << std::endl;
#define M5_TIMING2(X) memory->ds->getTimingStream() << X << std::endl;
#define M5_TIMING3(X) memory->ds->getTimingStream() << X;
#else
#define M5_TIMING(X)
#define M5_TIMING2(X)
#define M5_TIMING3(X)
#endif


/// @brief wrapper class to allow M5 to work with DRAMsimII
class M5dramSystem: public PhysicalMemory
{
	// taken from m5 interface
private:

	/// allows event-based simulation, scheduling wake ups for the simulator
	/// @details derived class that allows the wrapper class to interact with the event model
	class TickEvent : public Event
	{

	private:

		M5dramSystem *memory;	///< pointer to the wrapper class so static functions may see the single instance

	public:
		// constructor
		TickEvent(M5dramSystem *c);

		void process();			///< process to call when a tick event happens

		const char *description();	///< return a string that describes this event

		void schedule(tick when) { mainEventQueue.schedule(this,when);}

		void deschedule() { mainEventQueue.deschedule(this);}
	};

	/// allows this to receive packets and attach to a bus
	class MemoryPort : public SimpleTimingPort
	{
	private:

		M5dramSystem *memory;	///< pointer to the wrapper class so static functions may see the single instance

		Random randomGen;		///< random number generator to help when simulating delays

	public:	

		MemoryPort(const std::string &_name, M5dramSystem *_memory);

		// accessor that was needed to allow the memory system to handle incoming transactions
		void doSendTiming(PacketPtr pkt, Tick t)
		{
			schedSendTiming(pkt,t);
		}	

	protected:

		virtual Tick recvAtomic(PacketPtr pkt);

		virtual void recvFunctional(PacketPtr pkt);

		virtual void recvStatusChange(Status status);

		virtual void getDeviceAddressRanges(AddrRangeList &resp, bool &snoop);

		virtual int deviceBlockSize();

		virtual bool recvTiming(PacketPtr pkt);		
	};



protected:
	TickEvent tickEvent;									///< instance of TickEvent to allow the wrapper to receive/send events to the global queue
	std::vector<MemoryPort*> ports;							///< ports to send/recv data to other simulator components
	std::queue<std::pair<unsigned, tick> > finishedTransactions;	///< channels will return values as they finish transactions
	
	int lastPortIndex;										///< the last port accessed
	typedef std::vector<MemoryPort*>::iterator PortIterator;
	DRAMsimII::System *ds;									///< pointer to the DRAMsimII class
	bool needRetry;											///< if the memory system needs to issue a retry statement before any more requests will come in
	unsigned mostRecentChannel;								///< the most recent channel that a request was sent to
	int cpuRatio;											///< the ratio of the cpu frequency to the memory frequency

	std::tr1::unordered_map<unsigned,Packet*> transactionLookupTable;
	unsigned currentTransactionID;

	unsigned int drain(Event *de);
	void virtual init();

public:
#ifdef TRACE_GENERATE
	boost::iostreams::filtering_ostream traceOutStream;
#endif

	typedef M5dramSystemParams Params;						///< the parameters used to initialize the memory system object

	M5dramSystem(const Params *);							///< constructor

	// allows other components to get a port which they can send packets to
	virtual Port *getPort(const std::string &if_name, int idx = -1);

	void getAddressRanges(AddrRangeList &resp, bool &snoop);

	int getCPURatio() const { return cpuRatio; }			///< returns the ratio of the cpu frequency to the memory frequency

	//float getInvCPURatio() const { return invCpuRatio; }	///< returns the ratio of the memory frequency to the cpu frequency

	void moveToTime(tick now);

	virtual ~M5dramSystem();

	virtual void serialize(std::ostream &os);
	virtual void unserialize(Checkpoint *cp, const std::string &section);

};

#if 0
namespace boost
{
	template<class Archive, class U, class Allocator>
	inline void serialize(Archive &ar, Packet &t, const unsigned file_version)
	{
		boost::serialization::split_free(ar,t,file_version);
	}


	template<class Archive, class U, class Allocator>
	inline void save(Archive &ar, const Packet &t, const unsigned int file_version)
	{
		//const circular_buffer<U>::size_type maxCount(t.capacity());
		//ar << BOOST_SERIALIZATION_NVP(maxCount);
		typename circular_buffer<U, Allocator>::capacity_type maxCount(t.capacity());
		ar << (maxCount);

		//const circular_buffer<U>::size_type count(t.size());
		//ar << BOOST_SERIALIZATION_NVP(count);
		typename circular_buffer<U, Allocator>::size_type count(t.size());
		ar << (count);

		for (typename circular_buffer<U, Allocator>::const_iterator i = t.begin(); i != t.end(); i++)
			//for (boost::circular_buffer<U,Allocator>::size_type i = 0; i < t.size(); i++)
		{
			ar << *i;
			//ar << t[i];
		}
	}

	template<class Archive, class U, class Allocator>
	inline void load(Archive &ar, Packet &t, const unsigned file_version)
	{
		//ar >> BOOST_SERIALIZATION_NVP(maxCount);
		typename circular_buffer<U, Allocator>::capacity_type maxCount;
		ar >> (maxCount);
		t.set_capacity(maxCount);

		//circular_buffer<U>::size_type count;
		//ar >> BOOST_SERIALIZATION_NVP(count);
		unsigned count;
		ar >> (count);

		while (count-- > 0)
		{
			U value;
			ar >> value;
			//t.push_front(value);
			t.push_back(value);
		}
	}

}

#endif // 0
#endif
