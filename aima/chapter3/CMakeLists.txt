cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/aima/chapter3)
add_executable(c3 chapter3.cpp)
