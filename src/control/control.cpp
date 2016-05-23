#include "control.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/interest.hpp>
#include <unistd.h>
#include <pthread.h>
#include "communication.hpp"
#include <ndn-cxx/util/scheduler.hpp>

namespace autondn {

Control::Control( ndn::util::Scheduler& sched )
  : m_scheduler(sched)
{
  loadRoute();

  m_currentRoad = constructRoadName(primary_route[0].first, primary_route[0].second,
                 primary_route[1].first, primary_route[1].second);

  //Mark the first road accessible
  roadStatusMap[m_currentRoad] = "Yes";

  m_it = primary_route.begin();
  m_prev_x = m_it->first;
  m_prev_y = m_it->second;
  m_alter_it = alternate_route.begin();
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
    std::cout << "[control] primary route file does not exists" << std::endl;
    exit(-1);
  }

  if( !alternate_route_config ) {
    std::cout << "[control] alternate route file does not exists" << std::endl;
    exit(-1);
  }

  while (primary_route_config >> x >> y) {
    primary_route.push_back(std::make_pair(x, y));
  }

  while (alternate_route_config >> x >> y) {
    alternate_route.push_back(std::make_pair(x, y));
  }

  alternate_route_config.close();
  primary_route_config.close();
}

// "yes" / "no"
void
Control::runPrimaryRoute() {

    m_current_x = m_it->first;
    m_current_y = m_it->second;
    int m_next_x = (m_it + 1)->first;
    int m_next_y = (m_it + 1)->second;

    m_currentRoad = constructRoadName(m_current_x, m_current_y, m_next_x, m_next_y);
    m_nextRoad = constructRoadName(m_next_x, m_next_y, (m_it+2)->first, (m_it+2)->second);
    //Set next road as yes, if response is negative it will change in onInterest
    roadStatusMap[m_nextRoad] = "Yes";

    std::cout << "[control] current_road: " << m_currentRoad << std::endl;
    std::cout << "[control] next_road: " << m_nextRoad << std::endl;

    if(m_currentRoad == "") {
       std::cout << "[control] Path finished" << std::endl;
       //m_motion.forwardDist(250);
       return;
       //exit(0);
    }

    ndn::Name interestName("/autondn");
    interestName.append(m_nextRoad);

    m_communication->sendInterest( ndn::Interest(interestName) );

    std::cout << "[control] " << m_currentRoad << ": " << roadStatusMap[m_currentRoad] << std::endl;
    //Take alternative route if road status is No
    if(roadStatusMap[m_currentRoad] == "Yes") {
        // first check whether need to turn or not
        if ((m_prev_x == m_current_x && m_current_x == m_next_x) ||
            (m_prev_y == m_current_y && m_current_y == m_next_y)) {
             // no need to turn, go forward
            std::cout << "[control] moving forward" << std::endl;
            //Sleep for 5 seconds even if motion is done before that - will change this later
            m_scheduler.scheduleEvent( ndn::time::seconds(5), ndn::bind(&Control::runPrimaryRoute, this) );
            m_motion.forwardDist(520);
        }
        else {
          if (m_next_x > m_current_x) {
            // turn right
            std::cout << "[control] turning right" << std::endl;
            m_scheduler.scheduleEvent( ndn::time::seconds(5), ndn::bind(&Control::runPrimaryRoute, this) ) ;
            m_motion.turnRight();
          }
          else {
            // turn left
            std::cout << "[control] turning left" << std::endl;
            m_scheduler.scheduleEvent( ndn::time::seconds(5), ndn::bind(&Control::runPrimaryRoute, this) );
            m_motion.turnLeft();
          }
        }
        std::cout << std::endl;
        m_prev_x = m_current_x;
        m_prev_y = m_current_y;
    }
    else {
      // take alternate-route
      runAlternateRoute();
    }

    if(m_it != primary_route.end()) {
       ++m_it;
    } else {
       std::cout << "[control] Path finished" << std::endl;
       //m_motion.forwardDist(250);
       return;
       //exit(0);
    }
}

