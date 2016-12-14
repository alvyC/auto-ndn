#include "test-common.hpp"

#include "auto-ndn.hpp"
#include "control/control.hpp"
#include "communication/communication.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>
#include <boost/test/unit_test.hpp>

namespace autondn{

namespace tests {

using ndn::shared_ptr;

class CommunicationFixture : public BaseFixture
{
public:
  CommunicationFixture()
    : face(make_shared<ndn::util::DummyClientFace>(g_ioService))
    , control(g_scheduler)
    , communication(ndn::ref(*face), &control)
    , autondn(ndn::ref(*face), g_scheduler, &control, &communication)
    {
    }

public:
  shared_ptr<ndn::util::DummyClientFace> face;
  Control control;
  Communication communication;
  AutoNdn autondn;
};

BOOST_FIXTURE_TEST_SUITE(TestCommunication, CommunicationFixture)

BOOST_AUTO_TEST_CASE(Basic) {
  // here
  communication.runProducer();
  ndn::Interest interest("/autondn/1%2C2-1%2C3");
  face->receive(interest);
  face->processEvents(ndn::time::milliseconds(10));
  //BOOST_REQUIRE_EQUAL(face->sentInterests.size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

} // end of namespace autondn

} // end of namespace test