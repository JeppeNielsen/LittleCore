cmake_minimum_required(VERSION 4.0)
project(Source)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Source
        main_new.cpp
        SimpleSimulation.cpp
        SimpleSimulation.hpp
        TestRendering.cpp
        TestRendering.hpp)
