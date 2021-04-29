cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(imm VERSION 0.0.10 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# Set RPATH only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

# merge request fix: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/1799
if(CMAKE_VERSION VERSION_GREATER 3.12.0)
    # Hide things by default
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif()

find_package(logaddexp REQUIRED)
find_package(elapsed REQUIRED)
find_package(containers REQUIRED)

add_library(imm
    src/abc.c
    src/abc_lprob.c
    src/amino.c
    src/cartes.c
    src/common/error.c
    src/dp/bitarr.c
    src/dp/code.c
    src/dp/dp.c
    src/dp/emission.c
    src/dp/eseq.c
    src/dp/matrix.c
    src/dp/path.c
    src/dp/state_table.c
    src/dp/trans_table.c
    src/hmm.c
    src/log.c
    src/lprob.c
    src/mute_state.c
    src/normal_state.c
    src/path.c
    src/seq.c
    src/state.c
    src/task.c
    src/tsort.c
)
add_library(IMM::imm ALIAS imm)

# Write out mytools_export.h to the current binary directory
# Must include ${CMAKE_CURRENT_BINARY_DIR} in the target's
# search path.
include(GenerateExportHeader)
generate_export_header(imm BASE_NAME IMM
    INCLUDE_GUARD_NAME IMM_EXPORT_H
    EXPORT_MACRO_NAME IMM_API
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/imm/export.h)

if(${CMAKE_C_COMPILER_ID} MATCHES "MSVC")
    set(WARNING_FLAGS /W3)
else()
    set(WARNING_FLAGS -Wall -Wextra -pedantic -Wstrict-prototypes -Wshadow
        -Wconversion -Wmissing-prototypes -Wno-unused-parameter -Wsign-conversion
        -Wno-gnu-statement-expression
        -Wno-unused-function # FIXME: added only to please clangd for now
        -Wno-empty-translation-unit # FIXME: added only to please clangd for now
    )
endif()

option(CI_BUILD "CI, extra flags will be set" OFF)

if(CI_BUILD)
    message(STATUS "CI build enabled")
    add_compile_options(-Werror)
endif()

option(CLANG_ASAN_UBSAN "Enable Clang address & undefined behavior sanitizer for nvim binary." OFF)
option(CLANG_MSAN "Enable Clang memory sanitizer for nvim binary." OFF)

if((CLANG_ASAN_UBSAN AND CLANG_MSAN))
  message(FATAL_ERROR "Sanitizers cannot be enabled simultaneously")
endif()

if((CLANG_ASAN_UBSAN OR CLANG_MSAN) AND NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "Sanitizers are only supported for Clang")
endif()

option(IMM_DOUBLE_PRECISION "Enable double-precision floating-point format for internals" Off)
if(IMM_DOUBLE_PRECISION)
    message(STATUS "Double-precision floating-point format for internals - On")
else()
    message(STATUS "Double-precision floating-point format for internals - Off")
endif()

target_include_directories(imm
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(imm PUBLIC LOGADDEXP::logaddexp)
target_link_libraries(imm PUBLIC ELAPSED::elapsed)
target_link_libraries(imm PUBLIC CONTAINERS::containers)
set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
target_compile_features(imm PRIVATE c_std_11)
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:__extension__>)
target_compile_options(imm PRIVATE ${WARNING_FLAGS})
target_compile_options(imm PRIVATE "-fverbose-asm")
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/imm/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/imm/config.h @ONLY)

include(CheckCSourceCompiles)
check_c_source_compiles(
    "
        static void f(char const* err, ...) __attribute__((format(printf, 1, 2)));
        int main(void) {return 0;}
    "
    HAVE_ATTR_FMT
)
target_compile_definitions(imm PRIVATE $<$<BOOL:${HAVE_ATTR_FMT}>:HAVE_ATTR_FMT>)

include(CheckTypeSize)

check_type_size("int" SIZEOF_INT)
if(NOT HAVE_SIZEOF_INT)
    message(FATAL_ERROR "Could not determine the size of int.")
else()
    message(STATUS "Size of int: ${SIZEOF_INT} bytes.")
    if (SIZEOF_INT LESS 4)
        message(FATAL_ERROR "Int is too small.")
    endif()
endif()
target_compile_definitions(imm PRIVATE SIZEOF_INT=${SIZEOF_INT})

check_type_size("long" SIZEOF_LONG)
if(NOT HAVE_SIZEOF_LONG)
    message(FATAL_ERROR "Could not determine the size of long.")
else()
    message(STATUS "Size of long: ${SIZEOF_LONG} bytes.")
endif()
target_compile_definitions(imm PRIVATE SIZEOF_LONG=${SIZEOF_LONG})

if(CLANG_ASAN_UBSAN)
    message(STATUS "Enabling Clang address sanitizer and undefined behavior sanitizer.")
    target_compile_options(imm PRIVATE -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined)
    set_property(TARGET imm APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=address -fsanitize=undefined ")
elseif(CLANG_MSAN)
    message(STATUS "Enabling Clang memory sanitizer.")
    target_compile_options(imm PRIVATE -fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    set_property(TARGET imm APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins ")
endif()

install(TARGETS imm EXPORT imm-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
install(DIRECTORY include/ DESTINATION include)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/imm/export.h
    ${CMAKE_CURRENT_BINARY_DIR}/imm/config.h
    DESTINATION include/imm
)
install(EXPORT imm-targets
    FILE imm-targets.cmake
    NAMESPACE IMM::
    DESTINATION lib/cmake/imm
)
include(CMakePackageConfigHelpers)
configure_package_config_file(imm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
    INSTALL_DESTINATION lib/cmake/imm
    NO_SET_AND_CHECK_MACRO
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
    DESTINATION lib/cmake/imm
)

enable_testing()
add_subdirectory(test)
# add_subdirectory(perf)

option(COVERAGE "Enable coverage")
if(COVERAGE)
    string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
    if(NOT ${uppercase_CMAKE_BUILD_TYPE} MATCHES "DEBUG")
        message(FATAL_ERROR "CMAKE_BUILD_TYPE must be set to Debug.")
    endif()

    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE ctest
        DEPENDENCIES all_tests)
endif()
