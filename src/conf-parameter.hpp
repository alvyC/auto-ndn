#ifndef CONF_PARAMETER_HPP
#define CONF_PARAMETER_HPP

#include <ndn-cxx/name.hpp>

namespace autondn {

class ConfParameter { 
public:
  ConfParameter();

  void
  setCarModel(std::string carModel) {
    m_carModel = carModel;
  }

  std::string
  getCarModel() const {
    return m_carModel;
  }

  void
  setCarMake(std::string carMake) {
    m_carMake = carMake;
  }

  std::string
  getCarMake() const {
    return m_carMake;
  }

  void
  setCarId(std::string carId) {
    m_carId = carId;
  }

  std::string
  getCarId() const {
    return m_carId;
  }  

  void
  printConfParameters();

  ndn::Name
  getCarName();

private:
    std::string m_carMake;
    std::string m_carModel;
    std::string m_carId;
    ndn::Name m_carName;
};

} // end of namespace autondn

#endif //  CONF_PARAMETER_HPP