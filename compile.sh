#!/bin/bash

if [ $# -lt 1 ]; then
        echo "Usage: $0 <test.cpp>"
        exit
fi

out=`echo "$1" | cut -d '.' -f 1`
g++ "$1" -o "$out.o" --std=c++11 -lboost_system -lndn-cxx -lopencv_imgcodecs -lopencv_core
