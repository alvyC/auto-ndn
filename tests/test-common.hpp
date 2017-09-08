#ifndef AUTONDN_TEST_COMMON_HPP
#define AUTONDN_TEST_COMMON_HPP

#include "common.hpp"
#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>
#include <ndn-cxx/util/scheduler.hpp>
#include <ndn-cxx/util/time-unit-test-clock.hpp>

namespace autondn {

namespace tests {

using ndn::shared_ptr;

class BaseFixture
{
public:
  BaseFixture()
    : steadyClock(std::make_shared<ndn::time::UnitTestSteadyClock>())
    , systemClock(std::make_shared<ndn::time::UnitTestSystemClock>())
    , g_scheduler(g_ioService)
  {
    ndn::time::setCustomClocks(steadyClock, systemClock);
  }

void
advanceClocks(const ndn::time::nanoseconds& tick, size_t nTicks = 1)
{
  for (size_t i = 0; i < nTicks; ++i) {
    steadyClock->advance(tick);
    systemClock->advance(tick);

    if (g_ioService.stopped()) {
      g_ioService.reset();
    }

    g_ioService.poll();
  }
}

protected:
  std::shared_ptr<ndn::time::UnitTestSteadyClock> steadyClock;
  std::shared_ptr<ndn::time::UnitTestSystemClock> systemClock;

  boost::asio::io_service g_ioService;
  ndn::Scheduler g_scheduler;
  ndn::KeyChain g_keychain;
};

} //end of namespace test
} //end of namespace autondn

#endif
