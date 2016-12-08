#include "test-common.hpp"
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
    {
    }

public:
  shared_ptr<ndn::util::DummyClientFace> face;
  Control control;
  Communication communication;
};
} // end of namespace autondn
} // end of namespace test