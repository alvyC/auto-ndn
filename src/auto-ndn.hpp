#ifndef AUTO_NDN_HPP
#define AUTO_NDN_HPP

#include "communication/communication.hpp"
#include "control/control.hpp"

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/validator-config.hpp>


#include <boost/property_tree/ptree.hpp>
#include <boost/cstdint.hpp>

#include "conf-parameter.hpp"

namespace autondn {

class AutoNdn {
public:
  AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler, Control* control, Communication* communication);

  ndn::Face&
  getFace() const {
    return m_face;
  }

  Control*
  getControl() const {
    return m_control;
  }

  Communication*
  getCommunication() const {
    return m_communication;
  }

  ConfParameter
  getConfParameter() const {
    return m_confParameter;
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
  initializeKey();

private:
  ndn::Face& m_face;
  ndn::Scheduler& m_scheduler;
  Control* m_control;
  Communication* m_communication;
  ConfParameter m_confParameter;
  ndn::ValidatorConfig m_validator;

  //ndn::KeyChain& m_keyChain;
};

} // end of namespace autondn

#endif //  AUTO_NDN_HPP
