#include <eeros/logger/Logger.hpp>
#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/safety/SafetySystem.hpp>
#include <eeros/control/TimeDomain.hpp>
#include <eeros/task/Periodic.hpp>
#include <eeros/core/Executor.hpp>
#include <eeros/control/PathPlannerConstAcc.hpp>
#include <eeros/math/Matrix.hpp>
#include <eeros/task/Lambda.hpp>

using namespace eeros;
using namespace eeros::safety;
using namespace eeros::logger;
using namespace eeros::control;
using namespace eeros::task;
using namespace eeros::math;

double period = 1;

class ControlSystem {
 public:
  ControlSystem() : pp(1.0, 0.2, 0.2, period) {
    pp.setName("ppca");
    pp.getPosOut().getSignal().setName("pp pos out");
    pp.getVelOut().getSignal().setName("pp vel out");
    pp.getAccOut().getSignal().setName("pp acc out");
  }

  PathPlannerConstAcc<Matrix<2,1,double>> pp;
};

class SafetyPropertiesTest : public SafetyProperties {
 public:
  SafetyPropertiesTest() : slState1("state 1") {
    addLevel(slState1);
    setEntryLevel(slState1);	
  };
  SafetyLevel slState1;
};

int main() {
  StreamLogWriter w(std::cout);
  Logger::setDefaultWriter(&w);
  Logger log;
  log.info() << "Pathplanner test 1 started...";
  
  ControlSystem cs;
  TimeDomain td("td", period, true);
  td.addBlock(cs.pp);
  Periodic p1("p1", period, td);
  
  SafetyPropertiesTest sp;
  SafetySystem safetySys(sp, period);
    
  // create periodic function for logging
  Lambda l1 ([&] () { });
  Periodic p2("p2", period, l1);
  p2.monitors.push_back([&](PeriodicCounter &pc, Logger &log) {
    static int count = 0;
    log.info() << cs.pp.getAccOut().getSignal().getValue() << "  " 
               << cs.pp.getVelOut().getSignal().getValue() << "  " 
               << cs.pp.getPosOut().getSignal().getValue();
    if (count == 3) {
      log.warn() << "start trajectory";
      Matrix<2,1,double> start{0, 10}, end{10, 20};
      cs.pp.move(start, end);
    }
    if (count == 23) {
      log.warn() << "start trajectory";
      cs.pp.setStart({{{15, 30}, {0, 0}, {0, 0}}});
      cs.pp.move({{{5, 20}, {0, 0}, {0, 0}}});
    }
    count++;
  });
  
  auto& executor = Executor::instance();
  executor.setMainTask(safetySys);
  p1.after.push_back(p2); // make sure that logging happens after running of path planner
  executor.add(p1);
  executor.run();

  log.info() << "Test finished...";
}
