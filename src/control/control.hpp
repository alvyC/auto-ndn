#ifndef CONTROL
#define CONTROL

#include <vector>
#include <string>
#include <map>
#include <ndn-cxx/util/scheduler.hpp>
//#include "motion.hpp"

namespace autondn {

class Communication;

class Control {

public:
  Control( ndn::util::Scheduler& );

  std::string
  getNextRoad() const { return m_nextRoad; }

  std::string
  getCurrentRoad() const { return m_currentRoad; }

  void
  setRoadStatus( std::string& road, std::string& decision );

  void
  setCommunication(Communication* comm) {
    m_communication = comm;
  }

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

  void motionStub();

public:
  void
  run();

private:
  std::vector<std::pair <int, int>> primary_route;
  std::vector<std::pair <int, int>> alternate_route;

  std::string m_currentRoad;
  std::string m_nextRoad;
  std::map< std::string, std::string > roadStatusMap;
  Communication* m_communication;
  ndn::util::Scheduler& m_scheduler;
  bool m_isMotionDone = false;
  std::vector<std::pair <int, int>>::iterator m_it, m_alter_it;

  int m_prev_x, m_prev_y, m_current_x, m_current_y, m_next_x, m_next_y;
  //Motion m_motion;
};

} //end of namespace autondn
#endif
