#include <wiringPi.h>
#include "pca9685/Adafruit_PWMServoDriver.h"
#include <iostream>

//i2c constants
#define PCA_ADDRESS 40

class Motion {

public:
  Motion()
    : m_PWM(PCA_ADDRESS)
    {
      m_PWM.begin();
      setSpeed(2000);
      setup();
    }

  void
  setSpeed(int speed) {
    if( speed > 0 && speed < 4096 ) {
      m_speed = speed;
      m_PWM.setPWM(m_EN_M0, 0, m_speed);    //50*40, 0-4095 value
      m_PWM.setPWM(m_EN_M1, 0, m_speed);
    }
  }

  int
  getSpeed() {
    return m_speed;
  }

  //need to modify to incorporate
  void
  forward() {
    //motor 0
    digitalWrite(m_Motor0_A, LOW);
    digitalWrite(m_Motor0_B, HIGH);

    //motor 1
    digitalWrite(m_Motor1_A, HIGH);
    digitalWrite(m_Motor1_B, LOW);
  }

  void
  stop() {
    digitalWrite(m_Motor0_B, LOW);
    digitalWrite(m_Motor1_A, LOW);
  }

  void
  turnWheelsLeft() {
    std::cout << "Turning wheels left" << std::endl;
    m_PWM.setPWM(m_dirServo, 0, 375);
  }

private:

  void
  setup() {
    //wiringPiSetup();    //Not sure what happens if in some other class someone would call this again
    pinMode (m_Motor0_A, OUTPUT);
    pinMode (m_Motor0_B, OUTPUT);
    pinMode (m_Motor1_A, OUTPUT);
    pinMode (m_Motor1_B, OUTPUT);

    m_PWM.setPWM(m_EN_M0, m_speed, 0);
    m_PWM.setPWM(m_EN_M1, m_speed, 0);
  }

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
