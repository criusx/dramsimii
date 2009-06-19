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

#include <boost/iostreams/filtering_stream.hpp>

#include "globals.h"
#include "transaction.h"
#include "Address.h"
#include "queue.h"
#include "System.h"
#include "Settings.h"
#include "SystemConfiguration.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


#ifndef WIN32
// allow dynamic library linking
#define BOOST_TEST_DYN_LINK
#endif

// undefine these to allow the explicit main
#define BOOST_TEST_MODULE testA
#define BOOST_TEST_MAIN

#include <boost/array.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/serialization/vector.hpp>

using namespace DRAMsimII;
using namespace boost::serialization;

//BOOST_CLASS_TRACKING(DRAMsimII::System,boost::serialization::track_never)
//BOOST_CLASS_TRACKING(PowerConfig,track_always)
//BOOST_CLASS_TRACKING(TimingSpecification,track_always)
//BOOST_CLASS_TRACKING(SystemConfiguration,track_always)
//BOOST_CLASS_TRACKING(Bank,track_always)
//BOOST_CLASS_TRACKING(TimingSpecification,track_never)
//BOOST_CLASS_TRACKING(SystemConfiguration,track_never)

//BOOST_CLASS_TRACKING(DRAMsimII::Queue<DRAMsimII::Address>,boost::serialization::track_never)

template<class T>
void backup(const char *filename,const T& item)
{
	std::ofstream outStream(filename);
	boost::archive::text_oarchive oa(outStream);

	oa << item;
	outStream.close();
}

template<class T>
void backup(const char *filename,const T * const item)
{
	std::ofstream outStream(filename);
	boost::archive::text_oarchive oa(outStream);

	oa << item;
	outStream.close();
}

template<class T>
void restore(const char *filename, T& item)
{
	std::ifstream ifs(filename,std::ios::binary);
	boost::archive::text_iarchive ia(ifs);
	ia >> item;

	ifs.close();
}

template<class T>
void restore(const char *filename, T* &item)
{
	std::ifstream ifs(filename,std::ios::binary);
	boost::archive::text_iarchive ia(ifs);
	ia >> item;

	ifs.close();
}


using namespace boost::unit_test;
using namespace boost::unit_test::framework;
using std::vector;

BOOST_AUTO_TEST_CASE( serialize_vector)
{
	
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
	Address::initialize(settings);
	{

		vector<Address *> vecA(18);

		vecA[9] = new Address(0xfaceffee % Address::maxAddress());

		backup("serializeVectorTest",vecA);

		vector<Address *> vecB;

		restore("serializeVectorTest",vecB);

		BOOST_CHECK(vecA == vecB);	
	}
	{
		vector<unsigned> vecA(18);

		backup("serializeVectorTest2",vecA);

		vector<unsigned> vecB;

		restore("serializeVectorTest2",vecB);

		BOOST_CHECK(vecA == vecB);	
	}
}

BOOST_AUTO_TEST_CASE( rank_copy_test)
{
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
	const TimingSpecification timing(settings);
	const SystemConfiguration sysConfig(settings);

	Rank rk1(settings, timing, sysConfig);
	Rank rk2(rk1);

	BOOST_CHECK_EQUAL(rk1,rk2);

	std::vector<Rank> rkV1(1,Rank(settings,timing,sysConfig));
	std::vector<Rank> rkV2(rkV1);

	BOOST_CHECK(rkV1 == rkV2);

}

BOOST_AUTO_TEST_CASE( circular_buffer_test)
{
	std::srand(std::time(0));
	const int size = std::rand() % 500;
	const int iterations = (std::rand() % 750) + 500;
	boost::circular_buffer<tick> cb1(size);

	BOOST_ASSERT(cb1.empty());

	for (tick i = 0; i <= iterations; i++)
	{
		BOOST_ASSERT(cb1.size() == std::min(i,static_cast<tick>(size)));
		cb1.push_front(i*i);
		BOOST_CHECK(cb1.front() == static_cast<tick>(i*i));
		if (i >= size)
			BOOST_CHECK(cb1.back() == (i - size + 1) * (i - size + 1));
		else
			BOOST_CHECK(cb1.back() == 0);
	}

	BOOST_CHECK(cb1.front() == static_cast<tick>((iterations) * (iterations)));
	BOOST_CHECK(cb1.back() == static_cast<tick>((iterations - size + 1) * (iterations - size + 1)));

	boost::circular_buffer<tick> cb2(cb1);
	BOOST_CHECK(cb1 == cb2);
	boost::circular_buffer<tick> cb3 = cb1;
	BOOST_CHECK(cb1 == cb3);

	boost::circular_buffer<tick> cb4(5);
	boost::circular_buffer<tick> cb5(cb4);
	BOOST_CHECK(cb5 == cb4);

	backup("circularBufferTest",cb1);
	boost::circular_buffer<tick> cb6(3);
	restore("circularBufferTest",cb6);
	BOOST_CHECK(cb1 == cb6);
}

