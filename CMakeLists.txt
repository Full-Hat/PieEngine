cmake_minimum_required(VERSION 3.5.0)

project(tutorial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_GENERATOR Ninja)

add_executable(exe main.cpp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(exe PRIVATE glfw)