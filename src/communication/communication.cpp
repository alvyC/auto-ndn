#define BOOST_LOG_DYN_LINK 1

#include "communication.hpp"
#include "control/control.hpp"
#include "auto-ndn.hpp"
#include "logging.hpp"
#include <unistd.h>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include <ndn-cxx/util/time.hpp>

#include <ctime>
#include <chrono>

namespace autondn {

_LOG_INIT(Communication);

Communication::Communication(AutoNdn& autondn, ndn::Face& face, Control& cont, ndn::security::v2::KeyChain& keyChain)
    : m_autondn(autondn)
    , m_face(face)
    , control(cont)
    , m_confParam(m_autondn.getConfParameter())
    , m_keyChain(keyChain)
  {
  }

void
Communication::onInterest(const ndn::InterestFilter& filter, const ndn::Interest& interest) {

  _LOG_DEBUG("Interest received for: " << interest.toUri());

  bool hasMatched = false;
  for(auto road: control.roadsTaken) {
    std::string rName = interest.getName().getSubName(-1).toUri();
    rName = rName.substr(1, rName.length()-1);
    rName.replace(rName.find("%2C"),3,",");
    rName.replace(rName.find("%2C"),3,",");
    if (road == rName) {
      hasMatched = true;
      break;
    }
  }
  if (!hasMatched) {
    _LOG_TRACE("Don't have data");
    return;
  }

  //if interest is coming from this car itself discard
  /*if(interest.getName().getSubName(1,1).toUri() == m_carName) {
    std::cout << "Interest comming from this car itself: " << interest.toUri() <<  std::endl;
    return;
  }*/

  // respond with the road status
  ndn::Name dataName = interest.getName();
  std::cout << dataName.toUri() << std::endl;
  //dataName.append("road-status");
  //dataName.append(interest.getName().getSubName(1, interest.getName().size()));
  dataName.append("V1.Vehicle");
  dataName.append(m_confParam.getCarName());
  const ndn::time::system_clock::TimePoint tp = ndn::time::system_clock::now();
  dataName.appendTimestamp(tp);
  _LOG_DEBUG("Sending data: " << dataName.toUri());

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
  //data->setFreshnessPeriod(ndn::time::seconds(10));
  data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());
  _LOG_DEBUG("Data Sign Start.");
  m_keyChain.sign(*data, m_autondn.getSigningInfo());
  /* ndn::security::SigningInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID,
                                                  m_rootIdentity));*/
   _LOG_DEBUG("Data Sign End. ");
   m_face.put(*data);
}

void
Communication::sendInterest(const ndn::Interest& interest) {
  // sends interest
  // schedule interest
  m_face.expressInterest(interest,
                         bind(&Communication::onData, this, _1, _2),
                         bind(&Communication::onTimeout, this, _1));

  _LOG_DEBUG("Sending interest for: " << interest.toUri());
}

void
Communication::onData(const ndn::Interest& interest, const ndn::Data& data) {
  _LOG_DEBUG("Got data for : " << data.getName() << " for interest " << interest.getName());

  if (data.getSignature().hasKeyLocator()) {
    if (data.getSignature().getKeyLocator().getType() == ndn::KeyLocator::KeyLocator_Name) {
      _LOG_DEBUG("Data is signed with " << data.getSignature().getKeyLocator().getName());
    }
  }

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  m_autondn.getValidator().validate(data,
                                    ndn::bind(&Communication::onDataValidated, this, _1),
                                    ndn::bind(&Communication::onValidationFailed, this, _1, _2));
  end = std::chrono::system_clock::now();

  //std::chrono::duration<double> elapsed_seconds = end - start;
  //std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  //_LOG_DEBUG("finished computation at " << std::ctime(&end_time) << "elapsed time: " << elapsed_seconds.count());
}

void
Communication::onDataValidated(const std::shared_ptr<const ndn::Data>& data) {
  std::string dataStr(reinterpret_cast<const char*>(data->getContent().value()), data->getContent().value_size());
  //_LOG_DEBUG("Data name: " << data->getName() << std::endl;
  // m_decision = dataStr;
  std::string roadName = (data->getName().getSubName(2, 1)).toUri();
  //std::cout << "[communication.onDataValidated] Road name: " << roadName << std::endl;
  roadName = roadName.substr(1, roadName.length()-1);
  roadName.replace(roadName.find("%2C"),3,",");
  roadName.replace(roadName.find("%2C"),3,",");
  //std::cout << "[communication.onDataValidated] roadname: " << roadName << std::endl;
  // road-name and status
  control.setRoadStatus(roadName, dataStr);

  //std::cout << "[communication] got Data: " << dataStr << " for Interest: " << interest.toUri() << std::endl;
  _LOG_DEBUG("Data " << data->getName() << " got validated");
}

void
Communication::onValidationFailed(const std::shared_ptr<const ndn::Data>& data, const std::string& msg) {
  _LOG_DEBUG(data->getName() << " validation failed. " << msg);
}

void
Communication::runProducer()
{
  _LOG_DEBUG("Starting producer");
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
  _LOG_DEBUG("Timeout " << interest);
}

} //namespace autondn
