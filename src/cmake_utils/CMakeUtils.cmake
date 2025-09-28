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
    pie_list_remove_by_regex(SRC_WITHOUT_TESTS ".*[Tt][Ee][Ss][Tt].*\\.(cpp|cppm)$" "${DEBUG_NAME}")

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

# pie_list_remove_by_regex(<listVar> <regex> [debugName])
#
# Removes every element of the list variable <listVar> that matches <regex>.
# <debugName> is an optional string shown in the status message.
function(pie_list_remove_by_regex LISTVAR REGEX)
    set(debugName "")
    if (ARGC GREATER 2)
        set(debugName " (${ARGV2})")
    endif()

    # Remove matching elements in-place
    list(FILTER ${LISTVAR} EXCLUDE REGEX "${REGEX}")

    message(STATUS "[Pie] [${debugName}] Removed by regex: ${REGEX}")
    set(${LISTVAR} "${${LISTVAR}}" PARENT_SCOPE)
endfunction()


# pie_list_add_items(<listVar> <items...> [debugName])
#
# Appends all given <items> to <listVar> if they are not already present.
function(pie_list_add_items LISTVAR)
    set(debugName "")
    set(args ${ARGV})
    list(REMOVE_AT args 0) # drop the list variable name
    if (args AND args MATCHES ";") # last arg might be debug string
        list(GET args -1 maybe_debug)
        if (NOT EXISTS "${maybe_debug}" AND NOT IS_ABSOLUTE "${maybe_debug}")
            # crude heuristic: if the last argument is not a file path,
            # treat it as the optional debug name
            list(POP_BACK args debugName)
            set(debugName " (${debugName})")
        endif()
    endif()

    list(APPEND ${LISTVAR} ${args})
    list(REMOVE_DUPLICATES ${LISTVAR})

    message(STATUS "[Pie] [${debugName}] Added items: ${args}")
    set(${LISTVAR} "${${LISTVAR}}" PARENT_SCOPE)
endfunction()

# pie_list_move_by_regex(<srcListVar> <dstListVar> <regex> [debugName])
#
# Finds all elements in <srcListVar> matching <regex>, appends them to
# <dstListVar> (without duplicates), and removes them from <srcListVar>.
#
# Example:
#   pie_list_move_by_regex(ALL_SRC PRIVATE_SRC ".*internal.*" "move-internal")
#
function(pie_list_move_by_regex SRCVAR DSTVAR REGEX)
    set(debugName "")
    if (ARGC GREATER 3)
        set(debugName " (${ARGV3})")
    endif()

    set(matches "")
    foreach(item IN LISTS ${SRCVAR})
        if (item MATCHES "${REGEX}")
            list(APPEND matches "${item}")
        endif()
    endforeach()

    if (matches)
        # Add matches to destination, remove duplicates
        list(APPEND ${DSTVAR} ${matches})
        list(REMOVE_DUPLICATES ${DSTVAR})

        # Remove them from the source
        list(REMOVE_ITEM ${SRCVAR} ${matches})

        message(STATUS "[Pie] [${debugName}] Moved ${matches} by regex")
    endif()

    # Update parent scope
    set(${SRCVAR} "${${SRCVAR}}" PARENT_SCOPE)
    set(${DSTVAR} "${${DSTVAR}}" PARENT_SCOPE)
endfunction()
