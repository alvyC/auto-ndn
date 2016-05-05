#ifndef CONTROL
#define CONTROL

#include <vector>
#include <string>
#include <map>
#include <ndn-cxx/util/scheduler.hpp>

namespace autondn {

class Control
{
public:
  Control( ndn::util::Scheduler& );

  std::string
  getNextRoad() const { return m_nextRoad; }

  std::string
  getCurrentRoad() const { return m_currentRoad; }

  void
  setRoadStatus( std::string& road, std::string& decision );

private:
  void
  getDataFromSensor();

  void
  loadRoute();

  void
  runPrimaryRoute();

  void
  runAlternateRoute();

  // for broadcasting
  void
  passDecisionToCommunication();

  std::string
  constructRoadName( int& , int& , int& , int& ) const;

public:
  void
  run();

private:
  std::vector<std::pair <int, int>> primary_route;
  std::vector<std::pair <int, int>> alternate_route;

  std::string m_currentRoad;
  std::string m_nextRoad;
  std::map< std::string, std::string > roadStatusMap;

  ndn::util::Scheduler& m_scheduler;
};

} //end of namespace autondn
#endif
