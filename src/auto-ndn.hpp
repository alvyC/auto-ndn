#ifndef AUTO_NDN_HPP
#define AUTO_NDN_HPP

#include "communication/communication.hpp"
#include "control/control.hpp"

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-cxx/security/signing-info.hpp>
#include <ndn-cxx/security/certificate-cache-ttl.hpp>
#include <ndn-cxx/util/signal.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/cstdint.hpp>

#include "conf-parameter.hpp"
#include "security/certificate-store.hpp"
#include "validator.hpp"

namespace autondn {

static ndn::Name DEFAULT_BROADCAST_PREFIX("/autondn/broadcast");

class AutoNdn {
public:
  using AfterGetCipKey = ndn::util::Signal<AutoNdn, const ndn::Data&, const ndn::Name&>;

  AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler);

  ndn::Face&
  getFace() {
    return m_face;
  }

  Control&
  getControl() {
    return m_control;
  }

  Communication&
  getCommunication() {
    return m_communication;
  }

  ConfParameter&
  getConfParameter() {
    return m_confParameter;
  }

  ndn::security::SigningInfo&
  getSigningInfo() {
    return m_signingInfo;
  }

  void
  run();

  void
  loadValidator(boost::property_tree::ptree section,
                         const std::string& fileName) {
    m_validator.load(section, fileName);
  }

  ndn::ValidatorConfig&
  getValidator() {
    return m_validator;
  }

  void
  loadCertToPublish(ndn::shared_ptr<ndn::IdentityCertificate> certificate) {
    m_certStore.insert(certificate);
  }

  const ndn::Name&
  getDefaultCertName() {
    return m_defaultCertName;
  }

  ndn::security::KeyChain&
  getKeyChain() {
    return m_keyChain;
  }

  void
  initializeKey();

  void
  initialize();

  void
  setKeyInterestFilter();

  void
  onKeyInterest(const ndn::Name& name, const ndn::Interest& interest);

  void
  onKeyPrefixRegSuccess(const ndn::Name& name);

  void
  onRegistrationFailed(const ndn::Name& name);

  ndn::shared_ptr<const ndn::IdentityCertificate>
  getCertificate(const ndn::Name& certificateNameWithoutVersion) {
    std::shared_ptr<const ndn::IdentityCertificate> cert =
     m_certStore.find(certificateNameWithoutVersion);

    if (cert != nullptr) {
     std::cout << "Certificate Store: " << certificateNameWithoutVersion << std::endl;
     return cert;
    }

    std::cout << "Certificate Cache: " << certificateNameWithoutVersion << std::endl;
    return m_certificateCache->getCertificate(certificateNameWithoutVersion);
  }

  void
  addCertificateToCache(ndn::shared_ptr<ndn::IdentityCertificate> certificate) {
    if (certificate != nullptr) {
      m_certificateCache->insertCertificate(certificate);
    }
  }

  void
  generateAndAddPseudonym();

  void
  requestCertForPnym(const ndn::Data&, const ndn::Name&);

  void
  installVehicleCert(const ndn::Interest&, const ndn::Data&, const ndn::Name&);

//private:
  //| Currently returns the same value each time, for the same car
  ndn::Name
  getNewPseudonym();

public:
  //| connect requestcertforpnym to this signal
  AfterGetCipKey m_afterGetCipKey;
  static const ndn::Interest CIP_KEY_INTEREST;


  ndn::Face& m_face;
  ndn::Scheduler& m_scheduler;
  Control m_control;
  Communication m_communication;
  ConfParameter m_confParameter;
  security::CertificateStore m_certStore;                       // used for storing own certs
  ndn::shared_ptr<ndn::CertificateCacheTtl> m_certificateCache; // used for storing other's certs

  Validator m_validator;
  ndn::security::KeyChain m_keyChain;
  ndn::security::SigningInfo m_signingInfo;
  ndn::Name m_defaultCertName;

  ndn::Name m_vehicleCurrentPnym;
  //ndn::Name m_vehicleNewPnym;
  ndn::Name m_makeCurrentPnym;
};

} // end of namespace autondn

#endif //  AUTO_NDN_HPP
