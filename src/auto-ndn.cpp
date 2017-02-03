#include <ndn-cxx/util/time.hpp>

#include "auto-ndn.hpp"

namespace autondn {

AutoNdn::AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler)
  : m_face(face)
  , m_scheduler(scheduler)
  , m_control(scheduler, m_communication)
  , m_communication(*this, m_face, m_control, m_keyChain)
  , m_confParameter()
  , m_validator(m_face)
  {
  }

  void
  AutoNdn::run() {
    std::cout << "In AutoNdn::run() " << std::endl;
    initialize();
    m_control.run();

    try {
      m_communication.runProducer();
    }
    catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  void
  AutoNdn::initializeKey() {
    m_keyChain.addIdentity(m_confParameter.getCarName());
  }

  void
  AutoNdn::initialize() {
    m_confParameter.buildCarName();
    std::cout << m_confParameter.getCarName() << std::endl;
    initializeKey();
  }
} // end of namespace autondn
