cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/aima/chapter7)
add_executable(c7 agent.cpp c7.cpp map.cpp)
