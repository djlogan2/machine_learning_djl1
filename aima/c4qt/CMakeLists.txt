cmake_minimum_required(VERSION 3.7)

project(c4qt)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(SOURCES lrtastar.cpp map.cpp main.cpp mainwindow.cpp onlinedfs.cpp)

# Find the QtWidgets library
find_package(Qt5Widgets)

# Tell CMake to create the helloworld executable
add_executable(c4qt ${SOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries(c4qt Qt5::Widgets)