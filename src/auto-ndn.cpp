#define BOOST_LOG_DYN_LINK 1
#include "logging.hpp"
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/v1/certificate.hpp>

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
    m_keyChain.addIdentity(defaultIdentity);
    m_defaultCertName = m_keyChain.getDefaultCertificateName();
    //std::cout << "Initialize key: default certificate = " << m_defaultCertName << std::endl;
  }

  void
  AutoNdn::initialize() {
    m_confParameter.buildCarName();
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
    m_vehiclePnym = ndn::Name("/pseudonym1");
    m_makePnym = m_confParameter.getCarMake();
    m_vehiclePnym.append(m_makePnym);

    ndn::Name keyName = m_keyChain.generateRsaKeyPairAsDefault(m_vehiclePnym, true);
    std::shared_ptr<ndn::PublicKey> pubKey = m_keyChain.getPublicKey(keyName);

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
    interestName.append(ndn::Name(i.getName().get(-1))); // get the name (id) of the proxy

    ndn::Interest interest(interestName); // interest name: /autondn/CIP/<proxy-id>

    ndn::security::v1::Certificate cert(d);

  }
} // end of namespace autondn
