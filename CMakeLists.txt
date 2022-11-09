cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
  imm
  VERSION 2.0.10
  LANGUAGES C)

set(IMM_MAIN_PROJECT OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(IMM_MAIN_PROJECT ON)
endif()

if(IMM_MAIN_PROJECT)
  set(IMM_BUILD_TESTS_DEFAULT ON)
else()
  set(IMM_BUILD_TESTS_DEFAULT OFF)
endif()

option(IMM_BUILD_TESTS "Build the unit tests" ${IMM_BUILD_TESTS_DEFAULT})
message(STATUS "IMM_MAIN_PROJECT: " ${IMM_MAIN_PROJECT})
message(STATUS "IMM_BUILD_TESTS: " ${IMM_BUILD_TESTS})

include(cmake/compiler-options.cmake)
include(cmake/sanitizers.cmake)
include(cmake/CPM.cmake)

cpmaddpackage("gh:horta/elapsed@3.0.8")
cpmaddpackage("gh:horta/logaddexp@2.1.10")
cpmaddpackage("gh:EBI-Metagenomics/lite-pack@0.4.0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(
  imm
  src/abc.c
  src/abc_types.c
  src/amino.c
  src/bitmap.c
  src/cartes.c
  src/code.c
  src/codon_marg.c
  src/codon_state.c
  src/dna.c
  src/dp/dp.c
  src/dp/emis.c
  src/dp/io.c
  src/dp/matrix.c
  src/dp/path.c
  src/dp/state_table.c
  src/dp/trans_table.c
  src/error.c
  src/eseq.c
  src/example1.c
  src/example2.c
  src/expect.c
  src/frame_cond.c
  src/frame_state.c
  src/gc.c
  src/gc_auto.c
  src/hmm.c
  src/lprob.c
  src/mute_state.c
  src/normal_state.c
  src/nuclt.c
  src/nuclt_code.c
  src/nuclt_lprob.c
  src/path.c
  src/reallocf.c
  src/rna.c
  src/rnd.c
  src/seq.c
  src/state.c
  src/task.c
  src/tsort.c)
add_library(IMM::imm ALIAS imm)

# Enable interprocedural optimization
macro(enable_ipo lib)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported()
    set_target_properties(${lib} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endmacro()

enable_ipo(imm)

set(EXPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/imm/export.h)
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
  ${EXPORT_FILE})

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

target_include_directories(
  imm
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_compile_features(imm PRIVATE c_std_23)
target_link_libraries(imm PUBLIC LOGADDEXP::logaddexp)
target_link_libraries(imm PRIVATE ELAPSED::elapsed)
target_link_libraries(imm PRIVATE LITE_PACK::lite_pack)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)
set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

include(CheckSymbolExists)
check_symbol_exists(reallocf "stdlib.h" HAVE_REALLOCF)

target_compile_definitions(imm
                           PRIVATE $<$<BOOL:${HAVE_REALLOCF}>:HAVE_REALLOCF>)

target_compile_options(imm PRIVATE ${WARNING_FLAGS})

set(CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/imm/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/imm/config.h.in
               ${CONFIG_FILE} @ONLY)

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

install(TARGETS imm EXPORT imm-targets)

install(DIRECTORY include/imm/ DESTINATION include/imm)

install(
  EXPORT imm-targets
  FILE imm-targets.cmake
  NAMESPACE IMM::
  DESTINATION lib/cmake/imm)

install(FILES ${EXPORT_FILE} ${CONFIG_FILE} DESTINATION include/imm)

set(CMAKE_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake)
configure_package_config_file(imm-config.cmake.in ${CMAKE_CONFIG_FILE}
                              INSTALL_DESTINATION lib/cmake/imm)

set(CMAKE_VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/imm-config-version.cmake)
write_basic_package_version_file(${CMAKE_VERSION_FILE}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_VERSION_FILE} ${CMAKE_CONFIG_FILE}
        DESTINATION lib/cmake/imm)
install(FILES ${EXPORT_FILE} ${CONFIG_FILE} DESTINATION include/imm)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
              ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md DESTINATION share/docs/imm)

if(IMM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

set(CPACK_PACKAGE_NAME imm)
set(CPACK_PACKAGE_VENDOR "Danilo Horta")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Invisible Markov model library")
set(CPACK_PACKAGE_CONTACT "Danilo Horta")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/package)
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_RELOCATABLE YES)
set(CPACK_MONOLITHIC_INSTALL YES)
include(CPack)
