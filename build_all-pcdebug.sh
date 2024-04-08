#!/bin/bash
cmake -S ./cht2bin -B ./cht2bin/build
cmake --build ./cht2bin/build
cmake --preset "pc-debug"
cmake --build ./build-debug
./build_chart.sh
