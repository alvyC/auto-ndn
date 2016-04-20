#ifndef CONTROL
#define CONTROL

#include "communication.hpp"

namespace autondn {

class Control
{
public:
  void
  getDataFromSensor();

  void
  loadMap();

  // "yes" / "no"
  void
  passDecisionToCalibration(Communication& communication);

  // for broadcasting
  void
  passDecisionToCommunication();

  void
  run(Communicaton& communicaton);

private:
  std::string m_decision;
  vector<pair <int, int> > route;
  vector<pair <int, int> > alternate_route;

};

#endif
