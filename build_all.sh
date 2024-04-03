export PSPDEV=/usr/local/pspdev
export PATH=$PATH:$PSPDEV/bin
cmake -S . -B ./build -DCMAKE_TOOLCHAIN_FILE="${PSPDEV}/psp/share/pspdev.cmake"
cmake --build ./build
./build_chart.sh
