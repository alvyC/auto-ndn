#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
// #include <ndn-cxx/security/validator-regex.hpp>
// #include <ndn-cxx/security/sec-rule-relative.hpp>
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/util/scheduler.hpp>

//#include "motion.h"
//#include "control.hpp"

namespace autondn {

class Control;
class AutoNdn;

class Communication
{
public:
  Communication(AutoNdn& autondn, ndn::Face& face, Control& cont)
    : m_autondn(autondn)
    , m_face(face)
    , control(cont)
  {
    /*ndn::shared_ptr<ndn::ValidatorRegex> validator(new ndn::ValidatorRegex(m_face));
    validator->addDataVerificationRule(ndn::make_shared<ndn::SecRuleRelative>("^(<>*)$",
                                                                              "^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$",
                                                                              ">", "\\1", "\\1\\2", true));
    m_validator = validator;

    m_rootIdentity = ndn::Name("autondn");

    //generate ecdsa key pair and make it default
    ndn::Name rootKeyName = m_keyChain.generateEcdsaKeyPairAsDefault(m_rootIdentity);

    //self sign root key
    ndn::shared_ptr<ndn::IdentityCertificate> rootAnchor = m_keyChain.selfSign(rootKeyName);*/
  }

  void
  onInterest(const ndn::InterestFilter& filter, const ndn::Interest& interest);

  void
  onData(const ndn::Interest& interest, const ndn::Data& data);

  void
  sendInterest(const ndn::Interest& interest);

  void
  runProducer();

  void onRegisterFailed(const ndn::Name& prefix, const std::string& reason);

  void
  onTimeout(const ndn::Interest& interest);

  // broadcasting data for the current road
  void
  sendData(const ndn::Data& data);

  void
  onDataValidated(const std::shared_ptr<const ndn::Data>&);

  void
  onValidationFailed(const std::shared_ptr<const ndn::Data>&, const std::string&);

  /*std::string
  getDecision() const {
    return m_decision;
  }

  void
  setDecision(const std::string& decision) {
    m_decision = decision;
  }

  void
  startInfoRequest();

  void
  run();*/

private:
  //security::CertificateStore m_certStore;

  //std::string m_decision;
  AutoNdn& m_autondn;
  ndn::Face& m_face;
  Control& control;


  ndn::KeyChain m_keyChain;
  //std::string m_nextRoad;
  //bool m_interestSent = false;
  //std::string m_carName;
};

} // namespcace autondn

#endif // COMMUNICATION_HPP
