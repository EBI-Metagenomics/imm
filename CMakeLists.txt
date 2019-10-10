cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

include(imm.cmake)

imm_get_version(${CMAKE_CURRENT_SOURCE_DIR}/include/imm.h IMM_VERSION_MAJOR IMM_VERSION_MINOR IMM_VERSION_PATCH)
set(IMM_VERSION ${IMM_VERSION_MAJOR}.${IMM_VERSION_MINOR}.${IMM_VERSION_PATCH})

project(imm VERSION ${IMM_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Use FOLDER target property to organize targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "IMM_VERSION=${IMM_VERSION}")
message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

imm_enable_default_build_type()
imm_enable_sanitizer_option()
imm_enable_coverage_option()

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -pedantic -Wstrict-prototypes -Wshadow -Wconversion
        -Wmissing-prototypes -Wno-unused-parameter)
endif()

include_directories(include)
include_directories(.)

# Interface header that that the user is used to include.
set(IMM_PUBLIC_HEADER
    include/imm.h
)

# Headers that are included by the interface header and
# installed alongside it.
set(IMM_PRIVATE_HEADERS
    include/imm/abc.h
    include/imm/api.h
    include/imm/hmm.h
    include/imm/log.h
    include/imm/math.h
    include/imm/mute_state.h
    include/imm/normal_state.h
    include/imm/path.h
    include/imm/report.h
    include/imm/state.h
    include/imm/table_state.h
)

set(IMM_SOURCES
    src/imm/abc.c
    src/imm/counter.c
    src/imm/dp.c
    src/imm/hmm.c
    src/imm/log.c
    src/imm/math.c
    src/imm/matrix.c
    src/imm/mm_state.c
    src/imm/mm_trans.c
    src/imm/mute_state.c
    src/imm/normal_state.c
    src/imm/path.c
    src/imm/report.c
    src/imm/state.c
    src/imm/state_idx.c
    src/imm/table_state.c
)

add_library(immobj OBJECT ${IMM_SOURCES})
set_property(TARGET immobj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(imm SHARED $<TARGET_OBJECTS:immobj>)
set_target_properties(imm PROPERTIES VERSION ${IMM_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${IMM_VERSION_MAJOR})
set_target_properties(imm PROPERTIES PUBLIC_HEADER ${IMM_PUBLIC_HEADER})
if(NOT WIN32)
    target_link_libraries(imm m)
endif()
target_compile_definitions(imm PUBLIC IMM_API_EXPORTS)
imm_sanitizer_add_target(imm)

enable_testing()

imm_add_test(abc imm)
imm_add_test(state imm)
imm_add_test(hmm imm)
imm_add_test(path imm)

imm_enable_coverage("${IMM_SOURCES}")
