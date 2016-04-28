#include "control.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/interest.hpp>

namespace autondn {

Control::Control(Communication &communication, Motion &motion)
  : m_communication(communication)
  , m_motion(motion)
{
}

void
Control::getDataFromSensor() {

}

void
Control::loadRoute() {
  std::ifstream primary_route_config;
  std::ifstream alternate_route_config;
  int x, y;

  primary_route_config.open("primary_route_config");
  alternate_route_config.open("alternate_route_config");

  while (primary_route_config >> x >> y) {
    primary_route.push_back(std::make_pair(x, y));
  }

  while (alternate_route_config >> x >> y) {
    alternate_route.push_back(std::make_pair(x, y));
  }

  /*for (vector<pair <int, int> >::iterator it = route.begin(); it != route.end(); ++it) {
    cout << it->first << ", " << it->second << endl;
  }*/

  alternate_route_config.close();
  primary_route_config.close();
}

// "yes" / "no"
void
Control::runPrimaryRoute() {
  auto it = primary_route.begin();
  int prev_x = it->first;
  int prev_y = it->second;

  for (; it != primary_route.end(); ++it) {

    int current_x = it->first;
    int current_y = it->second;
    int next_x = (it + 1)->first;
    int next_y = (it + 1)->second;

    std::string road = std::to_string(current_x);
    //road += "/";
    road += std::to_string(current_y);
    //road += "/";
    road += std::to_string(next_x);
    //road += "/";
    road += std::to_string(next_y);
    std::cout << road << std::endl;

    ndn::Name roadName(road);
    ndn::Name interestName("/autondn");
    interestName.append(roadName);

    // make interest with the next road name
    ndn::Interest interest(interestName);

    //interest.setInterestLifetime(ndn::time::milliseconds(10000));
    //interest.setMustBeFresh(true);

    // send interest for the next road
    m_communication.sendInterests(interestName);

    // broadcasting data for the current
    //communication.sendData();

    if (m_communication.getDecision() == "Yes")  {  
        // first check whether need to turn or not
        if ((prev_x == current_x && current_x == next_x) ||
            (prev_y == current_y && current_y == next_y)) {
             // no need to turn, go forward
            m_motion.forward();
        }
        else {
          if (next_x > current_x) {
            // turn right
            m_motion.turnRight();
          }
          else {
            // turn left
            m_motion.turnLeft();
          }
        }
        prev_x = current_x;
        prev_y = current_y;
    }
    else {
      runAlternateRoute();
      // take alternate-route
      break;
    }
  }
}

void
Control::runAlternateRoute() {
    auto it = alternate_route.begin();
    int prev_x = it->first;
    int prev_y = it->second;

    for (; it != alternate_route.end(); ++it) {

    int current_x = it->first;
    int current_y = it->second;
    int next_x = (it + 1)->first;
    int next_y = (it + 1)->second;

    std::string road = std::to_string(current_x);
    //road += "/";
    road += std::to_string(current_y);
    //road += "/";
    road += std::to_string(next_x);
    //road += "/";
    road += std::to_string(next_y);
    //std::cout << road << std::endl;
    
    ndn::Name roadName(road);
    ndn::Name interestName("/autondn");
    interestName.append(roadName);
    
    // make interest with the next road name
    ndn::Interest interest(interestName);
    
    // send interest for the next road
    m_communication.sendInterests(interestName);

    // broadcasting data for the current
    //communication.sendData();

    // first check whether need to turn or not
      if ((prev_x == current_x && current_x == next_x) ||
        (prev_y == current_y && current_y == next_y)) {
         // go forward
        m_motion.forward();
      }
      else {
        if (next_x > current_x) {
          // turn right
          m_motion.turnRight();
        }
        else {
          // turn left
          m_motion.turnLeft();
        }
      }
      prev_x = current_x;
      prev_y = current_y;
    }
}

// for broadcasting
void
Control::passDecisionToCommunication() {
  // ToDO
}

void
Control::run() {
  loadRoute();
  runPrimaryRoute();
}

} //namespace autondn
