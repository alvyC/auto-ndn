#ifndef CONTROL
#define CONTROL

#include "communication.hpp"
#include <vector>

using namespace std;

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
  run(Communication& communication);

private:
  string m_decision;
  vector<pair <int, int> > route;
  vector<pair <int, int> > alternate_route;

};

} //end of namespace autondn
#endif
