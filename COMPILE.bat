@echo off
g++ main.cpp -static-libgcc -static-libstdc++ -Wall -Wno-unused-result --std=c++17 -O3 -o vox2obj.exe
echo Done!