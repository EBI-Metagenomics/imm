cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Fetch nhmm version from header file
include(GetNHMMVersion)
GetNHMMVersion(${CMAKE_CURRENT_SOURCE_DIR}/include/nhmm.h NHMM_VERSION_MAJOR NHMM_VERSION_MINOR NHMM_VERSION_PATCH)
set(NHMM_VERSION ${NHMM_VERSION_MAJOR}.${NHMM_VERSION_MINOR}.${NHMM_VERSION_PATCH})

project(nhmm VERSION ${NHMM_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Use FOLDER target property to organize targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "NHMM_VERSION=${NHMM_VERSION}")
message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

# Build type.
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "CMAKE_BUILD_TYPE not specified, default is 'Debug'")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
else()
  message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(DEBUG 1)
else()
  set(DEBUG 0)
endif()

option(CLANG_ASAN_UBSAN "Enable Clang address & undefined behavior sanitizer for nhmm library." OFF)
if(CLANG_ASAN_UBSAN AND NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "Sanitizers are only supported for Clang")
endif()


if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -pedantic -Wstrict-prototypes)
endif()

# Simple Dynamic Strings library: https://github.com/antirez/sds
set(SDS_INCLUDE_DIRS deps/sds)
set(SDS_SOURCES deps/sds/sds.c)

include_directories(include)
include_directories(${SDS_INCLUDE_DIRS})
include_directories(src)

# Interface header that that the user is used to include.
set(NHMM_PUBLIC_HEADER
  include/nhmm.h
)

# Headers that are included by the interface header, and therefore
# installed alongside it.
set(NHMM_PRIVATE_HEADERS
  include/nhmm/api.h
  include/nhmm/alphabet.h
)

set(NHMM_SOURCES
  src/alphabet.c
  src/emission.c
  src/hmm.c
  src/state/normal.c
  src/state/silent.c
  src/state/state.c
)

add_library(nhmm SHARED ${NHMM_SOURCES} ${SDS_SOURCES})
set_target_properties(nhmm PROPERTIES VERSION ${NHMM_VERSION})
set_target_properties(nhmm PROPERTIES SOVERSION ${NHMM_VERSION_MAJOR})
set_target_properties(nhmm PROPERTIES PUBLIC_HEADER ${NHMM_PUBLIC_HEADER})

if(CLANG_ASAN_UBSAN)
  message(STATUS "Enabling Clang address sanitizer and undefined behavior sanitizer for nhmm library.")
  set_property(TARGET nhmm APPEND PROPERTY COMPILE_DEFINITIONS EXITFREE)
  set_property(TARGET nhmm APPEND PROPERTY COMPILE_OPTIONS -fno-sanitize-recover=all
    -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined)
  set_property(TARGET nhmm APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=address -fsanitize=undefined ")
endif()

# Begin of test setup block.
enable_testing()

# Unit library for testing C projects: https://github.com/ThrowTheSwitch/Unity
set(UNITY_INCLUDE_DIRS test/unity)
set(UNITY_SOURCES test/unity/unity.c)

function(nhmm_add_test name)
  add_executable(test_${name} test/${name}.c ${UNITY_SOURCES})
  target_link_libraries(test_${name} nhmm)
  target_include_directories(test_${name} PUBLIC ${UNITY_INCLUDE_DIRS})
  target_compile_definitions(test_${name} PUBLIC UNITY_INCLUDE_DOUBLE)
  add_test(NAME ${name} COMMAND test_${name})
endfunction()

nhmm_add_test(alphabet)
nhmm_add_test(state)
# End of test setup block.
