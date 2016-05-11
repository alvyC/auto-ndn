#include <iostream>
#include "conf-parameter.hpp"

namespace autondn {

  void
  ConfParameter::printConfParameters() {
    std::cout << "Car make: " << m_carMake << std::endl;
    std::cout << "Car model: " << m_carModel << std::endl;
    std::cout << "Car ID: " << m_carId << std::endl;
  }

  ndn::Name
  ConfParameter::getCarName() {
    m_carName =  ndn::Name(m_carMake).append(m_carModel).append(m_carId);
    return m_carName;
  }

} // end of namespace autondnK