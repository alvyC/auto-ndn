#ifndef COMMUNICATION
#define COMMUNICATION

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/time.hpp>

#include "motion.h"

namespace autondn {

class Communication
{
public:
  Communication();

  void
  onInterest(const ndn::InterestFilter& filter, const ndn::Interest& interest);

  void
  onData(const ndn::Interest& interest, const ndn::Data& data);

  void
  sendInterests(const ndn::Interest& interest);

  void
  runProducer();

  void onRegisterFailed(const ndn::Name& prefix, const std::string& reason);

  void
  onTimeout(const ndn::Interest& interest);

  // broadcasting data for the current road
  void
  sendData(const ndn::Data& data);

  std::string
  getDecision() const {
    return m_decision;
  }

  void
  setDecision(const std::string& decision) {
    m_decision = decision;
  }

private:
  ndn::Face m_face;
  ndn::KeyChain m_keyChain;
  std::string m_decision;
};

} // namespcace autondn

#endif
