#!/bin/bash
cmake -S ./cht2bin -B ./cht2bin/build
cmake --build ./cht2bin/build
cmake --preset "psp-debug"
cmake --build ./build
./build_chart.sh
