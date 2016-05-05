#include "communication/communication.hpp"
#include "control/control.hpp"
#include "calibration/motion.h"

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <thread>

int main(){

//!!!!!!!!!!!REMOVE IOSERVICE AND FACE FROM HERE SINCE DONT NEED THEM ANYMORE?

  ndn::Face face;
  ndn::util::Scheduler scheduler(face.getIoService());

  autondn::Control *control = new autondn::Control(scheduler);
  autondn::Communication *communication = new autondn::Communication(*control, face, scheduler);

  //order here does not matter, check when they are scheduled for
  std::thread t1(&autondn::Control::run, control);
  std::thread t2(&autondn::Communication::run, communication);

//  communication->runProducer();

  t1.join();
  t2.join();

  return 0;
}
