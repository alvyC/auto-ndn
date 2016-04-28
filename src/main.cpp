#include "communication/communication.hpp"
#include "control/control.hpp"
#include "calibration/motion.h"


int main(){
  //Face face;
  //KeyChain keyChain;
  autondn::Communication communication;
  Motion motion;
  autondn::Control control(communication, motion);
  control.run();
  return 0;
}