cmake_minimum_required(VERSION 3.26.0)

project(tutorial VERSION 1.0.0)

option(Pie_ENABLE_TESTING "Should CMake compiles test binary and libs as well" ON)
################################################# Build #################################################

set(CMAKE_CXX_STANDARD 23)
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

find_package(spdlog CONFIG REQUIRED)

set(Pie_ENABLE_TESTING_PACK 1)

add_subdirectory(src/test)
add_subdirectory(src/window)
add_subdirectory(src/vk_renderer)

# Main executable
add_executable(exe)
target_compile_features(exe PUBLIC cxx_std_23)

target_sources(exe
    PRIVATE
        main.cpp
)

target_link_libraries(exe PRIVATE
        spdlog::spdlog
        window_lib
        vk_renderer_lib
)

# Test executable
enable_testing()
add_executable(test_exe)

target_sources(test_exe
    PRIVATE
        test_main.cpp
        ${NON_TEST_CPP_UNITS}
    PUBLIC
        FILE_SET CXX_MODULES
        FILES ${NON_TEST_MODULE_UNITS}
)

target_link_libraries(test_exe PRIVATE
        spdlog::spdlog
        window_lib_test
        test_lib
)

add_test(NAME global_test COMMAND test_exe)

################################################# Install #################################################

include(GNUInstallDirs)

set(CMAKE_INSTALL_BINDIR "${CMAKE_SOURCE_DIR}/pack/bin")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_SOURCE_DIR}/pack/lib")
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_SOURCE_DIR}/pack/include")

# add targets to the build tree
message(STATUS "[Make installation rule]: export exe (runtime), window_lib, vk_renderer_lib as PieTargets.
Installation directory:
runtime:${CMAKE_INSTALL_BINDIR}
static: ${CMAKE_INSTALL_LIBDIR}
shared: ${CMAKE_INSTALL_LIBDIR}
interface modules: ${CMAKE_INSTALL_INCLUDEDIR}/pie")
install(
        TARGETS exe window_lib vk_renderer_lib
        EXPORT PieTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pie
)
message(STATUS "[Make installation rule]: export test_exe (runtime), window_lib_test, vk_renderer_lib_test, test_lib as PieTargets.
Installation directory:
runtime:${CMAKE_INSTALL_BINDIR}
static: ${CMAKE_INSTALL_LIBDIR}
shared: ${CMAKE_INSTALL_LIBDIR}
interface modules: ${CMAKE_INSTALL_INCLUDEDIR}/pie")
if (Pie_ENABLE_TESTING_PACK) # we’ll set this cache var from CPack
    install(
            TARGETS test_lib window_lib_test vk_renderer_lib_test test_exe # TODO: why there is no EXPORT
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pie
    )
endif()

# export targets so that find_package(Pie) works for consumers
install(EXPORT PieTargets
        NAMESPACE Pie::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Pie
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/PieConfigVersion.cmake
        COMPATIBILITY SameMajorVersion)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/PieConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/PieConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Pie) # TODO: why we still add install FILES if we set INSTALL_DESTINATION

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/PieConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/PieConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Pie)

################################################# CPack #################################################

include(CPack)

# Common metadata
set(CPACK_PACKAGE_NAME "PieEngine")
set(CPACK_PACKAGE_VENDOR "Artem Voron")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE") TODO: add license

# Two flavours: runtime and full-test
set(CPACK_ARCHIVE_RUNTIME_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-runtime")
set(CPACK_ARCHIVE_TEST_FILE_NAME    "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-full")

# default generator
set(CPACK_GENERATOR "ZIP;TGZ")

# Runtime archive: no tests
add_custom_target(package-runtime
        COMMAND ${CMAKE_COMMAND}
        -DPie_ENABLE_TESTING_PACK:BOOL=OFF
        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package
        COMMENT "Building runtime package"
)

# Full package: includes tests
add_custom_target(package-full
        COMMAND ${CMAKE_COMMAND}
        -DPie_ENABLE_TESTING_PACK:BOOL=ON
        -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package
        COMMENT "Building full (with tests) package"
)