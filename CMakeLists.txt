cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Fetch imm version from header file
include(GetIMMVersion)
GetIMMVersion(${CMAKE_CURRENT_SOURCE_DIR}/include/imm.h IMM_VERSION_MAJOR IMM_VERSION_MINOR IMM_VERSION_PATCH)
set(IMM_VERSION ${IMM_VERSION_MAJOR}.${IMM_VERSION_MINOR}.${IMM_VERSION_PATCH})

project(imm VERSION ${IMM_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Use FOLDER target property to organize targets into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "IMM_VERSION=${IMM_VERSION}")
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

option(CLANG_ASAN_UBSAN "Enable Clang address & undefined behavior sanitizer for imm library" OFF)
if(CLANG_ASAN_UBSAN AND NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "Sanitizers are only supported for Clang")
endif()

option(CLANG_CODE_COVERAGE "Enable code coverage metrics in Clang" OFF)
if (CLANG_CODE_COVERAGE)
    message(STATUS "Code coverage metrics enabled for debug build")
    set(CMAKE_C_FLAGS_DEBUG
      "${CMAKE_C_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -pedantic -Wstrict-prototypes -Wshadow
    -Wconversion -Wmissing-prototypes -Wno-unused-parameter)
endif()

# rapidstring: https://github.com/boyerjohn/rapidstring
set(RS_INCLUDE_DIRS deps/rapidstring)

# uthash: https://github.com/troydhanson/uthash
set(UTHASH_INCLUDE_DIRS deps/uthash)

# logaddexp: https://github.com/horta/logaddexp
set(LOGADDEXP_INCLUDE_DIRS deps/logaddexp)

include_directories(include)
include_directories(${RS_INCLUDE_DIRS})
include_directories(${UTHASH_INCLUDE_DIRS})
include_directories(${LOGADDEXP_INCLUDE_DIRS})
include_directories(src)

# Interface header that that the user is used to include.
set(IMM_PUBLIC_HEADER
  include/imm.h
)

# Headers that are included by the interface header, and therefore
# installed alongside it.
set(IMM_PRIVATE_HEADERS
  include/imm/api.h
  include/imm/alphabet.h
)

set(IMM_SOURCES
  src/alphabet.c
  src/array.c
  src/codon.c
  src/counter.c
  src/dp.c
  src/hmm.c
  src/matrix.c
  src/path.c
  src/report.c
  src/state_idx.c
  src/state/frame.c
  src/state/normal.c
  src/state/silent.c
  src/state/state.c
  src/tbl_state.c
  src/tbl_trans.c
)

add_library(immlib OBJECT ${IMM_SOURCES})

add_library(imm SHARED $<TARGET_OBJECTS:immlib>)
set_target_properties(imm PROPERTIES VERSION ${IMM_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${IMM_VERSION_MAJOR})
set_target_properties(imm PROPERTIES PUBLIC_HEADER ${IMM_PUBLIC_HEADER})

if(CLANG_ASAN_UBSAN)
  message(STATUS "Enabling Clang address sanitizer and undefined behavior sanitizer for imm library.")
  set_property(TARGET imm APPEND PROPERTY COMPILE_DEFINITIONS EXITFREE)
  set_property(TARGET imm APPEND PROPERTY COMPILE_OPTIONS -fno-sanitize-recover=all
    -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined)
  set_property(TARGET imm APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=address -fsanitize=undefined ")
endif()

# Begin of test setup block.
enable_testing()

# Unit library for testing C projects: https://github.com/ThrowTheSwitch/Unity
set(UNITY_INCLUDE_DIRS test/unity)
set(UNITY_SOURCES test/unity/unity.c)

function(imm_add_test name)
  add_executable(test_${name} test/${name}.c ${UNITY_SOURCES})
  target_link_libraries(test_${name} imm)
  target_include_directories(test_${name} PUBLIC ${UNITY_INCLUDE_DIRS})
  target_compile_definitions(test_${name} PUBLIC UNITY_INCLUDE_DOUBLE)
  add_test(NAME ${name} COMMAND test_${name})
  if (CLANG_CODE_COVERAGE)
    set_tests_properties(${name} PROPERTIES ENVIRONMENT LLVM_PROFILE_FILE=test_${name}.profraw)
  endif()
endfunction()

imm_add_test(alphabet)
imm_add_test(state)
imm_add_test(hmm)
imm_add_test(path)
# End of test setup block.

foreach(SRC ${IMM_SOURCES})
  set(IMM_SOURCES_PATH ${IMM_SOURCES_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
endforeach()

message(STATUS "${IMM_SOURCES_PATH}")

if (CLANG_CODE_COVERAGE)
  add_custom_target(coverage
    COMMAND llvm-profdata merge -sparse *.profraw -o all.profdata
    COMMAND llvm-cov show -format=html -o coverage -instr-profile=all.profdata
      $<TARGET_FILE:imm>
      -object $<TARGET_FILE:test_alphabet>
      -object $<TARGET_FILE:test_state>
      -object $<TARGET_FILE:test_hmm>
      -object $<TARGET_FILE:test_path>
      ${IMM_SOURCES_PATH}
    BYPRODUCTS coverage/index.html
    COMMENT "The coverage summary will be written to coverage/index.html")
endif()
