cmake_minimum_required(VERSION 3.26.0)

project(window LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON) # <- fixes error "module not found"

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake_utils/CMakeUtils.cmake")
pie_collect_sources(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ALL_CPP
    ALL_CPPM
    NON_TEST_CPP
    NON_TEST_CPPM
    "window_lib"
)

message(STATUS "Add window_lib library")
add_library(window_lib STATIC)
target_compile_features(window_lib PUBLIC cxx_std_23)
target_sources(window_lib
    PRIVATE
        ${NON_TEST_CPP}
    PUBLIC FILE_SET CXX_MODULES
        FILES ${NON_TEST_CPPM}
)

find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(window_lib PRIVATE
        glfw
        spdlog::spdlog
        test_lib
)

option(window_lib_test "Should this CMake build test version" ON)
if (window_lib_test)
    add_library(window_lib_test SHARED)
    target_sources(window_lib_test
        PRIVATE
            ${ALL_CPP}
        PUBLIC FILE_SET CXX_MODULES
            FILES ${ALL_CPPM}
    )
    target_link_libraries(window_lib_test PRIVATE
            glfw
            spdlog::spdlog
            test_lib
    )
endif ()
