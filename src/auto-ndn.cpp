#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/certificate-subject-description.hpp>

#include "auto-ndn.hpp"

namespace autondn {

AutoNdn::AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler,
                               Control* control, Communication* communication)
  : m_face(face)
  , m_scheduler(scheduler)
  , m_control(control)
  , m_communication(communication)
  , m_confParameter()
  , m_validator(face)
  {
    m_control->setCommunication(communication);
  }

  void
  AutoNdn::run() {
    initialize();
    m_control->run();

    try {
      m_communication->runProducer();
    }
    catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  void
  AutoNdn::initializeKey() {
    ndn::Name defaultIdentity = m_confParameter.getNetwork();
    defaultIdentity.append(m_confParameter.getCarName());

    try {
      m_keyChain.deleteIdentity(defaultIdentity);
    }
    catch (std::exception& e) {
    }

    m_signingInfo = ndn::security::SigningInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID, defaultIdentity);
    ndn::Name keyName = m_keyChain.generateRsaKeyPairAsDefault(defaultIdentity, true);
    ndn::shared_ptr<ndn::IdentityCertificate> certificate = 
      ndn::make_shared<ndn::IdentityCertificate>();
    ndn::shared_ptr<ndn::PublicKey> pubKey = m_keyChain.getPublicKey(keyName);
    
    ndn::Name certificateName = keyName.getPrefix(-1); 
    certificateName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();
    
    certificate->setName(certificateName);
    certificate->setNotBefore(ndn::time::system_clock::now() - ndn::time::days(1));
    certificate->setNotAfter(ndn::time::system_clock::now() + ndn::time::days(7300)); // ~20 years
    certificate->setPublicKeyInfo(*pubKey);
    certificate->addSubjectDescription(ndn::CertificateSubjectDescription(ndn::oid::ATTRIBUTE_NAME,
                                                        keyName.toUri()));
    certificate->encode();
    //m_keyChain.signByIdentity(*certificate, m_confParam.getManufacturerPrefix());

    m_keyChain.addCertificateAsIdentityDefault(*certificate);
    loadCertToPublish(certificate);

    m_defaultCertName = certificate->getName();
  }

  void
  AutoNdn::initialize() {
    m_confParameter.buildCarName();
    initializeKey();
  }
} // end of namespace autondn