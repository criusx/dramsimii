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

BOOST_AUTO_TEST_CASE( serialize_vector ) 
{
	vector<Address *> vecA(18);

	vecA[9] = new Address(0xfaceffee);

	backup("serializeVectorTest",vecA);

	vector<Address *> vecB;

	restore("serializeVectorTest",vecB);

	BOOST_CHECK(vecA == vecB);
}

BOOST_AUTO_TEST_CASE( rank_copy_test )
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

BOOST_AUTO_TEST_CASE( test_queue)
{
	// transaction tests
	Transaction t(READ_TRANSACTION,0xface, 8, PHYSICAL_ADDRESS_MAX, 0, 0, NULL);
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

	q3->push(new Command(t,0xfacedeef,true,true,ACTIVATE));
	q3->push(new Command(t,0xfbcedeef,true,true,ACTIVATE));
	q3->push(new Command(t,0xfccedeef,true,true,ACTIVATE));
	q3->push(new Command(t,0xfdcedeef,true,true,ACTIVATE));
	q3->push(new Command(t,0xfecedeef,true,true,ACTIVATE));
	q3->push(new Command(t,0xffcedeef,true,true,ACTIVATE));
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
			*(new Transaction(
			READ_TRANSACTION,
			std::rand(),
			std::rand()%9,
			*(new Address(std::rand())),
			0,
			0,
			NULL)),
			std::rand(),
			true,true,8
			)
			);
	}

	backup("queueTest",q5);
	Queue<Command> q6(0);
	restore("queueTest",q6);
	BOOST_CHECK_EQUAL(q5,q6);
}


BOOST_AUTO_TEST_CASE( test_system_serialization )
{
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
	
	System *ds3 = new System(settings);	

	//for (int i = std::rand() % 30; i > 0; i--)
	{

		ds3->runSimulations(500 + (std::rand() % 50000));

		backup("systemTest",ds3);

		System *ds4;

		restore("systemTest",ds4);

		BOOST_CHECK_EQUAL(*ds4,*ds3);

		delete ds3;

		ds4->runSimulations(600);

		backup("systemTest2",ds4);

		restore("systemTest2",ds3);

		BOOST_CHECK_EQUAL(*ds3,*ds4);

		ds4->runSimulations();
		delete ds4;
	}
}


BOOST_AUTO_TEST_CASE( serialize_inputstream )
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

BOOST_AUTO_TEST_CASE( test_serialization )
{
	const Settings settings(master_test_suite().argc, (char **)master_test_suite().argv);
#if 0
	const Settings settings2;

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

	const Statistics st1(settings);
	backup("aaab",&st1);
	Statistics *st2;
	restore("aaab",st2);
	BOOST_CHECK_EQUAL(st1 , *st2);

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
	backup("chanTest1",&ch1);
	Channel *ch2;
	restore("chanTest1",ch2);
	BOOST_CHECK_EQUAL(ch1, *ch2);
#endif

	System *ds2 = new System(settings);	
	backup("abbc",ds2);
	System *ds5;
	restore("abbc",ds5);
	BOOST_CHECK_EQUAL(*ds2,*ds5);

	System *ds3 = new System(settings);
	ds3->runSimulations(500);
	backup("systemTest",ds3);
	restore("abbc",ds3);
	BOOST_CHECK_EQUAL(*ds2,*ds3);
	System *ds4 = new System(settings);
	restore("systemTest",ds4);
	BOOST_CHECK_EQUAL(*ds3,*ds4);
	delete ds2;
	delete ds3;
	delete ds4;


	//ds.runSimulations();
	//timingOutStream << ds << endl;
}
