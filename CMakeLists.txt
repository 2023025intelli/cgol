cmake_minimum_required(VERSION 3.20)
project(cgol C)

set(CMAKE_C_STANDARD 99)

set(SRC main.c)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})