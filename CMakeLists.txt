cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(
    imm
    VERSION 0.0.25
    LANGUAGES C)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# Set RPATH only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
     "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

find_package(logaddexp REQUIRED)
find_package(elapsed REQUIRED)
find_package(cco REQUIRED)

add_library(
    imm
    src/abc.c
    src/amino.c
    src/bitmap.c
    src/cartes.c
    src/codon_marg.c
    src/codon_state.c
    src/dna.c
    src/dp/code.c
    src/dp/dp.c
    src/dp/emis.c
    src/dp/eseq.c
    src/dp/io.c
    src/dp/matrix.c
    src/dp/path.c
    src/dp/state_table.c
    src/dp/trans_table.c
    src/error.c
    src/example1.c
    src/example2.c
    src/frame_state.c
    src/gc.c
    src/gc_auto.c
    src/hmm.c
    src/log.c
    src/lprob.c
    src/mute_state.c
    src/normal_state.c
    src/nuclt.c
    src/path.c
    src/rna.c
    src/rnd.c
    src/seq.c
    src/state.c
    src/task.c
    src/third-party/cmp.c
    src/tsort.c)
add_library(IMM::imm ALIAS imm)

# Hide functions by default.
set_property(TARGET imm PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET imm PROPERTY VISIBILITY_INLINES_HIDDEN ON)

# Write out imm/export.h to the current binary directory.
include(GenerateExportHeader)
generate_export_header(
    imm
    BASE_NAME
    IMM
    INCLUDE_GUARD_NAME
    IMM_EXPORT_H
    EXPORT_MACRO_NAME
    IMM_API
    EXPORT_FILE_NAME
    ${CMAKE_CURRENT_BINARY_DIR}/imm/export.h)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # /wd5105:
    # https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5105?view=msvc-160
    set(WARNING_FLAGS /W3 /wd5105)
else()
    set(WARNING_FLAGS
        -Wall
        -Wextra
        -Wstrict-prototypes
        -Wshadow
        -Wconversion
        -Wmissing-prototypes
        -Wno-unused-parameter
        -Wsign-conversion
        -Wno-unused-function)
endif()

if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
    list(APPEND WARNING_FLAGS -Wno-gnu-designator -Wno-empty-translation-unit
         -Wno-gnu-statement-expression -Wno-nullability-extension)
endif()

option(CI_BUILD "CI, extra flags will be set" OFF)
if(CI_BUILD)
    message(STATUS "CI build enabled")
    add_compile_options(-Werror)
endif()

if(DEFINED ENV{IMM_DOUBLE_PRECISION})
    set(IMM_DOUBLE_PRECISION "$ENV{IMM_DOUBLE_PRECISION}")
else()
    option(IMM_DOUBLE_PRECISION
           "Enable double-precision floating-point format for internals" OFF)
endif()

if(IMM_DOUBLE_PRECISION)
    message(STATUS "Double-precision floating-point - ON")
else()
    message(STATUS "Double-precision floating-point - OFF")
endif()

option(FUZZ "Enable fuzz testing." OFF)
if(FUZZ)
    message(STATUS "Fuzz testing - ON")
else()
    message(STATUS "Fuzz testing - OFF")
endif()

target_include_directories(
    imm
    PUBLIC $<INSTALL_INTERFACE:include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(imm PUBLIC LOGADDEXP::logaddexp)
target_link_libraries(imm PUBLIC ELAPSED::elapsed)
target_link_libraries(imm PUBLIC CCO::cco)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)
set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
target_compile_features(imm PRIVATE c_std_11)
target_compile_definitions(
    imm PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:__extension__>)
target_compile_options(imm PRIVATE ${WARNING_FLAGS})
target_compile_options(imm PRIVATE "-fverbose-asm")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/imm/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/imm/config.h @ONLY)

include(CheckTypeSize)

check_type_size("int" SIZEOF_INT)
if(NOT HAVE_SIZEOF_INT)
    message(FATAL_ERROR "Could not determine the size of int.")
else()
    message(STATUS "Size of int: ${SIZEOF_INT} bytes.")
    if(SIZEOF_INT LESS 4)
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

install(
    TARGETS imm
    EXPORT imm-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
install(DIRECTORY src/imm/ DESTINATION include/imm)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/imm/export.h
              ${CMAKE_CURRENT_BINARY_DIR}/imm/config.h DESTINATION include/imm)
install(
    EXPORT imm-targets
    FILE imm-targets.cmake
    NAMESPACE IMM::
    DESTINATION lib/cmake/imm)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    imm-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
    INSTALL_DESTINATION lib/cmake/imm
    NO_SET_AND_CHECK_MACRO)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
        DESTINATION lib/cmake/imm)

enable_testing()
add_subdirectory(test)
if(FUZZ)
    add_subdirectory(fuzz)
endif()
