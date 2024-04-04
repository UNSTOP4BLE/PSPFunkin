#!/bin/bash
cmake -S ./cht2bin -B ./cht2bin/build
cmake --build ./cht2bin/build
cmake --preset "psp-release"
cmake --build ./build
./build_chart.sh
