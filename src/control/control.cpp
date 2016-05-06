#include "control.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/interest.hpp>
#include <unistd.h>
#include <pthread.h>

namespace autondn {

Control::Control( )
{
  loadRoute();

  m_currentRoad = constructRoadName(primary_route[0].first, primary_route[0].second,
                 primary_route[1].first, primary_route[1].second);
  std::cout << "set current road to: " << m_currentRoad << std::endl;
  m_nextRoad = constructRoadName(primary_route[1].first, primary_route[1].second,
              primary_route[2].first, primary_route[2].second);
  std::cout << "set next road to: " << m_nextRoad << std::endl;

  //Mark the road as accessible for now
  roadStatusMap[m_currentRoad] = "Yes";
  roadStatusMap[m_nextRoad] = "Yes";
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

  if( !primary_route_config ) {
    std::cout << "Primary route file does not exists" << std::endl;
    exit(-1);
  }

  if( !alternate_route_config ) {
    std::cout << "Alternate route file does not exists" << std::endl;
    exit(-1);
  }

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

  int roadNum;

  for (; it != primary_route.end(); ++it) {

    std::cout << "Control::current_road: " << m_currentRoad << std::endl;
    std::cout << "Control::next_road: " << m_nextRoad << std::endl;

    int current_x = it->first;
    int current_y = it->second;
    int next_x = (it + 1)->first;
    int next_y = (it + 1)->second;

    std::cout << "Control:: " << m_currentRoad << ": " << roadStatusMap[m_currentRoad] << std::endl;
    if(roadStatusMap[m_currentRoad] == "Yes") {
        // first check whether need to turn or not
        if ((prev_x == current_x && current_x == next_x) ||
            (prev_y == current_y && current_y == next_y)) {
             // no need to turn, go forward
            //m_motion.forward();
            std::cout << "Control::moving forward" << std::endl;
            //sleep 5 seconds
            sleep(5);
        }
        else {
          if (next_x > current_x) {
            // turn right
            //m_motion.turnRight();
            std::cout << "Control::turning right" << std::endl;
            //sleep 5 seconds
            sleep(5);
          }
          else {
            // turn left
            //m_motion.turnLeft();
            std::cout << "Control::turning left" << std::endl;
            //sleep 5 seconds
            sleep(5);
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

    //update roads after motion is done
    //for the first iteration of the loop
    //these are already set in constructor
    m_currentRoad = m_nextRoad;

    next_x = (it+2)->first;
    next_y = (it+2)->second;

    m_nextRoad = constructRoadName(next_x, next_y, (it+3)->first, (it+3)->second);

  } //end of for loop
}

void
Control::runAlternateRoute() {
    auto it = alternate_route.begin();
    int prev_x = it->first;
    int prev_y = it->second;


    std::cout << "alternative route" << std::endl;
/*    for (; it != alternate_route.end(); ++it) {

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
    }*/
}

// for broadcasting
void
Control::passDecisionToCommunication() {
  // ToDO
}

std::string
Control::constructRoadName(int &current_x, int &current_y, int &next_x, int &next_y) const {

 std::string road = std::to_string(current_x);

 road += std::to_string(current_y);
 road += std::to_string(next_x);
 road += std::to_string(next_y);

 return road;
}

void
Control::run() {
//  m_scheduler.scheduleEvent( ndn::time::seconds(3), ndn::bind(&Control::runPrimaryRoute, this) );
  usleep(2000000);
  runPrimaryRoute();
}

void
Control::setRoadStatus( std::string& road, std::string& decision ) {
    roadStatusMap[road] = decision;
    std::cout << "Control:: road: " << road << " status: " << roadStatusMap[road] << std::endl;
}


} //namespace autondn
