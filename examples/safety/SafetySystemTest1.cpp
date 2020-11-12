#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/hal/HAL.hpp>
#include <eeros/core/Executor.hpp>
#include <eeros/task/Lambda.hpp>
#include <signal.h>

#include "SafetyPropertiesTest1.hpp"

using namespace eeros;
using namespace eeros::hal;
using namespace eeros::safety;
using namespace eeros::logger;

void signalHandler(int signum) {
	SafetySystem::exitHandler();
}

int main(int argc, char **argv) {
	signal(SIGINT, signalHandler);
	
  Logger::setDefaultStreamLogger(std::cout);
  Logger log = Logger::getLogger();
	
	log.info() << "Safety System Example started...";
	
	// Get HAL instance and initialize
	HAL& hal = HAL::instance();
	hal.readConfigFromFile(&argc, argv);
	
	// Create and initialize safety system
	double period = 1;
	SafetyPropertiesTest1 ssProperties;
	SafetySystem safetySys(ssProperties, period);

	safetySys.triggerEvent(ssProperties.seStartInitializing);

	// Create and run executor
	auto& executor = eeros::Executor::instance();
	executor.setMainTask(safetySys);
	
	eeros::task::Lambda l1 ([&] () {
		static int count = 0;
		if (count++ == 5) safetySys.triggerEvent(ssProperties.seInitializingDone);
	});
	eeros::task::Periodic t1("t1", period, l1);
	executor.add(t1);

	executor.run();

	log.info() << "Test finished...";
}