BOOST_AUTO_TEST_CASE( test_transactions)
{
	Settings s(master_test_suite().argc, (char **)master_test_suite().argv);
	s.channelCount = 4;
	s.rankCount = 4;
	s.bankCount = 16;
	s.rowCount = 1024;
	s.columnCount = 16384;
	s.dramType = DDR2;
	s.addressMappingScheme = Address::CLOSE_PAGE_BASELINE;
	Address::initialize(s);
	Address addr(0x00fdbca3);
	Address addr2(0x0000f0d0);

	Transaction *t = new Transaction();

	Transaction *t2 = new Transaction(Transaction::READ_TRANSACTION,1337,64,addr, 0xdeadbeef, 2000, 44);

	BOOST_CHECK(*t != *t2);

	Transaction *t3 = new Transaction(*t2);

	BOOST_CHECK(*t2 == *t3);

	Transaction *t4 = new Transaction(Transaction::WRITE_TRANSACTION,234234223,33, addr2,  33);

	delete t3;

	t3 = new Transaction(*t4);

	//Transaction t5 = *t4;

	BOOST_CHECK(*t3 == *t4);

	//BOOST_CHECK(*t4 == t5);

	t4->setArrivalTime(234230);
	t4->setEnqueueTime(234232);
	t4->setDecodeTime(234234);
	t4->setStartTime(234238);
	t4->setCompletionTime(234242);
	BOOST_CHECK(t4->getExecuteTime() == 4);
	BOOST_CHECK(t4->getDelayTime() == 6);
	BOOST_CHECK(t4->getLatency() == 10);
	BOOST_CHECK(t4->getDecodeDelay() == 2);

	std::cerr << *t << std::endl;
	std::cerr << *t2 << std::endl;
	std::cerr << *t3 << std::endl;
	std::cerr << *t4 << std::endl;


}

BOOST_AUTO_TEST_CASE( test_commands)
{
	Settings s(master_test_suite().argc, (char **)master_test_suite().argv);
	s.channelCount = 4;
	s.rankCount = 4;
	s.bankCount = 16;
	s.rowCount = 1024;
	s.columnCount = 16384;
	s.dramType = DDR2;
	s.addressMappingScheme = Address::CLOSE_PAGE_BASELINE;
	Address::initialize(s);
	Address addr(0x00fdfbca);
	Address addr2(0x0000f0ad0);

	Command *t = new Command();

	Transaction *trans = new Transaction(Transaction::READ_TRANSACTION,3400,8,addr2,42);

	Command *t2 = new Command(trans,1234,false,4); 

	BOOST_CHECK(t2->getAddress() == trans->getAddress());

	BOOST_CHECK(*t != *t2);

	Command *t3 = new Command(*t2);

	BOOST_CHECK(*t2 == *t3);

	Command *t4 = new Command(trans,1235,false,4);

	delete t3;

	t3 = new Command(*t4);

	//Transaction t5 = *t4;
	//delete t3->removeHost();
	//t4->removeHost();
	t3->setEnqueueTime(1234);

	BOOST_CHECK(*t3 != *t4);

	t4->setEnqueueTime(1234);

	BOOST_CHECK(*t3 == *t4);

	//BOOST_CHECK(*t4 == t5);

	t4->setArrivalTime(234230);
	t4->setEnqueueTime(234232);
	t4->setStartTime(234238);
	t4->setCompletionTime(234242);
	BOOST_CHECK(t4->getExecuteTime() == 4);
	BOOST_CHECK(t4->getDelayTime() == 6);
	BOOST_CHECK(t4->getLatency() == 10);

	std::cerr << *t << std::endl;
	std::cerr << *t2 << std::endl;
	std::cerr << *t3 << std::endl;
	std::cerr << *t4 << std::endl;


}

