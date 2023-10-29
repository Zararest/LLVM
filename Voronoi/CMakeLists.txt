cmake_minimum_required(VERSION 3.7)
project(Voronoi)

find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})

set(CMAKE_C_COMPILER clang-12)
set(CMAKE_CPP_COMPILER clang++-12)
set (CMAKE_CXX_STANDARD 17)

include_directories(lib include)

# Utils
add_library(Utils STATIC Utils.cpp)

# Main target
message("Building files for target: ${PROJECT_NAME}")
find_package(SDL2 REQUIRED)
add_executable(${PROJECT_NAME} Engine.c lib/Graphics.c)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} m)

# IR dump
set(IR_FILE_NAME "Engine-IR.ll")
add_library(IR-dump Engine.c)
set_target_properties(IR-dump PROPERTIES SUFFIX "")
set_target_properties(IR-dump PROPERTIES PREFIX "")
set_target_properties(IR-dump PROPERTIES OUTPUT_NAME ${IR_FILE_NAME})
target_compile_options(IR-dump PUBLIC ${CMAKE_C_FLAGS} -O1 -S -emit-llvm)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${IR_FILE_NAME}"
        DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR})

# Profiler
find_package(LLVM REQUIRED CONFIG)
add_library(Profiler SHARED Profiler-pass.cpp)
# FIXME: strange dependecies 
target_include_directories(Profiler PUBLIC /usr/lib/llvm-12/include)
target_compile_definitions(Profiler PUBLIC ${LLVM_DEFINITIONS})
set_target_properties(Profiler PROPERTIES POSITION_INDEPENDENT_CODE True)

# IR with profiler
find_package(SDL2 REQUIRED)
add_executable(${PROJECT_NAME}-profiled Engine.c lib/Graphics.c Logger.c)
target_compile_options(${PROJECT_NAME}-profiled PUBLIC -O2)
target_link_libraries(${PROJECT_NAME}-profiled ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME}-profiled m)
add_dependencies(${PROJECT_NAME}-profiled Profiler)
message("Current coompiler ${CMAKE_C_COMPILER}")
# I don't know how to get target path right
# As long as I use clang-12, no additional options are needed
target_compile_options(${PROJECT_NAME}-profiled PUBLIC "-fplugin=${CMAKE_CURRENT_BINARY_DIR}/libProfiler.so")

# Trace analyzer
add_executable(Analyzer Trace-analyzer.cpp)
target_include_directories(Analyzer PUBLIC ${LLVM_INCLUDE_DIRS})
target_compile_definitions(Analyzer PUBLIC ${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(Analyzer ${llvm_libs} Utils)