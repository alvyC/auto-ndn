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

#include "conf-parameter.hpp"

namespace autondn {

class Control;
class AutoNdn;

class Communication
{
public:
  Communication(AutoNdn&, ndn::Face&, Control&, ndn::KeyChain&);

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
  ConfParameter& m_confParam;

  ndn::KeyChain& m_keyChain;
  //std::string m_nextRoad;
  //bool m_interestSent = false;
  //std::string m_carName;
};

} // namespcace autondn

#endif // COMMUNICATION_HPP
