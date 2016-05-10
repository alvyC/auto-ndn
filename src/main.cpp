#include "communication.hpp"
#include "control.hpp"
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

int main(/*int argc, char* argv[]*/){

  /*if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <carName>" << std::endl;
    return -1;
  }*/

  //std::string name = std::string(argv[1]);
  ndn::Face face;

  ndn::util::Scheduler scheduler(face.getIoService());

  autondn::Control *control = new autondn::Control(scheduler);
  autondn::Communication *communication = new autondn::Communication(face, control);
  control->setCommunication(communication);

  control->run();

  try {
    communication->runProducer();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}
