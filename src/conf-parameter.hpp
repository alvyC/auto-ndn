#ifndef CONF_PARAMETER_HPP
#define CONF_PARAMETER_HPP

#include <ndn-cxx/name.hpp>

namespace autondn {

class ConfParameter {
public:
  ConfParameter();

  void
  setNetwork(ndn::Name network) {
    m_network = network;
  }

  const ndn::Name&
  getNetwork() const {
    return m_network;
  }

  void
  setCarMake(ndn::Name carMake) {
    m_carMake = carMake;
  }

  const ndn::Name&
  getCarMake() const {
    return m_carMake;
  }

  void
  setCarId(ndn::Name carId) {
    m_carId = carId;
  }

  const ndn::Name&
  getCarId() const {
    return m_carId;
  }

  void
  printConfParameters() const;

  ndn::Name&
  getCarName();

  void
  buildCarName();

private:
    ndn::Name m_network;
    ndn::Name m_carMake;
    ndn::Name m_carId;
    ndn::Name m_carName;
};

} // end of namespace autondn

#endif //  CONF_PARAMETER_HPP
