cmake_minimum_required(VERSION 3.6)
project(cse434Project1Part2Client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES client/client.cpp)
add_executable(cse434Project1Part2Client ${SOURCE_FILES})