#include "communication/communication.hpp"
#include "control/control.hpp"
#include "calibration/motion.h"

#include <thread>

int main(int argc, char* argv[]){

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <carName>" << std::endl;
    return -1;
  }

  std::string name = std::string(argv[1]);

  autondn::Control *control = new autondn::Control();
  autondn::Communication *communication = new autondn::Communication(*control, name);

  std::thread t1(&autondn::Control::run, control);
  std::thread t2(&autondn::Communication::run, communication);

  communication->runProducer();

  t1.join();
  t2.join();

  return 0;
}
