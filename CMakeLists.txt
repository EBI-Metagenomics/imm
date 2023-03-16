cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(
  imm
  VERSION 2.2.1
  LANGUAGES C)
set(PROJECT_DESCRIPTION "Invisible Markov model library.")

include(cmake/sanitizers.cmake)
include(cmake/warnings.cmake)
include(cmake/CPM.cmake)

cpmaddpackage("gh:horta/elapsed@3.1.2")
cpmaddpackage("gh:horta/logaddexp@2.1.14")
cpmaddpackage("gh:EBI-Metagenomics/lip@0.5.0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
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
  src/range.c
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
  src/frame_state_15.c
  src/frame_state_24.c
  src/frame_state_33.c
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
  src/str.c
  src/task.c
  src/tsort.c)
add_library(IMM::imm ALIAS imm)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(imm PUBLIC -DIMM_STATIC_DEFINE)
endif()

target_include_directories(
  imm
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
  PRIVATE ${PROJECT_SOURCE_DIR}/src)

set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
target_compile_options(imm PRIVATE ${WARNING_FLAGS})

target_link_libraries(imm PUBLIC LOGADDEXP::logaddexp)
target_link_libraries(imm PRIVATE ELAPSED::elapsed)
target_link_libraries(imm PRIVATE LIP::lip)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)

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

include(CheckSymbolExists)
check_symbol_exists(reallocf "stdlib.h" HAVE_REALLOCF)

target_compile_definitions(imm
                           PRIVATE $<$<BOOL:${HAVE_REALLOCF}>:HAVE_REALLOCF>)

set(CONFIG_FILE ${PROJECT_BINARY_DIR}/imm/config.h)
configure_file(${PROJECT_SOURCE_DIR}/include/imm/config.h.in ${CONFIG_FILE}
               @ONLY)

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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported()
  set_target_properties(imm PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

install(
  TARGETS imm
  EXPORT imm-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(DIRECTORY include/imm/ DESTINATION include/imm)
install(FILES ${CONFIG_FILE} DESTINATION include/imm)

export(
  TARGETS imm
  NAMESPACE IMM::
  FILE imm-targets.cmake)
install(
  EXPORT imm-targets
  NAMESPACE IMM::
  DESTINATION lib/cmake/imm)

install(FILES ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/LICENSE.md
        DESTINATION share/docs/imm)

include(CMakePackageConfigHelpers)

set(project_config ${PROJECT_BINARY_DIR}/imm-config.cmake)
set(version_config ${PROJECT_BINARY_DIR}/imm-config-version.cmake)

configure_package_config_file(imm-config.cmake.in ${project_config}
                              INSTALL_DESTINATION lib/cmake/imm)

write_basic_package_version_file(${version_config}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${project_config} ${version_config} DESTINATION lib/cmake/imm)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  enable_testing()
  add_subdirectory(test)
endif()
