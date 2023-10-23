cmake_minimum_required(VERSION 3.7)
project(Voronoi)

find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})

set(CMAKE_C_COMPILER clang)
set(CMAKE_CPP_COMPILER clang++)

include_directories(lib)

# Main target
find_package(SDL2 REQUIRED)
add_executable(${PROJECT_NAME} Engine.c
                               lib/Graphics.c)
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
add_library(Profiler Profiler-pass.cpp)
target_include_directories(Profiler PUBLIC ${LLVM_INCLUDE_DIRS})
target_compile_definitions(Profiler PUBLIC ${LLVM_DEFINITIONS})
set_target_properties(Profiler PROPERTIES POSITION_INDEPENDENT_CODE True)
llvm_map_components_to_libnames(llvm_libs support)
target_link_libraries(Profiler ${llvm_libs})