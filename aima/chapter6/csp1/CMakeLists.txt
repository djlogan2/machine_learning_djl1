cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/aima/chapter6/csp1)
add_executable(csp3 ac3.cpp backtrack.cpp csp3.cpp var.cpp)
