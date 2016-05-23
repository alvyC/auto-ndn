#ifndef AUTO_NDN_HPP
#define AUTO_NDN_HPP

#include <boost/cstdint.hpp>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include "communication/communication.hpp"
#include "control/control.hpp"

namespace autondn {

class AutoNdn {
public:
  AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler, Control* control, Communication* communication);

private:
  ndn::Face& m_face;
  ndn::Scheduler& m_scheduler;
  Control* m_control;
  Communication* m_communication;
  //ndn::KeyChain& m_keyChain;
};

} // end of namespace autondn

#endif //  AUTO_NDN_HPP
