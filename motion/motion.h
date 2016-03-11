#include <wiringPi.h>
#include "pca9685/Adafruit_PWMServoDriver.h"

//i2c constants
#define PCA_ADDRESS 40

class Motion {

public:
  Motion();

  void setSpeed(int speed);
  int getSpeed();
  void forward();
  void stop();
  void turnWheelsLeft();

private:
  void setup();

private:
  Adafruit_PWMServoDriver m_PWM;
  //Should all these be declared static
  //Raspberry Pi pins connected to L298N driver
  int m_Motor0_A = 0;  //11 in GPIO python, do gpio readall
  int m_Motor0_B = 1;  //12
  int m_Motor1_A = 2;  //13
  int m_Motor1_B = 3;  //15

  // Not any pins on Raspberry Pi, pin numbers that we need to send from RPI to
  // PCA driver via i2C, will generate PWM for L298N
  uint8_t m_EN_M0 = 4;  //servo driver IC CH4, not any pins on the PI
  uint8_t m_EN_M1 = 5;  //servo driver IC CH5

  //For controlling the servo that controls direction
  uint8_t m_dirServo = 0;  //servo driver IC CH0

  int m_speed;
};