#include <boost/random.hpp>
#include <boost/random/variate_generator.hpp>


BOOST_AUTO_TEST_CASE(addressTest)
{
#if 0
	boost::mt19937 rng;
	boost::uniform_int<PhysicalAddress> rngen(0,Address::maxAddress());
	boost::variate_generator<boost::mt19937, boost::uniform_int<PhysicalAddress>> vargen(rng,rngen);
	Settings s(master_test_suite().argc, (char **)master_test_suite().argv);
	Address::initialize(s);

	Address a;

	for (unsigned i = 0; i < s.channelCount; i++)
		for (unsigned j = 0; j < s.rankCount; j++)
			for (unsigned k = 0; k < s.bankCount; k++)
				for (unsigned l = 0; l < s.columnCount; l++)
					for (unsigned m = 0; m < s.rowCount; m++)
						a.setAddress(i,j,k,l,m);
	for (unsigned i = 0; i < 1500000000; i++)
	{
		Address *add = new Address(vargen());
		delete add;
	}
#endif
}

BOOST_AUTO_TEST_CASE( serialize_command )
{
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
	Address::initialize(settings);
	Transaction *trans1 = new Transaction(Transaction::IFETCH_TRANSACTION,23423, 8, 0xFACEFACE, 0x1234, 0x4);
	// serialize normally
	const Command cmd1(trans1, Address(0xBFEAECFE), 123456, true, 8);
	backup("abce",cmd1);
	Command cmd2;
	restore("abce",cmd2);
	BOOST_CHECK_EQUAL(cmd1, cmd2);

	// serialize via pointer
	const Command *cmd3 = new Command(trans1, Address(0xBFEAECFE), 123456, true, 8);
	backup("abce", cmd3);
	Command *cmd4;
	restore("abce",cmd4);
	BOOST_CHECK_EQUAL(*cmd3, *cmd4);

	delete cmd3;
	delete cmd4;
}

BOOST_AUTO_TEST_CASE(testQueue0)
{
	for (int i = 10; i > 0; i--)
	{
		Queue<std::string> queueA(16);

		// randomize the head/tail pointers
		int numberOfTimes = std::rand() % queueA.depth();

		for (int j = numberOfTimes; j > 0; j--)
		{
			if (std::rand() % 2 == 0)
				queueA.push(new std::string());
			else
				queueA.push_front(new std::string());
		}
		while (!queueA.isEmpty())
			delete queueA.pop();


		char letter = 'a';

		for (int i = queueA.depth(); i > 0; --i)
		{
			std::string *newOne = new std::string();
			*newOne += letter++;
			queueA.push(newOne);
		}

		BOOST_CHECK(queueA.size() == 16);

		std::string *val = queueA.remove(0);
		BOOST_CHECK(queueA.size() == 15);
		BOOST_CHECK(*val == "a");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "b");
		BOOST_CHECK(*(queueA.back()) == "p");

		val = queueA.remove(3);
		BOOST_CHECK(queueA.size() == 14);
		BOOST_CHECK(*val == "e");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "b");
		BOOST_CHECK(*(queueA.back()) == "p");

		val = queueA.remove(13);
		BOOST_CHECK(queueA.size() == 13);
		BOOST_CHECK(*val == "p");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "b");
		BOOST_CHECK(*(queueA.back()) == "o");

		val = queueA.remove(5);
		BOOST_CHECK(queueA.size() == 12);
		BOOST_CHECK(*val == "h");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "b");
		BOOST_CHECK(*(queueA.back()) == "o");

		val = queueA.remove(10);
		BOOST_CHECK(queueA.size() == 11);
		BOOST_CHECK(*val == "n");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "b");
		BOOST_CHECK(*(queueA.back()) == "o");

		val = queueA.remove(0);
		BOOST_CHECK(queueA.size() == 10);
		BOOST_CHECK(*val == "b");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "c");
		BOOST_CHECK(*(queueA.back()) == "o");

		val = queueA.remove(3);
		BOOST_CHECK(queueA.size() == 9);
		BOOST_CHECK(*val == "g");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "c");
		BOOST_CHECK(*(queueA.back()) == "o");

		val = queueA.remove(6);
		BOOST_CHECK(queueA.size() == 8);
		BOOST_CHECK(*val == "l");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "c");
		BOOST_CHECK(*(queueA.back()) == "o");

		val = queueA.remove(7);
		BOOST_CHECK(queueA.size() == 7);
		BOOST_CHECK(*val == "o");
		delete val;
		BOOST_CHECK(*(queueA.front()) == "c");
		BOOST_CHECK(*(queueA.back()) == "m");

		while (!queueA.isEmpty())
			delete queueA.pop();
	}
}

