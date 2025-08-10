function(pie_collect_sources
        TARGET_DIR
        OUT_ALL_CPP
        OUT_ALL_CPPM
        OUT_NON_TEST_CPP
        OUT_NON_TEST_CPPM
        DEBUG_NAME
)

    # Find all .cpp and .cppm recursively
    file(GLOB_RECURSE ALL_SRC
            "${TARGET_DIR}/*.cpp"
            "${TARGET_DIR}/*.cppm"
    )

    message(STATUS "[Pie] All sources: ${ALL_SRC}")

    # Filter out test files
    set(SRC_WITHOUT_TESTS "${ALL_SRC}")
    list(FILTER SRC_WITHOUT_TESTS EXCLUDE REGEX ".*[Tt][Ee][Ss][Tt].*\\.(cpp|cppm)$")

    # Optional: allow exclusion/inclusion lists (via cache or parent set)
    set(PE_EXCLUDE_RELEASE ${PE_EXCLUDE_RELEASE} CACHE STRING "")
    set(PE_INCLUDE_RELEASE ${PE_INCLUDE_RELEASE} CACHE STRING "")

    foreach(f IN LISTS PE_EXCLUDE_RELEASE)
        list(REMOVE_ITEM ALL_SRC "${f}")
    endforeach()
    foreach(f IN LISTS PE_INCLUDE_RELEASE)
        list(APPEND ALL_SRC "${f}")
    endforeach()

    # Separate .cpp and .cppm lists
    set(ALL_CPP "")
    set(ALL_CPPM "")
    foreach(f IN LISTS ALL_SRC)
        if(f MATCHES "\\.cpp$")
            list(APPEND ALL_CPP "${f}")
        elseif(f MATCHES "\\.cppm$")
            list(APPEND ALL_CPPM "${f}")
        endif()
    endforeach()

    set(NON_TEST_CPP "")
    set(NON_TEST_CPPM "")
    foreach(f IN LISTS SRC_WITHOUT_TESTS)
        if(f MATCHES "\\.cpp$")
            list(APPEND NON_TEST_CPP "${f}")
        elseif(f MATCHES "\\.cppm$")
            list(APPEND NON_TEST_CPPM "${f}")
        endif()
    endforeach()

    # Print info
    message(STATUS "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
    message(STATUS "[${DEBUG_NAME}] Manually excluded files: ${PE_EXCLUDE_RELEASE}")
    message(STATUS "[${DEBUG_NAME}] Manually included files: ${PE_INCLUDE_RELEASE}")
    message(STATUS "[${DEBUG_NAME}] Non-test .cppm files: ${NON_TEST_CPPM}")
    message(STATUS "[${DEBUG_NAME}] Non-test .cpp files: ${NON_TEST_CPP}")
    message(STATUS "[${DEBUG_NAME}] All .cppm files: ${ALL_CPPM}")
    message(STATUS "[${DEBUG_NAME}] All .cpp files: ${ALL_CPP}")
    message(STATUS "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")

    # Return via output variables
    set(${OUT_ALL_CPP} ${ALL_CPP} PARENT_SCOPE)
    set(${OUT_ALL_CPPM} ${ALL_CPPM} PARENT_SCOPE)
    set(${OUT_NON_TEST_CPP} ${NON_TEST_CPP} PARENT_SCOPE)
    set(${OUT_NON_TEST_CPPM} ${NON_TEST_CPPM} PARENT_SCOPE)
endfunction()
