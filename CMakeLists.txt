cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(imm)

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
  add_compile_options(-Wall -Wextra -pedantic -Wstrict-prototypes -Wshadow
    -Wconversion -Wmissing-prototypes -Wno-unused-parameter)
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
  src/imm/matrix.c
  src/imm/normal_state.c
  src/imm/path.c
  src/imm/report.c
  src/imm/mute_state.c
  src/imm/state.c
  src/imm/state_idx.c
  # src/imm/table_state.c
  src/imm/mm_state.c
  src/imm/mm_trans.c
)

add_library(immobj OBJECT ${IMM_SOURCES})

add_library(imm SHARED $<TARGET_OBJECTS:immobj>)
set_target_properties(imm PROPERTIES VERSION ${IMM_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${IMM_VERSION_MAJOR})
set_target_properties(imm PROPERTIES PUBLIC_HEADER ${IMM_PUBLIC_HEADER})
target_compile_definitions(imm PUBLIC IMM_API_EXPORTS)
imm_sanitizer_add_target(imm)

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

imm_add_test(abc)
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
      -object $<TARGET_FILE:test_abc>
      -object $<TARGET_FILE:test_state>
      -object $<TARGET_FILE:test_hmm>
      -object $<TARGET_FILE:test_path>
      ${IMM_SOURCES_PATH}
    BYPRODUCTS coverage/index.html
    COMMENT "The coverage summary will be written to coverage/index.html")
endif()
