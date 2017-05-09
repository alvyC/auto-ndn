#define BOOST_LOG_DYN_LINK 1
#include "logging.hpp"
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/v1/certificate.hpp>
#include <ndn-cxx/security/v1/public-key.hpp>
#include <ndn-cxx/security/transform/public-key.hpp>
#include <ndn-cxx/encoding/buffer.hpp>
#include <ndn-cxx/security/pib/identity.hpp>

#include <iostream>

#include "auto-ndn.hpp"

namespace autondn {

_LOG_INIT(autondn);

AutoNdn::AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler)
  : m_face(face)
  , m_scheduler(scheduler)
  , m_control(scheduler, m_communication)
  , m_communication(*this, m_face, m_control, m_keyChain)
  , m_confParameter()
  , m_certificateCache(new ndn::CertificateCacheTtl(m_face.getIoService()))
  , m_validator(m_face, DEFAULT_BROADCAST_PREFIX, m_certificateCache, m_certStore)
  {
  }

  void
  AutoNdn::run() {
    //std::cout << "In AutoNdn::run() " << std::endl;
    initialize();
    m_control.run();

    try {
      m_communication.runProducer();
    }
    catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  void
  AutoNdn::initializeKey() {
    ndn::Name defaultIdentity = m_confParameter.getCarName();
    m_signingInfo = ndn::security::SigningInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID, defaultIdentity);
    //m_keyChain.setDefaultIdentity(ndn::security::pib::Identity(defaultIdentity));
    m_keyChain.addIdentity(defaultIdentity);
    m_defaultCertName = m_keyChain.getDefaultCertificateName();
    //std::cout << "Initialize key: default certificate = " << m_defaultCertName << std::endl;
  }

  void
  AutoNdn::initialize() {
    m_confParameter.buildCarName();
    // TODO: initialize m_vehicleCurrentPnym and schedule generateAndAddPseudonym()
    _LOG_DEBUG("Vehicle name: " << m_confParameter.getCarName());
    initializeKey();
    setKeyInterestFilter();
  }

  void
  AutoNdn::setKeyInterestFilter() {
    ndn::Name keyPrefix = DEFAULT_BROADCAST_PREFIX;
    keyPrefix.append("KEYS");
    m_face.setInterestFilter(keyPrefix,
			     std::bind(&AutoNdn::onKeyInterest,
				       this, _1, _2),
			     std::bind(&AutoNdn::onKeyPrefixRegSuccess, this, _1),
		       std::bind(&AutoNdn::onRegistrationFailed, this, _1),
			     m_signingInfo);
  }

  void
  AutoNdn::onKeyInterest(const ndn::Name& name, const ndn::Interest& interest) {
    const ndn::Name& interestName = interest.getName();
    ndn::Name certName = interestName.getSubName(name.size());

    _LOG_TRACE("Got interest for certificate: " << certName);

    ndn::shared_ptr<const ndn::IdentityCertificate> cert = getCertificate(certName);

    if (!static_cast<bool>(cert))
      return; // cert is not found

    ndn::shared_ptr<ndn::Data> data = ndn::make_shared<ndn::Data>();
    data->setName(interestName);
    data->setContent(cert->wireEncode());
    m_keyChain.sign(*data, m_signingInfo);

    m_face.put(*data);
  }

  void
  AutoNdn::onKeyPrefixRegSuccess(const ndn::Name& name) {
  }

  void
  AutoNdn::onRegistrationFailed(const ndn::Name& name) {
  }

  void
  AutoNdn::generateAndAddPseudonym() {
    /* (1) generate vehicle pseudonym: <vehicle-pnym>/ <make-pnym>
       (2) Create key for the pseudonym
    */
    m_vehicleNewPnym = ndn::Name("/pseudonym1");
    m_makePnym = m_confParameter.getCarMake();
    m_vehicleNewPnym.append(m_makePnym);

    ndn::Name keyName = m_keyChain.generateRsaKeyPairAsDefault(m_vehicleNewPnym, true);
    //std::shared_ptr<ndn::PublicKey> pubKey = m_keyChain.getPublicKey(keyName);

    /*(3) Ask for proxy key, Interest: /autondn/CIP/request-key
          autondn-cip: onKeyRequestInitInterest()*/
    ndn::Interest interest("/autondn/CIP/request-key");
    m_face.expressInterest(interest,
                           std::bind(&AutoNdn::requestCertForPnym, this, _1, _2), // step (4) and (5) here
                           std::bind([] {}));
    /*(4) After getting proxy key, create an encrypted interest
           Interest: /autondn/CIP/<cip-id>/E-CIP{manufacturer, E-Man{vid, K-VCurr, K-VNew}}
           autondn-cip: onCertInterest()*/

    /*(5) Send the Interest to manufacturer via proxy for cert*/
  }

  void
  AutoNdn::requestCertForPnym(const ndn::Interest& i, const ndn::Data& d) {
    /* (1) Encrypt Vehicle's ID, current key, new key with **manufacturer's key**
       (2) Encrypt the step (1)'s encrypted items and manufacturer's name with **proxy's key**
    */
    ndn::Name interestName("/autondn/CIP");
    // get the name (id) of the proxy from interest and append it to the interest name; interest name: /autondn/CIP/<proxy-id>
    interestName.append(ndn::Name(i.getName().get(-1)));

    // make certificate object from the certificate data packet
    ndn::security::v1::Certificate cert(d);
    ndn::security::v1::PublicKey proxyKeyInfo = cert.getPublicKeyInfo();

    ndn::security::transform::PublicKey proxyKey;
    proxyKey.loadPkcs8(proxyKeyInfo.get().buf(), proxyKeyInfo.get().size());

    // encrypt make's name using proxy's key
    std::string makeName = m_confParameter.getCarMake().toUri();
    std::vector<uint8_t> myVector(makeName.begin(), makeName.end());
    uint8_t *p = &myVector[0];
    ndn::ConstBufferPtr cipherMakeName = proxyKey.encrypt(p, myVector.size());

    // append encrypted make name to the interest; interest name: /autondn/CIP/<proxy-id>/E-proxy{make-name}
    interestName.append(cipherMakeName->buf(), cipherMakeName->size());

    // get manufacturer's key ()
    // v2::keyChain.getPib().getIdentity(identityName)
    // ndn::security::pib::Pib pib = m_keyChain.getPib().getIdentity(m_makePnym);
    // m_keyChain.getPib().
    ndn::Name manKeyName = m_keyChain.getDefaultKeyNameForIdentity(m_makePnym);
    std::shared_ptr<ndn::PublicKey> manPubKeyInfo = m_keyChain.getPublicKey(manKeyName);
    ndn::security::transform::PublicKey manKey;
    manKey.loadPkcs8(manPubKeyInfo->get().buf(), manPubKeyInfo->get().size());

    // Get vehicle's current key
    ndn::Name vehicleCurrentKeyName = m_keyChain.getDefaultKeyNameForIdentity(m_vehicleCurrentPnym);
    std::shared_ptr<ndn::PublicKey> vehicleCurrentKeyInfo = m_keyChain.getPublicKey(vehicleCurrentKeyName);
    /*ndn::security::transform::PublicKey vehicleCurrentKey;
    vehicleCurrentKey.loadPkcs8(vehicleCurrentKeyInfo->get().buf(), vehicleCurrentKeyInfo->get().size());*/

    // Get vehicle's new key
    ndn::Name vehicleNewKeyName = m_keyChain.getDefaultKeyNameForIdentity(m_vehicleNewPnym);
    std::shared_ptr<ndn::PublicKey> vehicleNewKeyInfo = m_keyChain.getPublicKey(vehicleNewKeyName);
    /*ndn::security::transform::PublicKey vehicleNewKey;
    vehicleNewKey.loadPkcs8(vehicleNewKeyInfo->get().buf(), vehicleNewKeyInfo->get().size());*/

    // encrypt vehicle's current key using manufacturer's key
    ndn::ConstBufferPtr cipherVehicleCurrentKey = manKey.encrypt(vehicleCurrentKeyInfo->get().buf(), vehicleCurrentKeyInfo->get().size());
    interestName.append(cipherVehicleCurrentKey->buf(), cipherVehicleCurrentKey->size());
    // encrypt vehicle's new key using manufacturer's key
    ndn::ConstBufferPtr cipherVehicleNewKey = manKey.encrypt(vehicleNewKeyInfo->get().buf(), vehicleNewKeyInfo->get().size());
    interestName.append(cipherVehicleNewKey->buf(), cipherVehicleNewKey->size());
    //Interest: /autondn/CIP/<cip-id>/E-CIP{manufacturer, E-Man{vid, K-VCurr, K-VNew}}

  }
} // end of namespace autondn
