//#include "motion.h"
#include "communication.hpp"
#include <unistd.h>

namespace autondn {

Communication::Communication( Control& cont, ndn::Face& face, ndn::util::Scheduler& sched )
   : m_face(face), m_scheduler(sched), control(cont), m_nextRoad(control.getCurrentRoad())
{
  m_decision = "Yes";
}

void
Communication::onInterest(const ndn::InterestFilter& filter, const ndn::Interest& interest) {

  // respond with the road status
  std::cout << "Interest received for: " <<interest.toUri() <<  std::endl;
  ndn::Name dataName(interest.getName());

  static const std::string content = "Yes"; // get this data from visual module

  std::shared_ptr<ndn::Data> data = ndn::make_shared<ndn::Data>();
  data->setName(dataName);
  data->setFreshnessPeriod(ndn::time::seconds(10));
  data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

  m_keyChain.sign(*data);

  m_face.put(*data);
}

void
Communication::onData(const ndn::Interest& interest, const ndn::Data& data) {
  // decode data
  // pass to control module
  std::string dataStr(reinterpret_cast<const char*>(data.getContent().value()), data.getContent().value_size());
  m_decision = dataStr;

  std::string roadName = data.getName().getSubName(1, 1).toUri();
  roadName = roadName.substr(1, 4);
  std::cout << "Communication:: roadname: " << roadName << std::endl;
  //roadname and status
  control.setRoadStatus(roadName, dataStr);

  std::cout << "Got Data: " << dataStr << " for Interest: " << interest.toUri() << std::endl;
}

void
Communication::sendInterest(const ndn::Interest& interest) {
  // sends interest
  // schedule interest
  m_face.expressInterest(interest,
                         bind(&Communication::onData, this, _1, _2),
                         bind(&Communication::onTimeout, this, _1));


  std::cout << "Sending interest for: " << interest.toUri() << std::endl;
}

void
Communication::runProducer()
{
  m_face.setInterestFilter(ndn::InterestFilter("/autondn"),
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

void
Communication::startInfoRequest(){
  std::cout << "Communication:: run" << std::endl;
  std::cout << "Communication::nextRoad: " << m_nextRoad << std::endl;
  std::cout << "Communication::current road: " << control.getCurrentRoad() << std::endl;

  while(control.getNextRoad() != "") {
    //std::cout << "Communication:: getNextRoad: " << control.getNextRoad() << std::endl;
    if(m_nextRoad != control.getCurrentRoad()) {
      if( !m_interestSent ) {
        m_interestSent = true;

        ndn::Name interestName("/autondn");
        interestName.append(control.getNextRoad());

        sendInterest( ndn::Interest(interestName) );
      } //end of inner if
    }
    else {
      m_nextRoad = control.getNextRoad();
      m_interestSent = false;
    }
    usleep(10000);
  } //end of while
} //end of startReqInfo

void
Communication::run(){
//  m_scheduler.scheduleEvent( ndn::time::seconds(2), ndn::bind(&Communication::startInfoRequest, this) );
   usleep(3000000);
   startInfoRequest();
}

} //namespace autondn
