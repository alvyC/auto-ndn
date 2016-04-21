#include "control.hpp"
#include "communication.hpp"
#include <fstream>

namespace autondn {

void
Control::getDataFromSensor() {

}

void
Control::loadMap() {
  ifstream route_config;
  ifstream alternate_route_config;
  int x, y;

  route_config.open("route_config");


  while (route_config >> x >> y) {
    route.push_back(make_pair(x, y));
  }

  while (route_config >> x >> y) {
    alternate_route.push_back(make_pair(x, y));
  }

  /*for (vector<pair <int, int> >::iterator it = route.begin(); it != route.end(); ++it) {
    cout << it->first << ", " << it->second << endl;
  }*/

  alternate_route_config.close();
  route_config.close();
}

// "yes" / "no"
void
Control::passDecisionToCalibration(Communication& communication) {
  auto it = route.begin();
  int prev_x = it->first;
  int prev_y = it->second;

  for (; it != route.end(); ++it) {

    int current_x = it->first;
    int current_y = it->second;
    int next_x = (it + 1)->first;
    int next_y = (it + 1)->second;

    // make interest with the next road name
    // communication.sendInterests();

    // first check whether need to turn or not
    if ((prev_x == current_x && current_x == next_x) ||
        (prev_y == current_y && current_y == next_y)) {
         // go forward
    }
    else {
      if (next_x > current_x) {
        // turn right
      }
      else {
        // turn left
      }
    }
    prev_x = current_x;
    prev_y = current_y;
  }
}

// for broadcasting
void
Control::passDecisionToCommunication() {

}

void
Control::run(Communication& communication) {
  loadMap();
  // start running
  passDecisionToCalibration(communication);
}

} //namespace autondn
