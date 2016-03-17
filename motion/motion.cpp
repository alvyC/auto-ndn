#include "motion.h"
#include <iostream>
#include <math.h>
#include <unistd.h>

using namespace std;

Motion::Motion()
 : m_PWM(PCA_ADDRESS)
{
   //m_PWM.begin();
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
Motion::fwdDist(int dist) {
  forward();
  double rps = (20.0/6.0)/(4096.0/m_speed);  //180-200 rev/min = (200)/60 rev/sec, 4096 - pwm high, m_speed
  double diameter = 65; //mm
  double numRev = dist/(diameter*M_PI);  //calculate the number of revolutions needed
  double t = numRev/rps;
  cout << t << endl;
  usleep(t*1000000);
  stop();
}

void
Motion::stop() {
  digitalWrite(m_Motor0_B, LOW);
  digitalWrite(m_Motor1_A, LOW);
}

void Motion::steerHome(){
  m_PWM.setPWM(m_dirServo, 0, homePWM);
}

void
Motion::steerLeft() {
  m_PWM.setPWM(m_dirServo, 0, leftPWM);
}

void
Motion::steerRight(int pwm) {
  //m_PWM.setPWM(m_dirServo, 0, pwm);
  for (uint16_t pulselen = 160; pulselen < 600; pulselen++) {
    m_PWM.setPWM(m_dirServo, 0, pulselen);
  }
}

void
Motion::setup() {
  //Important statement
  //I think if not set it defaults to a higher rate
  //good enough to fry the servo
  m_PWM.setPWMFreq(60);

  //wiringPiSetup();    //already declared in adafruit library
  pinMode (m_Motor0_A, OUTPUT);
  pinMode (m_Motor0_B, OUTPUT);
  pinMode (m_Motor1_A, OUTPUT);
  pinMode (m_Motor1_B, OUTPUT);

  m_PWM.setPWM(m_EN_M0, m_speed, 0);
  m_PWM.setPWM(m_EN_M1, m_speed, 0);
}
