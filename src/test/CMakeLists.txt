cmake_minimum_required(VERSION 3.26.0)

project(test LANGUAGES CXX VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON) # <- fixes error "module not found"

add_library(test_lib STATIC)
target_compile_features(test_lib PUBLIC cxx_std_20)
target_sources(test_lib
    PUBLIC FILE_SET CXX_MODULES
        FILES include/test_utils.cppm
)
target_include_directories(test_lib INTERFACE include
)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(test_lib PUBLIC
        spdlog::spdlog
)
