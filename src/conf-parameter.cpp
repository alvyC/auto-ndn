#include "conf-parameter.hpp"

#include <iostream>

namespace autondn {

  ConfParameter::ConfParameter(){}

  void
  ConfParameter::printConfParameters() const {
    std::cout << "Car make: " << m_carMake << std::endl;
    std::cout << "Car model: " << m_carModel << std::endl;
    std::cout << "Car ID: " << m_carId << std::endl;
  }

  void
  ConfParameter::buildCarName() {
    m_carName =  ndn::Name(m_carMake).append(m_carModel).append(m_carId);
  }

  ndn::Name&
  ConfParameter::getCarName(){
    return m_carName;
  }

} // end of namespace autondnK
