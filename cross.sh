#!/bin/bash

#export PATH=/home/mini-ndn/pi/x-tools/arm-unknown-linux-gnueabihf/bin:$PATH
export PATH=/home/mini-ndn/pi/x-tools/arm-bcm2708-linux-gnueabihf/bin:$PATH

#arch="arm-unknown-linux-gnueabihf"
arch="arm-bcm2708-linux-gnueabihf"

export AR="${arch}-ar"
export AS="${arch}-as"
export LD="${arch}-ld"
export CC="${arch}-cc"
export CXX="${arch}-c++"
export RANLIB="${arch}-ranlib"

export CFLAGS="-O2 -std=gnu99 -I/home/mini-ndn/pi/pi/include -L/home/mini-ndn/pi/pi/lib"
export CXXFLAGS="-O2 -g -std=c++11 -I/home/mini-ndn/pi/pi/include -L/home/mini-ndn/pi/pi/lib"
export LDFLAGS="-Wl,-rpath=/home/mini-ndn/pi/pi/lib -L/home/mini-ndn/pi/pi/lib"
export PKG_CONFIG_PATH="/home/mini-ndn/pi/pi/lib/pkgconfig"
#echo $PKG_CONFIG_PATH

./waf configure --prefix=/home/mini-ndn/pi/pi --boost-includes=/home/mini-ndn/pi/pi/include --boost-libs=/home/mini-ndn/pi/pi/lib

if [ $? = 0 ]
then
    ./waf clean
    ./waf
fi

