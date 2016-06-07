#include "test-common.hpp"
#include "auto-ndn.hpp"
#include "communication/communication.hpp"
#include "control/control.hpp"
#include "conf-file-processor.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>
#include <boost/test/unit_test.hpp>

namespace autondn {

namespace tests {

using ndn::shared_ptr;

const std::string SECTION_GENERAL =
 "general\n"
 "{\n"
 "network /autondn/              ; name of the network in ndn uri format\n"
 "make /toyota               ; name of the manufacturer of the vehicle in ndn uri format\n"
 "model /camri               ; name of the model in ndn uri format\n"
 "id /789F12345E            ; 10-digit id of the car in ndn uri format\n"
 "}\n\n";

class ConfFileProcessorFixture : public BaseFixture
{
public:
  ConfFileProcessorFixture()
    : CONFIG_FILE("unit-test-auto-ndn.conf")
    , face(make_shared<ndn::util::DummyClientFace>())
    , control(g_scheduler)
    , communication(ndn::ref(*face), &control)
    , autoNdn(ndn::ref(*face), g_scheduler, &control, &communication)
    , fileProcessor(autoNdn, CONFIG_FILE)
  {
    control.setCommunication(&communication);
  }

  ~ConfFileProcessorFixture(){
    remove("unit-test-auto-ndn.conf");
  }

  bool processConfigurationString(std::string confString)
  {
    std::ofstream config;
    config.open("unit-test-auto-ndn.conf");
    config << confString;
    config.close();

    ConfFileProcessor processor(autoNdn, CONFIG_FILE);
    return processor.processConfFile();
  }

private:
  std::string CONFIG_FILE;
  shared_ptr<ndn::util::DummyClientFace> face;
  Control control;
  Communication communication;
public:
  AutoNdn autoNdn;
private:
  ConfFileProcessor fileProcessor;
};

BOOST_FIXTURE_TEST_SUITE(TestConfFileProcessor, ConfFileProcessorFixture)

BOOST_AUTO_TEST_CASE(ConfFileProcessorGeneral)
{
  processConfigurationString(SECTION_GENERAL);
  ConfParameter& conf = autoNdn.getConfParameter();
  conf.buildCarName();

  // General
  BOOST_CHECK_EQUAL(conf.getNetwork(), "autondn");
  BOOST_CHECK_EQUAL(conf.getCarMake(), "toyota");
  BOOST_CHECK_EQUAL(conf.getCarModel(), "camri");
  BOOST_CHECK_EQUAL(conf.getCarId(), "789F12345E");
}

BOOST_AUTO_TEST_SUITE_END()

} //end of namespace tests
} //end of namespace autondn
