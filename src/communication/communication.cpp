#include "communication.hpp"
#include "control/control.hpp"
#include "auto-ndn.hpp"
#include <unistd.h>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-cxx/util/time.hpp>

#include <ctime>
#include <chrono>

namespace autondn {

Communication::Communication(AutoNdn& autondn, ndn::Face& face, Control& cont, ndn::KeyChain& keyChain)
    : m_autondn(autondn)
    , m_face(face)
    , control(cont)
    , m_confParam(m_autondn.getConfParameter())
    , m_keyChain(keyChain)
  {
  }

void
Communication::onInterest(const ndn::InterestFilter& filter, const ndn::Interest& interest) {

  //if interest is coming from this car itself discard
  /*if(interest.getName().getSubName(1,1).toUri() == m_carName) {
    std::cout << "Interest comming from this car itself: " << interest.toUri() <<  std::endl;
    return;
  }*/

  // respond with the road status
  std::cout << "[in-communication] interest received for: " << interest.toUri() <<  std::endl;
  ndn::Name dataName = interest.getName();
  std::cout << dataName.toUri() << std::endl;
  //dataName.append("road-status");
  //dataName.append(interest.getName().getSubName(1, interest.getName().size()));
  dataName.append("V1.Vehicle");
  dataName.append(m_confParam.getCarName());
  std::cout << "Name of the data: " << dataName.toUri() << std::endl;
  const ndn::time::system_clock::TimePoint tp = ndn::time::system_clock::now();
  dataName.appendTimestamp(tp);
  std::cout << "Name of the data: " << dataName.toUri() << std::endl;

  std::string content = "Yes";
  /*/80% of the time answer Yes, that is road is plyable
  if( rand() % 100 < 80 ) {
    content = "Yes"; // get this data from visual module
  } else {
    content = "No";
  }*/
  //Just a test for now
  if(interest.getName().getSubName(1,1).toUri() == "/3%2C3-3%2C4" ||
     interest.getName().getSubName(1,1).toUri() == "/1%2C3-2%2C3" ) {
    content = "No";
  }

  std::shared_ptr<ndn::Data> data = ndn::make_shared<ndn::Data>();
  data->setName(dataName);
  data->setFreshnessPeriod(ndn::time::seconds(10));
  data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

  m_keyChain.sign(*data, m_autondn.getSigningInfo());
  /* ndn::security::SigningInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID,
                                                  m_rootIdentity));*/
  m_face.put(*data);
}

void
Communication::sendInterest(const ndn::Interest& interest) {
  // sends interest
  // schedule interest
  m_face.expressInterest(interest,
                         bind(&Communication::onData, this, _1, _2),
                         bind(&Communication::onTimeout, this, _1));

  std::cout << "[communication] sending interest for: " << interest.toUri() << std::endl;
}

void
Communication::onData(const ndn::Interest& interest, const ndn::Data& data) {
  std::cout << "Interest Name: " << interest.getName() << std::endl;
  std::cout << "[communication] Got data: " << data.getName() << std::endl;
  if (data.getSignature().hasKeyLocator()) {
    if (data.getSignature().getKeyLocator().getType() == ndn::KeyLocator::KeyLocator_Name) {
      std::cout << "Data is signed with " << data.getSignature().getKeyLocator().getName() << std::endl;
    }
  }

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  m_autondn.getValidator().validate(data,
                                    ndn::bind(&Communication::onDataValidated, this, _1),
                                    ndn::bind(&Communication::onValidationFailed, this, _1, _2));
  end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end - start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
            << "elapsed time: " << elapsed_seconds.count() << "s\n";
}

void
Communication::onDataValidated(const std::shared_ptr<const ndn::Data>& data) {
  std::string dataStr(reinterpret_cast<const char*>(data->getContent().value()), data->getContent().value_size());
  std::cout << "Data name: " << data->getName() << std::endl;
  // m_decision = dataStr;
  std::string roadName = (data->getName().getSubName(2, 1)).toUri();
  std::cout << "Road name: " << roadName << std::endl;
  roadName = roadName.substr(1, roadName.length()-1);
  roadName.replace(roadName.find("%2C"),3,",");
  roadName.replace(roadName.find("%2C"),3,",");
  std::cout << "[communication] roadname: " << roadName << std::endl;
  // road-name and status
  control.setRoadStatus(roadName, dataStr);

  //std::cout << "[communication] got Data: " << dataStr << " for Interest: " << interest.toUri() << std::endl;
  std::cout << "Data for road " << roadName << " got validated" << std::endl;
}

void
Communication::onValidationFailed(const std::shared_ptr<const ndn::Data>& data, const std::string& msg) {
  std::cout << data->getName() << " validation failed. " << msg << std::endl;
}

void
Communication::runProducer()
{
  std::cout << "[communication] Starting producer" << std::endl;
  m_face.setInterestFilter(ndn::InterestFilter("/autondn/road-status"),
                           bind(&Communication::onInterest, this, _1, _2),
                           ndn::RegisterPrefixSuccessCallback(),
                           bind(&Communication::onRegisterFailed, this, _1, _2) );

  m_face.processEvents();
}

void
Communication::onRegisterFailed(const ndn::Name& prefix, const std::string& reason) {
  std::cerr << "Error: Failed to register prefix \""
            << prefix << "\" in local hub's daemon (" << reason << ")"
            << std::endl;

  //m_face.shutdown();
}

void
Communication::onTimeout(const ndn::Interest& interest) {
  std::cout << "Timeout " << interest << std::endl;
}

} //namespace autondn
