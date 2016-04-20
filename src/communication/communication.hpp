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
  void
  onInterest(const ndn::Interest& interest);

  void
  onData(const ndn::Interest& interest, const ndn::Data& data);

  void
  sendInterests(const ndn::Interest& interest);

  void
  runProducer();

  void onRegisterFailed(const ndn::Name& prefix, const std::string& reason);

  void
  onTimeout(const ndn::Interest& interest);

private:
  ndn::Face m_face;
  ndn::KeyChain m_keyChain;
};

} // namespcace autondn

#endif
