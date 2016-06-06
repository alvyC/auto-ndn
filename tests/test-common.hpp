#ifndef AUTONDN_TEST_COMMON_HPP
#define AUTONDN_TEST_COMMON_HPP

#include <boost/asio.hpp>

#include <ndn-cxx/util/dummy-client-face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

namespace autondn {

namespace tests {

using ndn::shared_ptr;

class BaseFixture
{
public:
  BaseFixture()
    : t_scheduler(t_ioService)
  {
  }

protected:
  shared_ptr<ndn::util::DummyClientFace> t_face;
  ndn::Scheduler t_scheduler;
  boost::asio::io_service t_ioService;
};

} //end of namespace test
} //end of namespace autondn

#endif
