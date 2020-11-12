#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/safety/SafetyProperties.hpp>
#include <eeros/safety/InputAction.hpp>
#include <eeros/control/TimeDomain.hpp>
#include <eeros/core/Executor.hpp>
#include <eeros/control/KeyboardInput.hpp>
#include <signal.h>

using namespace eeros;
using namespace eeros::logger;
using namespace eeros::control;
using namespace eeros::task;
using namespace eeros::safety;

double period = 0.01;

class ControlSystem {
public:
	ControlSystem() : keyboard(), td("td1", period, true) {
		keyboard.setName("keyboard");
//		keyboard.getOut().getSignal().setName("position");
		keyboard.getEsc().getSignal().setName("esc event");
		td.addBlock(keyboard);
	}

	KeyboardInput keyboard;
	TimeDomain td;
};

class MySafetyProperties : public SafetyProperties {
public:
	MySafetyProperties() : slFirst("first level"), slSecond("second level"), seGoUp("go to second level"), seGoDown("go to first level") {
		// ############ Define critical outputs ############
		Input<bool>* in1 = HAL::instance().getLogicInput("escKeyboardButton", false);
		criticalInputs = { in1 };

		// ############ Add levels ############
		addLevel(slFirst);
		addLevel(slSecond);
		
		// ############ Define input states and events for all levels ############
		slFirst.setInputActions({ check(in1, false, seGoUp) });
		slSecond.setInputActions({ check(in1, true, seGoDown) });
		
		// ############ Add events to the levels ############
		slFirst.addEvent(seGoUp, slSecond, kPrivateEvent);
		slSecond.addEvent(seGoDown, slFirst, kPrivateEvent);

		setEntryLevel(slFirst);
	}
	virtual ~MySafetyProperties() { }
	SafetyLevel slFirst;
	SafetyLevel slSecond;
	SafetyEvent seGoUp;
	SafetyEvent seGoDown;
};

void signalHandler(int signum) {
	Executor::instance().stop();
}

int main() {
	signal(SIGINT, signalHandler);
  Logger::setDefaultStreamLogger(std::cout);
  Logger log = Logger::getLogger();
	log.info() << "Keyboard Test started ...";
	
	ControlSystem controlSystem;
	MySafetyProperties safetyProperties;
	SafetySystem safetySystem(safetyProperties, period);
		
	Periodic periodic("per1", period, controlSystem.td);
	periodic.monitors.push_back([&](PeriodicCounter &pc, Logger &log){
		static int ticks = 0;
		if ((++ticks * period) < 1) return;
		ticks = 0;
//		log.info() << controlSystem.keyboard.getEsc().getSignal();
//		log.info() << controlSystem.keyboard.getIsHomed().getSignal();
		log.info() << controlSystem.keyboard.getOut().getSignal();
	});
		
	// Create and run executor
	auto& executor = eeros::Executor::instance();
	executor.setMainTask(safetySystem);
	executor.add(periodic);
	executor.run();

	log.info() << "Test finished...";
}
