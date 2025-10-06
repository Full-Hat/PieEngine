cmake_minimum_required(VERSION 3.26.0)

project(test LANGUAGES CXX VERSION 1.0)

add_compile_definitions(VULKAN_HPP_NO_STRUCT_CONSTRUCTORS) # <- must be added before vulkan.cppm compilation to bmi

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake_utils/CMakeUtils.cmake")
pie_collect_sources(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ALL_CPP
        ALL_CPPM
        NON_TEST_CPP
        NON_TEST_CPPM
        "vk_renderer_lib"
)
# Vulkan.cppm is public and should be exported (installed), remove from other non-public modules
pie_list_remove_by_regex(NON_TEST_CPPM ".*Vulkan\.cppm")
pie_list_remove_by_regex(ALL_CPPM ".*Vulkan\.cppm")

find_package(Vulkan REQUIRED)
add_library(vk_renderer_lib STATIC)
target_include_directories(vk_renderer_lib
        PRIVATE
        "${Vulkan_INCLUDE_DIR}"
)
target_compile_features(vk_renderer_lib PUBLIC cxx_std_23)
target_sources(vk_renderer_lib
        PRIVATE ${NON_TEST_CPP}
        PUBLIC FILE_SET CXX_MODULES
            FILES "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan.cppm"
        PRIVATE FILE_SET nonExported TYPE CXX_MODULES BASE_DIRS "${Vulkan_INCLUDE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
            FILES ${NON_TEST_CPPM} "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
)

# link libs
target_compile_definitions(vk_renderer_lib PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)
find_package(spdlog CONFIG REQUIRED)
set_target_properties(vk_renderer_lib PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "$<$<PLATFORM_ID:Darwin>:@executable_path/;@loader_path/>;$<$<PLATFORM_ID:Linux>:$ORIGIN/>"
)
target_link_libraries(vk_renderer_lib
        PUBLIC spdlog::spdlog
        PRIVATE Vulkan::Vulkan
)

if (Pie_ENABLE_TESTING_PACK)
    add_library(vk_renderer_lib_test STATIC)
    target_sources(vk_renderer_lib_test
            PRIVATE ${ALL_CPP}
            PUBLIC FILE_SET CXX_MODULES
            FILES "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan.cppm"
            PRIVATE FILE_SET nonExported TYPE CXX_MODULES BASE_DIRS "${Vulkan_INCLUDE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
            FILES ${ALL_CPPM} "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
    )
    set_target_properties(vk_renderer_lib_test PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "$<$<PLATFORM_ID:Darwin>:@executable_path/;@loader_path/>;$<$<PLATFORM_ID:Linux>:$ORIGIN/>"
    )
    target_link_libraries(vk_renderer_lib_test
            PUBLIC spdlog::spdlog
                   test_lib
            PRIVATE Vulkan::Vulkan
    )
endif ()
