#!/bin/bash
INCLUDE=""
LIBS="-L/usr/X11R6/lib -L/usr/local/lib "
LIBS_FLAG="-lm -lpthread"
g++ main.cpp -Wall -Wno-unused-result --std=c++17 -O3 $INCLUDE $LIBS $LIBS_FLAG -o vox2obj
echo Done!
exit