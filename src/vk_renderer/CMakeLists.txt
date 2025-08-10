cmake_minimum_required(VERSION 3.26.0)

project(test LANGUAGES CXX VERSION 1.0)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake_utils/CMakeUtils.cmake")
pie_collect_sources(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ALL_CPP
        ALL_CPPM
        NON_TEST_CPP
        NON_TEST_CPPM
        "vk_renderer_lib"
)

add_library(vk_renderer_lib STATIC)
target_compile_features(vk_renderer_lib PUBLIC cxx_std_20)
target_sources(vk_renderer_lib
        PRIVATE ${NON_TEST_CPP}
        PUBLIC FILE_SET CXX_MODULES
        FILES ${NON_TEST_CPPM}
)
target_include_directories(vk_renderer_lib INTERFACE include)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(vk_renderer_lib PUBLIC
        spdlog::spdlog
        Vulkan::Vulkan
)