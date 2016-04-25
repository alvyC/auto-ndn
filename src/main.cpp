#include <iostream>
#include <fstream>
#include <string>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/util/time.hpp>

#include "motion.h"

std::vector<std::pair <int, int>> route;
std::vector<std::pair <int, int>> alternate_route;
Motion motion;
std::string decision;
ndn::Face face;
ndn::KeyChain keyChain;

void loadRoute() {
  std::ifstream route_config;
  std::ifstream alternate_route_config;
  int x, y;

  route_config.open("route_config");


  while (route_config >> x >> y) {
    route.push_back(std::make_pair(x, y));
  }

  while (route_config >> x >> y) {
    alternate_route.push_back(std::make_pair(x, y));
  }

  /*for (vector<pair <int, int> >::iterator it = route.begin(); it != route.end(); ++it) {
    cout << it->first << ", " << it->second << endl;
  }*/

  alternate_route_config.close();
  route_config.close();
}

void
onInterest(const ndn::Interest& interest) {
  // respond with the road status
  std::cout << interest.toUri() << " received" << std::endl;
  ndn::Name dataName(interest.getName());
  dataName.append("testApp").appendVersion();

  static const std::string content = "Yes"; // get this data from visual module

  std::shared_ptr<ndn::Data> data = ndn::make_shared<ndn::Data>();
  data->setName(dataName);
  data->setFreshnessPeriod(ndn::time::seconds(10));
  data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());
}

void
onData(const ndn::Interest& interest, const ndn::Data& data) {
  // decode data
  // pass to control module
  std::string dataStr(reinterpret_cast<const char*>(data.getContent().value()), data.getContent().value_size());
  decision = "Yes";
  std::cout << "Got Data: " << data.getName().toUri() << " for Interest: " << interest.toUri() << std::endl;
}

void
onTimeout(const ndn::Interest& interest) {
  std::cout << "Timeout " << interest << std::endl;
}

void
sendInterests(const ndn::Interest& interest) {
  // sends interest
  // schedule interest
  face.expressInterest(interest,
                         bind(onData, _1, _2),
                         bind(onTimeout, _1));

  face.processEvents();

  std::cout << interest.toUri() << " sent" << std::endl;
}

void
onRegisterFailed(const ndn::Name& prefix, const std::string& reason) {
  std::cerr << "Error: Failed to register prefix \""
            << prefix << "\" in local hub's daemon (" << reason << ")"
            << std::endl;

  face.shutdown();
}

void
runAlternateRoute() {
  auto it = route.begin();
  int prev_x = it->first;
  int prev_y = it->second;

  for (; it != route.end(); ++it) {

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

    // send interest for the next road
    sendInterests(interestName);

    // broadcasting data for the current
    //communication.sendData();
    if ((prev_x == current_x && current_x == next_x) ||
            (prev_y == current_y && current_y == next_y)) {
             // no need to turn, go forward
      motion.forward();
    }
    else {
      if (next_x > current_x) {
        // turn right
        motion.turnRight();
      }
      else {
        // turn left
        motion.turnLeft();
      }
    }
    prev_x = current_x;
    prev_y = current_y;
  }
}

void
runPrimaryRoute() {
  auto it = route.begin();
  int prev_x = it->first;
  int prev_y = it->second;

  for (; it != route.end(); ++it) {

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

    // send interest for the next road
    sendInterests(interestName);

    // broadcasting data for the current
    //communication.sendData();

    if (decision == "Yes")  {
        // first check whether need to turn or not
        if ((prev_x == current_x && current_x == next_x) ||
            (prev_y == current_y && current_y == next_y)) {
             // no need to turn, go forward
            motion.forward();
        }
        else {
          if (next_x > current_x) {
            // turn right
            motion.turnRight();
          }
          else {
            // turn left
            motion.turnLeft();
          }
        }
        prev_x = current_x;
        prev_y = current_y;
    }
    else {
      runAlternateRoute();
      break;
    }
  }
}

void run() {
  loadRoute();
  runPrimaryRoute();
}

int main() {
  run();
}
