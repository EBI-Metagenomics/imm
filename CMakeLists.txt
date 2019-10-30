cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(imm VERSION 0.0.1 LANGUAGES C)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(imm
    src/abc.c
    src/ascii.c
    src/dp.c
    src/hmm.c
    src/lprob.c
    src/matrix.c
    src/mm_state.c
    src/mm_state_sort.c
    src/mm_trans.c
    src/mute_state.c
    src/normal_state.c
    src/path.c
    src/report.c
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
target_compile_definitions(imm PRIVATE IMM_API_EXPORTS)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)
target_compile_options(imm PRIVATE
    $<$<BOOL:${WIN32}>:/W4 /WX>
    $<$<NOT:$<BOOL:${WIN32}>>:-Wall -Wextra -pedantic -Wstrict-prototypes 
        -Wshadow -Wconversion -Wmissing-prototypes -Wno-unused-parameter
        -Wno-empty-translation-unit>
)

install(TARGETS imm EXPORT imm-targets)
install(DIRECTORY include/ DESTINATION include)

# Export the targets to a script
install(EXPORT imm-targets
    FILE imm-config.cmake
    NAMESPACE IMM::
    DESTINATION lib/cmake/imm
)

enable_testing()
add_subdirectory(test)