BOOST_AUTO_TEST_CASE( test_queue)
{
	// transaction tests	
	Transaction t(Transaction::READ_TRANSACTION,0xface, 8, Address::maxAddress(), 0, 0, NULL);
	Queue<std::string> queueA(5);
	bool result = queueA.push(new std::string("abc"));
	BOOST_CHECK(result == true);
	result = queueA.push(new std::string("def"));
	BOOST_CHECK(result == true);
	result = queueA.push(new std::string("ghi"));
	BOOST_CHECK(result == true);
	result = queueA.push(new std::string("jkl"));
	BOOST_CHECK(result == true);
	result = queueA.push(new std::string("mno"));
	BOOST_CHECK(result == true);
	result = queueA.push(new std::string("pqr"));
	BOOST_CHECK(result == false);
	result = queueA.push(new std::string("stu"));
	BOOST_CHECK_EQUAL(result , false);
	BOOST_CHECK_EQUAL(queueA.size() , 5);


	Queue<Address> *q0 = new Queue<Address>(25,true);
	delete q0;

	Queue<Command> *q3 = new Queue<Command>(5);
	backup("cmdtest1",q3);
	Queue<Command> *q2;
	restore("cmdtest1",q2);
	BOOST_CHECK_EQUAL(*q3,*q2);

	Queue<Command> *q4 = new Queue<Command>(1);
	backup("cmdtest3",q4);
	restore("cmdtest3",q4);

	q3->push(new Command(&t,0xfacedeef,true,64,Command::ACTIVATE));
	q3->push(new Command(&t,0xfbcedeef,true,64,Command::ACTIVATE));
	q3->push(new Command(&t,0xfccedeef,true,64,Command::ACTIVATE));
	q3->push(new Command(&t,0xfdcedeef,true,64,Command::ACTIVATE));
	q3->push(new Command(&t,0xfecedeef,true,64,Command::ACTIVATE));
	q3->push(new Command(&t,0xffcedeef,true,64,Command::ACTIVATE));
	backup("cmdtest2",q3);
	restore("cmdtest2",q2);
	BOOST_CHECK_EQUAL(*q3,*q2);

	delete q2;
	delete q3;

	std::srand(std::time(0));
	const int iterations = (std::rand() % 500);
	const int size = std::rand() % 500;	

	Queue<Command> q5(size);
	for (int i = iterations; i > 0; --i)
	{
		q5.push(
			new Command(
			(new Transaction(
			Transaction::READ_TRANSACTION,
			std::rand(),
			std::rand()%9,
			*(new Address(std::rand())),
			0,
			0,
			NULL)),
			std::rand(),
			true,8));
	}

	backup("queueTest",q5);
	Queue<Command> q6(0);
	restore("queueTest",q6);
	BOOST_CHECK_EQUAL(q5,q6);

	Queue<int> q7(15);
	for (int a = 6; a > 0; --a)
	{
		for (int i = 5; i > 0; --i)
		{
			bool result = q7.push(new int(5));
			BOOST_ASSERT(result);
		}	
		q7.insert(new int(4),0);
		q7.insert(new int(6),q7.size());
		q7.insert(new int(1),3);
		q7.insert(new int(7),0);
		q7.insert(new int(8),3);
		int *val;
		BOOST_CHECK_EQUAL(*q7.front() , 7);
		//delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 4);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 5);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 8);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 5);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 1);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 5);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 5);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 5);
		delete val;
		val = q7.pop();
		BOOST_CHECK_EQUAL(*q7.front() , 6);
		delete val;
		val = q7.pop();
		delete val;
		BOOST_CHECK_EQUAL(q7.size() , 0);
	}
	
	//q7.resize(20);
	Queue<int> q8(20);

	for (int a = 5; a > 0; --a)
	{
		result = q8.push(new int(5));
		BOOST_ASSERT(result);
		result = q8.push_front(new int(-5));
		BOOST_ASSERT(result);
		result = q8.insert(new int(-1),1);
		BOOST_ASSERT(result);
		result = q8.insert(new int(-2),q8.size() - 1);
		BOOST_ASSERT(result);

		BOOST_CHECK_EQUAL(*q8.front(), -5);
		BOOST_CHECK_EQUAL(*q8.back(), 5);
		BOOST_CHECK_EQUAL(*q8.at(1), -1);
		BOOST_CHECK_EQUAL(*q8.at(q8.size() - 2), -2);
	}
	BOOST_CHECK_EQUAL(q8.size(), 20);
}


