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
    m_control->run();

    try {
      m_communication->runProducer();
    }
    catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
} // end of namespace autondn