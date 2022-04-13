cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

project(
    imm
    VERSION 0.0.49
    LANGUAGES C)

find_package(cco REQUIRED)
find_package(elapsed REQUIRED)
find_package(lite_pack REQUIRED)
find_package(logaddexp REQUIRED)

include(external/cmake-boilerplate/cbp.cmake)
include(cmake/compiler-options.cmake)
include(cmake/sanitizers.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

add_compile_definitions(_XOPEN_SOURCE=700)
add_compile_definitions(_DARWIN_C_SOURCE=1)

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
    src/log.c
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
add_library(IMM::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Enable interprodecural optimization
macro(enable_ipo lib)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported()
    set_target_properties(${lib} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endmacro()

enable_ipo(${PROJECT_NAME})

cbp_generate_export_header(imm EXPORT_FILE)

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
target_link_libraries(imm PUBLIC LITE_PACK::lite_pack)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)
set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

include(CheckSymbolExists)
check_symbol_exists(reallocf "stdlib.h" HAVE_REALLOCF)

target_compile_definitions(
    imm PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:__extension__>)
target_compile_definitions(imm
                           PRIVATE $<$<BOOL:${HAVE_REALLOCF}>:HAVE_REALLOCF>)

target_compile_options(imm PRIVATE ${WARNING_FLAGS})
target_compile_options(imm PRIVATE "-fverbose-asm")

set(CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/imm/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/imm/config.h.in ${CONFIG_FILE}
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

cbp_install(imm MERGED)
install(FILES ${EXPORT_FILE} ${CONFIG_FILE} DESTINATION include/imm)

enable_testing()
add_subdirectory(test)
if(FUZZ)
    add_subdirectory(fuzz)
endif()