BOOST_AUTO_TEST_CASE( test_serialization)
{
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
	Settings settings2(master_test_suite().argc, (char **)master_test_suite().argv);
	settings2.decodeWindow = 8;

	Queue<Address> t3(5);
	t3.push(new Address(0xfeeddeef));
	t3.push(new Address(0xfeeddeed));
	t3.push(new Address(0xfeeddeeb));
	t3.push(new Address(0xfeeddeec));
	t3.push(new Address(0xfeeddeea));
	t3.push(new Address(0xfeeddee0));
	BOOST_CHECK_EQUAL(t3.size() , 5);

	backup("aaaa",&t3);
	Queue<Address> *q1;
	restore("aaaa",q1);
	BOOST_CHECK_EQUAL(t3,*q1);

	const Address t4(0xfedefefd);

	Queue<Address> t5(12);

	// serialize normally
	const TimingSpecification ts1(settings);
	backup("abcd",ts1);
	TimingSpecification ts2(settings2);
	restore("abcd",ts2);
	BOOST_CHECK_EQUAL(ts1 , ts2);


	// serialize through a pointer
	const TimingSpecification *ts3 = new TimingSpecification(settings);
	backup("aaaab",ts3);
	TimingSpecification *ts4;
	restore("aaaab",ts4);
	BOOST_CHECK_EQUAL(*ts3,*ts4);

	// serialize normally
	const SystemConfiguration sc1(settings);
	backup("abcc",sc1);
	SystemConfiguration sc2(settings2);
	restore("abcc",sc2);
	BOOST_CHECK_EQUAL(sc1 , sc2);

	// serialize via pointer
	const SystemConfiguration *sc4 = new SystemConfiguration(settings);
	backup("sysconfigbackup", sc4);
	SystemConfiguration *sc5;
	restore("sysconfigbackup",sc5);
	BOOST_CHECK_EQUAL(*sc4,*sc5);

	//TimingSpecification ts3(settings2);

	// serialize normally
	Address::initialize(settings);
	const Address ad1(0xf5fedef34234234);
	backup("abce",ad1);
	Address ad2;
	restore("abce",ad2);
	BOOST_CHECK_EQUAL(ad1 , ad2);

	// serialize via pointer
	const Address *ad3 = new Address(0xf5fedef34234234);
	backup("abce",ad3);
	Address *ad4;
	restore("abce",ad4);
	BOOST_CHECK_EQUAL(*ad3 , *ad4);

	SystemConfiguration sc3(settings2);

	
	const SimulationParameters sp1(settings);
	backup("aaad",&sp1);
	SimulationParameters *sp2;
	restore("aaad",sp2);
	BOOST_CHECK_EQUAL(sp1,*sp2);

	//const PowerConfig pc1(settings);
	//backup("aaac",&pc1);
	const PowerConfig *pc0 = new PowerConfig(settings);
	backup("aaac", pc0);
	PowerConfig *pc3 = new PowerConfig(settings2);
	restore("aaac",pc3);
	BOOST_CHECK_EQUAL(*pc0, *pc3);

	// serialize normally
	const Statistics st1(settings);
	backup("statsReg",&st1);
	Statistics *st2;
	restore("statsReg",st2);
	BOOST_CHECK_EQUAL(st1 , *st2);
#if 0
	// serialize via a pointer
	const Statistics *st3 = new Statistics(settings);
	backup("statsPtr",st3);
	Statistics *st4;
	restore("statsPtr",st4);
	BOOST_CHECK_EQUAL(*st3,*st4);


	//BOOST_CHECK_EQUAL(pc1 , pc2);
	//BOOST_CHECK_EQUAL(pc1, *pc3);

	const Bank bk1(settings, ts1, sc1);
	backup("banktest",&bk1);
	//Bank bk2(settings2, *ts3, sc3);
	Bank *bk2;
	//BOOST_CHECK_EQUAL(bk1 , bk2);
	//restore("aaad",bk2);
	restore("banktest",bk2);
	BOOST_CHECK_EQUAL(bk1 , *bk2);
	delete bk2;
	Bank *bk3;
	restore("banktest",bk3);
	BOOST_CHECK_EQUAL(*bk3,bk1);
	delete bk3;

	const Bank *bk4 = new Bank(settings, ts1, sc1);
	backup("aaae",bk4);
	Bank *bk5 = new Bank(settings2, *ts3, sc3);
	//BOOST_CHECK_EQUAL(bk1 , bk2);
	restore("aaae",bk5);
	BOOST_CHECK_EQUAL(*bk5 , *bk4);

	std::vector<Bank> *bk6 = new std::vector<Bank>(6,Bank(settings, ts1, sc1));

	backup("testvector1",bk6);
	std::vector<Bank> *bk7;
	restore("testvector1",bk7);

	const Rank rk1(settings,ts1,sc1);
	backup("rankTest1",&rk1);
	Rank *rk2;
	restore("rankTest1",rk2);
	BOOST_CHECK_EQUAL(rk1,*rk2);

	Channel ch1(settings, sc1, *st2);
	ch1.setChannelID(0);
	Transaction *t1 = new Transaction(Transaction::IFETCH_TRANSACTION, 1590, 8, Address(0x14584), 0x20, 0x2);
	ch1.moveToTime(150);
	ch1.enqueue(t1);
	ch1.moveToTime(3523);
	backup("chanTest1",&ch1);
	Channel *ch2;
	restore("chanTest1",ch2);
	BOOST_CHECK_EQUAL(ch1, *ch2);

	System *ds2 = new System(settings);	
	backup("ds2",ds2);
	System *ds5;
	restore("ds2",ds5);
	BOOST_CHECK_EQUAL(*ds2,*ds5);

	System *ds3 = new System(settings);
	ds3->runSimulations(500);
	backup("systemTest",ds3);

	restore("ds2",ds3);
	BOOST_CHECK_EQUAL(*ds2,*ds3);

	System *ds4 = new System(settings);
	restore("systemTest",ds4);
	BOOST_CHECK_EQUAL(*ds3,*ds4);

	delete ds2;
	delete ds3;
	delete ds4;
#endif

	//ds.runSimulations();
	//timingOutStream << ds << endl;
}

