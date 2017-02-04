#include <ndn-cxx/name.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include "control.hpp"
#include "communication/communication.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

namespace autondn {

Control::Control( ndn::util::Scheduler& sched, Communication& comm)
  : m_scheduler(sched)
  , m_communication(comm)
{
  std::string start = "1,1";
  std::string end = "4,4";
  m_map.calculatePath(start, end);
  m_path = m_map.getPath();

  m_currentRoad = m_path[0].first + "-" + m_path[1].first;

  //Mark the first road accessible
  roadStatusMap[m_currentRoad] = "Yes";

  m_prev_x = getCoordinate(m_path[0].first, 0); // get the x coordinate
  m_prev_y = getCoordinate(m_path[0].first, 1); // get the y coordinate
  //std::cout << m_prev_x << " " <<m_prev_y << std::endl;
}

int
Control::getCoordinate(std::string s, int axis) {
  int x = 0, y = 0;
  x = std::stoi(s.substr(0, s.find(",")));
  s.erase(0, s.find(",") + 1);
  y = std::stoi(s);
  return (axis == 0) ? x:y;
}

// "yes" / "no"
void
Control::runPrimaryRoute() {

    if(m_roadTracker+1 == m_path.size()) {
      std::cout << "The end" << std::endl;
      return;
    }

    m_current_x = getCoordinate(m_path[m_roadTracker].first, 0);
    m_current_y = getCoordinate(m_path[m_roadTracker].first, 1);
    m_next_x = getCoordinate(m_path[m_roadTracker+1].first, 0);
    m_next_y = getCoordinate(m_path[m_roadTracker+1].first, 1);

    std::cout << m_current_x << "," << m_current_y << std::endl;
    std::cout << m_next_x << "," << m_next_y << std::endl;

    m_currentRoad = m_path[m_roadTracker].first + "-" + m_path[m_roadTracker+1].first;
    if(m_roadTracker + 2 < m_path.size()) {
      m_nextRoad = m_path[m_roadTracker+1].first + "-" + m_path[m_roadTracker+2].first;
    } else {
      m_nextRoad = "";
    }

    //Set next road as yes, if response is negative it will change in onInterest
    roadStatusMap[m_nextRoad] = "Yes";

    std::cout << "[control] current_road: " << m_currentRoad << std::endl;
    std::cout << "[control] next_road: " << m_nextRoad << std::endl;

    if(m_nextRoad != "") {
      ndn::Name interestName("/autondn/road-status");
      interestName.append(m_nextRoad);

      m_communication.sendInterest(ndn::Interest(interestName));
    }

    std::cout << "[control] " << m_currentRoad << ": " << roadStatusMap[m_currentRoad] << std::endl;
    //Take alternative route if road status is No
    if(roadStatusMap[m_currentRoad] == "Yes") {
        // first check whether need to turn or not
        if ((m_prev_x == m_current_x && m_current_x == m_next_x) ||
            (m_prev_y == m_current_y && m_current_y == m_next_y)) {
             // no need to turn, go forward
            std::cout << "[control] moving forward: " << m_path[m_roadTracker].second << std::endl;
            //Sleep for 5 seconds even if motion is done before that - will change this later
            m_scheduler.scheduleEvent( ndn::time::seconds(5), ndn::bind(&Control::runPrimaryRoute, this) );
            //m_motion.forwardDist(500);
        }
        else {
          if (m_next_x > m_current_x) {
            // turn right
            std::cout << "[control] turning right: " << m_path[m_roadTracker].second << std::endl;
            m_scheduler.scheduleEvent( ndn::time::seconds(5), ndn::bind(&Control::runPrimaryRoute, this) ) ;
            //m_motion.turnRight();
          }
          else {
            // turn left
            std::cout << "[control] turning left: " << m_path[m_roadTracker].second << std::endl;
            m_scheduler.scheduleEvent( ndn::time::seconds(5), ndn::bind(&Control::runPrimaryRoute, this) );
            //m_motion.turnLeft();
          }
        }
        std::cout << std::endl;
        m_prev_x = m_current_x;
        m_prev_y = m_current_y;

      if(m_roadTracker == m_path.size()) {
         std::cout << "[control] 2 Path finished" << std::endl;
         exit(0);
      }
      ++m_roadTracker;
    }
    else {
      std::cout << "[control] Changing route due to congestion on " << m_currentRoad << std::endl;

      std::cout << m_path[m_roadTracker].first << "," << m_path[m_roadTracker+1].first << std::endl;
      //Remove next road
      m_map.removeLink(m_path[m_roadTracker].first, m_path[m_roadTracker+1].first);
      //Calculate dijkstra from next coordinate to final
      std::string end = "4,4";
      m_map.calculatePath(m_path[m_roadTracker].first, end);
      //update m_path
      m_path = m_map.getPath();
      m_roadTracker = 0; //reset roadtracker for new path
      m_currentRoad = m_path[0].first + "-" + m_path[1].first;

      //Mark the first road accessible
      roadStatusMap[m_currentRoad] = "Yes";

      m_scheduler.scheduleEvent( ndn::time::seconds(5), ndn::bind(&Control::runPrimaryRoute, this) ) ;
    }
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
