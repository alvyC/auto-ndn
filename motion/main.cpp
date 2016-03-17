#include "motion.h"
#include <wiringPi.h>
#include <stdlib.h>
#include <cstdlib>

int main(int argc, char* argv[]) {
  Motion motion;


  //motion.steerHome();
  motion.steerRight(atoi(argv[1]));

  //motion.fwdDist(300);
  //delay(500);
  //motion.stop();

  //delay(500);
  //motion.steerLeft();

  return 0;
}
