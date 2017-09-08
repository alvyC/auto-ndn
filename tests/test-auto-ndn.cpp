#include "auto-ndn.hpp"
#include "test-common.hpp"
#include <iostream>

#include <ndn-cxx/util/dummy-client-face.hpp>

namespace autondn {
namespace tests {

class TestAutoNdnFixture : public BaseFixture
{
public:
  TestAutoNdnFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(TestAutoNdn, TestAutoNdnFixture)

/* This tests that class AutoNdn can send an Interest to a pre-defined
   CIP prefix and will signal when it receives a reply.
 */
BOOST_AUTO_TEST_CASE(TestGetCipKey)
{
  const ndn::Name REPLY_NAME = {"/some/data/name"};
  const std::string REPLY_CONTENT = "this is some data";

  ndn::util::DummyClientFace face{g_ioService};
  AutoNdn autoNdn{face, g_scheduler};

  // Connect the observer to the signal
  autoNdn.m_afterGetCipKey.connect([&, this] (const ndn::Data& data,
                                              const ndn::Name& vehiclePseudonym) {
    BOOST_CHECK_EQUAL(data.getName(), REPLY_NAME);
    std::string dataContent(reinterpret_cast<const char*>(data.getContent().value()));
    BOOST_CHECK_EQUAL(dataContent, REPLY_CONTENT);
  });

  autoNdn.generateAndAddPseudonym();
  advanceClocks(ndn::time::milliseconds(1));

  // Check that we sent any interests at all
  BOOST_REQUIRE(face.sentInterests.size() > 0);
  bool didMatchInterest = false;
  for (const auto& sentInterest : face.sentInterests) {
    // Check that any of the sent Interests match the CIP key init interest
    didMatchInterest = didMatchInterest ||
      sentInterest.getName() == AutoNdn::CIP_KEY_INTEREST.getName();
  }
  BOOST_CHECK(didMatchInterest);

  // Construct a simple, bogus reply to the data
  std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>("/some/data/name");
  data->setContent(reinterpret_cast<const uint8_t*>(REPLY_CONTENT.c_str()), REPLY_CONTENT.size());
  g_keychain.sign(*data);

  // Receive the data to trigger the signal. The contents emitted to
  // the signal are checked by the function above.
  face.receive(*data);
  advanceClocks(ndn::time::milliseconds{1});
}

/* This tests that, given a data D, with a name
   /autondn/CIP/request-key/<proxy-id> and with content that is the
   certificate of the CIP given by <proxy-id>, will send an
   certificate request interest to that same CIP. The format of that
   request is:

   /autondn/CIP/A/B/C/D

   where A = the CIP's id
         B = the name of the vehicle's manufacturer, encrypted with the CIP's public key
         C = a current, valid public key from the vehicle, encrypted with the manufacturer's public
             key
         D = the cert for the pseudonym that the vehicle wants to use
 */
BOOST_AUTO_TEST_CASE(TestGetPseudonymCert)
{

}

BOOST_AUTO_TEST_SUITE_END() // TestAutoNdn

} // namespace tests
} // namespace autondn
