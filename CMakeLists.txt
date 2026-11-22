cmake_minimum_required(VERSION 3.20)
project(gare C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# Library with the bus logic
add_library(bus bus.c)
target_include_directories(bus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Docker (spawns buses + register)
add_executable(docker docker.c)
target_link_libraries(docker PRIVATE bus Threads::Threads)

# Screen
add_executable(screen screen.c)

# Controller (reads the register)
add_executable(controller controller.c)
target_link_libraries(controller PRIVATE bus Threads::Threads)
