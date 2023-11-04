cmake_minimum_required(VERSION 3.7)
project(IR-gen)

set(CMAKE_C_COMPILER clang-12)
set(CMAKE_CPP_COMPILER clang++-12)
set (CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(Voronoi-IR lib/Voronoi-IR.cpp)