void
Control::runAlternateRoute() {
  std::cout << "[control] taking alternative route" << std::endl;

  m_current_x = m_alter_it->first;
  m_current_y = m_alter_it->second;
  int m_next_x = (m_alter_it + 1)->first;
  int m_next_y = (m_alter_it + 1)->second;

  m_currentRoad = constructRoadName(m_current_x, m_current_y, m_next_x, m_next_y);
  m_nextRoad = constructRoadName(m_next_x, m_next_y, (m_alter_it+2)->first, (m_alter_it+2)->second);
  //Set next road as yes, if response is negative it will change in onInterest
  roadStatusMap[m_nextRoad] = "Yes";
  roadStatusMap[m_currentRoad] = "Yes";

  std::cout << "[control] current_road: " << m_currentRoad << std::endl;
  std::cout << "[control] next_road: " << m_nextRoad << std::endl;

  if(m_currentRoad == "") {
    std::cout << "[control] Path finished" << std::endl;
    m_motion.forwardDist(100);
    exit(0);
  }

  ndn::Name interestName("/autondn");
  interestName.append(m_nextRoad);

  m_communication->sendInterest( ndn::Interest(interestName) );

  std::cout << "[control] " << m_currentRoad << ": " << roadStatusMap[m_currentRoad] << std::endl;
  //Take alternative route if road status is No
  if(roadStatusMap[m_currentRoad] == "Yes") {
    // first check whether need to turn or not
    if ( (m_prev_x == m_current_x && m_current_x == m_next_x) ||
         (m_prev_y == m_current_y && m_current_y == m_next_y) )
    {
      // no need to turn, go forward
      //m_motion.forward();
      std::cout << "[control] moving forward" << std::endl;
      //This is what motion will do, sleep for 4 seconds then set motion done as true
      m_scheduler.scheduleEvent( ndn::time::seconds(4), ndn::bind(&Control::runAlternateRoute, this) );
      m_motion.forwardDist(450);
    }
    else {
      if (m_next_x > m_current_x) {
        // turn right
        std::cout << "[control] turning right" << std::endl;
        m_scheduler.scheduleEvent( ndn::time::seconds(4), ndn::bind(&Control::runAlternateRoute, this) );
        m_motion.turnRight();
      }
      else {
        // turn left
        std::cout << "[control] turning left" << std::endl;
        m_scheduler.scheduleEvent( ndn::time::seconds(4), ndn::bind(&Control::runAlternateRoute, this) );
        m_motion.turnLeft();
       }
    }
      m_prev_x = m_current_x;
      m_prev_y = m_current_y;
  }  //end of outer if
  else {
    // take alternate-route
    //runAlternateRoute();
  }

  if(m_alter_it != alternate_route.end()) {
    ++m_alter_it;
  } else {
    m_motion.forwardDist(100);
    std::cout << "[control] Path finished" << std::endl;
    exit(0);
  }
}

// for broadcasting
void
Control::passDecisionToCommunication() {
  // ToDO
}

std::string
Control::constructRoadName(int &current_x, int &current_y, int &next_x, int &next_y) const {

  std::cout << "[control] constructing roadname: current_x: " << current_x << ", current_y: " << current_y << ", next_x: " << next_x << ", next_y: "<< next_y << std::endl;
  if(current_x == 0 || current_y == 0 || next_x == 0 || next_y == 0) {
    return "";
  }

 std::string road = std::to_string(current_x);

 road += std::to_string(current_y);
 road += std::to_string(next_x);
 road += std::to_string(next_y);

 return road;
}

void
Control::run() {
  m_scheduler.scheduleEvent( ndn::time::seconds(3), ndn::bind(&Control::runPrimaryRoute, this) );
  //usleep(3000000);
  //runPrimaryRoute();
}

void
Control::setRoadStatus( std::string& road, std::string& decision ) {
    roadStatusMap[road] = decision;
    std::cout << "[control] road: " << road << " status: " << roadStatusMap[road] << std::endl;
}


} //namespace autondn
