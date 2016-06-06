#include "test-common.hpp"
#include "auto-ndn.hpp"
#include "communication/communication.hpp"
#include "control/control.hpp"
#include "conf-file-processor.hpp"

namespace autondn {

namespace tests {

class ConfFileProcessorFixture : public BaseFixture
{
public:
  ConfFileProcessorFixture()
    : confFileName("auto-ndn.conf")
  {
    control = new Control(t_scheduler);
    communication = new Communication(ndn::ref(*t_face), control);
    control->setCommunication(communication);

    autoNdn = new AutoNdn(ndn::ref(*t_face), t_scheduler, control, communication);
    fileProcessor = new ConfFileProcessor (*autoNdn, confFileName);
  }

  ~ConfFileProcessorFixture(){
    delete control;
    delete communication;
    delete autoNdn;
    delete fileProcessor;
  }

private:
  Control *control;
  Communication *communication;
  std::string confFileName;
  AutoNdn *autoNdn;
  ConfFileProcessor *fileProcessor;
};

} //end of namespace tests
} //end of namespace autondn
