#ifndef COMMUNICATION
#define COMMUNICATION

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/validator.hpp>
#include <ndn-cxx/security/validator-regex.hpp>
#include <ndn-cxx/security/sec-rule-relative.hpp>
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/util/scheduler.hpp>

//#include "motion.h"
//#include "control.hpp"

namespace autondn {

class Control;

class Communication
{
public:
  Communication( ndn::Face& face, Control* cont )
    : m_face(face)
    , control(cont) 
  {
    ndn::shared_ptr<ndn::ValidatorRegex> validator(new ndn::ValidatorRegex(m_face));
    validator->addDataVerificationRule(ndn::make_shared<ndn::SecRuleRelative>("^(<>*)$", 
                                                                              "^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$", 
                                                                              ">", "\\1", "\\1\\2", true));
    m_rootIdentity = ndn::Name("autondn");
    //ndn::Name certificateName = m_keyChain.createIdentity(m_rootIdentity);
    
    ndn::Name rootKeyName = m_keyChain.generateRsaKeyPair(m_rootIdentity);
    m_keyChain.setDefaultKeyNameForIdentity(rootKeyName);
    //ndn::Name rootCertName = m_keyChain.getDefaultCertificateNameForIdentity(m_rootIdentity);
                                                          
    //ndn::shared_ptr<ndn::IdentityCertificate> rootAnchor = m_keyChain.getCertificate(rootCertName);
    ndn::shared_ptr<ndn::IdentityCertificate> rootAnchor = m_keyChain.selfSign(rootKeyName);
    m_validator = validator;
  };

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
  onValidated(const ndn::shared_ptr<const ndn::Data>& data);

  void
  onValidationFailed(const ndn::shared_ptr<const ndn::Data>& data, const std::string& failinfo);

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
  ndn::Face& m_face;
  ndn::KeyChain m_keyChain;
  ndn::shared_ptr<ndn::Validator> m_validator;
  ndn::Name m_rootIdentity;
  //std::string m_decision;

  Control* control;
  //std::string m_nextRoad;
  //bool m_interestSent = false;
  //std::string m_carName;
};

} // namespcace autondn

#endif
