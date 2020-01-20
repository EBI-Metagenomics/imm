cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)
project(imm VERSION 0.0.3 LANGUAGES C)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# Set RPATH only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

add_library(imm
    src/abc.c
    src/dp.c
    src/hmm.c
    src/lprob.c
    src/matrix.c
    src/mstate.c
    src/mstate_sort.c
    src/mtrans.c
    src/mute_state.c
    src/normal_state.c
    src/path.c
    src/report.c
    src/sequence_table.c
    src/state.c
    src/state_idx.c
    src/step.c
    src/table_state.c
)
add_library(IMM::imm ALIAS imm)

find_package(logaddexp REQUIRED)

target_include_directories(imm
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(imm PRIVATE LOGADDEXP::logaddexp)
set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
target_compile_features(imm PRIVATE c_std_11)
set_target_properties(imm PROPERTIES DEFINE_SYMBOL "IMM_EXPORTS")
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:_CRT_NONSTDC_NO_DEPRECATE>)
target_compile_definitions(imm PRIVATE $<$<BOOL:${WIN32}>:__extension__>)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)
target_compile_options(imm PRIVATE
    $<$<STREQUAL:${CMAKE_C_COMPILER_ID},MSVC>:/W3 /WX>
    $<$<NOT:$<STREQUAL:${CMAKE_C_COMPILER_ID},MSVC>>:-Wall -Wextra -pedantic -Wstrict-prototypes
        -Wshadow -Wconversion -Wmissing-prototypes -Wno-unused-parameter
        -Wno-empty-translation-unit>
)

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
)
install(DIRECTORY include/ DESTINATION include)
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
