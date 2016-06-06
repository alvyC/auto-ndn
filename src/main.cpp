#include "communication/communication.hpp"
#include "control/control.hpp"
#include "map/map.hpp"
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

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

int main(){

  ndn::Face face;

  ndn::util::Scheduler scheduler(face.getIoService());

  autondn::Control *control = new autondn::Control(scheduler);
  autondn::Communication *communication = new autondn::Communication(face, control);
  control->setCommunication(communication);

/*  control->run();

  try {
    communication->runProducer();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

*/
  std::string confFileName = "auto-ndn.conf";
  autondn::AutoNdn autoNdn(face, scheduler, control, communication);
  autondn::ConfFileProcessor fileProcessor(autoNdn, confFileName);
  if (fileProcessor.processConfFile()) {
    std::cout << "success" << std::endl;
    //autoNdn.run();
  }
  else {
    throw Error("Error in configuration file processing! Exiting from Autondn");
  }

  return 0;
}
