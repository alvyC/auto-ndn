#!/bin/bash

export PATH=/home/mini-ndn/pi/x-tools/arm-bcm2708-linux-gnueabihf/bin:$PATH

arch="arm-bcm2708-linux-gnueabihf"

"$arch-g++" main.cpp motion.cpp pca9685/Adafruit_PWMServoDriver.cpp -lwiringPi -std=c++11 -I/home/mini-ndn/pi/pi/include -L/home/mini-ndn/pi/pi/lib

#"$arch-g++" -c motion.cpp pca9685/Adafruit_PWMServoDriver.cpp -lwiringPi -std=c++11 -I/home/mini-ndn/pi/pi/include -L/home/mini-ndn/pi/pi/lib
