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
target_compile_features(vk_renderer_lib PUBLIC cxx_std_23)
target_sources(vk_renderer_lib
        PRIVATE ${NON_TEST_CPP}
        PUBLIC FILE_SET CXX_MODULES
        FILES ${NON_TEST_CPPM}
)

# include vulkan
find_package(Vulkan REQUIRED)
add_library(VulkanCppModule)
add_library(Vulkan::cppm ALIAS VulkanCppModule)

target_compile_definitions(VulkanCppModule PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)
target_include_directories(VulkanCppModule
        PRIVATE
        "${Vulkan_INCLUDE_DIR}"
)
target_link_libraries(VulkanCppModule
        PUBLIC
        Vulkan::Vulkan
)
set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 23)
target_sources(VulkanCppModule
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS
        "${Vulkan_INCLUDE_DIR}"
        FILES
        "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
)

# link to lib
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(vk_renderer_lib PUBLIC
        spdlog::spdlog
        VulkanCppModule
)

option(vk_renderer_lib_test "Should this CMake build test version" ON)
if (vk_renderer_lib_test)
    add_library(vk_renderer_lib_test SHARED)
    target_sources(vk_renderer_lib_test
            PRIVATE
            ${ALL_CPP}
            PUBLIC FILE_SET CXX_MODULES
            FILES ${ALL_CPPM}
    )
    target_link_libraries(vk_renderer_lib PRIVATE
            spdlog::spdlog
            test_lib
            VulkanCppModule
    )
endif ()