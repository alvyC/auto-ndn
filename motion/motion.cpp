#include "motion.h"
#include <iostream>

//i2c constants
#define PCA_ADDRESS 40

Motion::Motion()
 : m_PWM(PCA_ADDRESS)
{
   m_PWM.begin();
   setSpeed(2000);
   setup();
}

void
Motion::setSpeed(int speed) {
  if( speed > 0 && speed < 4096 ) {
    m_speed = speed;
    m_PWM.setPWM(m_EN_M0, 0, m_speed);    //50*40, 0-4095 value
    m_PWM.setPWM(m_EN_M1, 0, m_speed);
  }
}

int
Motion::getSpeed() {
   return m_speed;
}

//need to modify to incorporate
void
Motion::forward() {
  //motor 0
  digitalWrite(m_Motor0_A, LOW);
  digitalWrite(m_Motor0_B, HIGH);

  //motor 1
  digitalWrite(m_Motor1_A, HIGH);
  digitalWrite(m_Motor1_B, LOW);
}

void
Motion::stop() {
  digitalWrite(m_Motor0_B, LOW);
  digitalWrite(m_Motor1_A, LOW);
}

void
Motion::turnWheelsLeft() {
  std::cout << "Turning wheels left" << std::endl;
  m_PWM.setPWM(m_dirServo, 0, 375);
}

void
Motion::setup() {
  //wiringPiSetup();    //Not sure what happens if in some other class someone would call this again
  pinMode (m_Motor0_A, OUTPUT);
  pinMode (m_Motor0_B, OUTPUT);
  pinMode (m_Motor1_A, OUTPUT);
  pinMode (m_Motor1_B, OUTPUT);

  m_PWM.setPWM(m_EN_M0, m_speed, 0);
  m_PWM.setPWM(m_EN_M1, m_speed, 0);
}
