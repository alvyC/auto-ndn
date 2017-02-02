#include <ndn-cxx/util/time.hpp>

#include "auto-ndn.hpp"

namespace autondn {

AutoNdn::AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler)
  : m_face(face)
  , m_scheduler(scheduler)
  , m_control(scheduler, m_communication)
  , m_communication(*this, m_face, m_control, m_keyChain)
  , m_confParameter()
  , m_validator(m_face)
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
    /*ndn::Name defaultIdentity = m_confParameter.getCarName();
    //ndn::Name defaultIdentity = "/test";
    std::cout << "Default Identity: " << defaultIdentity << std::endl;
    try {
      m_keyChain.deleteIdentity(defaultIdentity);
    }
      catch (std::exception& e) {
    }

    m_signingInfo = ndn::security::SigningInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID, defaultIdentity);
    ndn::Name keyName = m_keyChain.generateRsaKeyPairAsDefault(defaultIdentity, true);
    /*keyname*/ std::cout << "Keyname: " << keyName.toUri() << std::endl;
    /*ndn::shared_ptr<ndn::IdentityCertificate> certificate =
                                              ndn::make_shared<ndn::IdentityCertificate>();
    ndn::shared_ptr<ndn::PublicKey> pubKey = m_keyChain.getPublicKey(keyName);

    ndn::Name certificateName = keyName.getPrefix(-1);
    certificateName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();
    std::cout << "Certificate name: " << certificateName.toUri() << std::endl;
    certificate->setName(certificateName);
    certificate->setNotBefore(ndn::time::system_clock::now() - ndn::time::days(1));
    certificate->setNotAfter(ndn::time::system_clock::now() + ndn::time::days(7300)); // ~20 years
    certificate->setPublicKeyInfo(*pubKey);
    certificate->addSubjectDescription(ndn::CertificateSubjectDescription(ndn::oid::ATTRIBUTE_NAME,
                                       keyName.toUri()));
    certificate->encode();
    m_keyChain.signByIdentity(*certificate, m_confParameter.getCarMake());

    m_keyChain.addCertificateAsIdentityDefault(*certificate);
    loadCertToPublish(certificate);

    m_defaultCertName = certificate->getName();*/

    m_keyChain.addIdentity(m_confParameter.getCarName());
  }

  void
  AutoNdn::initialize() {
    m_confParameter.buildCarName();
    std::cout << m_confParameter.getCarName() << std::endl;
    initializeKey();
  }
} // end of namespace autondn
