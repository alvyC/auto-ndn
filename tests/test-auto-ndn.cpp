#include "auto-ndn.hpp"
#include "test-common.hpp"
#include <iostream>

#include <ndn-cxx/util/dummy-client-face.hpp>

namespace autondn {
namespace tests {


ndn::ConstBufferPtr
getConstBufferFromEncryptedName(const ndn::Name& encryptedName, ndn::security::KeyChain& keychain,
                         const ndn::Name& identityName) {
  std::vector<uint8_t> nameByteArray = std::vector<uint8_t>{encryptedName.toUri().begin(),
                                                            encryptedName.toUri().end()};
  ndn::ConstBufferPtr namePlain =
    keychain.getTpm().decryptInTpm(&nameByteArray[0], nameByteArray.size(),
                                   keychain.getDefaultKeyForIdentity(identityName), false);
  return namePlain;
}

class TestAutoNdnFixture : public BaseFixture
{
public:
  TestAutoNdnFixture() {}

public:
  const ndn::Name NETWORK = ndn::Name{"autondn"};
  const ndn::Name CAR_MAKE = ndn::Name{"test-mfr"};
  const ndn::Name CAR_ID = ndn::Name{"test-car-id"};
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
  // Have to create keypair for mfr, cip?

  ndn::Name cipId{"test-cip"};
  ndn::Name cipFullName{"/autondn/CIP/test-cip"};
  ndn::Name cipIdentity = g_keychain.createIdentity(cipFullName);
  ndn::Name dataName{AutoNdn::CIP_KEY_INTEREST.getName()};
  dataName.append(cipId);
  // Create the manufacturer key
  ndn::Name mfrIdentity = autoNdn.m_keyChain.createIdentity(m_makeCurrentPnym);
  ndn::util::DummyClientFace face{g_ioService};
  // Create and initialize the AutoNdn instance
  AutoNdn autoNdn{face, g_scheduler};
  autoNdn.getConfParameter().setNetwork(NETWORK);
  autoNdn.getConfParameter().setCarMake(CAR_MAKE);
  autoNdn.getConfParameter().setCarId(CAR_ID);
  autoNdn.initialize();
  ndn::Name pseudonym = autoNdn.getNewPseudonym();

  // Fetch the CIP cert
  std::shared_ptr<ndn::IdentityCertificate> cipCert = g_keychain.getCertificate(cipIdentity);
  // Encode it and attach it to the data
  std::shared_ptr<ndn::Data> data = std::make_shared<ndn::Data>(dataName);
  data->setContent(cipCert->wireEncode());
  // Trigger the requesting of the pseudonym
  autoNdn.requestCertForPnym(*data, pseudonym);

  BOOST_REQUIRE(face.sentInterests().size() > 0);

  std::shared_ptr<ndn::PublicKey> vehicleNewKey = autoNdn.m_keyChain.getPublicKey(pseudonym);
  std::shared_ptr<ndn::PublicKey> vehicleCurrentKey =
    autoNdn.m_keyChain.getPublicKey(m_vehicleCurrentPnym);
  std::vector<uint8_t> nameByteArray;
  bool didMatchInterest = false;
  for (const auto& interest : face.sentInterests().size()) {
    ndn::ConstBufferPtr pseudoKeyBuffer = getConstBufferFromEncryptedName(interest[-1], m_keyChain,
                                                                          mfrIdentity);
    ndn::security::v1::PublicKey decryptedPseudoKey = {pseudoKeyBuffer.buf(),
                                                       pseudoKeyBuffer.size()};

    ndn::ConstBuffer currentKeyBuffer = getConstBufferFromEncryptedName(interest[-2], m_keyChain,
                                                                        mfrIdentity);
    ndn::security::v1::PublicKey decryptedCurrentKey = {currentKeyBuffer.buf(),
                                                        currentKeyBuffer.size()};

    ndn::Name decryptedMfrName =
      ndn::Name{ndn::Block{getConstBufferFromEncryptedName(interest[-3], g_keyChain, cipIdentity)}};

    didMatchInterest = didMatchInterest || (decryptedMfrName == CAR_MAKE
                                            && decryptedPseudoKey == vehicleNewKey
                                            && decryptedCurrentKey == vehicleCurrentKey);
  }
  BOOST_CHECK(didMatchInterest);

  /// Check sent interest
  /// Should have three encrypted components
  /// So, get last one, decrypt with man key, check equal to new pseudo key
  /// Get second to last one, decrypt with man key, check equal to current key
  /// Get third to last one, decrypt with CIP key, check equal to manufacturer name
  /// Get fourth to last component, check equal to last component of data's name
  ///
}

BOOST_AUTO_TEST_SUITE_END() // TestAutoNdn

} // namespace tests
} // namespace autondn
