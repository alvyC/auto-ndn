#include <ndn-cxx/util/time.hpp>

#include "auto-ndn.hpp"

namespace autondn {

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
    std::cout << "In AutoNdn::run() " << std::endl;
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
    std::cout << "Initialize key: default certificate = " << m_defaultCertName << std::endl;
  }

  void
  AutoNdn::initialize() {
    m_confParameter.buildCarName();
    std::cout << "Vehicle name: " << m_confParameter.getCarName() << std::endl;
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
    
    std::cout << "Got interest for certificate: " << certName << std::endl;

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
} // end of namespace autondn
