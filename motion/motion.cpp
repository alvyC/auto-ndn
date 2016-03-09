#include "motion.h"
#include <wiringPi.h>

int main() {
  Motion motion;

  //motion.forward();
  //delay(500);
  //motion.stop();

  motion.turnWheelsLeft();

  return 0;
}
