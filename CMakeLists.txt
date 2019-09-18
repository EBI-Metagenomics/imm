cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(GetNHMMVersion)
GetNHMMVersion(${CMAKE_CURRENT_SOURCE_DIR}/include/nhmm.h NHMM_VERSION_MAJOR NHMM_VERSION_MINOR NHMM_VERSION_PATCH)
set(NHMM_VERSION ${NHMM_VERSION_MAJOR}.${NHMM_VERSION_MINOR}.${NHMM_VERSION_PATCH})

project(nhmm VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
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

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/deps/sds)
include_directories(${PROJECT_SOURCE_DIR}/src)

set(NHMM_PUBLIC_HEADERS
  include/nhmm.h
)

set(NHMM_PRIVATE_HEADERS
  include/nhmm/api.h
  include/nhmm/alphabet.h
)

set(NHMM_SOURCES
  src/alphabet.c
  src/hmm.c
  src/state/state.c
  src/state/normal.c
  src/state/silent.c
  deps/sds/sds.c
)

add_library(nhmm SHARED ${NHMM_SOURCES})
set_target_properties(nhmm PROPERTIES VERSION ${NHMM_VERSION})
set_target_properties(nhmm PROPERTIES SOVERSION ${NHMM_VERSION_MAJOR})
set_target_properties(nhmm PROPERTIES PUBLIC_HEADER ${NHMM_PUBLIC_HEADERS})

# Test setup
enable_testing()

function(nhmm_add_test name)
  add_executable(test_${name} test/${name}.c test/unity/unity.c)
  target_link_libraries(test_${name} nhmm)
  target_include_directories(test_${name} PUBLIC test/unity)
  add_test(NAME ${name} COMMAND test_${name})
endfunction()

nhmm_add_test(alphabet)
nhmm_add_test(state)
