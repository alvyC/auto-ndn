#ifndef CONTROL
#define CONTROL

//#include "communication/communication.hpp"
//#include "../"
#include <vector>
#include <string>

#include "calibration/motion.h"
#include "communication.hpp"

namespace autondn {

class Control
{
public:
  Control(Communication& communication, Motion& motion);

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

public:
  void
  run();

private:
  std::vector<std::pair <int, int>> primary_route;
  std::vector<std::pair <int, int>> alternate_route;
  Communication& m_communication;
  Motion& m_motion;
};

} //end of namespace autondn
#endif
