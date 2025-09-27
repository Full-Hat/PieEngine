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

################################################# Build #################################################

add_library(vk_renderer_lib STATIC)
target_compile_features(vk_renderer_lib PUBLIC cxx_std_23)
target_sources(vk_renderer_lib
        PRIVATE ${NON_TEST_CPP}
        PUBLIC FILE_SET CXX_MODULES
        FILES ${NON_TEST_CPPM}
)

# include vulkan
find_package(Vulkan REQUIRED)
add_library(VulkanCppModule STATIC)
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
        PRIVATE
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
        PRIVATE VulkanCppModule
)

option(vk_renderer_lib_test "Should this CMake build test version" OFF)
if (vk_renderer_lib_test)
    add_library(vk_renderer_lib_test SHARED)
    target_sources(vk_renderer_lib_test
            PRIVATE
            ${ALL_CPP}
            PUBLIC FILE_SET CXX_MODULES
            FILES ${ALL_CPPM}
    )
    target_link_libraries(vk_renderer_lib_test PRIVATE
            spdlog::spdlog
            test_lib
            VulkanCppModule
    )
endif ()

################################################# Install #################################################

install(TARGETS VulkanCppModule
        EXPORT vk_renderer_libInternalTargets
        FILE_SET cxx_modules DESTINATION include
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(EXPORT vk_renderer_libInternalTargets
        FILE vk_renderer_libInternalTargets.cmake
        NAMESPACE vk_renderer_lib::
        DESTINATION lib/cmake/vk_renderer_lib
)

target_include_directories(vk_renderer_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS vk_renderer_lib
        EXPORT vk_renderer_libTargets
        FILE_SET CXX_MODULES DESTINATION include
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

# Export the targets to a CMake package
install(EXPORT vk_renderer_libTargets
        FILE vk_renderer_libTargets.cmake
        NAMESPACE vk_renderer_lib::
        DESTINATION lib/cmake/vk_renderer_lib
)
include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/vk_renderer_libConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/vk_renderer_libConfig.cmake"
        INSTALL_DESTINATION lib/cmake/vk_renderer_lib
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/vk_renderer_libConfig.cmake"
        DESTINATION lib/cmake/vk_renderer_lib
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/vk_renderer_libConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/vk_renderer_libConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/vk_renderer_libConfigVersion.cmake"
        DESTINATION lib/cmake/vk_renderer_lib
)

################################################# Install #################################################

#include(CPack)
#set(CPACK_PACKAGE_NAME "vk_renderer_lib")
#set(CPACK_PACKAGE_VENDOR "PikemenStudio")
#set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vulkan-based rendering library with C++23 support")
#set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
#set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
#set(CPACK_PACKAGE_VERSION_PATCH 0)
#
#set(CPACK_GENERATOR "ZIP")  # Or "TGZ", "DEB", "NSIS", etc.
