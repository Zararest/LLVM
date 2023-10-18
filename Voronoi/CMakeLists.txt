cmake_minimum_required(VERSION 3.7)
project(Voronoi)

find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})

set(CMAKE_C_COMPILER clang)
set(CMAKE_CPP_COMPILER clang++)

include_directories(lib)

add_executable(${PROJECT_NAME} Engine.c
                               lib/Graphics.c)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} m)

set(CMAKE_STATIC_LIBRARY_SUFFIX "")
set(IR_FILE_NAME "Engine-IR.ll")
add_library(IR-dump Engine.c)
set_target_properties(IR-dump PROPERTIES SUFFIX "")
set_target_properties(IR-dump PROPERTIES PREFIX "")
set_target_properties(IR-dump PROPERTIES OUTPUT_NAME ${IR_FILE_NAME})
target_compile_options(IR-dump PUBLIC ${CMAKE_C_FLAGS} -O1 -S -emit-llvm)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${IR_FILE_NAME}"
        DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR})