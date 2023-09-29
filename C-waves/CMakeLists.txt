cmake_minimum_required(VERSION 3.7)
project(Waves)

find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})

include_directories(include)

add_executable(SDL2Test test.c)
target_link_libraries(SDL2Test ${SDL2_LIBRARIES})

add_executable(${PROJECT_NAME} main.c
                               lib/Engine.c
                               lib/Graphics.c)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} m)