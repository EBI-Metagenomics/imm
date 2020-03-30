cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(imm VERSION 0.0.6 LANGUAGES C)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# Set RPATH only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

# merge request fix: https://gitlab.kitware.com/cmake/cmake/-/merge_requests/1799
if(CMAKE_VERSION VERSION_GREATER 3.12.0)
    # Hide things by default
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif()

find_package(logaddexp REQUIRED)
find_package(OpenMP)

add_library(imm
    src/abc.c
    src/abc_table.c
    src/cartes.c
    src/dp.c
    src/dp_emission.c
    src/dp_matrix.c
    src/dp_states.c
    src/dp_trans.c
    src/eseq.c
    src/hmm.c
    src/ipow.c
    src/mstate.c
    src/mstate_sort.c
    src/mstate_table.c
    src/mtrans.c
    src/mtrans_table.c
    src/mute_state.c
    src/normal_state.c
    src/path.c
    src/report.c
    src/results.c
    src/seq.c
    src/seq_code.c
    src/seq_table.c
    src/state.c
    src/state_idx.c
    src/step.c
    src/table_state.c
    src/window.c
)
add_library(IMM::imm ALIAS imm)

# Write out mytools_export.h to the current binary directory
# Must include ${CMAKE_CURRENT_BINARY_DIR} in the target's
# search path.
include(GenerateExportHeader)
generate_export_header(imm BASE_NAME IMM
    INCLUDE_GUARD_NAME IMM_EXPORT_H
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/imm/export.h)

add_library(warning INTERFACE)
target_compile_options(warning INTERFACE
    $<$<STREQUAL:${CMAKE_C_COMPILER_ID},MSVC>:/W3>
    $<$<NOT:$<STREQUAL:${CMAKE_C_COMPILER_ID},MSVC>>:-Wall -Wextra -pedantic
    -Wstrict-prototypes -Wshadow -Wconversion -Wmissing-prototypes
    -Wno-unused-parameter -Wsign-conversion>
)

target_include_directories(imm
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(imm PUBLIC LOGADDEXP::logaddexp)
target_link_libraries(imm PRIVATE $<$<BOOL:${OpenMP_C_FOUND}>:OpenMP::OpenMP_C>)
set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
target_compile_features(imm PRIVATE c_std_11)
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:__extension__>)
target_compile_definitions(imm PRIVATE $<$<BOOL:${OpenMP_C_FOUND}>:OPENMP>)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)
target_link_libraries(imm PRIVATE warning)

include(CheckCSourceCompiles)
check_c_source_compiles(
    "
        static void f(char const* err, ...) __attribute__((format(printf, 1, 2)));
        int main(void) {return 0;}
    "
    HAVE_ATTR_FORMAT
)
target_compile_definitions(imm PRIVATE $<$<BOOL:${HAVE_ATTR_FORMAT}>:HAVE_ATTR_FORMAT>)

install(TARGETS imm EXPORT imm-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
install(DIRECTORY include/ DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/imm/export.h
    DESTINATION include/imm
)
install(EXPORT imm-targets
    FILE imm-targets.cmake
    NAMESPACE IMM::
    DESTINATION lib/cmake/imm
)
include(CMakePackageConfigHelpers)
configure_package_config_file(imm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
    INSTALL_DESTINATION lib/cmake/imm
    NO_SET_AND_CHECK_MACRO
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
    DESTINATION lib/cmake/imm
)

enable_testing()
add_subdirectory(test)
