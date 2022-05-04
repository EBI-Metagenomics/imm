cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

project(
    imm
    VERSION 2.0.0
    LANGUAGES C)

include(cmake/compiler-options.cmake)
include(cmake/sanitizers.cmake)
include(cmake/CPM.cmake)

cpmaddpackage(NAME cco GIT_TAG 0.0.9 GITHUB_REPOSITORY
              EBI-Metagenomics/c-containers)
cpmaddpackage(NAME elapsed GIT_TAG 3.0.5 GITHUB_REPOSITORY horta/elapsed)
cpmaddpackage(NAME logaddexp GIT_TAG 2.1.8 GITHUB_REPOSITORY horta/logaddexp)
cpmaddpackage(NAME lite_pack VERSION 0.1.4 GITHUB_REPOSITORY
              EBI-Metagenomics/lite-pack)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

add_compile_definitions(_XOPEN_SOURCE=700)
add_compile_definitions(_DARWIN_C_SOURCE=1)

add_library(
    ${PROJECT_NAME}
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
add_library(IMM::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Enable interprodecural optimization
macro(enable_ipo lib)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        include(CheckIPOSupported)
        check_ipo_supported()
        set_target_properties(${lib} PROPERTIES INTERPROCEDURAL_OPTIMIZATION
                                                TRUE)
    endif()
endmacro()

enable_ipo(${PROJECT_NAME})

set(EXPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/export.h)
include(GenerateExportHeader)
generate_export_header(
    ${PROJECT_NAME}
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
    ${PROJECT_NAME}
    PUBLIC $<INSTALL_INTERFACE:include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(${PROJECT_NAME} PUBLIC LOGADDEXP::logaddexp)
target_link_libraries(${PROJECT_NAME} PUBLIC ELAPSED::elapsed)
target_link_libraries(${PROJECT_NAME} PUBLIC CCO::cco)
target_link_libraries(${PROJECT_NAME} PUBLIC LITE_PACK::lite_pack)
target_link_libraries(${PROJECT_NAME} INTERFACE $<$<BOOL:${UNIX}>:m>)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION
                                                 ${PROJECT_VERSION_MAJOR})

include(CheckSymbolExists)
check_symbol_exists(reallocf "stdlib.h" HAVE_REALLOCF)

target_compile_definitions(
    ${PROJECT_NAME} PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_compile_definitions(${PROJECT_NAME}
                           PRIVATE $<$<BOOL:${WIN32}>:__extension__>)
target_compile_definitions(${PROJECT_NAME}
                           PRIVATE $<$<BOOL:${HAVE_REALLOCF}>:HAVE_REALLOCF>)

target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})
target_compile_options(${PROJECT_NAME} PRIVATE "-fverbose-asm")

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
target_compile_definitions(${PROJECT_NAME} PRIVATE SIZEOF_INT=${SIZEOF_INT})

check_type_size("long" SIZEOF_LONG)
if(NOT HAVE_SIZEOF_LONG)
    message(FATAL_ERROR "Could not determine the size of long.")
else()
    message(STATUS "Size of long: ${SIZEOF_LONG} bytes.")
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE SIZEOF_LONG=${SIZEOF_LONG})

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets)

install(DIRECTORY include/${PROJECT_NAME}/ DESTINATION include/${PROJECT_NAME})

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE IMM::
    DESTINATION lib/cmake/${PROJECT_NAME})

install(FILES ${EXPORT_FILE} ${CONFIG_FILE} DESTINATION include/imm)

set(CMAKE_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake)
configure_package_config_file(
    ${PROJECT_NAME}-config.cmake.in ${CMAKE_CONFIG_FILE}
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})

set(CMAKE_VERSION_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake)
write_basic_package_version_file(${CMAKE_VERSION_FILE}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_VERSION_FILE} ${CMAKE_CONFIG_FILE}
        DESTINATION lib/cmake/${PROJECT_NAME})
install(FILES ${EXPORT_FILE} ${CONFIG_FILE} DESTINATION include/${PROJECT_NAME})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
              ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md
        DESTINATION share/docs/${PROJECT_NAME})

enable_testing()
add_subdirectory(test)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
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
