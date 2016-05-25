#include "communication.hpp"
#include "control.hpp"
#include "map.hpp"
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include "communication.hpp"
#include "control.hpp"
#include "auto-ndn.hpp"
#include "conf-file-processor.hpp"

class Error : public std::runtime_error {
public:
  explicit
  Error(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

int main(/*int argc, char* argv[]*/){

  /*if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <carName>" << std::endl;
    return -1;
  }*/

  //std::string name = std::string(argv[1]);
/*  ndn::Face face;

  ndn::util::Scheduler scheduler(face.getIoService());

  autondn::Control *control = new autondn::Control(scheduler);
  autondn::Communication *communication = new autondn::Communication(face, control);
  //control->setCommunication(communication);

  /*control->run();

  try {
    communication->runProducer();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }*/
  std::string confFileName = "auto-ndn.conf";
  autondn::AutoNdn autoNdn(face, scheduler, control, communication);
  autondn::ConfFileProcessor fileProcessor(autoNdn, confFileName);
  if (fileProcessor.processConfFile()) {
    autoNdn.run();
  }
  else {
    throw Error("Error in configuration file processing! Exiting from Autondn");
  }
*/
  autondn::Map my_map;
  my_map.processMap();
  return 0;
}
