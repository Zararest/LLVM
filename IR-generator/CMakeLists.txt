cmake_minimum_required(VERSION 3.7)
project(IR-gen)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CPP_COMPILER clang++)
set (CMAKE_CXX_STANDARD 17)

include_directories(include)

find_package(LLVM REQUIRED CONFIG)
add_executable(Voronoi-IR lib/Voronoi-IR.cpp)
target_include_directories(Voronoi-IR PUBLIC ${LLVM_INCLUDE_DIRS})
target_compile_definitions(Voronoi-IR PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(Voronoi-IR LLVM-12)