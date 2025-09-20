cmake --preset pc-debug && \
cmake --build build-debug && \
env/bin/python buildassets.py && \
./build-debug/main
