#include "auto-ndn.hpp"

namespace autondn {

AutoNdn::AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler, Control* control, Communication* communication)
  : m_face(face)
  , m_scheduler(scheduler)
  , m_control(control)
  , m_communication(communication)
  {
    m_control->setCommunication(communication);
  }
} // end of namespace autondn