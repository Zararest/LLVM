cmake_minimum_required(VERSION 3.7)
project(Assembler)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CPP_COMPILER clang++)
set (CMAKE_CXX_STANDARD 17)

include_directories(./include)

find_package(LLVM REQUIRED CONFIG)
find_package(SDL2 REQUIRED)

add_executable(Voronoi-IR lib/Voronoi-IR.cpp lib/Graphics.cpp)
target_include_directories(Voronoi-IR PUBLIC ${LLVM_INCLUDE_DIRS})
target_compile_definitions(Voronoi-IR PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(Voronoi-IR LLVM)
target_link_libraries(Voronoi-IR ${SDL2_LIBRARIES})
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(Voronoi-IR ${llvm_libs})

add_executable(${PROJECT_NAME} main.cpp lib/Translator.cpp 
                                        lib/Utils.cpp 
                                        lib/Graphics.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${LLVM_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${LLVM_DEFINITIONS})
target_link_libraries(${PROJECT_NAME} LLVM)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(${PROJECT_NAME} ${llvm_libs})
