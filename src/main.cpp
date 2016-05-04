#include "communication/communication.hpp"
#include "control/control.hpp"
#include "calibration/motion.h"

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

int main(){

  ndn::Face face;
  ndn::util::Scheduler scheduler(face.getIoService());

  autondn::Control control(scheduler);
  autondn::Communication communication(control, face, scheduler);

  control.run();
  communication.run();
  communication.runProducer();

  return 0;
}
