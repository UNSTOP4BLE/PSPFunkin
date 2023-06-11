cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD           11)
set(CMAKE_CXX_STANDARD         17)
project(cht2bin LANGUAGES CXX)

add_executable(cht2bin cht2bin.cpp)