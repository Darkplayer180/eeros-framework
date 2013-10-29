#include "SequencerTest.hpp"
#include "MySequence.hpp"
#include "CallingSubSequence.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(SequencerTest);

void SequencerTest::setUp(){
}

void SequencerTest::tearDown(){
}

void SequencerTest::testSimpleSequence(){
	//The first allocation of a sequencer is automatically the Main-Sequencer!!
	MySequencer mainSequencer("MainSequencer");
	MySequence sequence("MySequence", mainSequencer);

	//Thread erzeugen:
	mainSequencer.start();
	
	//Thread stoppen wird in Step Stopping gemacht
	//callerThread.stop();
	
	while(!mainSequencer.isTerminated()){
		//std::cout << "waiting for executor to terminate..." << std::endl;
	}
	
	CPPUNIT_ASSERT(sequence.getCalledMethode().compare("Init Initialising Initialised Homed Move Stopping") == 0);
}

void SequencerTest::testSimpleSubSequence(){
	MySequencer mainSequencer("MainSequencer");
	CallingSubSequence sequence("CallingSubSequence", mainSequencer);
	//Thread erzeugen:
	mainSequencer.start();
	
	//Thread stoppen wird in Step Stopping gemacht
	//callerThread.stop();
	
	while(!mainSequencer.isTerminated()){
		//std::cout << "waiting for executor to terminate..." << std::endl;
	}
	
	CPPUNIT_ASSERT(sequence.getCalledMethode().compare("Init Initialising Initialised Homed Move MoveToA MoveToB MoveToC Stopping") == 0);
}

MySequencer::MySequencer(std::string name) : eeros::sequencer::Sequencer(name){
}