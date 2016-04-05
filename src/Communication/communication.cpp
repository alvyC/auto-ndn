#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/time.hpp>

namespace autondn {

class Communication
{
public:
  void
  onInterest(const ndn::Interest& interest) {
    // respond with the road status
    std::cout << interest.toUri() << " received" << std::endl;
    ndn::Name dataName(interest.getName());
    dataName.append("testApp").appendVersion();

    static const std::string content = "Yes"; // get this data from visual module

    std::shared_ptr<ndn::Data> data = ndn::make_shared<ndn::Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(ndn::time::seconds(10));
    data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());
  }

  void
  onData(const ndn::Interest& interest, const ndn::Data& data) {
    // decode data
    // pass to control module
    std::cout << "Got Data: " << data.getName().toUri() << " for Interest: " << interest.toUri() << std::endl;
  }

  void
  sendInterests(const ndn::Interest& interest) {
    // sends interest
    // schedule interest
    m_face.expressInterest(interest,
                           bind(&Communication::onData, this, _1, _2),
                           bind(&Communication::onTimeout, this, _1));

    m_face.processEvents();

    std::cout << interest.toUri() << " sent" << std::endl;
  }

  void
  runProducer()
  {
    // const std::string interestFilterString = "/example/testApp";
    /*m_face.setInterestFilter(ndn::InterestFilter("/example/testApp"),
                             std::bind(&Communication::onInterest, this, _1),
                             ndn::RegisterPrefixSuccessCallback(),
                             std::bind(&Communication::onRegisterFailed, this, _1, _2));*/

    m_face.setInterestFilter(ndn::InterestFilter("/example/testApp"),
                             std::bind(&Communication::onInterest, this, _1)
                            );

     /*m_face.setInterestFilter(ndn::InterestFilter("/Hello/World"),
                            std::bind([] { }),
                            std::bind([] { }),
                            std::bind([] { })
                           );*/

   // m_face.processEvents();
  }

  void onRegisterFailed(const ndn::Name& prefix, const std::string& reason) {
    std::cerr << "Error: Failed to register prefix \""
              << prefix << "\" in local hub's daemon (" << reason << ")"
              << std::endl;

    m_face.shutdown();
  }

  void
  onTimeout(const ndn::Interest& interest) {
    std::cout << "Timeout " << interest << std::endl;
  }

private:
  ndn::Face m_face;
  ndn::KeyChain m_keyChain;
};

} // namespcace autondn

int main(int argc, char** argv)
{
  autondn::Communication communication;
  try {
    communication.runProducer();
    communication.sendInterests(ndn::Interest(ndn::Name("/autondn/state1/city1/road2/car1/")));
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}