function(imm_get_version _header _major _minor _patch)
    # Read file content
    file(READ ${_header} CONTENT)

    string(REGEX MATCH ".*define IMM_VERSION_MAJOR *([0-9]+)" VERSION_REGEX "${CONTENT}")
    set(${_major} ${CMAKE_MATCH_1} PARENT_SCOPE)

    string(REGEX MATCH ".*define IMM_VERSION_MINOR *([0-9]+)" VERSION_REGEX "${CONTENT}")
    set(${_minor} ${CMAKE_MATCH_1} PARENT_SCOPE)

    string(REGEX MATCH ".*define IMM_VERSION_PATCH *([0-9]+)" VERSION_REGEX "${CONTENT}")
    set(${_patch} ${CMAKE_MATCH_1} PARENT_SCOPE)
endfunction()

function(imm_enable_default_build_type)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "CMAKE_BUILD_TYPE not specified, default is 'Debug'")
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
    else()
        message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
    endif()
endfunction()

function(imm_enable_sanitizer_option)
    option(CLANG_ASAN_UBSAN "Enable Clang address & undefined behavior sanitizer for imm library" OFF)
    if(CLANG_ASAN_UBSAN AND NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Sanitizers are only supported for Clang")
    endif()
endfunction()

function(imm_sanitizer_add_target target)
    if(CLANG_ASAN_UBSAN)
        message(STATUS "Enabling Clang address sanitizer and undefined behavior sanitizer for ${target} target.")
        set_property(TARGET ${target} APPEND PROPERTY COMPILE_DEFINITIONS EXITFREE)
        set_property(TARGET ${target} APPEND PROPERTY COMPILE_OPTIONS -fno-sanitize-recover=all
            -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined)
        set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=address -fsanitize=undefined ")
    endif()
endfunction()

function(imm_enable_coverage_option)
    option(CLANG_CODE_COVERAGE "Enable code coverage metrics in Clang" OFF)
    if (CLANG_CODE_COVERAGE)
        message(STATUS "Code coverage metrics enabled for debug build")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
    endif()
endfunction()

