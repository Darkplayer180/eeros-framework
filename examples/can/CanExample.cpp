#include "CanExample.hpp"

#include <iostream>
#include <signal.h>

#include <eeros/control/can/CanHandle.hpp>
#include <eeros/control/can/CanReceiveFaulhaber.hpp>
#include <eeros/control/can/CanSendFaulhaber.hpp>
#include <eeros/control/TimeDomain.hpp>
#include <eeros/core/Executor.hpp>
#include <eeros/sequencer/Sequencer.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>

using namespace eeros;
using namespace eeros::logger;
using namespace eeros::control;
using namespace eeros::safety;
using namespace eeros::sequencer;


int main(int argc, char **argv) {
  double dt = 0.2;
  
  StreamLogWriter w(std::cout);
  Logger::setDefaultWriter(&w);
  Logger log;
//   w.show();

  log.info() << "CAN test start";

  ControlSystem cs(dt);
  MySafetyProperties sp;
  SafetySystem ss(sp, dt);
  auto& sequencer = Sequencer::instance();
  MainSequence mainSeq("Main Sequence", sequencer, cs, ss, sp);
  sequencer.addSequence(mainSeq);
  mainSeq.start();
    
  auto& executor = Executor::instance();
  executor.setMainTask(ss);
  executor.run();

  log.info() << "CAN test end";	
  return 0;
}
