cmake_minimum_required(VERSION 3.6)
project(cse434Project1Part2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(CLIENT_SOURCE_FILES client/client.cpp)
set(SERVER_SOURCE_FILES server/server.cpp)

add_executable(cse434Project1Part2Client ${CLIENT_SOURCE_FILES})
add_executable(cse434Project1Part2Server ${SERVER_SOURCE_FILES})
