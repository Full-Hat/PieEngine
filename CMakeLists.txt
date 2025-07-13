cmake_minimum_required(VERSION 3.5.0)

project(tutorial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON) # <- fixes error "module not found"

message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

set(ENGINE_SOURCES
    window.cpp
)

# Main executable
add_executable(exe)

target_sources(exe PRIVATE
        main.cpp
        ${ENGINE_SOURCES}
)
file(GLOB MODULES *.cppm)
message(STATUS "Found Modules: ${MODULES}")
target_sources(exe PUBLIC
        FILE_SET CXX_MODULES
        FILES ${MODULES}
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(exe PRIVATE glfw)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(exe PRIVATE spdlog::spdlog)

# Test executable
enable_testing()
add_executable(test_exe)

target_sources(test_exe PRIVATE
        test_main.cpp
        ${ENGINE_SOURCES}
)

# Include all modules for test executable
file(GLOB ALL_MODULES *.cppm)
target_sources(test_exe PUBLIC
        FILE_SET CXX_MODULES
        FILES ${ALL_MODULES}
)

target_link_libraries(test_exe PRIVATE glfw)
target_link_libraries(test_exe PRIVATE spdlog::spdlog)

add_test(NAME global_test COMMAND test_exe)
