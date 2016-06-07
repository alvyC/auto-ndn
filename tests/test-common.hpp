#ifndef AUTONDN_TEST_COMMON_HPP
#define AUTONDN_TEST_COMMON_HPP

#include "common.hpp"
#include <boost/asio.hpp>

#include <ndn-cxx/util/scheduler.hpp>

namespace autondn {

namespace tests {

using ndn::shared_ptr;

class BaseFixture
{
public:
  BaseFixture()
    : g_scheduler(g_ioService)
  {
  }

protected:
  boost::asio::io_service g_ioService;
  ndn::Scheduler g_scheduler;
};

} //end of namespace test
} //end of namespace autondn

#endif
