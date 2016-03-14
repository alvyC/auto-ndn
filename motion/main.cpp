#include "motion.h"
#include <wiringPi.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  Motion motion;

  //motion.steerHome();
  //motion.steerLeft();

  motion.fwdDist(300);
  //delay(500);
  //motion.stop();

  //delay(500);
  //motion.steerLeft();

  return 0;
}