#if 0
#include <sstream>
using std::stringstream;
using std::string;

BOOST_AUTO_TEST_CASE( test_system_serialization)
{
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
	
	System *ds3 = new System(settings);	

	const string name("systemTest");

	stringstream fullname;

	unsigned counter = 0;

	for (int i = std::rand() % 16384; i > 0; i--)
	{

		ds3->runSimulations(500 + (std::rand() % 2*65536));

		fullname.str("");
		fullname << name << counter++;

		backup(fullname.str().c_str(),ds3);

		System *ds4;

		restore(fullname.str().c_str(),ds4);

		BOOST_CHECK_EQUAL(*ds4,*ds3);

		delete ds3;

		ds4->runSimulations(500 + (std::rand() % 2*65536));

		fullname.str("");
		fullname << name << counter++;

		backup(fullname.str().c_str(),ds4);

		restore(fullname.str().c_str(),ds3);

		BOOST_CHECK_EQUAL(*ds3,*ds4);

		ds4->runSimulations();
		delete ds4;
	}
}


BOOST_AUTO_TEST_CASE( serialize_inputstream)
{
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
	const SystemConfiguration sc1(settings);

	Statistics st1(settings);
	std::vector<Channel> ch1(4, Channel(settings,sc1, st1));

	const InputStream is1(settings, sc1,ch1);
	backup("testInputStream",&is1);
	InputStream *is2;
	restore("testInputStream",is2);
	BOOST_CHECK_EQUAL(is1, *is2);
}

#endif