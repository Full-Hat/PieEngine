cmake_minimum_required(VERSION 3.5.0)

project(tutorial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON) # <- fixes error "module not found"

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

if(APPLE)
    set(SCRIPT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/remove_from_malware_list.command)

    file(WRITE ${SCRIPT_PATH} "#!/usr/bin/env bash\n")
    file(APPEND ${SCRIPT_PATH} "xattr -d com.apple.quarantine exe")
endif()

find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

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

target_link_libraries(exe PRIVATE glfw spdlog::spdlog)

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

target_link_libraries(test_exe PRIVATE glfw spdlog::spdlog)

add_test(NAME global_test COMMAND test_exe)
